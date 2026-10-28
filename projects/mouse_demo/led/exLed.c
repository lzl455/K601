/**
 * @file exLogo.c
 * @author cfn
 * @brief 
 * @version 0.1
 * @date 2025-09-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "om_driver.h"

#include "main.h"
#include "time_delay.h"
#include "exLed.h"

//-----------------------------------------------------------------------------;
// LOCAL Variables 
//
uint8_t                         exled_ctrl;
uint32_t                        exled;
bool                            bdisp_off;

//-----------------------------------------------------------------------------;
// LOCAL Table 
//
#define rgb_54e_uart_h()        (OM_GPIO0->DATAOUTSET = (1<<UART_54E_PIN))
#define rgb_54e_uart_l()        (OM_GPIO0->DATAOUTCLR = (1<<UART_54E_PIN))

#define UART_54E_BAUD           115200

//-----------------------------------------------------------------------------;
// LOCAL FUNCTION
//
/*******************************************************************************
* 函数名称: exLogo_send_byte()
* 功能描述: P00 模拟串口发一个字节(115200 8N1, 低位先发)
* 输入参数: data: 要发的字节; bit_q8: 一位的 CPU 周期数(Q24.8)
* 说    明: 每个跳变沿都按 SysTick(随主频计数)定在第 k 位的准确时刻, 误差不随位数累积,
*           也和数据位是 0 还是 1 无关(原来的空循环计时在蓝牙 12MHz 下一圈约占一位的 6%,
*           分支长短还随数据位变, 误差逐位累积)。
*           帧形同原来: 起始位前空闲一位, 空闲位~停止位关中断, 停止位在开中断后保持一位
*******************************************************************************/
__RAM_CODE static void exLogo_send_byte(uint8_t data, uint32_t bit_q8)
{
    volatile uint32_t *dataout;
    uint32_t bits = ((uint32_t)data<<1)|(1u<<9);    // bit0 起始位(0), bit1~8 数据, bit9 停止位(1)
    uint32_t edge_q8 = 0;
    uint32_t target;
    uint32_t start;
    uint8_t i;

    OM_CRITICAL_BEGIN();
    start = SysTick->VAL;
    rgb_54e_uart_h();                               // 空闲位
    for(i=0; i<10; i++)
    {
        dataout = (bits&0x01) ? &OM_GPIO0->DATAOUTSET : &OM_GPIO0->DATAOUTCLR;
        bits >>= 1;
        edge_q8 += bit_q8;
        target = edge_q8>>8;
        while(((start-SysTick->VAL)&SysTick_VAL_CURRENT_Msk) < target);
        *dataout = (1<<UART_54E_PIN);               // 先算好写哪个寄存器, 到点只剩这一条写, 每个沿延迟相同
    }
    OM_CRITICAL_END();

    target = (edge_q8+bit_q8)>>8;                   // 停止位保持一位, 期间来中断只会拉长
    while(((start-SysTick->VAL)&SysTick_VAL_CURRENT_Msk) < target);
}

/**
 * @brief  
 * @note   
 * @param  mode_data: 
 * @retval None
 */
void exLogo_send(void)
{
    uint32_t dig;
    uint32_t bit_q8;
    uint8_t cs;

    dig = exled;
    bdisp_off = false;
    if(0x02 == exled_ctrl)
    {                                           // 关机帧: 灯号全 0; exled 保留, 亮屏时原样恢复
        dig = 0;
        bdisp_off = true;
    }

    if(((SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_ENABLE_Msk)
            != (SysTick->CTRL&(SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_TICKINT_Msk|SysTick_CTRL_ENABLE_Msk)))
        ||(SysTick_LOAD_RELOAD_Msk != SysTick->LOAD))
    {                                           // SystemInit() 已让 SysTick 按主频空转计数, 这里只是兜底: 不在计数的话发送会卡死
        SysTick->CTRL = 0;
        SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
        SysTick->VAL  = 0;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_ENABLE_Msk;
    }
                                                // 一位的 CPU 周期数(Q24.8): 主频 = 48MHz 按 CLKCTRL 分频
    bit_q8 = (SYS_CLK>>(OM_RCC->CLKCTRL&0x03))/(UART_54E_BAUD/256); // 2.4G/USB/充电档 48MHz: 416.7; 蓝牙 12MHz: 104.2

    OM_GPIO0->OUTENSET = (1<<UART_54E_PIN);
    exLogo_send_byte(CMD_HEARDER_0, bit_q8);
    exLogo_send_byte(CMD_HEARDER_1, bit_q8);
    exLogo_send_byte(exled_ctrl, bit_q8);
    cs = exled_ctrl;
    exLogo_send_byte((dig>>0)&0xFF, bit_q8);
    cs += ((dig>>0)&0xFF);
    exLogo_send_byte((dig>>8)&0xFF, bit_q8);
    cs += ((dig>>8)&0xFF);
    exLogo_send_byte((dig>>16)&0xFF, bit_q8);
    cs += ((dig>>16)&0xFF);
    exLogo_send_byte((dig>>24)&0xFF, bit_q8);
    cs += ((dig>>24)&0xFF);
    exLogo_send_byte(CMD_DIG5, bit_q8);
    cs += CMD_DIG5;
    cs = 0-cs;
    exLogo_send_byte(cs, bit_q8);
    exled_ctrl = 0;
}
