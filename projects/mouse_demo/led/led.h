/*******************************************************************************
* Copyright (c) 2013, SHENZHEN Co.,LTD.
* All rights reserved.
*
* 文件名称: led.h
*
* 功能说明: 54E 外挂显示/状态指示 头文件(本机无背光)
* 修改说明:
*******************************************************************************/
#ifndef _LED_H_
#define _LED_H_

//=============================================================================;
// 配置引用区                                                                  ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 配置区
//
#define iPWM_MAX                64

//-----------------------------------------------------------------------------;
// 配置区
//
enum led_level
{
    iLED_LEV_MIN=0,             // 最暗(灭)
    iLED_LEV_0 = 0,
    iLED_LEV_1 = 1,
    iLED_LEV_2 = 2,
    iLED_LEV_3 = 3,
    iLED_LEV_4 = 4,
    iLED_LEV_MAX = 4,           // 最亮
};

enum speed_level
{
    iSPEED_LEV_MIN = 0,         // 最快
    iSPEED_LEV_1 = 0,
    iSPEED_LEV_2 = 1,
    iSPEED_LEV_3 = 2,
    iSPEED_LEV_4 = 3,
    iSPEED_LEV_5 = 4,
    iSPEED_LEV_MAX = 4,         // 最慢
};

typedef enum {
    iMODE_LED_OFF       =0,
    iMODE_LED_ON        =1,                     // 常亮
    iMODE_LED_BREATH    =2,                     // 呼吸
    iMODE_LED_HB        =3,                     // 心跳

    iMODE_LED_MAX       =3,
    iMODE_LED_INVALID   =4,
} logo_mode_t;

//=============================================================================;
// 内部定义外部引用区                                                          ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 寄存器引用
//


extern uint32_t                 exled_rtc_count;    // 54E 显示刷新节拍(定义在 led.c)

extern bool                     bpwm_valid;         // true: 装饰灯 PWM 已打开
extern bool                     bback_led_status;   // 呼吸方向: false 渐暗, true 渐亮

extern uint8_t                  led_cur_mode;       // 装饰灯模式, 见 logo_mode_t
extern uint8_t                  led_pre_mode;
extern enum led_level           led_light_lev;
extern enum speed_level         led_speed_lev;

extern uint8_t                  pwm_value;
extern uint8_t                  pwm_time_count;
extern uint16_t                 delay_time_count;

//-----------------------------------------------------------------------------;
// 函数引用
//

extern void disp_off(void);
extern void disp_refresh(void);
extern void device_led_on(void);
extern void device_led_off(void);

extern void device_led_process(void);
extern void logo_process(void);

extern void pwm_init(void);
extern void logo_off(void);
extern void deco_led_process(void);
extern void effect_breatch(void);
extern void effect_heartbeat(void);

//
//-----------------------------------------------------------------------------;
#endif
