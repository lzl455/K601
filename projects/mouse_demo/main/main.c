/* ----------------------------------------------------------------------------
 * Copyright (c) 2020-2030 OnMicro Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distributon.
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
 * @file     main.c
 * @brief    main entry
 * @date     18 Sept. 2023
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
#include "om_driver.h"
#include "evt.h"
#include <string.h>

#include "mouse.h"
#include "ex2_app_mouse.h"
#include "main_usb.h"
#include "main_24g.h"
#include "ex2_hal.h"
#include "exLed.h"
#include "led.h"
#include "lvd.h"

//-----------------------------------------------------------------------------;
//常量定义区
//
#define iDEBUG_SWD_OPEN     0                   // 1:打开SWD调试口(调试阶段使用) 0:关闭(量产务必改回0)

//-----------------------------------------------------------------------------;
//变量定义区
//
bool                        brf_rate;
bool                        bmode_swtich_valid;

volatile uint8_t            ble_24g_ctrl;
volatile uint8_t            sys_ctrl;

uint8_t                     sys_type;
device_type_t               device_type;
device_type_t               bt_index;           // 硬件切换模式，蓝牙多通道情况会使用到

volatile uint32_t           sys_tick_count;
uint32_t                    rtc_init_value;
uint32_t                    rtc_cur_value;
uint32_t                    rtc_temp_value;

//----------------------------------------------
// 鼠标键盘数据相关寄存器
//
uint8_t                     m_data_flag;
uint8_t                     kb_led_status;

uint8_t                     m_tx_count;
uint8_t                     key_tx_count;
uint8_t                     key_ex_tx_count;
uint8_t                     mmk_tx_count;
uint8_t                     def_tx_count;

uint8_t                     mouse_data_key;
uint16_t                    mouse_x;
uint16_t                    mouse_y;
uint8_t                     mouse_data_z;

uint8_t                     std_key[22];
uint8_t                     mmk_data_l;
uint8_t                     mmk_data_h;

//
//----------------------------------------------
// 鼠标ID
//
uint8_t                     kb_bd_addr[6];

//-----------------------------------------------------------------------------;
//外部引用区
//
extern void rf_freq_make(void);
extern pmu_lpm_t pm_checker_callback(void);
extern uint32_t adv_interval_ms;
extern void bat_check(void);

/******************************************************************************
* 函数名称: mouse_periph_wakeup_enable()
* 功能描述: 外设唤醒设置
* 输入参数: 唤醒屏蔽位
* 输出参数: 无
* 说    明: 
******************************************************************************/
void mouse_periph_wakeup_enable(uint8_t mask)
{
    OM_CRITICAL_BEGIN();
    drv_gpio_set_trig(OM_GPIO0, (1<<KEY_R0_PIN)|(1<<KEY_R1_PIN)|(1<<KEY_R2_PIN)|(1<<KEY_R3_PIN)| \
                                (1<<KEY_R4_PIN)|(1<<KEY_R5_PIN)|(1<<KEY_R6_PIN)|(1<<KEY_R7_PIN), \
                                 GPIO_TRIG_NONE);
    NVIC_ClearPendingIRQ(WAKEUP_GPIO0_IRQn);
    drv_pmu_clear_wakeup();

    if(mask & WAKEUP_BTN_MASK) { /* button only faling edge wakeup*/
        MATRX_ALL_LOW;
        KEY_EX_LOW;
        OM_PMU->PAD_PU_CTRL_1 = 0x02000001;  // 睡眠只留P20(MODE_24G)30K, P34(CHR_OK)150K; P21空脚输出低不上拉(原0x05漏电)

        drv_gpio_set_trig(OM_GPIO0, (1<<KEY_R0_PIN)|(1<<KEY_R1_PIN)|(1<<KEY_R2_PIN)|(1<<KEY_R3_PIN)| \
                                    (1<<KEY_R4_PIN)|(1<<KEY_R5_PIN)|(1<<KEY_R6_PIN)|(1<<KEY_R7_PIN), \
                                     GPIO_TRIG_FALLING_EDGE);
        drv_pmu_wakeup_pin_set(KEY_R0_PIN, PMU_PIN_WAKEUP_TYPE_FALL);
        drv_pmu_wakeup_pin_set(KEY_R1_PIN, PMU_PIN_WAKEUP_TYPE_FALL);
        drv_pmu_wakeup_pin_set(KEY_R2_PIN, PMU_PIN_WAKEUP_TYPE_FALL);
        drv_pmu_wakeup_pin_set(KEY_R3_PIN, PMU_PIN_WAKEUP_TYPE_FALL);
        drv_pmu_wakeup_pin_set(KEY_R4_PIN, PMU_PIN_WAKEUP_TYPE_FALL);
        drv_pmu_wakeup_pin_set(KEY_R5_PIN, PMU_PIN_WAKEUP_TYPE_FALL);
        drv_pmu_wakeup_pin_set(KEY_R6_PIN, PMU_PIN_WAKEUP_TYPE_FALL);
        drv_pmu_wakeup_pin_set(KEY_R7_PIN, PMU_PIN_WAKEUP_TYPE_FALL);
        NVIC_SetPriority(WAKEUP_GPIO0_IRQn, RTE_GPIO0_IRQ_PRIORITY);
        NVIC_EnableIRQ(WAKEUP_GPIO0_IRQn);
    }
    else {
        drv_pmu_wakeup_pin_set(KEY_R0_PIN, PMU_PIN_WAKEUP_TYPE_DISABLE);
        drv_pmu_wakeup_pin_set(KEY_R1_PIN, PMU_PIN_WAKEUP_TYPE_DISABLE);
        drv_pmu_wakeup_pin_set(KEY_R2_PIN, PMU_PIN_WAKEUP_TYPE_DISABLE);
        drv_pmu_wakeup_pin_set(KEY_R3_PIN, PMU_PIN_WAKEUP_TYPE_DISABLE);
        drv_pmu_wakeup_pin_set(KEY_R4_PIN, PMU_PIN_WAKEUP_TYPE_DISABLE);
        drv_pmu_wakeup_pin_set(KEY_R5_PIN, PMU_PIN_WAKEUP_TYPE_DISABLE);
        drv_pmu_wakeup_pin_set(KEY_R6_PIN, PMU_PIN_WAKEUP_TYPE_DISABLE);
        drv_pmu_wakeup_pin_set(KEY_R7_PIN, PMU_PIN_WAKEUP_TYPE_DISABLE);
        NVIC_DisableIRQ(WAKEUP_GPIO0_IRQn);

        OM_PMU->PAD_PU_CTRL_1 = 0x020AA821;
        MATRX_ALL_IN;
    }

    OM_CRITICAL_END();
}

/******************************************************************************
* 函数名称: charge_only_process()
* 功能描述: 充电位处理：不做有线键盘(USB 不枚举)，仅充电 + 电量显示
* 输入参数: 无
* 输出参数: 无
* 说    明: USB 枚举只在 mouse_usb_init() 里使能，此处不调用即等于去掉
*           有线功能；充电由硬件 5V 直充，与协议栈无关。
*           插线时由 5V 供电，轮询刷新 54E 充电/电量显示；未插线时发关屏
*           帧后 STOP1，唤醒源 = 按键 / 模式开关两脚上升沿 / USB 5V 插入
*           上升沿，醒来刷新显示后继续睡。拨离充电位立即复位重新分派模式。
******************************************************************************/
#define iCHG_USB5V_PIN         (3*8+5)         // USB 5V 检测脚，同 USB_5V_PIN

void charge_only_process(void)
{
    uint32_t rtc_temp;

    device_led_off();                       // 无线模式灯不亮
    logo_off();                             // 背光关(PWM恒灭)
    exio_ctrl = 0;                          // 54E 保持工作态，显示电量
    exled_rtc_count = 0;

    while(1)
    {
        drv_wdt_keep_alive(OM_WDT);

        if(MODE_DATA & ((1u<<MODE_BT_PIN)|(1u<<MODE_24G_PIN)))
        {                                   // 拨离充电位：复位重新分派模式
            NVIC_SystemReset();
        }

        if(0 != USB_5V_PIN)
        {                                   // 充电中：5V 供电，轮询刷新显示
            exled |= bled_WIR;              // 充电档点亮 Wir(关屏帧会清 exled, 每次刷新前补上)
            led_hint_process();             // 电量%/充电图标/低电提示(lvd)

            rtc_temp = drv_rtc_read(OM_RTC);
            if((mMS_TO_TICK(500) < (rtc_temp-exled_rtc_count))||(0 == exled_rtc_count))
            {
                exled_rtc_count = rtc_temp;
                exLogo_send();
            }

            delayus(20000);
        }
        else
        {                                   // 未插线：关屏，STOP1 等事件唤醒
            exled_ctrl = 0x02;              // 54E 关屏帧
            exLogo_send();

            mouse_periph_wakeup_enable(WAKEUP_SLEEP_LEVEL2);
            drv_gpio_set_trig(OM_GPIO0, (1u<<MODE_BT_PIN)|(1u<<MODE_24G_PIN)|(1u<<iCHG_USB5V_PIN),
                              GPIO_TRIG_RISING_EDGE);
            drv_pmu_wakeup_pin_set(MODE_BT_PIN, PMU_PIN_WAKEUP_TYPE_RISE);
            drv_pmu_wakeup_pin_set(MODE_24G_PIN, PMU_PIN_WAKEUP_TYPE_RISE);
            drv_pmu_wakeup_pin_set(iCHG_USB5V_PIN, PMU_PIN_WAKEUP_TYPE_RISE);
            NVIC_ClearPendingIRQ(WAKEUP_GPIO0_IRQn);
            NVIC_SetPriority(WAKEUP_GPIO0_IRQn, RTE_GPIO0_IRQ_PRIORITY);
            NVIC_EnableIRQ(WAKEUP_GPIO0_IRQn);

            drv_pmu_set_low_power_mode(PMU_LPM_STOP1);

            OM_CRITICAL_BEGIN();            // 醒来：清唤醒标志，恢复54E工作态
            drv_pmu_clear_wakeup();
            OM_CRITICAL_END();
            exio_ctrl = 0;
            exled_rtc_count = 0;
        }
    }
}

/******************************************************************************
* 函数名称: main()
* 功能描述: 主程序
* 输入参数: 无
* 输出参数: 无
* 说    明:
******************************************************************************/
int main(void)
{
    /* Initializing RCC */
    drv_rcc_init();

    drv_wdt_init(OM_WDT);                       // A3版本芯片看门狗常开
    drv_wdt_keep_alive(OM_WDT);

    /* 获取蓝牙地址(2.4G同步地址），生产时烧录 */
    drv_flash_read(CFG_START_SYNC_WORD_ADDR, kb_bd_addr+2, 4);
    kb_bd_addr[0] = iVERSION_ID_L;
    kb_bd_addr[1] = iVERSION_ID_H;
#if (0)
    kb_bd_addr[2]=0x02;                      // 调试使用，固定ID，在接收器也固定ID
    kb_bd_addr[3]=0x02;                      // 直接通信避免串码
    kb_bd_addr[4]=0x02;
#endif

    #if (RTE_ICACHE)
    drv_icache_enable();
    #endif

    OM_RCC->CLKGATE |= RCC_DEV_CLK_GATE_GPIO_CLK_GATE_MASK;
//------------------------------------------------------------------------------
// 不同个案，需要根据实际原理图设置初始化IO状态
    OM_PMU->PAD_PU_CTRL = 0x00AAAA00;       // KEY_R0~R7(P04~P13)行输入150K上拉, 0x00A80A00误删R2/R3/R4
    OM_PMU->PAD_PU_CTRL_1 = 0x020AA821;     // 上拉编码 01=30K 10=150K: P20 30K, P22 150K, P25~P27/P30/P31 列线150K, P34(CHR_OK) 150K
                                            // P35(USB_5V)/P36(蓝牙档) 无上下拉, 由外部电路决定电平
                                            // P21/P24 输出低, 不加上拉(否则 P21 30K 漏电约110uA, P24 150K 约22uA)
    OM_PMU->PAD_PD_CTRL = 0x0000000C;       // 只留 P02/P03(USB D-/D+)下拉; P34 改上拉
    OM_PMU->PAD_BUF_EN &= ~LVD_PIN_MASK;
    OM_PMU->PAD_BUF_EN |= (1<<MODE_BT_PIN | 1<<MODE_24G_PIN);

    OM_GPIO0->DATAOUT = 0xFFE5FFFF;           
    OM_GPIO0->OUTENSET = 0x841EA003;          
    OM_GPIO0->OUTENCLR = ~0x841EA003;         
//------------------------------------------------------------------------------
    device_type = iDEVICE_TYPE_ERR;

                                                // ???????????????????????
#if (!iDEBUG_SWD_OPEN)
    OM_RCC->CLKGATE &= ~RCC_DEV_CLK_GATE_SWD_CLK_GATE_MASK; // disable jtag
#endif
    kb_led_status = 0;
    mouse_periph_wakeup_enable(WAKEUP_ALL_DISABLE);
    if(RCC_RST_TYPE_DEEP_SLEEP == (OM_RCC->RSTCTRL & 0x07))
    {
        if(OM_PMU->SW_RSVD1 & PMU_SW_RSVD1_POWER_OFF)
        {                                       // 低压关机
            while(1)
            {                                   // 目前设置的低压关机之后，电量灯闪一下再关机，
                if(0 != USB_5V_PIN)             // 检测到充电的时候，退出低压关机状态
                    break;

                exled_ctrl = 0;                 // 外挂显示屏的时候，用显示屏提示，否则用低压灯提示
                exled = 0;
                exled |= bled_BAT;              // 低压关机: 电池图标闪一下
                exio_ctrl = 0;
                exLogo_send();
                delayus(500000);
                exled_ctrl = 0x02;
                exled = 0;
                exLogo_send();

                mouse_periph_wakeup_enable(WAKEUP_SLEEP_LEVEL2);
                drv_pmu_set_low_power_mode(PMU_LPM_DEEP_SLEEP);
            }
            OM_PMU->SW_RSVD1 &= ~PMU_SW_RSVD1_POWER_OFF;
        }
    }

    ble_24g_ctrl &= ~bom6239_sync_ok;
    rtc_init_value = 0;
    exled_rtc_count=0;
    exio_ctrl = 0;
    exled = 0;
    exLogo_send();                              // 初始化唤醒外挂芯片

    bat_init();

    laod_flash_cfg();
    scan_mode_swtich(iMODE_CHECK_POWERUP);

    memset(matrix_status, 0x00, iMATRIX_COL_NUM);
    memset(key_fn_status, 0x00, 128);

    #if(RF_RATE_2MHz==RF_RATE)                  // 由于测试模式有1M，2M的情况，RF初始化采用标志位判断的方式确定速率
        brf_rate = true;
    #else
        brf_rate = false;
    #endif

    if((RCC_RST_TYPE_POWER_ON == (OM_RCC->RSTCTRL & 0x07))
        || (iSCRATCH_BOOT_POWER == OM_PMU->SW_RSVD7))
    { // 上电进入
        OM_RCC->CLKGATE |= RCC_DEV_CLK_GATE_SWD_CLK_GATE_MASK; // 打开SWD调试口时钟
#if (iDEBUG_SWD_OPEN)
        delayus(1000000);                     // 调试的时候一定需要打开，并且时间设置1s以上，不然芯片下载不了程序
#else
        delayus(50000);
#endif

        sys_ctrl = 0;
        OM_PMU->SW_RSVD7 = 0;
#if (!iDEBUG_SWD_OPEN)
        OM_RCC->CLKGATE &= ~RCC_DEV_CLK_GATE_SWD_CLK_GATE_MASK; // disable jtag
#endif

        //OM_PMU->SW_RSVD1 = 0;
        OM_PMU->SW_RSVD1 = (device_type&PMU_SW_RSVD1_RF_MDOE_MASK);
        //OM_PMU->SW_RSVD1 |= iDEVICE_TYPE_BT0; // 调试使用
        //OM_PMU->SW_RSVD1 |= iDEVICE_TYPE_USB;
        //OM_PMU->SW_RSVD1 |= iDEVICE_TYPE_24G;
        OM_PMU->SW_RSVD1 |= (0<<8);
        OM_PMU->SW_RSVD1 |= ((0&0x0F)<<24);

        OM_PMU->SW_RSVD2 = iSEARCH_SYNC_WORD;
        sync_word = iSEARCH_SYNC_WORD;
        rf_ctrl_reg = (bsearch_dongle|brf_sync_time|brf_sync_status);

        sys_tick_count = 0;
        ble_24g_ctrl &= ~bom6239_conning;
        OM_PMU->SW_RSVD1 &= ~RSVD1_RF_CONN_EN;
    }
    else
    {  // 休眠唤醒进入
soft_reset_process:
        sys_ctrl |= bmcu_reset;

        ble_24g_ctrl &= ~bom6239_conning;
        if(OM_PMU->SW_RSVD1 & RSVD1_RF_CONN_EN)
            ble_24g_ctrl |= bom6239_conning;
        OM_PMU->SW_RSVD1 &= ~RSVD1_RF_CONN_EN;

        if(iDEVICE_TYPE_24G == device_type)
        {
            if(iSEARCH_SYNC_WORD == OM_PMU->SW_RSVD2)
            {
                rf_ctrl_reg |= (bsearch_dongle|brf_sync_time|brf_sync_status);
                sync_word = iSEARCH_SYNC_WORD;
            }
            else
            {
                rf_ctrl_reg |= (brf_sync_time|brf_sync_status);
                sync_word = OM_PMU->SW_RSVD2;
                rf_freq_make();
            }
        }
    }

    while(1)
    {
        if(0 == (MODE_DATA & ((1u<<MODE_BT_PIN)|(1u<<MODE_24G_PIN))))
        {                                   // 充电位：去掉有线键盘功能，只充电
            charge_only_process();          // 不返回，拨离充电位时内部复位
        }

        sys_tick_count = 0;
        bmode_swtich_valid = false;
        rtc_init();
        b24g_nromal = false;
        flash_load_ble_addr();
        device_led_on();

        key_ctrl &= ~bwin_mac_sataus;           // 目前品旭的要求系统与模式强关联，需要先加载模式，再设置系统类型
        if(sys_mode&(1<<(device_type&0x07)))
            key_ctrl |= bwin_mac_sataus;

        if(iDEVICE_TYPE_USB == device_type)
        {
            mouse_usb_init();
            main_usb();
        }
        else if(iDEVICE_TYPE_24G == device_type)
        {
            mMCU_MODE_NORMAL;
            mouse_24g_init();
            main_24g();
        }
        else //if(iDEVICE_TYPE_BT2 >= device_type)
        {
            OM_RCC->CLKCTRL = RCC_DIV_CLK_SEL_4;
            __NOP();__NOP();__NOP();__NOP();    // 蓝牙跑12M，降低系统功耗

            device_type &= 0x03;
            evt_init();

            if(0xFF == ble_id[device_type])
            {
                ble_24g_ctrl |= bom6239_conning;

                flash_read(iFLASH_BLE_ROLL_ADDR, 4);
                ble_id[device_type] = flash_fifo[device_type+1];
            }

            if(ble_24g_ctrl&bom6239_conning)
            {                                   // 蓝牙配对
                ble_24g_ctrl &= ~bom6239_conn_ok;
                ble_id[device_type]++;
                if(0xFF == ble_id[device_type])
                    ble_id[device_type]++;
            }
            kb_bd_addr[4] = ble_id[device_type];
            kb_bd_addr[5] &= 0xFC;
            kb_bd_addr[5] |= device_type;

            ex2_app_mouse_ble_init();
            while(1)
            {
                evt_schedule();
                if(true == bmode_swtich_valid)
                {
                    ex2_hal_uninit();
                    break;                      // 切换模式，返回主程序
                }

                OM_CRITICAL_BEGIN();
                // if no event, do power manage
                if (evt_get_all() == 0U)
                {
                    drv_pmu_set_low_power_mode(pm_checker_callback());
                }
                OM_CRITICAL_END();
            }
        }
    }
}

/** @} */
