#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define APP_BOOT_SELECT_MS           3000U
#define APP_LED_BLINK_HALF_MS        100U
#define APP_HOST_TASK_BUDGET_MS      24000U
#define APP_DISPLAY_PERIOD_MS        100U
#define APP_RELEASE_X_MM             215.0f
#define APP_RELEASE_Y_MM             105.0f
#define APP_HOME_X_MM                0.0f
#define APP_HOME_Y_MM                0.0f
#define APP_PARK_AFTER_TASK          1U
#define APP_CLASSIFICATION_SLOTS     6U
#define APP_HOST_RING_SIZE           256U
#define APP_SHELL_RING_SIZE          256U
#define APP_SHELL_BUFFER_SIZE        512U

/* DEBUG console: PF2 -> adapter RXD, PF4 <- adapter TXD, 3.3 V TTL. */
#define APP_SHELL_SOFT_UART_TX_PORT   GPIOF
#define APP_SHELL_SOFT_UART_TX_PIN    GPIO_PIN_2
#define APP_SHELL_SOFT_UART_RX_PORT   GPIOF
#define APP_SHELL_SOFT_UART_RX_PIN    GPIO_PIN_4
#define APP_SHELL_SOFT_UART_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define APP_SHELL_SOFT_UART_BAUD      4800U
#define APP_SHELL_SOFT_UART_SAMPLES   8U
#define APP_SHELL_SOFT_UART_TIMER_HZ  84000000U
#define APP_SHELL_SOFT_UART_CPU_HZ    168000000U
#define APP_SHELL_SOFT_UART_DIVISOR   ((APP_SHELL_SOFT_UART_TIMER_HZ + \
  APP_SHELL_SOFT_UART_BAUD * APP_SHELL_SOFT_UART_SAMPLES / 2U) / \
  (APP_SHELL_SOFT_UART_BAUD * APP_SHELL_SOFT_UART_SAMPLES))
#define APP_SHELL_SOFT_UART_TX_SIZE   512U
/* 100 ms covers one existing display service interval; twice that plus
 * 50 ms scheduler margin is added to the complete queued wire time. */
#define APP_SHELL_SOFT_UART_MARGIN_MS (2U * APP_DISPLAY_PERIOD_MS + 50U)
#define APP_SHELL_SOFT_UART_JITTER_US 10U

#define XY_STAGE_X_MAX_MM            215.0f
#define XY_STAGE_Y_MAX_MM            205.0f
#define XY_BELT_PITCH_MM             2.0f
#define XY_PULLEY_TEETH              20U
#define XY_MOTOR_FULL_STEPS          200U
#define XY_MOTOR_MICROSTEPS          16U
#define XY_PULSE_PER_REV             (XY_MOTOR_FULL_STEPS * XY_MOTOR_MICROSTEPS)
#define XY_PULSE_PER_MM              ((float)XY_PULSE_PER_REV / (XY_BELT_PITCH_MM * XY_PULLEY_TEETH))
#define XY_POS_UNITS_PER_REV         65536U
#define XY_M1_DIR_POSITIVE           1U
#define XY_M2_DIR_POSITIVE           1U

#define TRAY_MOTOR_REVS_PER_TRAY_REV  1.0
#define TRAY_PULSE_PER_REV            3200U
#define TRAY_POS_UNITS_PER_REV        65536U
#define TRAY_DIR_POSITIVE             1U
#define TRAY_POS_TOLERANCE_PULSE      20U

#endif
