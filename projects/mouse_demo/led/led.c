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

extern bool                     b24g_nromal;

//=============================================================================;
// 代码区                                                                       ;
//=============================================================================;

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
*           20ms 后补发正式帧。调用间隔要小于 1s, 否则驱动板会再次休眠
*******************************************************************************/
void disp_refresh(void)
{
    uint32_t rtc_temp;

    rtc_temp = drv_rtc_read(OM_RTC);
    if(true == bdisp_off)
    {
        exLogo_send();
        exled_rtc_count = rtc_temp - mMS_TO_TICK(480);
    }
    else if((mMS_TO_TICK(500) < (rtc_temp-exled_rtc_count))||(0 == exled_rtc_count))
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
* 说    明: 对码快闪(250ms); 未连接(回连/同步)慢闪(1s); 连上常亮。蓝牙和 2.4G 相同
******************************************************************************/
void device_led_process(void)
{
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
* 功能描述: 54E 显示刷新、flash 保存
* 输入参数:
* 输出参数:
* 说    明: 只在亮屏的工作态调用(灭屏后再调用会唤醒驱动板亮屏)
******************************************************************************/
void logo_process(void)
{
    disp_refresh();

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
}
