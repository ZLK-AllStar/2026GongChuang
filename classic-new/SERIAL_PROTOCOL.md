# Jetson to STM32 pick protocol

Updated 2026-10-30 for CoreXY + a six-slot rotary tray. The Jetson Python,
recognition, calibration, service scripts, wire bytes, retries and timeouts are
unchanged. ST7789 is a diagnostic display; USART3 ApusIDE HMI shows production
records using a separate CRC frame carrying a firmware-assigned ID.

PE2 (active low, pull-up) is the shared boot/run button. During the initial full
3-second LED0 blink window a debounced press selects DEBUG, otherwise HOST.
LED0=PF9 and LED1=PF10 are active low. HOST lights LED0, DEBUG lights LED1;
mode remains locked until reset. Screen/servo initialization happens afterwards.
HOST requires stable release and then a fresh press after initialization, without
fabricating a run press from the boot window. DEBUG never triggers recognition
or accepts Jetson mechanical jobs.

Both XY motors and the tray use their current power-on physical positions as
software origins, without limit-switch homing. Failed origin validation blocks
production. The startup notification changes the online prompt only.

## Physical layer

- 115200 baud, 8 data bits, no parity, 1 stop bit, no flow control.
- Jetson Orin Nano 40-pin UART1 uses pin 8 (TX) and pin 10 (RX). The program
  opens `/dev/ttyTHS1` by default on this Jetson, so `python3 _cv2.py` starts
  camera and UART.
- The camera index is probed, not hard-coded: `--camera` is only the preferred
  index, and `open_camera()` also tries the other indices until one returns a
  readable frame. On this Jetson the webcam enumerates as `/dev/video0` and
  `/dev/video1` is an empty metadata node, so the probe is what makes the
  service start unattended.
- The systemd unit waits `HUATAI_CAMERA_DELAY_S` (5 s) before starting so the
  USB camera finishes enumerating after a cold boot.
- If the carrier board maps UART1 differently, override it with
  `--serial-port /dev/ttyTHS2`. Use `--no-serial` for visual-only debugging.
- Use USART2 on the STM32 (PA2/PA3). Do not connect Jetson TX in parallel with
  USART1, the motor bus, or the USART3 HMI port.
- PE2 is wired to GND, with an internal pull-up: idle high, pressed low.
  `button.c` samples every 5 ms and requires 20 ms stable input, including a
  button held at power-on. Protocol PRESS=1 and RELEASE=0 retain their meanings.
- UART5 PC12/PD2 is a separate 115200 8N1 DEBUG Shell, not the vision link.

## Jetson to STM32 pick frame

One frame is 10 bytes:

| Offset | Field | Description |
| --- | --- | --- |
| 0 | HEAD1 | `0xAA` |
| 1 | HEAD2 | `0x55` |
| 2 | TYPE | `0x01`, pick command |
| 3 | SEQ | Task sequence, `0..255` |
| 4 | CMD | Shape/color command, `1..90` (legacy 1..72; original white 73..80; cone 81..90) |
| 5 | X_L | X low byte |
| 6 | X_H | X high byte |
| 7 | Y_L | Y low byte |
| 8 | Y_H | Y high byte |
| 9 | CHECK | Low 8 bits of the sum of bytes 0..8 |

X and Y are unsigned 16-bit little-endian integers in millimetres.

STM32 deduplicates the full `(SEQ,CMD,X_raw,Y_raw)` key for its active job and
most recent terminal job. An active duplicate gets BUSY, a terminal duplicate
gets the saved result; neither rotates, picks or appends HMI again. A different
job while busy gets FAILED. SEQ can wrap; it is not a permanent deduplication key.
Firmware accepts X=0..215 mm and Y=0..205 mm directly, with no old -10 mm offset. The host caps finite Y values above MACHINE_Y_MAX_MM to 205 mm before target selection/display and again when encoding the pick packet. Firmware still rejects incoming Y>205 mm frames.

## Jetson to STM32 startup notification

The vision host announces itself once, right after opening the serial port, so
the STM32 screen can stop saying "waiting for host". Same 6-byte layout as the
pick frame, only TYPE differs:

| Offset | Field | Description |
| --- | --- | --- |
| 0 | HEAD1 | `0xAA` |
| 1 | HEAD2 | `0x55` |
| 2 | TYPE | `0x02`, host startup |
| 3 | SEQ | `0` for the single startup frame |
| 4 | READY | `0x01` |
| 5 | CHECK | Low 8 bits of the sum of bytes 0..4 |

It is pure information. Neither side replies, waits, or gates anything on it —
a run still starts solely from the PE2 press, and the frame may arrive before or
after the button press without changing behaviour. The STM32 keeps accepting it
in every state.

Both sides announce themselves so each can show the link state without an
operator:

| Direction | Frame | Meaning |
| --- | --- | --- |
| Jetson -> STM32 | `AA 55 02 00 01 CHK` | host is up; screen switches to `WAIT BUTTON` |
| STM32 -> Jetson | `55 AA 02 00 01 CHK` | board is online; Jetson logs it once |

The STM32 repeats its announce once per second until it receives a host frame.
The Jetson sends the startup frame immediately and repeats the same 6-byte frame
once per second while the process is alive. This keeps the board's online prompt
stable during an idle wait and lets it return to `WAIT PC` after the host stops.
That makes either boot order work: the Jetson picks up the board immediately,
and the board's screen recovers its prompt within a second if the Jetson restarts
or the UART is plugged in later. The first header byte keeps the directions
apart: `0xAA` frames come from the Jetson, `0x55` frames from the STM32.

## STM32 to Jetson status frame

One frame is 6 bytes:

| Offset | Field | Description |
| --- | --- | --- |
| 0 | HEAD1 | `0x55` |
| 1 | HEAD2 | `0xAA` |
| 2 | TYPE | `0x81`, task status |
| 3 | SEQ | Sequence from the pick frame |
| 4 | STATUS | Status code below |
| 5 | CHECK | Low 8 bits of the sum of bytes 0..4 |

Status codes:

```text
0x01 ACK           task accepted
0x02 BUSY          pick in progress
0x03 DONE          mechanical motion completed
0x04 FAILED        pick failed
0x05 OUT_OF_RANGE  target is outside the workspace
```

## STM32 to Jetson key-event frame

The PE2 button edge is reported with the same 6-byte layout, only TYPE differs:

| Offset | Field | Description |
| --- | --- | --- |
| 0 | HEAD1 | `0x55` |
| 1 | HEAD2 | `0xAA` |
| 2 | TYPE | `0x82`, key event |
| 3 | SEQ | Key-event counter, incremented per report |
| 4 | STATE | `0x01` pressed, `0x00` released |
| 5 | CHECK | Low 8 bits of the sum of bytes 0..4 |

The firmware sends this frame from the same UART it receives pick frames on, so
no extra handshake is needed. Because the two frame types have different first
header bytes (`0xAA` for pick frames, `0x55` for reports), both directions can
be parsed from the same byte stream.

## Run trigger

```text
Jetson: start -> send startup notification (AA 55 02), then WAIT_BUTTON
STM32:  WAIT PC --(startup notification)--> WAIT BUTTON
STM32:  WAIT BUTTON --(PE2 press)--> send key event --> BUSY
Jetson: WAIT_BUTTON --(key event)--> DETECT --> pick loop --> no target --> keep scanning
```

- Both sides start in the idle state. The Jetson stays in `WAIT_BUTTON` and does
  not run recognition at all until a press event arrives.
- The local screen shows `WAIT PC` until the startup notification arrives, then
  `WAIT BUTTON`. Neither string gates anything; they only tell the operator what
  the machine is still waiting for.
- The first press arms continuous recognition. The Jetson then detects and sends
  targets one at a time. If no pickable target is visible for
  `--empty-confirm-seconds` (default 1.0 s), it keeps scanning and resumes
  automatically when a new target appears; no second press is required.
- After the first debounced PE2 press, the STM32 keeps accepting pick frames
  indefinitely. A reset is required to arm the machine again.
- Presses during a run are ignored as noise: the idle timer is not restarted.

## State flow

```text
DETECT -> stable target -> WAIT_ACK -> PICKING -> DONE
                                             -> DETECT next target
```

Jetson sends only one target at a time. It advances as soon as STM32 reports
DONE for the current sequence; there is no additional visual removal check.
Color/shape recognition is paused during WAIT_ACK, PICKING, and ERROR. The
camera is still read continuously to avoid stale buffered frames. Recognition
resumes on the frame after DONE, then the next target must pass the normal
stable-frame check before it is sent.
ACK timeout causes one retransmission with the same SEQ. If no valid response
arrives after that, the task enters ERROR for two seconds. The failed target is
put on a ten-second cooldown, then Jetson may select another target so one
communication failure does not block the entire workflow.

## Firmware mechanical sequence and classification

After validation and ACK, each previously unseen code receives ID1..6 in order;
the seventh new category reuses ID1 while earlier categories retain their IDs.
Mappings persist until MCU reset, including a failed task's assigned mapping.
Invalid/pre-start tasks and Shell preview do not allocate IDs.

ID1..6 select tray phases 0/60/120/180/240/300 degrees. Motion is always in the
configured direction: 300 -> 0 advances 60 degrees, 0 -> 300 advances 300.
Firmware raises the cup, positions the tray, picks at the original host XY,
raises the cup, moves to fixed Release, releases with the cup up, appends one HMI
record, parks at HOME, then reports DONE. Release and HOME are independent
`app_config.h` macros, both initially (0,0) mm. The former six XY drop boxes are
not part of this production path.

The complete job budget is 24 seconds from ACK, including the return to HOME. Owning foreground code stops
motors and invalidates origins on expiry; background receive/display service
never starts another motor transaction. A tray fault prevents pickup. An XY
fault while holding material preserves controlled vacuum. A parking fault after
release reports FAILED but retains the real release record. HMI TX failure is
counted without repeating the mechanical job. UART TX success is not an HMI ACK.
The host uses a 1-second ACK timeout / one retry and a 24-second completion
timeout; actual cycle timing still requires hardware measurement.

## Shape/color command

```text
if shape_index == 8:  # cone
    command = 81 + color_index
elif color_index == 9:  # white, original eight shapes
    command = 73 + shape_index
else:
    command = shape_index * 9 + color_index + 1
```

## Startup and autostart on the Jetson

The vision host runs as the systemd unit `huatai-vision.service`, which needs no
desktop session and therefore works without HDMI:

```text
classic-new/scripts/run_vision.sh               entry point, sources yolov8_env/bin/activate
classic-new/scripts/huatai-vision.service       unit template (installed to /etc/systemd/system)
classic-new/scripts/install_service.sh          install + enable + start (sudo)
classic-new/scripts/uninstall_service.sh        stop + disable + remove (sudo)
classic-new/scripts/selftest_button_logic.py    offline check of key-frame parsing + state machine
```

- Install or update: `sudo /home/jetson/classic-new/scripts/install_service.sh`
- Logs: `journalctl -u huatai-vision -f`
- Offline self-test (no serial hardware or camera needed):
  `python3 /home/jetson/classic-new/scripts/selftest_button_logic.py`
- Camera settings live in `/etc/default/huatai-vision`, e.g.
  `HUATAI_VISION_ARGS="--camera 0 --empty-confirm-seconds 1.5"`. The index is a
  preference only; the program probes the other indices if it cannot read a
  frame, so a wrong value degrades to a slower start instead of a failed one.
- Override arguments (camera index, serial port, timing) by creating
  `/etc/default/huatai-vision` with e.g.
  `HUATAI_VISION_ARGS="--camera 1 --serial-port /dev/ttyTHS1 --empty-confirm-seconds 1.5"`.
- The service always passes `--no-display`, so no OpenCV window is created.

Shape order:

```text
0 regular_tetrahedron
1 triangular_prism
2 pentagonal_prism
3 hexagonal_prism
4 cylinder
5 cuboid
6 cube
7 sphere
8 cone
```

Color order:

```text
0 red
1 orange
2 yellow
3 green
4 cyan
5 blue
6 purple
7 gray
8 black
9 white (original shapes 73..80; cone 90)
```

The OpenCV classifier currently sends only unambiguous projection mappings:
pentagon, hexagon, rectangle, and square. Circle and triangle are not sent.
