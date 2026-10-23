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
// 驱动板按"灯号"驱动 30 颗 LED(6 脚交叉复用), exled bit(n-1) = 灯n,
// 发送时按字节拆成 DIG1(灯1~8) DIG2(灯9~16) DIG3(灯17~24) DIG4(灯25~30)。
// 灯号 = 驱动说明书"共阴数码管"图中 (阳极脚, 阴极脚) 的组合。
//
// 对照 HM-1515CWB-1 电原理图(阳极脚->阴极脚), 物理排列(左->右) DIG1 DIG2 DIG3:
//   屏 DIG1  A~H : 1-2 1-3 1-4 1-5 1-6 2-1 2-3 2-4 = 灯1~8   (规格书 F 标 "2-2" 为笔误)
//                  其中 B1/C1 组成百位"1", 其余为图标(按屏幕丝印):
//                  A1=2.4G D1=BT1 E1=BT2 F1=Wir(底排左->右) G1=% H1=电池(右上)
//   屏 DIG2  A~G : 2-5 2-6 3-1 3-2 3-4 3-5 3-6     = 灯9~15  (十位)
//   屏 DIG3  A~G : 4-1 4-2 4-3 4-5 4-6 5-1 5-2     = 灯16~22 (个位, 跨 DIG2.bit7 和 DIG3)
//   灯23~30 屏上无对应。
//
extern uint8_t                  exled_ctrl;

extern uint32_t                 exled;
//{
    //#define NULL                (1<<23)         // 灯24 屏上无对应
    //#define NULL                (1<<22)         // 灯23 屏上无对应
    #define bled_3_G            (1<<21)         // 个位(屏DIG3)
    #define bled_3_F            (1<<20)
    #define bled_3_E            (1<<19)
    #define bled_3_D            (1<<18)
    #define bled_3_C            (1<<17)
    #define bled_3_B            (1<<16)

    #define bled_3_A            (1<<15)
    #define bled_2_G            (1<<14)         // 十位(屏DIG2)
    #define bled_2_F            (1<<13)
    #define bled_2_E            (1<<12)
    #define bled_2_D            (1<<11)
    #define bled_2_C            (1<<10)
    #define bled_2_B            (1<<9)
    #define bled_2_A            (1<<8)

    #define bled_BAT            (1<<7)          // H1 电池图标(右上)
    #define bled_PERCENT        (1<<6)          // G1 "%"
    #define bled_WIR            (1<<5)          // F1 "Wir"
    #define bled_BT2            (1<<4)          // E1 "BT2"
    #define bled_BT1            (1<<3)          // D1 "BT1"
    #define bled_1_C            (1<<2)          // 百位"1"
    #define bled_1_B            (1<<1)          // 百位"1"
    #define bled_24G            (1<<0)          // A1 "2.4G"
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
