#ifndef SOFT_UART_H
#define SOFT_UART_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t rx_bytes;
  uint32_t tx_bytes; /* Frames completed locally, without remote ACK. */
  uint32_t frame_errors;
  uint32_t false_starts;
  uint32_t timing_errors;
  uint32_t tx_faults;
  uint32_t tx_timeouts;
  uint32_t max_gap_cycles;
  uint32_t max_late_timer_ticks;
} SoftUart_Diagnostics;

bool SoftUart_Init(void);
bool SoftUart_IsRunning(void);
void SoftUart_Stop(void);
void SoftUart_HandleIRQ(void); /* Checks/clears TIM13 UIF only. */
void SoftUart_TickFromISR(void);
/* Foreground only; copies data. Return value means accepted, not delivered. */
uint16_t SoftUart_Write(const uint8_t *data, uint16_t length);
/* Call with IRQs masked when coordinating a foreground input boundary. */
bool SoftUart_IsRxActive(void);
void SoftUart_GetDiagnostics(SoftUart_Diagnostics *result);

#endif
