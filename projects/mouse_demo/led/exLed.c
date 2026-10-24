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

//-----------------------------------------------------------------------------;
// LOCAL FUNCTION 
//
__RAM_CODE void uart_delay(void)
{
    uint8_t time= 64;

    if(iDEVICE_TYPE_BT2 >= device_type)
        time= 14;
    do
    {
        __NOP();
    }
    while(--time);
}

__RAM_CODE static void exLogo_send_byte(uint8_t data)
{
    OM_CRITICAL_BEGIN();

    rgb_54e_uart_h();
    uart_delay();
    rgb_54e_uart_l();
    uart_delay();
    if(data&(1<<0))
        rgb_54e_uart_h();
    else
        rgb_54e_uart_l();
    uart_delay();

    if(data&(1<<1))
        rgb_54e_uart_h();
    else
        rgb_54e_uart_l();
    uart_delay();

    if(data&(1<<2))
        rgb_54e_uart_h();
    else
        rgb_54e_uart_l();
    uart_delay();

    if(data&(1<<3))
        rgb_54e_uart_h();
    else
        rgb_54e_uart_l();
    uart_delay();

    if(data&(1<<4))
        rgb_54e_uart_h();
    else
        rgb_54e_uart_l();
    uart_delay();

    if(data&(1<<5))
        rgb_54e_uart_h();
    else
        rgb_54e_uart_l();
    uart_delay();

    if(data&(1<<6))
        rgb_54e_uart_h();
    else
        rgb_54e_uart_l();
    uart_delay();

    if(data&(1<<7))
        rgb_54e_uart_h();
    else
        rgb_54e_uart_l();
    uart_delay();

    rgb_54e_uart_h();

    OM_CRITICAL_END();
    uart_delay();
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
    uint8_t cs;

    dig = exled;
    bdisp_off = false;
    if(0x02 == exled_ctrl)
    {                                           // 关机帧: 灯号全 0; exled 保留, 亮屏时原样恢复
        dig = 0;
        bdisp_off = true;
    }

    OM_GPIO0->OUTENSET = (1<<UART_54E_PIN);
    exLogo_send_byte(CMD_HEARDER_0);
    exLogo_send_byte(CMD_HEARDER_1);
    exLogo_send_byte(exled_ctrl);
    cs = exled_ctrl;
    exLogo_send_byte((dig>>0)&0xFF);
    cs += ((dig>>0)&0xFF);
    exLogo_send_byte((dig>>8)&0xFF);
    cs += ((dig>>8)&0xFF);
    exLogo_send_byte((dig>>16)&0xFF);
    cs += ((dig>>16)&0xFF);
    exLogo_send_byte((dig>>24)&0xFF);
    cs += ((dig>>24)&0xFF);
    exLogo_send_byte(CMD_DIG5);
    cs += CMD_DIG5;
    cs = 0-cs;
    exLogo_send_byte(cs);
    exled_ctrl = 0;
}
