/*******************************************************************************
* Copyright (c) 2013, SHENZHEN Co.,LTD.
* All rights reserved.
*
* 文件名称: led.c
*
* 功能说明: 54E 外挂显示/状态指示(本机无背光)
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

uint8_t                         device_conn_index=0;
uint8_t                         device_hint_count;

uint32_t                        led_delay_count;


uint32_t                        exled_rtc_count=0;  // 54E 显示刷新节拍

bool                            bpwm_valid;
bool                            bback_led_status;

uint8_t                         led_cur_mode = iMODE_LED_ON;        // 装饰灯默认常亮
uint8_t                         led_pre_mode = iMODE_LED_INVALID;
enum led_level                  led_light_lev = iLED_LEV_MAX;
enum led_level                  led_light_lev_bak;
enum speed_level                led_speed_lev = iSPEED_LEV_1;

uint8_t                         pwm_value;
uint8_t                         pwm_time_count;
uint16_t                        delay_time_count;

extern bool                     b24g_nromal;

//=============================================================================;
// 代码区                                                                       ;
//=============================================================================;

/******************************************************************************
* 函数名称: pwm_init
* 功能描述: 装饰灯 PWM 初始化(通道1, P01)
* 输入参数:
* 输出参数:
* 说    明: 亮度按档位改周期: 占空比最大 iPWM_MAX, 周期越长越暗。
*           LED 负极接 P01, 低电平亮: 通道设为高有效, 非有效(低)宽度 PWMDC1 就是灯亮的宽度。
*           只开通道1, 不能碰通道0(P00 是 54E UART)
******************************************************************************/
void pwm_init(void)
{
    bpwm_valid = true;

    DRV_RCC_CLOCK_ENABLE(RCC_CLK_PWM, 1U);
    OM_PWM->PWMCON &= ~DECO_PWM_EN;
    OM_PWM->PWMCR0 |= DECO_PWM_EN;
    OM_PWM->PWMCR1 |= DECO_PWM_EN;
    OM_PWM->PWMDIV = 0x00000003;

    if(iLED_LEV_4 == led_light_lev)
        OM_PWM->PWMPRD = iPWM_MAX;
    else if(iLED_LEV_3 == led_light_lev)
        OM_PWM->PWMPRD = iPWM_MAX*2;
    else if(iLED_LEV_2 == led_light_lev)
        OM_PWM->PWMPRD = iPWM_MAX*4;
    else
        OM_PWM->PWMPRD = iPWM_MAX*8;

    DECO_PWM_DC = 0;
    OM_PWM->PWMCON |= DECO_PWM_EN;
    mDECO_LED_ON;                               // PWM 接管 P01 后再给 P23 上电, 不闪全亮
}

/******************************************************************************
* 函数名称: logo_off
* 功能描述: 装饰灯灭, 关 PWM
* 输入参数:
* 输出参数:
* 说    明: 先拉低 P23 再关 PWM: 关了 PWM 的 P01 是 GPIO 低电平, P23 还高的话灯会全亮
******************************************************************************/
void logo_off(void)
{
    mDECO_LED_OFF;
    bpwm_valid = false;
    OM_PWM->PWMCON = 0x00000000;
    led_pre_mode = iMODE_LED_INVALID;           // 下次打开按当前模式重新开始
}

/******************************************************************************
* 函数名称: deco_led_process
* 功能描述: 装饰灯按模式(常亮/呼吸/心跳)刷新
* 输入参数:
* 输出参数:
* 说    明: device_led_process() 在连上时每 15ms 左右调用一次
******************************************************************************/
void deco_led_process(void)
{
    if((iMODE_LED_OFF == led_cur_mode) || (iLED_LEV_MIN == led_light_lev))
    {
        logo_off();
        return;
    }

    if(false == bpwm_valid)
    {
        led_light_lev_bak = led_light_lev;
        pwm_init();
    }

    if(led_light_lev != led_light_lev_bak)
    {
        led_light_lev_bak = led_light_lev;
        pwm_init();
    }

    if(led_pre_mode != led_cur_mode)
    {
        led_pre_mode = led_cur_mode;

        switch(led_cur_mode)
        {
            case iMODE_LED_ON:
                DECO_PWM_DC = iPWM_MAX-1;
                break;
            case iMODE_LED_BREATH:
                pwm_time_count = 0;
                delay_time_count = 0;
                bback_led_status = true;        // 从灭开始渐亮
                DECO_PWM_DC = 0;
                break;
            case iMODE_LED_HB:
                delay_time_count = 0;
                break;
            default:
                break;
        }
    }
    else
    {
        switch(led_cur_mode)
        {
            case iMODE_LED_ON:
                DECO_PWM_DC = iPWM_MAX-1;
                break;
            case iMODE_LED_BREATH:
                effect_breatch();
                break;
            case iMODE_LED_HB:
                effect_heartbeat();
                break;
            default:
                break;
        }
    }
}

/*******************************************************************************
* 函数名称: device_led_on()
* 功能描述:
* 输入参数： 无
* 输出参数： 无
* 说    明: 图标有变化就强制刷新(exled_rtc_count=0), 马上上屏, 不等下一个 500ms
*******************************************************************************/
void device_led_on(void)
{
    uint32_t led_old = exled;

    exled &= ~(bled_24G|bled_BT1|bled_BT2);

    if(iDEVICE_TYPE_BT0 == device_type)
        exled |= bled_BT1;
    else if(iDEVICE_TYPE_BT1 == device_type)
        exled |= bled_BT2;
    else if(iDEVICE_TYPE_24G == device_type)
        exled |= bled_24G;

    if(led_old != exled)
        exled_rtc_count = 0;
}

/*******************************************************************************
* 函数名称: device_led_off()
* 功能描述:
* 输入参数： 无
* 输出参数： 无
* 说    明: 同 device_led_on(), 图标有变化就强制刷新
*******************************************************************************/
void device_led_off(void)
{
    if(0 != (exled&(bled_24G|bled_BT1|bled_BT2)))
        exled_rtc_count = 0;

    exled &= ~(bled_24G|bled_BT1|bled_BT2);
}

/*******************************************************************************
* 函数名称: disp_off()
* 功能描述: 54E 灭屏: 发一次关机帧, 驱动板灭灯后 1s 休眠
* 说    明: 本板 DIG5 脚未接, 灭屏只能靠关机帧。灭屏期间不能发显示帧,
*           任何一帧都会唤醒驱动板重新亮屏; 亮屏统一走 disp_refresh()
*******************************************************************************/
void disp_off(void)
{
    if(false == bdisp_off)
    {
        exled_ctrl = 0x02;
        exLogo_send();
    }
}

/*******************************************************************************
* 函数名称: disp_refresh()
* 功能描述: 54E 亮屏刷新, 每 500ms 发一帧 exled
* 说    明: 灭屏后的第一帧只用来唤醒驱动板(唤醒沿上的数据可能收不全),
*           20ms 后补发正式帧。调用间隔要小于 1s, 否则驱动板会再次休眠。
*           强制刷新(exled_rtc_count=0, 图标闪烁翻转等)后 20ms 再补发一帧:
*           驱动板偶尔漏收一帧时, 这次变化不会拖到下一个 500ms 才上屏
*******************************************************************************/
void disp_refresh(void)
{
    uint32_t rtc_temp;

    if((0 != USB_5V_PIN) && (0 == MODE_DATA))  // Wir 只在开关拨到中间且检测到 5V 时亮, 拨到两侧(蓝牙/2.4G)不亮
        exled |= bled_WIR;                  // 每次刷新直接读脚, 不依赖各模式里 led_hint_process() 的调用时机
    else
        exled &= ~bled_WIR;

    rtc_temp = drv_rtc_read(OM_RTC);
    if(true == bdisp_off)
    {
        exLogo_send();
        exled_rtc_count = rtc_temp - mMS_TO_TICK(480);
    }
    else if(0 == exled_rtc_count)
    {
        exled_rtc_count = rtc_temp - mMS_TO_TICK(480);
        exLogo_send();
    }
    else if(mMS_TO_TICK(500) < (rtc_temp-exled_rtc_count))
    {
        exled_rtc_count = rtc_temp;
        exLogo_send();
    }
}

extern void key_into_fcc_test(void);

/******************************************************************************
* 函数名称: device_led_process
* 功能描述: 模式图标(BT1/BT2/2.4G)回连/对码闪烁
* 输入参数:
* 输出参数:
* 说    明: 对码快闪(250ms); 未连接(回连/同步)慢闪(1s); 连上常亮。蓝牙和 2.4G 相同。
*           闪烁从亮开始, 第一拍也亮满一个周期; 亮灭一变, device_led_on/off() 马上强制刷新上屏
******************************************************************************/
void device_led_process(void)
{
    rtc_cur_value = drv_rtc_read(OM_RTC);

    if(ble_24g_ctrl&bom6239_sync_ok)
    {                                           // 装饰灯: 键盘能用才亮 = 蓝牙链路已加密且 HID 已就绪 / 2.4G 已同步接收器,
        deco_led_process();                     // 回连、对码、找接收器时灭。休眠由 mouse_periph_wakeup_enable()
    }                                           // 关灯, 睡眠里不调本函数, 醒来后在这里按连接状态再开
    else
    {
        logo_off();
    }

    if(iDEVICE_TYPE_USB != device_type)
    {
        if(ble_24g_ctrl & bom6239_conning)
        {
            if(0 == device_conn_index)
            {
                device_conn_index++;
                led_delay_count = rtc_cur_value;
                bdevice_led_status = true;      // 进闪烁前图标是亮的(开机/连接时常亮), 从亮开始
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
                        bdevice_led_status = true;
                        led_delay_count = rtc_cur_value;
                    }
                }
                else
                {
                    device_led_on();
                    if(mMS_TO_TICK(250) < (rtc_cur_value - led_delay_count))
                    {
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
                bdevice_led_status = true;      // 同对码: 从亮开始
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
                        bdevice_led_status = true;
                        led_delay_count = rtc_cur_value;
                    }
                }
                else
                {
                    device_led_on();
                    if(mMS_TO_TICK(1000) < (rtc_cur_value - led_delay_count))
                    {
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
* 功能描述: 54E 显示刷新、flash 保存
* 输入参数:
* 输出参数:
* 说    明: 只在亮屏的工作态调用(灭屏后再调用会唤醒驱动板亮屏)
******************************************************************************/
void logo_process(void)
{
    disp_refresh();
    flash_save_process();
}
