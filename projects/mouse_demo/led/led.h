/*******************************************************************************
* Copyright (c) 2013, SHENZHEN Co.,LTD.
* All rights reserved.
*
* 文件名称: led.h
*
* 功能说明: 单色背光 PWM 灯效(移植自 K102) 头文件
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
#define iPWM_RGB                0x07

#define iPWM_MAX                64

// 规格书：背光亮度“加关闭有六档” = 关闭(LEV_0) + 5 个亮度档
enum led_level
{
    iLED_LEV_MIN=0,// 关闭
    iLED_LEV_0 = 0,
    iLED_LEV_1 = 1,
    iLED_LEV_2 = 2,
    iLED_LEV_3 = 3,
    iLED_LEV_4 = 4,
    iLED_LEV_5 = 5,
    iLED_LEV_MAX = 5,	// 最亮
};

enum speed_level
{
    iSPEED_LEV_MIN = 0,
    iSPEED_LEV_1 = 0,
    iSPEED_LEV_2 = 1,
    iSPEED_LEV_3 = 2,
    iSPEED_LEV_4 = 3,
    iSPEED_LEV_5 = 4,
    iSPEED_LEV_MAX = 4,
};

typedef enum {
    iMODE_LED_ON        =0,                     // 常亮
    iMODE_LED_BREATH    =1,                     // 呼吸
    iMODE_LED_OFF       =2,
    iMODE_LED_MAX       =2,
    iMODE_LED_INVALID   =16,
    iLOGO_INVALID   = 0x0F,
} logo_mode_t;

//=============================================================================;
// 内部定义外部引用区                                                          ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 寄存器引用
//
extern bool                     bled_bright_en;
extern bool                     bback_led_status;
extern bool                     bpwm_valid;         // PWM 调光已开启

extern uint8_t                  led_cur_mode;

extern uint8_t                  led_pre_mode;
extern enum speed_level         led_speed_lev;
extern unsigned char            led_rgb_index;
extern unsigned char            pwm_value;
extern enum led_level           led_light_lev;
extern enum led_level           led_light_lev_bak;  // 亮度档位备份，变化时重算占空
extern uint8_t                  pwm_duty_cur;       // 当前占空 0~iPWM_MAX，改档位后重算用

extern uint8_t                  pwm_time_count;
extern uint16_t                 delay_time_count;
extern uint8_t                  hint_time_count;
extern uint32_t                 exled_rtc_count;    // 54E 显示刷新节拍(定义在 led.c)

//-----------------------------------------------------------------------------;
// 函数引用
//
extern void pwm_init(void);
extern void pwm_duty_set(uint8_t duty);
extern void effect_breatch(void);
extern void led_status_hint(void);

extern void logo_off(void);
extern void logo_off_deep(void);
extern void device_led_on(void);
extern void device_led_off(void);

extern void device_led_process(void);
extern void logo_process(void);

//
//-----------------------------------------------------------------------------;
#endif
