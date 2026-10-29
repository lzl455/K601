/* ----------------------------------------------------------------------------
 * Copyright (c) 2020-2030 OnMicro Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of OnMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------- */

/**
 * @file     mouse_sw.c
 * @brief    ble keyboard: key scan, sleep levels and BLE app events
 * @date     20 JUN 2024
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

/*******************************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "om_driver.h"
#include "ex2_app_mouse.h"
#include "mouse.h"
#include "ex2_api.h"
#include "ex2_hal.h"
#include "main.h"
#include "led.h"
#include "lvd.h"
#include "exLed.h"

#include "matrix_key.h"
#include "flash_process.h"
#include "userdefined_key.h"
#include "time_delay.h"

#include "usb_cfg_info.h"

/*******************************************************************************
 * CONST & VARIABLES
 */
 ms_sleep_level_t ms_sleep_level = MS_SLEEP_LEVEL0;
//-----------------------------------------------------------------------------
// BT5.4 KB-1
//
const char                  bt_kb_name_tbl_bt0[MOUSE_NAME_MAX_LEN] ="AUSDOM K98Pro BT1";
const char                  bt_kb_name_tbl_bt1[MOUSE_NAME_MAX_LEN] ="AUSDOM K98Pro BT2";
const char                  bt_kb_name_tbl_bt2[MOUSE_NAME_MAX_LEN] ="AUSDOM K98Pro BT3";

char                        bt_kb_name[MOUSE_NAME_MAX_LEN];

uint8_t                     ble_interval;
bool                        bble_encrypted;     // 本次连接已加密: ex2_ll_hci_ev_encrypt_chg() 置, 重新广播时清

extern bool                 bperiph_int;
extern uint32_t             adv_interval_ms;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
pmu_lpm_t pm_checker_callback(void)
{
    if(ms_sleep_level < MS_SLEEP_LEVEL2) {
        if(MS_SLEEP_LEVEL0 == ms_sleep_level)
        {                                       // 此处根据实际情况设置，有PWM等不同外设，休眠状态会有差异
            if(0 != OM_RTC->TICK)
                return PMU_LPM_IDLE;
        }

        /* 距离下次唤醒时间剩余300us， 不进入休眠, 否则进入休眠 */
        if ((ex2_stk_sleep_duration_get() > 10))
            return PMU_LPM_STOP1;
    }
    else
    {
        if(0 == (bat_status&bcharging))
        {
            ex2_stk_uninit();
            //进入深睡眠前需要存一些配置信息(如dpi、模式、地址等)到HS_PMU->SCRATCH, 醒来后可通过OM_CPM->RSTCTRL判断复位原因决定广播还是回连。

            disp_off();                         // 进 LEVEL2 时已灭屏, 这里只是兜底

            mouse_periph_wakeup_enable(WAKEUP_SLEEP_LEVEL2);
            drv_pmu_set_low_power_mode(PMU_LPM_DEEP_SLEEP);
            while(1);        
        }
        else
            {                                   // 充电状态不进人休眠
            if(bperiph_int)
                NVIC_SystemReset();

            if((ex2_stk_sleep_duration_get() > 10))
                return PMU_LPM_STOP1;
        }
    }

    return PMU_LPM_IDLE;
}

extern bool ex2_hid_tx_notification_mouse(uint8_t *buf);
extern bool ex2_hid_tx_notification_keyboard(uint8_t *buf);
extern bool ex2_hid_tx_notification_bitmap_kb(uint8_t *buf);
extern bool ex2_hid_tx_notification_consumer_control(uint8_t *buf);
extern bool profile_bas_tx_notification(uint8_t level);
extern  uint8_t hid_keyboard_out_report_value;

/*********************************************************************
 * 协议栈弱函数的实现: 收到 LL_START_ENC_RSP, 链路加密完成(回连和新配对都会走到)。
 * 回连时 RECONNECT 事件里就打开了 HID 通知, 比加密完成早; 加密没完成按键发不到主机,
 * 加密约 3s 还没完成协议栈报 CONN_TIMEOUT(复位)
 */
bool ex2_ll_hci_ev_encrypt_chg(void)
{
    bble_encrypted = true;
    return false;                               // 返回值协议栈不用, 同弱函数
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */
__RAM_CODE uint8_t ex2_app_mouse_scan(void)
{
    uint8_t temp_buff[8];

    if (!hid_notification_is_enabled()) {
        return 0;
    }

    if(true == bble_encrypted)                  // 链路加密后按键才真正发到主机, 这时才算连上(图标常亮、装饰灯亮)
        ble_24g_ctrl |= bom6239_sync_ok;

    if(MS_SLEEP_LEVEL0 == ms_sleep_level)
    {
        matrixkey_scan();
        logo_process();

        lvd_scan_count++;
        if(0==(lvd_scan_count&0x01))
            device_led_process();               // 15ms
        else
            led_hint_process();
    }
    else
    {                                           // 休眠: 不充电灭屏; 充电时 LEVEL1 继续显示电量, 到 LEVEL2 灭屏
        led_hint_process();
        flash_save_process();                   // 睡眠里电量变了马上存, 进深睡就丢了
        if((MS_SLEEP_LEVEL1 == ms_sleep_level)&&(0 != (bat_status&bcharging)))
            disp_refresh();
        else
            disp_off();
    }
	scan_mode_swtich(iMODE_CHECK_DELAY);

    kb_led_status = hid_keyboard_out_report_value;
    if(0!=(m_data_flag&bstd_kb_data_send))
    {
        temp_buff[0] = std_key[0];
        temp_buff[1] = 0;
        memcpy(temp_buff+2, std_key+1, 6);
        if(true == ex2_hid_tx_notification_keyboard(temp_buff))
        {
            rtc_init_value = drv_rtc_read(OM_RTC);
            m_data_flag &= ~bstd_kb_data_send;
            key_tx_count = 0;
        }
        return 1;
    }
    else if(0!=(m_data_flag&bmmk_data_send))
    {
        temp_buff[0] = mmk_data_l;
        temp_buff[1] = mmk_data_h;
        if(true == ex2_hid_tx_notification_consumer_control(temp_buff))
        {
            rtc_init_value = drv_rtc_read(OM_RTC);
            m_data_flag &= ~bmmk_data_send;
            mmk_tx_count = 0;
        }
        return 1;
    }
    else if(0!=(m_data_flag&bstd_ex_data_send))
    {
        if(true == ex2_hid_tx_notification_bitmap_kb(std_key+7))
        {
            rtc_init_value = drv_rtc_read(OM_RTC);
            m_data_flag &= ~bstd_ex_data_send;
            key_ex_tx_count = 0;
        }
        return 1;
    }
    else if(false != bbat_update)
    {
        if(true == profile_bas_tx_notification(bat_value))
            bbat_update=false;
        return 2;                               // 只发电量, 不算按键操作
    }
    else if(0!=m_tx_count)
    {                                           /* 未扫描到数据 */
        temp_buff[0] = mouse_data_key&0x1F;       // button
        temp_buff[1] = ((mouse_x>>0)&0xFF);       // x
        temp_buff[2] = (((mouse_x>>8)&0x0F)|((mouse_y<<4)&0xF0)); // y
        temp_buff[3] = ((mouse_y>>4)&0xFF);       //
        temp_buff[4] = mouse_data_z;              // wheel
        temp_buff[5] = 0;                         //til
        if(true == ex2_hid_tx_notification_mouse(temp_buff))
        {
            rtc_init_value = drv_rtc_read(OM_RTC);
            m_data_flag &=~(bmouse_data_send);
            mouse_x = 0;
            mouse_y = 0;
            mouse_data_z = 0;
            m_tx_count = 0;
            return 1;
        }
    }
    else
    {
        if(true == bsave_ble_id)
        {
            bsave_ble_id = false;
            flash_save_ble_addr();
        }
    }
    return 0;
}

extern ex2_gap_ppcp_t       ex2_gap_ppcp_value;
extern mouse_adv_state_t    mouse_state;
extern bool                 bscan_sensor_en;

extern uint16_t ll_ctrl_get_peer_company_id(void);
extern void stk_notify_event_app(uint32_t evt);

/*********************************************************************
 * 从工作状态直接进入 LEVEL2(广播超时 / 主机断开)
 * 不充电时 pm_checker_callback() 随后进深睡; 充电时停在 LEVEL2,
 * 所以这里要把休眠状态配齐, 与 LEVEL1 -> LEVEL2 路径的最终状态一致:
 *   - 灭屏(LEVEL2 充电时也灭屏)
 *   - 配按键唤醒, 关装饰灯
 */
static void ble_enter_sleep_level2(void)
{
    ms_sleep_level = MS_SLEEP_LEVEL2;
    disp_off();
    mouse_periph_wakeup_enable(WAKEUP_SLEEP_LEVEL2);
                                                // 没有连接也没有广播: 充电时不深睡, 也就没有射频中断喂狗,
    drv_rtc_control(OM_RTC, RTC_CONTROL_ALARM0_INT_EN, NULL);   // 开 RTC 16ms 节拍(drv_rtc_isr_callback 的
    drv_rtc_set_alarm0(drv_rtc_read(OM_RTC) + mMS_TO_TICK(16)); // LEVEL2 分支)喂狗, 否则 10s 后看门狗复位
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void app_ble_evt_dispatch(uint32_t events)
{
    static uint32_t connect_start_time = 0; /* work around intel bluetooth bug */
    if (events & EX2_EVENT_APP_CONNECT_IND) {
        mouse_periph_wakeup_enable(WAKEUP_ALL_DISABLE);
        drv_rf_set_tx_power(DRV_RF_PA_PWR_0dBm);
        /* stop polling the peripheral*/
        ex2_ll_polling_time_stop();
        connect_start_time = 0;
    }
    if (events & (EX2_EVENT_APP_SLAVE_TERMINATED)) {
        uint8_t bda[6];
        // 睡眠中掉线: 先回工作态再回连。停在 LEVEL1/2 的话, 回连后 CONNECT_IND 关了按键唤醒、
        // LEVEL1/2 又不扫键, 按键失灵; 广播中按键还会用旧连接参数调 ll_conn_sleep_prologue()
        ms_sleep_level = MS_SLEEP_LEVEL0;
        mouse_periph_wakeup_enable(WAKEUP_SLEEP_LEVEL0);
        drv_ble_get_bda(bda);
        /* reset rtc & rf module */
        ex2_hal_uninit();
        /* Avoid conn activites during ms delay. */
        __disable_irq();
        ex2_stk_init(bda);
        mouse_state = MOUSE_LE_STATE_DISCONN;
        ex2_app_mouse_broadcast_fsm();

        __enable_irq();
    }
    if (events & EX2_EVENT_APP_CONN_TIMEOUT) {
        __disable_irq();
        NVIC_SystemReset();
    }

    if(events & EX2_EVENT_APP_MASTER_TERMINATED) {
        ble_enter_sleep_level2();
    }
    if (events & (EX2_EVENT_APP_PAIR_COMPLETED | EX2_EVENT_APP_RECONNECT)) {
        if (events & EX2_EVENT_APP_RECONNECT) {
            hid_notification_enable(1);
        }
    }
    if (events & EX2_EVENT_APP_VERSION_EXCHANGE) {
        if(ll_ctrl_get_peer_company_id() == 0x0002/* Intel */) {  /* work around intel bluetooth bug */
            connect_start_time = drv_rtc_read(OM_RTC);
        }
    }
    if (events & EX2_EVENT_APP_CONN_SLAVE) {
        if (!(OM_RTC->INTMASK & RTC_INTMASK_ALARM0_INTRAW_STATUS_MASK)) {
            if (hid_notification_is_enabled())
            {
                ex2_l2cap_update_conn_para(&ex2_gap_ppcp_value);
                ex2_ll_polling_time_start();
                exled_rtc_count=0;
                if(0 == (bat_status&blvd_power_on))
                    bbat_update = true;         // 每次连上都把当前电量通知主机, 不让电脑沿用旧值

                if(ble_24g_ctrl&bom6239_conning)
                {
                    ble_24g_ctrl |= bom6239_conn_ok;
                    bsave_ble_id = true;
                }
            }
        }
        if(connect_start_time) { /* work around intel bluetooth bug */
            if (EX2_TIME_OLDER_THAN(connect_start_time+EX2_MS_TO_TICK(5000), drv_rtc_read(OM_RTC))) {
                if(!hid_notification_is_enabled()) {
                    connect_start_time = 0;
                    stk_notify_event_app(EX2_EVENT_APP_CONN_TIMEOUT);
                }
            }
        }
    }
    if (events & EX2_EVENT_APP_ADV_TIMEOUT) {
        if(mouse_state == MOUSE_LE_STATE_STOP) { /* 广播一直没有master连接，超时进入deepsleep*/
            ble_enter_sleep_level2();
        } else {
            ex2_app_mouse_broadcast_fsm();
        }
    }

    if(events & EX2_EVENT_APP_ADV_EVENT_DONE) { /* 一个广播事件结束 */
        sys_tick_count += adv_interval_ms;
        ble_24g_ctrl |= bom6239_sync_ing;
        matrixkey_scan();
        scan_mode_swtich(iMODE_CHECK_DELAY);
        led_hint_process();                     // 广播期间(上电回连/对码)也显示电量
        logo_process();
        device_led_process();
    }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

__RAM_CODE void drv_rtc_isr_callback(void)
{
    uint32_t rtc_count;
    uint8_t scan_result;

    rtc_count = drv_rtc_read(OM_RTC);
    if(!rtc_init_value)
    {
        rtc_init_value = rtc_count;
    }

    if(MS_SLEEP_LEVEL2 == ms_sleep_level)
    {                                           // LEVEL2(充电时不深睡): 广播/连接已停, 没有射频中断喂狗,
        drv_wdt_keep_alive(OM_WDT);             // 由这里每 16ms 喂一次, 否则会被看门狗周期性复位
        while (0 == (OM_RTC->CR & RTC_CR_ALARM0_WRITE_STATUS_MASK));
        OM_RTC->ALARM0 = (rtc_count + mMS_TO_TICK(16));
        led_hint_process();
        flash_save_process();
        return;
    }

    if (hid_notification_is_enabled())
    {
        switch (ms_sleep_level)
        {
            case MS_SLEEP_LEVEL0:
                if(ble_sleep_1_time_tbl[sleep_time_ble&0x07] > (rtc_count-rtc_init_value))
                {
                    ex2_ll_set_next_polling_time(0);
                }
                else
                {
                    rtc_init_value = rtc_count;
                    ms_sleep_level = MS_SLEEP_LEVEL1;
                    mouse_periph_wakeup_enable(WAKEUP_SLEEP_LEVEL1);

                    ex2_ll_set_next_polling_time(1); // 灭屏与否在随后的 ex2_app_mouse_scan() 里处理
                }
                sys_tick_count += 8;

                break;
            case MS_SLEEP_LEVEL1:
                if((ble_sleep_2_time_tbl[(sleep_time_ble>>4)&0x07] - ble_sleep_1_time_tbl[sleep_time_ble&0x07]) > (rtc_count-rtc_init_value))
                {
                    ex2_ll_set_next_polling_time(1);
                }
                else
                {
                    rtc_init_value = rtc_count;
                    ex2_ll_set_next_polling_time(0);
                    ms_sleep_level = MS_SLEEP_LEVEL2;
                }
                sys_tick_count += 300;

                break;
            default:
                break;
        }
    }

    scan_result = ex2_app_mouse_scan();
    if(scan_result)
    {
        if((1 == scan_result)&&(MS_SLEEP_LEVEL0 != ms_sleep_level))
        {                                       // 只发电量(2)时不退出休眠: 否则睡眠中电量一变就亮装饰灯、重新计休眠时间
            ms_sleep_level = MS_SLEEP_LEVEL0;
            mouse_periph_wakeup_enable(WAKEUP_SLEEP_LEVEL0);
            exled_rtc_count=0;
        }

        ex2_stk_interrupt();
    }
}
