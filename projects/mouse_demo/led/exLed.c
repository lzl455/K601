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
uint8_t                         exio_ctrl;

//-----------------------------------------------------------------------------;
// LOCAL Table 
//
#define rgb_54e_uart_h()        (OM_GPIO0->DATAOUTSET = (1<<UART_54E_PIN))
#define rgb_54e_uart_l()        (OM_GPIO0->DATAOUTCLR = (1<<UART_54E_PIN))

//-----------------------------------------------------------------------------;
// LOCAL FUNCTION 
//
//   54E 串口 115200 8N1, 每位 8.68us。位宽 = uart_delay() + exLogo_send_byte() 每位约 8.5 周期开销,
//   uart_delay() 约 5*time+10 周期(Keil armclang 6.16 生成: nop/subs/lsls/bne 每圈 5 周期)。
//   48MHz(上电/2.4G/充电档): 需 416.7 周期 -> time=80, 8.71us(+0.4%)
//   12MHz(蓝牙):             需 104.2 周期 -> time=17, 8.52us(-1.8%)
//   原值 64/14 位宽只有约 7.0~7.3us(快 16%~19%), 54E 收不到正确的帧, 数码管不亮。
//   改动 exLogo_send_byte()/uart_delay() 或编译选项后需用示波器复核位宽。
__RAM_CODE void uart_delay(void)
{
    uint8_t time= 80;

    if(iDEVICE_TYPE_BT2 >= device_type)
        time= 17;
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
    uint8_t cs;

    if(0x02 == exled_ctrl)
    {
        exled = 0;
        exio_ctrl = 0xFF;
    }

    OM_GPIO0->OUTENSET = (1<<UART_54E_PIN);
    exLogo_send_byte(CMD_HEARDER_0);
    exLogo_send_byte(CMD_HEARDER_1);
    exLogo_send_byte(exled_ctrl);
    cs = exled_ctrl;
    exLogo_send_byte((exled>>0)&0xFF);
    cs += ((exled>>0)&0xFF);
    exLogo_send_byte((exled>>8)&0xFF);
    cs += ((exled>>8)&0xFF);
    exLogo_send_byte((exled>>16)&0xFF);
    cs += ((exled>>16)&0xFF);
    exLogo_send_byte((exled>>24)&0xFF);
    cs += ((exled>>24)&0xFF);
    exLogo_send_byte(~exio_ctrl);
    cs += ~exio_ctrl;
    cs = 0-cs;
    exLogo_send_byte(cs);
    exled_ctrl = 0;
}
