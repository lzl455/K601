/**
 * @file exLed.h
 * @author cfn
 * @brief HM-1515CWB-1 三位数码管 + 博仪创6PIN驱动板 UART 协议
 * @version 0.3
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
// BYTE3-6 = DIG1~DIG4 灯号位图, 需要点亮的位为1(见下方灯号说明)
// BYTE7   = DIG5 驱动板MCU脚位电平, 为1输出高:
//           bit0=PB1(脚14) bit1=PB2(脚15) bit2=PB3(脚16) bit3=PB4(脚1)
//           bit4=PB5(脚2)  bit5=PA0(脚3)  bit6=PA1(脚4)
// BYTE8   = SUM = 0x100 - (BYTE2..BYTE7 之和的低8位)
//
// 驱动板按"灯号"驱动 30 颗 LED(6 脚交叉复用), DIGn 字节不是"第 n 位数码管",
// 而是灯号: DIG1 bit0..7 = 灯1..8, DIG2 = 灯9..16, DIG3 = 灯17..24, DIG4 = 灯25..30。
// 灯号 = 驱动说明书"共阴数码管"图中 (阳极脚, 阴极脚) 的组合。
//
// 对照 HM-1515CWB-1 电原理图(阳极脚->阴极脚), 物理排列(左->右) DIG1 DIG2 DIG3:
//   屏 DIG1  A~H : 1-2 1-3 1-4 1-5 1-6 2-1 2-3 2-4 = 灯1~8   (规格书 F 标 "2-2" 为笔误)
//                  其中 B1/C1 组成百位"1", A1/D1/E1/F1/G1/H1 为独立图标
//   屏 DIG2  A~G : 2-5 2-6 3-1 3-2 3-4 3-5 3-6     = 灯9~15  (十位)
//   屏 DIG3  A~G : 4-1 4-2 4-3 4-5 4-6 5-1 5-2     = 灯16~22 (个位, 跨 DIG2.bit7 和 DIG3)
//   灯23~30 屏上无对应。
//
extern uint8_t                  exled_ctrl;
//{
    #define bdisp_MODE_GONG_YIN 0x00                // 共阴(现用, 字库用共阴表)
    #define bdisp_MODE_GONG_YANG 0x01               // 共阳(若显示反了改用此模式+共阳字库)
    #define bdisp_MODE_OFF       0x02               // 关机休眠帧
//}

extern uint32_t                 exled;
//{
    // exled bit(n-1) = 灯n, 发送时按字节拆成 DIG1~DIG4
    #define bled_DIG1_SHIFT      0                   // 屏 DIG1: 百位"1"(B1/C1) + 图标
    #define bled_DIG2_SHIFT      8                   // 屏 DIG2: 十位
    #define bled_DIG3_SHIFT      15                  // 屏 DIG3: 个位(灯16起, 不是字节对齐)

    #define bled_DIG1_MASK       0x000000FFul
    #define bled_DIG2_MASK       0x00007F00ul
    #define bled_DIG3_MASK       0x003F8000ul
    #define bled_DIG_ALL         0x003FFFFFul

    #define bled_dig1_A          (1u<<(bled_DIG1_SHIFT+0))    // 图标 A1
    #define bled_dig1_B          (1u<<(bled_DIG1_SHIFT+1))    // 百位"1"
    #define bled_dig1_C          (1u<<(bled_DIG1_SHIFT+2))    // 百位"1"
    #define bled_dig1_D          (1u<<(bled_DIG1_SHIFT+3))    // 图标 D1
    #define bled_dig1_E          (1u<<(bled_DIG1_SHIFT+4))    // 图标 E1
    #define bled_dig1_F          (1u<<(bled_DIG1_SHIFT+5))    // 图标 F1
    #define bled_dig1_G          (1u<<(bled_DIG1_SHIFT+6))    // 图标 G1
    #define bled_dig1_H          (1u<<(bled_DIG1_SHIFT+7))    // 图标 H1
    #define bled_hundred_1       (bled_dig1_B|bled_dig1_C)

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
