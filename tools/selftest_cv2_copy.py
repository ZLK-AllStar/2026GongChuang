"""Exercise the deployed YOLO host without opening a UART or camera."""
import importlib.util
import itertools
import math
import sys
import time
from pathlib import Path

default_script = Path(__file__).resolve().parents[1] / 'cv2_copy.py'
if not default_script.is_file():
    default_script = Path(__file__).resolve().parents[1] / '上位机脚本/cv2_copy.py'
script = Path(sys.argv[1]) if len(sys.argv) > 1 else default_script
spec = importlib.util.spec_from_file_location('huatai_yolo_under_test', script)
module = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = module
spec.loader.exec_module(module)


class FakeSerial:
    def __init__(self):
        self.pending = bytearray()
        self.written = []

    @property
    def in_waiting(self):
        return len(self.pending)

    def read(self, count):
        result = bytes(self.pending[:count])
        del self.pending[:count]
        return result

    def write(self, packet):
        self.written.append(bytes(packet))
        return len(packet)


def make_link():
    link = module.SerialLink.__new__(module.SerialLink)
    link.connection = FakeSerial()
    link.debug = False
    link.receive_buffer = bytearray()
    link.key_presses = 0
    link.key_pressed = False
    link.board_ready = False
    link.received_total = 0
    link.last_hello_time = time.monotonic()
    return link


checks = 0


def check(name, condition):
    global checks
    assert condition, name
    checks += 1


# Compare all command IDs with Core/Inc/hmi_codes.h's stable mapping.
saved_argv = sys.argv
try:
    sys.argv = [str(script)]
    pick_timeout = module.parse_args().pick_timeout
finally:
    sys.argv = saved_argv
check('deployed CLI defaults to a 24-second completion timeout', pick_timeout == 24.0)
colors = (*module.COLOR_ORDER, 'white')
for shape_index, shape in enumerate(module.SHAPE_ORDER):
    for color_index, color in enumerate(colors):
        expected = (81 + color_index if shape_index == 8 else
                    73 + shape_index if color_index == 9 else
                    shape_index * 9 + color_index + 1)
        check(f'mapping {shape}/{color}', module.encode_hmi_command(shape, color) == expected)

check('workspace Y maximum is 205 mm', module.MACHINE_Y_MAX_MM == 205.0)
packet = module.build_pick_packet(42, 90, 215, 205)
check('10-byte little-endian pick frame', packet == bytes.fromhex('aa 55 01 2a 5a d7 00 cd 00 28'))
for y in (205.01, 205.6, 206, 225, 65536, 1e20):
    check('pick encoder caps Y before rounding/uint16 validation',
          module.build_pick_packet(42, 90, 215, y) == packet)
for y in (-1, math.nan, math.inf, -math.inf):
    try:
        module.build_pick_packet(42, 90, 215, y)
    except ValueError:
        check('pick encoder rejects invalid Y', True)
    else:
        check('pick encoder rejects invalid Y', False)
identity = module.np.eye(3, dtype=module.np.float64)
for y in (-1, 0, 204, 205, 206, 225, 70000):
    check('pixel conversion caps only the upper Y boundary',
          module.pixel_to_machine((100, y), identity) == (100.0, min(y, 205.0)))
check('pixel conversion preserves X for existing workspace validation',
      module.pixel_to_machine((216, 206), identity) == (216.0, 205.0))
check('pixel conversion still needs calibration', module.pixel_to_machine((100, 206), None) is None)
check('hello layout/checksum', module.build_hello_packet() == bytes.fromhex('aa 55 02 00 01 02'))
check('status values', list(module.STATUS_NAMES) == [1, 2, 3, 4, 5])

link = make_link()
for status in module.STATUS_NAMES:
    frame = module.build_status_packet(42, status)
    for split in range(1, 6):
        link.connection.pending.extend(b'noise!' + frame[:split])
        check('partial status waits', link.poll_status() == [])
        link.connection.pending.extend(frame[split:])
        check('fragmented status resumes', link.poll_status() == [(42, status)])
bad = bytearray(module.build_key_packet(7, module.KEY_STATE_PRESS))
bad[-1] ^= 1
link.connection.pending.extend(bad + module.build_hello_packet() + packet)
check('bad checksums and wrong direction ignored', link.poll_status() == [] and link.take_key_presses() == 0)
body = bytes((0x55, 0xAA, 0x02, 0, 1))
link.connection.pending.extend(body + bytes((sum(body) & 255,)))
check('board hello is informational', link.poll_status() == [] and link.board_ready and link.take_key_presses() == 0)
link.last_hello_time = time.monotonic() - 2
link.send_hello_if_due()
check('heartbeat sent', link.connection.written[-1] == module.build_hello_packet())
count = len(link.connection.written)
link.send_hello_if_due()
check('heartbeat rate limited', len(link.connection.written) == count)


def detection(x=100.0, y=100.0):
    return module.Detection(color='white', shape='cone', center=(600, 400),
                            box=(580, 380, 40, 40), area=1600, perimeter=160,
                            vertices=3, circularity=0.5, solidity=1.0,
                            machine_center=(x, y), command=90)


link = make_link()
controller = module.PickController(link, 2, 5.0, 1.0, pick_timeout)
start = time.monotonic()
for step in range(5):
    controller.update([detection()], start + step * 0.05)
check('no picks before PE2 press', controller.state == 'WAIT_BUTTON' and not link.connection.written)
link.connection.pending.extend(module.build_key_packet(1, module.KEY_STATE_PRESS) + module.build_key_packet(2, module.KEY_STATE_RELEASE))
controller.update([detection()], start + 0.3)
check('press arms and release keeps armed', controller.state == 'DETECT' and not controller.waiting_button and not link.key_pressed)
controller.update([detection()], start + 0.4)
check('stable target sends one job', controller.state == 'WAIT_ACK' and len(link.connection.written) == 1)
sent = link.connection.written[0]
controller.update([], start + 1.5)
check('retry preserves complete task key', link.connection.written == [sent, sent])
link.connection.pending.extend(module.build_status_packet(99, module.STATUS_DONE))
controller.update([], start + 1.6)
check('foreign sequence ignored', controller.state == 'WAIT_ACK')
link.connection.pending.extend(module.build_status_packet(controller.sequence, module.STATUS_BUSY))
controller.update([], start + 1.7)
check('BUSY recovers lost ACK', controller.state == 'PICKING')
controller.update([detection(120, 120)], start + 10)
check('motion blocks new picks', len(link.connection.written) == 2 and controller.state == 'PICKING')
link.connection.pending.extend(module.build_status_packet(controller.sequence, module.STATUS_DONE))
controller.update([], start + 10.1)
check('DONE resumes continuous scan', controller.state == 'DETECT' and controller.target is None)
controller.update([], start + 20)
check('empty scene stays armed', controller.state == 'DETECT')
for x, y in [(-1, 1), (1, -1), (215.01, 1), (216, 1), (1, 205.01), (1, 206), (math.nan, 1), (1, math.inf)]:
    check('workspace rejects invalid target', controller._select_target([detection(x, y)], start + 30) is None)
for x in (210, 211, 214.99, 215):
    check('workspace inclusive upper boundary', controller._select_target([detection(x, 205)], start + 30) is not None)
controller._send_candidate(detection(120, 120), start + 31)
link.connection.pending.extend(module.build_status_packet(controller.sequence, module.STATUS_ACK))
controller.update([], start + 31.1)
check('ACK enters PICKING', controller.state == 'PICKING')
link.connection.pending.extend(module.build_status_packet(controller.sequence, module.STATUS_FAILED))
controller.update([], start + 31.2)
check('FAILED enters cooldown', controller.state == 'ERROR')
controller.update([], start + 34)
controller._send_candidate(detection(150, 150), start + 35)
link.connection.pending.extend(module.build_status_packet(controller.sequence, module.STATUS_OUT_OF_RANGE))
controller.update([], start + 35.1)
check('OUT_OF_RANGE enters cooldown', controller.state == 'ERROR')
controller.update([], start + 38)
controller.sequence = 255
controller._send_candidate(detection(180, 180), start + 39)
check('sequence wraps to uint8', controller.sequence == 0)
link.connection.pending.extend(module.build_status_packet(0, module.STATUS_ACK))
controller.update([], start + 39.1)
ack_time = start + 39.1
for elapsed in (18.1, 20.1, 23.999):
    controller.update([], ack_time + elapsed)
    check('host waits beyond old deadlines until 24 seconds', controller.state == 'PICKING')
controller.update([], ack_time + 24.0)
check('host times out at 24 seconds without terminal status', controller.state == 'ERROR')

# A terminal result after the old 20-second deadline must still complete.
late_link = make_link()
late_controller = module.PickController(late_link, 2, 5.0, 1.0, pick_timeout)
late_controller._send_candidate(detection(), start + 64)
late_link.connection.pending.extend(module.build_status_packet(late_controller.sequence, module.STATUS_ACK))
late_controller.update([], start + 64.1)
late_controller.update([], start + 87.1)
check('23-second job remains in flight', late_controller.state == 'PICKING')
late_link.connection.pending.extend(module.build_status_packet(late_controller.sequence, module.STATUS_DONE))
late_controller.update([], start + 88.1)
check('queued DONE is processed before the 24-second timeout check', late_controller.state == 'DETECT' and late_controller.target is None)
# Shape order must dominate geometry, colors and detector result order.
expected_shapes = ('cuboid', 'cube', 'pentagonal_prism', 'cylinder', 'cone', 'regular_tetrahedron')
check('model triangular pyramid maps to protocol tetrahedron',
      module.resolve_protocol_shape('triangular pyramid', 'red') == 'regular_tetrahedron')
for color in ('red', 'white'):
    ordered = []
    for index, shape in enumerate(expected_shapes):
        item = detection(215 - index * 35, 205 - index * 35)
        item.shape = shape
        item.color = color
        item.command = module.encode_hmi_command(shape, color)
        ordered.append(item)
    clutter = detection(214, 204)
    clutter.command = None
    selector = module.PickController(make_link(), 2, 5.0, 1.0, pick_timeout)
    for permutation in itertools.permutations(ordered):
        check('cuboid wins every detection permutation',
              selector._select_target([*permutation, clutter], start) is ordered[0])
    for index, item in enumerate(ordered):
        check('remaining shapes follow requested order',
              selector._select_target([*reversed(ordered[index:]), clutter], start) is item)
    selector.failed_targets = [(ordered[0].command, *ordered[0].machine_center, start + 100)]
    check('failed high priority target yields to next eligible shape',
          selector._select_target(ordered, start) is ordered[1])

# Exercise the actual state machine and UART writes over six recognition cycles.
link = make_link()
controller = module.PickController(link, 2, 5.0, 1.0, pick_timeout)
link.connection.pending.extend(module.build_key_packet(1, module.KEY_STATE_PRESS))
now = start + 100
remaining = list(reversed(ordered))
for item in ordered:
    controller.update(remaining, now)
    controller.update(remaining, now + 0.1)
    check('stable priority target sent', controller.state == 'WAIT_ACK' and controller.target.shape == item.shape)
    packet = link.connection.written[-1]
    check('UART command follows shape priority', packet[4] == item.command and len(packet) == 10 and packet[-1] == (sum(packet[:-1]) & 255))
    check('UART preserves selected coordinates including X=215',
          int.from_bytes(packet[5:7], 'little') == item.machine_center[0] and
          int.from_bytes(packet[7:9], 'little') == item.machine_center[1])
    writes = len(link.connection.written)
    controller.update(remaining, now + 0.2)
    check('pending task blocks extra coordinate frames', len(link.connection.written) == writes)
    link.connection.pending.extend(module.build_status_packet(controller.sequence, module.STATUS_DONE))
    controller.update([], now + 0.3)
    remaining.remove(item)
    now += 1
check('one coordinate task per recognition cycle', len(link.connection.written) == 6)

# Equal shapes retain the existing geometry score.
near, crowded, neighbor = detection(10, 10), detection(150, 150), detection(151, 151)
selector = module.PickController(make_link(), 2, 5.0, 1.0, pick_timeout)
check('same shape retains clearance/travel ordering',
      selector._select_target([crowded, near, neighbor], now) is near)

# Actual recognition -> calibrated coordinates -> stable target -> UART/retry.
# The model is replaced; process_frame, calibration conversion and controller run unchanged.
class FakeDetector:
    names = {0: 'cuboid'}

    def predict(self, frame):
        return [[10, 10, 30, 30, 0.95, 0]]


frame = module.np.full((80, 80, 3), (0, 0, 255), dtype=module.np.uint8)
homography = identity.copy()
homography[1, 2] = 186.0  # Pixel center (20,20) converts to raw Y=206.
_, _, found = module.process_frame(frame, (0, 0, 80, 80), 1, 0.9, 0,
                                   homography=homography, draw_result=False,
                                   detector=FakeDetector())
check('recognition outputs capped coordinates with a valid command',
      len(found) == 1 and found[0].machine_center == (20.0, 205.0)
      and found[0].command == module.encode_hmi_command('cuboid', 'red'))
cap_link = make_link()
cap_controller = module.PickController(cap_link, 2, 5.0, 1.0, pick_timeout)
cap_link.connection.pending.extend(module.build_key_packet(1, module.KEY_STATE_PRESS))
cap_controller.update(found, now + 10)
cap_controller.update(found, now + 10.1)
check('capped target remains selectable and locks the capped position',
      cap_controller.state == 'WAIT_ACK' and cap_controller.target.machine_center == (20.0, 205.0))
capped_packet = cap_link.connection.written[-1]
check('UART sends capped Y and a valid checksum',
      int.from_bytes(capped_packet[7:9], 'little') == 205
      and capped_packet[-1] == sum(capped_packet[:-1]) & 255)
cap_controller.update([], now + 11.2)
check('retry preserves capped task coordinates', cap_link.connection.written == [capped_packet, capped_packet])
print(f'PASS: {checks} checks; 24-second timeout, priority, X=215/Y=205 boundaries, upper Y clamp, fake UART only, no physical motion')
