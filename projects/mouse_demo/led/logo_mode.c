/*******************************************************************************
* Copyright (c) 2013, SHENZHEN Co.,LTD.
* All rights reserved.
*
* 文件名称: logo_mode.c
*
* 功能说明: 装饰灯 PWM 灯效(呼吸、心跳)
* 修改说明:
*******************************************************************************/
#include <string.h>
#include "om_driver.h"

#include "main.h"
#include "led.h"

//-----------------------------------------------------------------------------;
// 常量定义区
//

//-----------------------------------------------------------------------------;
// 变量定义区
//


//-----------------------------------------------------------------------------;
// 表格定义区
//
const uint8_t heartbeat_table[80] = {
    0, 30, 120, 210, 230, 180, 80, 20,   // 第一跳急升急降
    30, 60, 120, 200, 255, 255, 240, 200, 140, 80, 30, 5, // 第二跳缓升缓降
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//=============================================================================;
// 代码区                                                                      ;
//=============================================================================;

/******************************************************************************
* 函数名称: effect_breatch
* 功能描述: 呼吸
* 输入参数:
* 输出参数:
* 说    明: 每 (led_speed_lev+1) 次调用走一步, 0 ~ iPWM_MAX-1 来回
******************************************************************************/
void effect_breatch(void)
{
    uint8_t i;
    if(led_speed_lev>delay_time_count)
    {
        delay_time_count++;
        return;
    }
    delay_time_count = 0;

    i = pwm_time_count;
    if(!bback_led_status)
    {
        if(pwm_time_count >= iPWM_MAX-1)
            pwm_value = 0;
        else
            pwm_value = (iPWM_MAX-1 - i);
    }
    else
    {
        if(pwm_time_count >= iPWM_MAX-1)
            pwm_value = iPWM_MAX-1;
        else
            pwm_value = i;
    }

    if(pwm_time_count == iPWM_MAX-1)   // 最亮和最暗的时间加长
    {
        pwm_time_count = 0;
        bback_led_status = !bback_led_status;
    }

    DECO_PWM_DC = pwm_value;
    pwm_time_count++;
}

/******************************************************************************
* 函数名称: effect_heartbeat
* 功能描述: 心跳
* 输入参数:
* 输出参数:
* 说    明: 每 (led_speed_lev+2) 次调用走一格, 80 格一个周期。
*           表是 0~255, 右移 2 位换成 0~63, 和常亮/呼吸同一个满量程(iPWM_MAX)
******************************************************************************/
void effect_heartbeat(void)
{
    static uint8_t heartbeat_index;
    if(led_speed_lev+1>delay_time_count)
    {
        delay_time_count++;
        return;
    }
    delay_time_count = 0;
    heartbeat_index++;
    if(heartbeat_index>=80)
        heartbeat_index = 0;
    DECO_PWM_DC = (heartbeat_table[heartbeat_index]>>2);
}
