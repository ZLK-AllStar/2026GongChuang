#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_link.h"
#include "app_controller.h"
#include "app_runtime.h"
#include "app_config.h"
#include "app_mode.h"
#include "xy_stage.h"
#include "tray_motor.h"
#include "hmi_link.h"
#include "classification.h"
#include "aux_servo.h"
UART_HandleTypeDef huart2={USART2,0}, huart3={3,0};
static uint8_t *rx;
static uint32_t tick;
static unsigned acks, done, fails, busy_reply, oor, moves, records, tray_moves, key_events;
static unsigned scenario, motor_stops;
static unsigned lowers, picks;
static uint16_t lower_angle;
static bool lower_ok=true;
static bool press, released=false, xy_valid=true, tray_valid=true, injected;
static uint8_t current_seq=255, current_code=19, record_id, record_code;
static uint16_t current_x=100, current_y=100;
static char order[128];
static unsigned order_n;
static void Mark(char c) { if (order_n < sizeof(order)-1) order[order_n++]=c; }
static void Inject(uint8_t seq, uint8_t code, uint16_t x, uint16_t y);
static void Bytes(const uint8_t *p,unsigned n);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart);
uint32_t HAL_GetTick(void) { return tick; }
void HAL_Delay(uint32_t ms) { tick+=ms; AppRuntime_Service(); }
void HAL_NVIC_SetPriority(unsigned a,unsigned b,unsigned c) { (void)a;(void)b;(void)c; }
void HAL_NVIC_EnableIRQ(unsigned a) { (void)a; }
int HAL_UART_Receive_IT(UART_HandleTypeDef *u,uint8_t *p,unsigned n) { (void)n; assert(u==&huart2); rx=p; return 0; }
int HAL_UART_AbortReceive(UART_HandleTypeDef *u) { (void)u; return 0; }
int HAL_UART_Transmit(UART_HandleTypeDef *u,uint8_t *p,unsigned n,uint32_t timeout) {
  unsigned i; (void)timeout;
  if (u==&huart3) {
    uint8_t data[10]; assert(n==22 && p[0]==0xF0 && p[21]==0xF1);
    for(i=0;i<10;i++) data[i]=(uint8_t)(((p[1+2*i]&15)<<4)|(p[2+2*i]&15));
    if(data[1]==1) { records++; record_id=data[7]; record_code=data[6]; Mark('H'); if(scenario==9) return 1; }
    return 0;
  }
  assert(n==6 && p[0]==0x55 && p[1]==0xAA);
  assert((uint8_t)(p[0]+p[1]+p[2]+p[3]+p[4])==p[5]);
  if(p[2]==0x82) { key_events++; return 0; }
  if(p[2]!=0x81) return 0;
  acks+=p[4]==1; busy_reply+=p[4]==2; done+=p[4]==3; fails+=p[4]==4; oor+=p[4]==5;
  if (p[4]==1) Mark('A');
  if (scenario==5 && p[4]==1) return 1; // local ACK lost
  return 0;
}
App_Mode_t AppMode_Get(void) { return scenario==6 ? APP_MODE_DEBUG : APP_MODE_HOST; }
void Button_Init(void) {}
bool Button_Poll(void) { bool result=press; press=false; return result; }
bool Button_IsReleasedStable(void) { return released; }
bool Button_IsPressed(void) { return !released; }
void AppDisplay_Service(void) { tick++; }
void AppDisplay_SetCommand(const char *s) { assert(s); }
void AppDisplay_SetMotionTarget(float x,float y) { (void)x;(void)y; }
void AppDisplay_SetHostFrame(uint8_t s,uint8_t c,uint16_t x,uint16_t y) { (void)s;(void)c;(void)x;(void)y; }
bool AuxServo_LiftUp(void) { Mark('U'); return !AppRuntime_Expired(); }
bool AuxServo_LiftDown(void) { Mark('L'); lowers++; lower_angle=AUX_SERVO_LIFT_DOWN_ANGLE; return lower_ok && !AppRuntime_Expired(); }
bool AuxServo_LiftDownFull(void) { Mark('F'); lowers++; lower_angle=AUX_SERVO_LIFT_DOWN_ANGLE_FULL; return lower_ok && !AppRuntime_Expired(); }
bool AuxServo_LiftTo(uint16_t angle) { Mark('C'); lowers++; lower_angle=angle; return lower_ok && !AppRuntime_Expired(); }
bool Vacuum_PickAndHold(void) {
  HMI_Shape_t shape=HMI_CodeShape(current_code);
  uint16_t expected=shape==HMI_SHAPE_CYLINDER ? 130U :
    shape==HMI_SHAPE_CUBOID ? AUX_SERVO_LIFT_DOWN_ANGLE_FULL : AUX_SERVO_LIFT_DOWN_ANGLE;
  assert(lower_angle==expected); picks++; Mark('P'); return !AppRuntime_Expired();
}
bool Vacuum_Release(void) { Mark('R'); return !AppRuntime_Expired(); }
bool XY_IsOriginValid(void) { return xy_valid; }
bool Tray_IsOriginValid(void) { return tray_valid; }
void XY_InvalidateOrigin(void) { xy_valid=false; }
void Tray_InvalidateOrigin(void) { tray_valid=false; }
void XY_Stop(void) { motor_stops++; }
void Tray_Stop(void) { motor_stops++; }
const char *XY_GetDiagnosticText(void) { return "mock XY"; }
const char *Tray_GetDiagnosticText(void) { return "mock tray"; }
Tray_Result_t Tray_MoveToAngle(float angle) { assert(angle>=0 && angle<=300); tray_moves++; Mark('T'); return scenario==2 ? TRAY_RESULT_TIMEOUT : TRAY_RESULT_OK; }
const char *Tray_ResultText(Tray_Result_t r) { (void)r; return "TRAY FAIL"; }
XY_Result_t XY_MoveTo(float x,float y) {
  moves++; Mark('M');
  if (moves==1) { assert(x==current_x && y==current_y); }
  else if (moves%3==2) { assert(x==APP_RELEASE_X_MM && y==APP_RELEASE_Y_MM); }
  if (scenario==8) {
    uint8_t hello[6]={0xAA,0x55,2,0,1,2};
    for(unsigned i=0;i<3000;i++) {
      if(i%1000==0) Bytes(hello,6);
      AppRuntime_Service();
    }
  }
  if (scenario==1 && !injected) {
    injected=true;
    Inject(current_seq,current_code,current_x,current_y);
    Inject(9,20,10,10); AppRuntime_Service();
  }
  if(scenario==3 && moves==3) return XY_RESULT_TIMEOUT; // park failure after true release
  if(scenario==4) { tick+=APP_HOST_TASK_BUDGET_MS; return XY_RESULT_TIMEOUT; }
  if(scenario==10 && moves==1) { tick+=23000U; }
  return XY_RESULT_OK;
}
static void Bytes(const uint8_t *p,unsigned n) { unsigned i; for(i=0;i<n;i++) { *rx=p[i]; HAL_UART_RxCpltCallback(&huart2); } }
static void Inject(uint8_t seq,uint8_t code,uint16_t x,uint16_t y) {
  uint8_t p[10]={0xAA,0x55,1,seq,code,(uint8_t)x,(uint8_t)(x>>8),(uint8_t)y,(uint8_t)(y>>8),0};
  unsigned i; for(i=0;i<9;i++) p[9]=(uint8_t)(p[9]+p[i]); Bytes(p,10);
}
int main(int argc,char **argv) {
  unsigned before,i;
  scenario=argc>1 ? (unsigned)atoi(argv[1]) : 0;
  if (scenario==7) {
    tick=0xFFFFFE00U; AppRuntime_BeginTask();
    tick+=18000U; assert(!AppRuntime_Expired());
    tick+=2000U; assert(!AppRuntime_Expired());
    tick+=3999U; assert(!AppRuntime_Expired());
    tick++; assert(AppRuntime_Expired() && !AppRuntime_Wait(1));
    AppRuntime_EndTask(); assert(!AppRuntime_Expired());
    puts("PASS 24-second task deadline: 18/20/23.999 seconds allowed, 24 expires, tick wrap"); return 0;
  }
  HMI_LinkInit(); AppController_Init();
  if(scenario==6) { AppController_Poll(); assert(!acks && !moves); puts("PASS DEBUG rejects HOST"); return 0; }
  // Existing startup/serial frames cannot arm the mechanism.
  Inject(1,19,100,100); AppController_Poll(); assert(!acks && !moves);
  released=false; press=true; AppController_Service(); assert(!key_events); // held from boot
  released=true; AppController_Service();
  released=false; press=true; AppController_Service(); assert(key_events==1);
  released=true; AppController_Service();
  // Bad command and range never allocate a category.
  Inject(2,0,100,100); Inject(4,91,100,100); Inject(3,20,216,0); AppController_Poll(); assert(!acks && !moves && oor==1);
  {
    uint8_t bad[10]={0xAA,0x55,1,10,1,0,0,0,0,0};
    Bytes(bad,10); AppController_Service(); assert(!acks);
    uint8_t hello[6]={0xAA,0x55,2,0,1,2};
    Bytes(hello,3); AppController_Service(); assert(!HostLink_GetHostReady());
    Bytes(hello+3,3); AppController_Service(); assert(HostLink_GetHostReady());
  }
  Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
  assert(acks==1 && tray_moves==1);
  if(scenario==2) { assert(!moves && !records && !done); }
  else if(scenario==3) { assert(records==1 && !done && moves==3); }
  else if(scenario==4) { assert(!records && !done && motor_stops==2 && !xy_valid && !tray_valid); }
  else { assert(records==1 && done==1 && record_id==1 && moves==3); assert(strstr(order,"T")<strstr(order,"P")); assert(strstr(order,"R")<strstr(order,"H")); }
  assert(Classification_Assign(19)==1);
  if(scenario==8) assert(HostLink_GetOverflowCount()==0);
  if(scenario==9) assert(HMI_GetErrorCount()==1);
  if(scenario==10) assert(tick>=23000U && tick<24000U && done==1 && xy_valid && tray_valid);
  before=moves;
  Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
  assert(moves==before && records<=1 && tray_moves==1 && acks==1);
  if(scenario==1) assert(busy_reply==1);
  if(scenario==0) {
    // Same SEQ with changed payload after wrap is a different task.
    for(i=1;i<=7;i++) {
      current_code=(uint8_t)(19+i); current_seq=(uint8_t)(255+i); moves=0;
      Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
      assert(record_id==(uint8_t)(i%6+1));
    }
    current_code=19; current_seq=10; moves=0;
    Inject(current_seq,current_code,current_x,current_y); AppController_Poll(); assert(record_id==1);
    // The real host path accepts all eight white codes and emits their byte.
    for (i=0;i<8;i++) {
      current_code=(uint8_t)(73+i); current_seq=(uint8_t)(20+i); moves=0;
      unsigned previous_records=records;
      Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
      assert(moves==3 && records==previous_records+1 && record_code==current_code);
      assert(record_id==(uint8_t)((i+2)%6+1));
      before=moves;
      Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
      assert(moves==before && records==previous_records+1);
    }
    // Every cone color is accepted end-to-end and keeps its own category.
    for (i=0;i<10;i++) {
      current_code=(uint8_t)(81+i); current_seq=(uint8_t)(40+i); moves=0;
      unsigned previous_records=records;
      Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
      assert(moves==3 && records==previous_records+1 && record_code==current_code);
      assert(record_id==(uint8_t)((i+4)%6+1));
      assert(HMI_CodeShape(current_code)==HMI_SHAPE_CONE);
      before=moves;
      Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
      assert(moves==before && records==previous_records+1);
    }
    // The wire/parser/controller path accepts the workspace corner exactly once.
    current_x=215; current_y=205; current_seq=60; current_code=19; moves=0;
    unsigned previous_records=records, previous_acks=acks, previous_oor=oor;
    Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
    assert(moves==3 && records==previous_records+1 && acks==previous_acks+1);
    Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
    assert(moves==3 && records==previous_records+1 && acks==previous_acks+1);
    Inject(61,19,216,205); Inject(62,19,215,206); AppController_Poll();
    assert(moves==3 && records==previous_records+1 && oor==previous_oor+2);
    current_x=100; current_y=100;
    // Ring damage must be counted and frame synchronization must recover.
    for(i=0;i<400;i++) { uint8_t noise=0; Bytes(&noise,1); }
    AppController_Service(); assert(HostLink_GetOverflowCount()>0);
    HAL_UART_ErrorCallback(&huart2); AppController_Service(); assert(HostLink_GetErrorCount()>0);
    current_seq=11; moves=0; Inject(current_seq,19,100,100); AppController_Poll(); assert(moves==3);
  }
  if (scenario==0) {
    // Exercise actual UART/controller/shape dispatch for all 90 color codes.
    for (i=1;i<=90;i++) {
      current_code=(uint8_t)i; current_seq=(uint8_t)(80+i); moves=0;
      unsigned previous_lowers=lowers, previous_records=records, previous_done=done;
      Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
      assert(moves==3 && lowers==previous_lowers+1 && records==previous_records+1 && done==previous_done+1);
      if ((i>=37 && i<=45) || i==77) assert(lower_angle==130U);
      Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
      assert(moves==3 && lowers==previous_lowers+1 && records==previous_records+1);
    }
    // A cylinder lowering failure cannot start suction or report a release.
    current_code=37; current_seq=171; moves=0; lower_ok=false;
    unsigned previous_picks=picks, previous_records=records, previous_fails=fails;
    Inject(current_seq,current_code,current_x,current_y); AppController_Poll();
    assert(moves==1 && picks==previous_picks && records==previous_records && fails==previous_fails+1);
    lower_ok=true;
    puts("PASS pickup depth: all 90 codes, ten cylinder colors at 130deg, duplicates, lowering failure guard");
    Classification_Init();
    assert(Classification_Assign(0)==0 && Classification_Assign(91)==0);
    for (i=1;i<=90;i++) assert(Classification_Assign((uint8_t)i)==(i-1)%6+1);
    for (i=1;i<=90;i++) assert(Classification_Assign((uint8_t)i)==(i-1)%6+1);
  }
  printf("PASS HOST scenario %u: real parser/controller/pick-place/protocol\n",scenario);
  return 0;
}
