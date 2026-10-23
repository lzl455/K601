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

uint8_t                         hint_time_count;

uint32_t                        exled_rtc_count=0;  // 54E 显示刷新节拍

extern bool                     b24g_nromal;

//=============================================================================;
// 代码区                                                                       ;
//=============================================================================;

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
* 功能描述: 关 54E 外挂显示(转关屏态)
* 说    明: 本机无背光, 只控制 54E
*******************************************************************************/
void logo_off(void)
{
    if(0xFF != exio_ctrl)
    {
        exled_rtc_count=0;
        exio_ctrl = 0xFF;
    }
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
    if((bat_status & bcharging)==0)
    {
        if(iLOW_POWER>=bat_value)
        {
            logo_off();
            return;
        }
    }

    if(0 != exio_ctrl)                      // 唤醒 54E 显示
    {
        exio_ctrl = 0;
        exled_rtc_count=0;
    }
}
