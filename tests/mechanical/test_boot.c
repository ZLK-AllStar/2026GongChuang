#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Core/Src/button.c"
#include "../../Core/Src/app_mode.c"
static uint32_t ticks, begin;
static int scenario;
uint32_t HAL_GetTick(void) { return ticks++; }
void HAL_GPIO_Init(int p, GPIO_InitTypeDef *g) { (void)p; assert(g->Pull == GPIO_PULLUP && g->Pin == GPIO_PIN_2); }
GPIO_PinState HAL_GPIO_ReadPin(int p, unsigned pin) {
  uint32_t elapsed=(uint32_t)(ticks-begin);
  bool down=false;
  (void)p; assert(pin==GPIO_PIN_2);
  if (scenario==1) down=true;
  if (scenario==2) down=elapsed>=500 && elapsed<600;
  if (scenario==3) down=elapsed>=100 && elapsed<120 && (elapsed/5)%2;
  if (scenario==4) down=elapsed>=2990;
  if (scenario==5) down=elapsed>=2950;
  return down ? GPIO_PIN_RESET : GPIO_PIN_SET;
}
void BoardIO_SetLeds(bool led0, bool led1) { if (!locked) assert(!led1); else assert(led0 != led1); }
int main(void) {
  unsigned test;
  assert(mode == APP_MODE_HOST);   /* 上电默认 HOST */
  for (test=0; test<12; test++) {
    ticks = begin = test>=6 ? 0xFFFFFA00U : 0;
    scenario=test%6; locked=false; mode=APP_MODE_HOST;
    Button_Init(); AppMode_SelectAtBoot();
    assert(mode == ((scenario==1 || scenario==2 || scenario==5) ? APP_MODE_DEBUG : APP_MODE_HOST));
    assert((uint32_t)(ticks-begin)>=3000 && (uint32_t)(ticks-begin)<3020);
  }
  ticks=begin=0; scenario=1; Button_Init();
  for (test=0; test<40; test++) (void)Button_Poll();
  assert(Button_IsPressed() && !Button_IsReleasedStable());
  scenario=0; for (test=0; test<40; test++) (void)Button_Poll();
  assert(!Button_IsPressed() && Button_IsReleasedStable());
  puts("PASS boot/button: hold, tap, bounce, boundary, stable release, tick wrap");
  return 0;
}
