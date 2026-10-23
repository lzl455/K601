/**
 * @file exLed.h
 * @author cfn
 * @brief HM-1515CWB-1 三位数码管 + 博仪创6PIN驱动板 UART 协议
 * @version 0.2
 * @date 2025-10-22
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __EX_LED_
#define __EX_LED_

//-----------------------------------------------------------------------------;
// UART 帧格式(博仪创 6PIN 数码管驱动说明书 V1.2.3, 115200 8N1)
//
// BYTE0-1 = 0x00 0xAA 帧头
// BYTE2   = 模式: 0x00共阴 0x01共阳 0x02关机(驱动板延时1s休眠)
// BYTE3-6 = DIG1~DIG4 段码, 需要点亮的位为1, A=bit0..G=bit6, H(小数点)=bit7
// BYTE7   = DIG5 驱动板MCU脚位电平, 为1输出高:
//           bit0=PB1(脚14) bit1=PB2(脚15) bit2=PB3(脚16) bit3=PB4(脚1)
//           bit4=PB5(脚2)  bit5=PA0(脚3)  bit6=PA1(脚4)
// BYTE8   = SUM = 0x100 - (BYTE2..BYTE7 之和的低8位)
//
// HM-1515CWB-1 只有3位数码管(无图标灯), 物理排列(左->右): DIG2 DIG3 DIG1,
// DIG1(最右位)带 H 小数点。DIG4 本模块无对应。
//
#define CMD_HEARDER_0           0x00
#define CMD_HEARDER_1           0xAA
#define CMD_CS_SUM              0x00

extern uint8_t                  exled_ctrl;
//{
    #define bdisp_MODE_GONG_YIN 0x00                // 共阴(现用, 字库用共阴表)
    #define bdisp_MODE_GONG_YANG 0x01               // 共阳(若显示反了改用此模式+共阳字库)
    #define bdisp_MODE_OFF       0x02               // 关机休眠帧
//}

extern uint32_t                 exled;
//{
    // exled 与帧 DIG 字节一一对应: bit0-7=DIG1 bit8-15=DIG2 bit16-23=DIG3 bit24-31=DIG4
    #define bled_DIG1_SHIFT      0                   // DIG1 最右位 = 个位
    #define bled_DIG2_SHIFT      8                   // DIG2 最左位 = 百位
    #define bled_DIG3_SHIFT      16                  // DIG3 中间位 = 十位

    #define bled_DIG1_MASK       0x000000FFul
    #define bled_DIG2_MASK       0x0000FF00ul
    #define bled_DIG3_MASK       0x00FF0000ul
    #define bled_DIG_ALL         0x00FFFFFFul

    #define bled_dig1_A          (1u<<(bled_DIG1_SHIFT+0))
    #define bled_dig1_B          (1u<<(bled_DIG1_SHIFT+1))
    #define bled_dig1_C          (1u<<(bled_DIG1_SHIFT+2))
    #define bled_dig1_D          (1u<<(bled_DIG1_SHIFT+3))
    #define bled_dig1_E          (1u<<(bled_DIG1_SHIFT+4))
    #define bled_dig1_F          (1u<<(bled_DIG1_SHIFT+5))
    #define bled_dig1_G          (1u<<(bled_DIG1_SHIFT+6))
    #define bled_dig1_H          (1u<<(bled_DIG1_SHIFT+7))    // 小数点

    #define bled_dig2_A          (1u<<(bled_DIG2_SHIFT+0))
    #define bled_dig2_B          (1u<<(bled_DIG2_SHIFT+1))
    #define bled_dig2_C          (1u<<(bled_DIG2_SHIFT+2))
    #define bled_dig2_D          (1u<<(bled_DIG2_SHIFT+3))
    #define bled_dig2_E          (1u<<(bled_DIG2_SHIFT+4))
    #define bled_dig2_F          (1u<<(bled_DIG2_SHIFT+5))
    #define bled_dig2_G          (1u<<(bled_DIG2_SHIFT+6))

    #define bled_dig3_A          (1u<<(bled_DIG3_SHIFT+0))
    #define bled_dig3_B          (1u<<(bled_DIG3_SHIFT+1))
    #define bled_dig3_C          (1u<<(bled_DIG3_SHIFT+2))
    #define bled_dig3_D          (1u<<(bled_DIG3_SHIFT+3))
    #define bled_dig3_E          (1u<<(bled_DIG3_SHIFT+4))
    #define bled_dig3_F          (1u<<(bled_DIG3_SHIFT+5))
    #define bled_dig3_G          (1u<<(bled_DIG3_SHIFT+6))
//}
extern uint8_t                  exio_ctrl;
//{
    #define bpwm_off            (1<<6)              // DIG5 bit6 = 驱动板 PA1
//}

//-----------------------------------------------------------------------------;
// EXTERN FUNCTION DECLARATION
//
extern void exLogo_send(void);

#endif  /* __EX_LED_ */
