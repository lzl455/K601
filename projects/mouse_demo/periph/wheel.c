/*******************************************************************************
* Copyright (c) 2013, SHENZHEN SEMITEK Co.,LTD.
* All rights reserved.
*
* 文件名称: wheel.c
*
* 功能说明: 滚轮扫描处理
* 修改说明:
*******************************************************************************/
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "om_driver.h"

#include "main.h"
#include "wheel.h"
#include "led.h"

#include "key_code_define.h"
#include "flash_process.h"
#include "hid_process.h"

//-----------------------------------------------------------------------------;
// 常量定义区
//

//-----------------------------------------------------------------------------;
// 变量定义区
//
uint8_t                     wheel_status;
uint32_t                    wheel_press_time_count;

//=============================================================================;
// 代码区                                                                      ;
//=============================================================================;

/*******************************************************************************
* 函数名称: wheel_scan()
* 功能描述: 滚轮扫描处理
* 输入参数：无
* 输出参数：无
* 说    明:
*******************************************************************************/
void wheel_scan(void)
{
#if (iWHEEL_ENCODER_EN)
    if(OM_ENCODER->VALID&ENCODER_VALID_CNT_MASK)
    {
        wheel_data = OM_ENCODER->CNT;
        if(wheel_data & 0x08)
            wheel_data |= 0xf0;
        else
            wheel_data &= 0x0f;
    }
#endif

    rtc_cur_value = drv_rtc_read(OM_RTC);
    if(0==(wheel_status&bwheel_press))
    {
        if(0!=(wheel_status&bwheel_press_bak))
        {
            wheel_status &= ~bwheel_press_bak;

            if(mMS_TO_TICK(1000) > (rtc_cur_value - wheel_press_time_count))
            {                                   // 短按: 静音
                wheel_press_time_count = 0;
                mmk_data_l = iMMK_MUTE_L;
                mmk_data_h = iMMK_MUTE_H;

                m_data_flag |= bmmk_data_send;
                mmk_tx_count = 8;
            }
        }
    }
    else
    {
        if(0==(wheel_status&bwheel_press_bak))
        {
            wheel_status |= bwheel_press_bak;
            wheel_press_time_count = rtc_cur_value;
        }
    }

    if(0 != wheel_data)
    {                                           // 转动: 音量加减
        if(0==(m_data_flag&bmmk_data_send))
        {
            if(0!=(wheel_data&(1<<7)))
            {
                mmk_data_l = iMMK_VOLUMEDOWN_L;
                mmk_data_h = iMMK_VOLUMEDOWN_H;
                wheel_data++;
            }
            else
            {
                mmk_data_l = iMMK_VOLUMEUP_L;
                mmk_data_h = iMMK_VOLUMEUP_H;
                wheel_data--;
            }
            m_data_flag |= bmmk_data_send;
            mmk_tx_count = 8;
        }
    }
}
