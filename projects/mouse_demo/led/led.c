/*******************************************************************************
* Copyright (c) 2013, SHENZHEN Co.,LTD.
* All rights reserved.
*
* 文件名称: led.c
*
* 功能说明: 单色背光 PWM 灯效(移植自 K102) + 54E 外挂显示/状态指示(本工程原有)
* 修改说明:
*******************************************************************************/
#include <string.h>
#include "om_driver.h"

#include "main.h"
#include "lvd.h"
#include "led.h"
#include "exLed.h"

#include "flash.h"
#include "flash_process.h"
#include "usb_cfg_info.h"
#include "matrix_key.h"
#include "time_delay.h"                    // delayus()

//-----------------------------------------------------------------------------;
// 变量定义区
//
bool                            bdevice_led_status;
bool                            bled_bright_en;
bool                            bback_led_status;
bool                            bpwm_valid;         // PWM 调光已开启
uint8_t                         pwm_duty_cur;       // 当前占空 0~iPWM_MAX，改档位后重算用
enum led_level                  led_light_lev_bak;

uint8_t                         device_conn_index=0;
uint8_t                         device_hint_count;

uint32_t                        led_delay_count;

uint8_t                         led_cur_mode;
uint8_t                         led_pre_mode = iMODE_LED_INVALID; // 上电首次进入呼吸时从暗处平滑起步
enum speed_level                led_speed_lev;
unsigned char                   led_rgb_index;
unsigned char                   pwm_value;
enum led_level                  led_light_lev;

uint8_t                         pwm_time_count;
uint16_t                        delay_time_count;
uint8_t                         hint_time_count;

uint32_t                        exled_rtc_count=0;  // 54E 显示刷新节拍

extern bool                     b24g_nromal;

//=============================================================================;
// 代码区                                                                       ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 单色背光 PWM 调光（依据 OM6239 数据手册 16 章）—— 移植自 K102
//
//   周期       = 1/frq x (M + 1)
//   有效电平宽 = 1/frq x (M - N)      M = PWMPRD, N = PWMDC0
//   非有效宽   = 1/frq x (N + 1)
//
// PWMCR1=0 有效电平为低，PWMCR0=0 初始电平取非有效(高)。背光低电平
// 点亮，所以：  亮度 = (M - N) / (M + 1)，即 N = M - 占空。
// 有效/非有效、初始/非有效两对极性是同一套比较输出，占空换算与极性无关。
//
// 两个端点不走上面的公式，而是用手册 16.3 描述的两种边界行为：
//   灭  ：N > M   —— 无比较匹配、输出不翻转，恒为初始电平(高)
//   满  ：N = M   —— 与周期同刻匹配时 PWMDC 优先，输出恒为有效(低) = 100%
// （若按公式算，占空上限只能到 M/(M+1)，到不了满占空）
//
// 亮度档位：周期固定为 iPWM_PRD，只缩放占空（而不是改周期），好处是
// PWM 频率不随档位变化。灯效引擎给出的 duty 恒为 0~iPWM_MAX(64)，
// 按 led_lev_scale[] 缩放： v = duty * scale / iPWM_MAX，得到 0~iPWM_PRD。
// 规格书要求“加关闭有六档”，所以是 关闭 + 5 个亮度档：
//   LEV_5 -> 100%   LEV_4 -> 80%   LEV_3 -> 60%   LEV_2 -> 40%   LEV_1 -> 20%
// 五个亮度档使用线性递增，每档相差 20%，避免每一级对半衰减。
// 要微调手感直接改表即可，不用动逻辑。
//
// 分频取 1/8，pwm_pclk 跟随系统时钟，PWM 频率 = pclk/8/(M+1)：
//   主频统一 48MHz，三种模式同频: 23.4 kHz
// 均落在手册标称的 2kHz~8MHz 内。
//
#define iPWM_PRD                255         // PWMPRD，固定周期 = 256 拍
#define iPWM_PRE_SCALE          2           // PWMDIV: 0=1/1  1=1/2  2=1/8  3=1/32

/*******************************************************************************
* 函数名称: pwm_init()
* 功能描述: 单色背光 PWM 调光初始化
* 输入参数： 无
* 输出参数： 无
* 说    明: 背光在 PWM 通道 0（手册 15.3 节：P0.0 属 PWM 复用脚）
*           PWMCR1 bit0=0：有效电平为低；PWMCR0 bit0=0：初始电平取非有效(高)
*******************************************************************************/
void pwm_init(void)
{
    DRV_RCC_CLOCK_ENABLE(RCC_CLK_PWM, 1U);

    OM_PWM->PWMPRD = iPWM_PRD;
    OM_PWM->PWMCR0 = 0x00000000;                // ch0 初始电平 = 非有效(高)
    OM_PWM->PWMCR1 = 0x00000000;                // ch0 有效电平 = 低
    OM_PWM->PWMDIV = iPWM_PRE_SCALE;

    pwm_duty_set(pwm_duty_cur);
    OM_PWM->PWMCON = 0x00000001;

    bpwm_valid = true;
}

/*******************************************************************************
* 函数名称: pwm_duty_set()
* 功能描述: 按“亮度”设置占空，内含亮度档位缩放
* 输入参数： duty  0 = 灭，iPWM_MAX = 本档位满占空
* 输出参数： 无
* 说    明: PWMDC0 存的是非有效宽度（反向），这里统一换算，灯效层不用关心
*******************************************************************************/
//   索引 = led_light_lev，值 = 该档满占空时的计数（对应 iPWM_PRD=255）
static const uint8_t led_lev_scale[iLED_LEV_MAX+1] =
{
     0,     // LEV_0  关闭（logo_process 已提前拦掉，此处兜底）
    51,     // LEV_1  20%
   102,     // LEV_2  40%
   153,     // LEV_3  60%
   204,     // LEV_4  80%
   255,     // LEV_5  100%
};

void pwm_duty_set(uint8_t duty)
{
    uint16_t v;

    if(duty > iPWM_MAX)
        duty = iPWM_MAX;
    pwm_duty_cur = duty;

    if(led_light_lev > iLED_LEV_MAX)            // 防上位机/flash 给出越界档位
        led_light_lev = iLED_LEV_MAX;
                                                // duty*scale/64，最大 64*255/64 = 255 = iPWM_PRD
    v = ((uint16_t)duty * led_lev_scale[led_light_lev]) / iPWM_MAX;

    if(0 == v)
        OM_PWM->PWMDC0 = -1;                    // N>M：无匹配，恒为初始电平(高) = 灭
    else if(iPWM_PRD == v)
        OM_PWM->PWMDC0 = iPWM_PRD;              // N=M：同刻匹配 PWMDC 优先 = 100%
    else
        OM_PWM->PWMDC0 = iPWM_PRD - v;          // 有效(高)宽度 = M - N = v
}

/******************************************************************************
* 函数名称: led_status_hint
* 功能描述: 根据模式提示、键盘锁灯刷新指示灯(54E 外挂显示)
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
void led_status_hint(void)
{
    mLED_VDD_DIS;
	if(hint_led_status&bled_hint_mode)
    {
        if(hint_led_status&bled_num)
        {
            mLED_NUM_LOW;
        }
        else
        {
            mLED_NUM_HIGH;
        }
        if(hint_led_status&bled_caps)
        {
            mLED_CAPS_LOW;
        }
        else
        {
            mLED_CAPS_HIGH;
        }
        if(hint_led_status&bled_win_lock)
        {
            mLED_WIN_LOW;
        }
        else
        {
            mLED_WIN_HIGH;
        }

        if(0==(hint_led_status&(bled_num|bled_caps|bled_win_lock)))
            return;
    }
    else
    {
        if(kb_led_status&bnum_lock)
        {
            mLED_NUM_LOW;
        }
        else
        {
            mLED_NUM_HIGH;
        }

        if(kb_led_status&bcaps_lock)
        {
            mLED_CAPS_LOW;
        }
        else
        {
            mLED_CAPS_HIGH;
        }

        if((key_ctrl&bwin_mac_sataus) && (key_ctrl&bwin_lock_status))
        {
            mLED_WIN_LOW;
        }
        else
        {
            mLED_WIN_HIGH;
        }
    }
    mLED_VDD_EN;
}

/*******************************************************************************
* 函数名称: device_led_on()
* 功能描述:
* 输入参数： 无
* 输出参数： 无
* 说    明:
*******************************************************************************/
void device_led_on(void)
{
    exled &= ~(bled_24G|bled_BT1|bled_BT2);

    if(iDEVICE_TYPE_BT0 == device_type)
        exled |= bled_BT1;
    else if(iDEVICE_TYPE_BT1 == device_type)
        exled |= bled_BT2;
    else if(iDEVICE_TYPE_24G == device_type)
        exled |= bled_24G;
}

/*******************************************************************************
* 函数名称: device_led_off()
* 功能描述:
* 输入参数： 无
* 输出参数： 无
* 说    明:
*******************************************************************************/
void device_led_off(void)
{
    exled &= ~(bled_24G|bled_BT1|bled_BT2);
}

/*******************************************************************************
* 函数名称: logo_off()
* 功能描述: 关背光
* 说    明: 只把占空设成恒灭，不停定时器。直接 PWMCON=0 / 关时钟会把引脚
*           冻在当前周期的电平上，可能停在高而变成残亮。
*           同时让 54E 外挂显示转关屏帧（保持原有行为）。
*******************************************************************************/
void logo_off(void)
{
    if(true == bpwm_valid)
    {
        bpwm_valid = false;
        OM_PWM->PWMDC0 = -1;                    // N>M：恒为初始电平(高)
    }

    if(0xFF != exio_ctrl)
    {
        exled_rtc_count=0;
        exio_ctrl = 0xFF;
    }
}

/*******************************************************************************
* 函数名称: logo_off_deep()
* 功能描述: 休眠/关机专用的关背光
* 说    明: 先关灯，等 >=1 个 PWM 周期让引脚落到低，再停模块并关时钟
*******************************************************************************/
void logo_off_deep(void)
{
    if(true == bpwm_valid)
    {
        logo_off();
        delayus(1000);
    }
    OM_PWM->PWMCON = 0x00000000;
    DRV_RCC_CLOCK_ENABLE(RCC_CLK_PWM, 0U);
}

extern void key_into_fcc_test(void);

/******************************************************************************
* 函数名称: logo_process
* 功能描述:
* 输入参数:
* 输出参数:
* 说    明:
******************************************************************************/
void device_led_process(void)
{
//-----------------------------------------------------------------------------
// 模式提示
//
    if(0 != hint_time_count)
    {
        hint_led_status |= bled_hint_mode;
        if(0 != (hint_time_count&0x08))
        {
            bdevice_led_status = true;
            hint_led_status |= (bled_num|bled_caps|bled_win_lock);
        }
        else
        {
            hint_led_status &= ~(bled_num|bled_caps|bled_win_lock);
        }

        hint_time_count--;
        if(2>hint_time_count)
        {
            hint_time_count = 0;
            hint_led_status = 0x00;
        }

        return;
    }

    rtc_cur_value = drv_rtc_read(OM_RTC);
    if(iDEVICE_TYPE_USB != device_type)
    {
        if(ble_24g_ctrl & bom6239_conning)
        {
            if(0 == device_conn_index)
            {
                device_conn_index++;
                led_delay_count = rtc_cur_value;
            }
            else if(1 == device_conn_index)
            {
                if(ble_24g_ctrl&bom6239_conn_ok)
                {
                    device_conn_index++;
                    led_delay_count = rtc_cur_value;
                    device_hint_count = 0;
                }

                key_into_fcc_test();
                if(false == bdevice_led_status)
                {
                    device_led_off();
                    if(mMS_TO_TICK(250) < (rtc_cur_value - led_delay_count))
                    {
                        exled_rtc_count=0;
                        bdevice_led_status = true;
                        led_delay_count = rtc_cur_value;
                    }
                }
                else
                {
                    device_led_on();
                    if(mMS_TO_TICK(250) < (rtc_cur_value - led_delay_count))
                    {
                        exled_rtc_count=0;
                        bdevice_led_status = false;
                        led_delay_count = rtc_cur_value;
                    }
                }
            }
            else if(2 == device_conn_index)
            {
                device_conn_index = 0;
                ble_24g_ctrl &= ~bom6239_conning;
            }
            return;
        }
        else if(ble_24g_ctrl&bom6239_sync_ing)
        {
            if(true == b24g_nromal)
                return;

            if(0 == device_conn_index)
            {
                device_conn_index++;
                led_delay_count = rtc_cur_value;
            }
            else if(1 == device_conn_index)
            {
                if(ble_24g_ctrl&bom6239_sync_ok)
                {
                    device_conn_index++;
                    led_delay_count = rtc_cur_value;
                    device_hint_count = 0;
                }

                if(false == bdevice_led_status)
                {
                    device_led_off();
                    if(mMS_TO_TICK(1000) < (rtc_cur_value - led_delay_count))
                    {
                        exled_rtc_count=0;
                        bdevice_led_status = true;
                        led_delay_count = rtc_cur_value;
                    }
                }
                else
                {
                    device_led_on();
                    if(mMS_TO_TICK(1000) < (rtc_cur_value - led_delay_count))
                    {
                        exled_rtc_count=0;
                        bdevice_led_status = false;
                        led_delay_count = rtc_cur_value;
                    }
                }
            }
            else if(2 == device_conn_index)
            {
                device_conn_index = 0;
                ble_24g_ctrl &= ~bom6239_sync_ing;
            }
            return;
        }

        device_led_on();
        device_conn_index = 0;
    }
    else
    {
        device_led_off();
        if(0 != (usb_ctrl_flag&busb_suspend_resume))
            return;
    }
}

/******************************************************************************
* 函数名称: logo_process
* 功能描述: 背光灯效主流程（PWM 单色背光，移植自 K102）
* 输入参数:
* 输出参数:
* 说    明:
******************************************************************************/
void logo_process(void)
{
    uint32_t rtc_temp;

    rtc_temp = drv_rtc_read(OM_RTC);
    if((mMS_TO_TICK(500) < (rtc_temp-exled_rtc_count))||(0 == exled_rtc_count))
    {
        exled_rtc_count = rtc_temp;
        exLogo_send();
    }

    if(true==bsave_status)
    {
        bsave_status = false;
        flash_save_status();
    }
    if(true==bsave_cfg_dpi)
    {
        bsave_cfg_dpi = false;
        flash_save_cfg_dpi();
    }
    if(iDEVICE_TYPE_24G == device_type)
    {
        if(true==bflash_ctrl)
            return;
    }

    if(iDEVICE_TYPE_USB == device_type)
    {
        if(0 != (usb_ctrl_flag&busb_suspend_resume))
        {
            logo_off();
            return;
        }
    }
    else
    {
        if(ble_24g_ctrl&bom6239_sync_ing)
        {
            logo_off();
            return;
        }
    }
    if((false == bled_bright_en) || (iLED_LEV_MIN==led_light_lev))
    {
        logo_off();
        return;
    }

    if((bat_status & bcharging)==0)
    {
        if(iLOW_POWER>=bat_value)
        {
            logo_off();
            return;
        }
    }

    if(false==bpwm_valid)
    {                                       // 不需要等待：pwm_init() 内部本身就是
        pwm_init();                         // 背靠背写寄存器，后续也只是再写 PWMDC0。
    }                                       // 这里阻塞会压到射频时隙(本函数在 RF 回调里跑)

    if(0 != exio_ctrl)                      // 背光点亮的同时唤醒 54E 显示
    {
        exio_ctrl = 0;
        exled_rtc_count=0;
    }

    if(led_light_lev != led_light_lev_bak)
    {                                       // 周期不变，只需按新档位重算占空
        led_light_lev_bak = led_light_lev;
        pwm_duty_set(pwm_duty_cur);
    }

    if(led_pre_mode != led_cur_mode)
    {                                       // 模式切换：呼吸按切换前的亮暗状态平滑起步
        if(iMODE_LED_BREATH == led_cur_mode)
        {
            if(iMODE_LED_ON == led_pre_mode)
                bback_led_status = false;   // 常亮 -> 呼吸：从最亮处逐渐变暗
            else
                bback_led_status = true;    // 关闭/其他 -> 呼吸：从最暗处逐渐变亮
        }
        led_pre_mode = led_cur_mode;
        pwm_time_count = 0;
        delay_time_count = 0;
    }

    switch(led_cur_mode)
    {
        case iMODE_LED_ON:                  // 常亮
            pwm_duty_set(iPWM_MAX);         // 最亮档下为真正的 100%
            break;
        case iMODE_LED_BREATH:              // 呼吸
            effect_breatch();
            break;
        case iMODE_LED_OFF:                 // 关闭
        default:
            pwm_duty_set(0);                // 只把占空置恒灭，不停定时器
            break;
    }
}
