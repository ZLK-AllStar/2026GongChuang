#ifndef SHELL_PORT_H
#define SHELL_PORT_H
#include <stdbool.h>
#include <stdint.h>
void ShellPort_Init(void);
void ShellPort_Poll(void);
uint32_t ShellPort_RxGenerationFromISR(void);
void ShellPort_RxByteFromISR(uint8_t byte, uint32_t generation);
void ShellPort_RxErrorFromISR(void);
bool ShellPort_BeginAction(void);
void ShellPort_EndAction(void);
uint32_t ShellPort_GetErrorCount(void);
#endif
