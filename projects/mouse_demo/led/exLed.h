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
// BYTE2   = 模式: 0x00共阴 0x01共阳 0x02关机(灭屏, 驱动板延时1s休眠)
// BYTE3-6 = DIG1~DIG4 灯号位图, 需要点亮的位为1(见下方灯号说明)
// BYTE7   = DIG5 驱动板MCU脚位电平, 为1输出高:
//           bit0=PB1(脚14) bit1=PB2(脚15) bit2=PB3(脚16) bit3=PB4(脚1)
//           bit4=PB5(脚2)  bit5=PA0(脚3)  bit6=PA1(脚4)
//           本板这些脚未接, 固定发 CMD_DIG5
// BYTE8   = SUM = 0x100 - (BYTE2..BYTE7 之和的低8位)
//
// 驱动板上电即灭灯休眠, 串口下降沿唤醒; 唤醒后 1s 内收不到正确的显示数据会再次休眠
// (驱动说明书"软件说明"第1条)。所以灭屏只能发关机帧, 亮屏时第一帧只当唤醒帧, 随后补发正式帧。
//
// 驱动板按"灯号"驱动 30 颗 LED(6 脚交叉复用), exled bit(n-1) = 灯n,
// 发送时按字节拆成 DIG1(灯1~8) DIG2(灯9~16) DIG3(灯17~24) DIG4(灯25~30)。
// 灯号 = 驱动说明书"共阴数码管"图中 (上方脚号, 下方脚号) 的组合(上方为阴极)。
//
// HM-1515CWB-1 电原理图(上方脚号-下方脚号, 方向与驱动板共阴图一致), 物理排列(左->右) DIG1 DIG2 DIG3:
//   屏 DIG1  A~H : 1-2 1-3 1-4 1-5 1-6 2-1 2-3 2-4   (规格书 F 标 "2-2" 为笔误)
//                  其中 B1/C1 组成百位"1", 其余为图标(按屏幕丝印):
//                  A1=2.4G D1=BT1 E1=BT2 F1=Wir(底排左->右) G1=% H1=电池(右上)
//   屏 DIG2  A~G : 2-5 2-6 3-1 3-2 3-4 3-5 3-6       (十位)
//   屏 DIG3  A~G : 4-1 4-2 4-3 4-5 4-6 5-1 5-2       (个位)
//
// 本板上屏的 1~6 脚与驱动板 PIN1~6 反接: 屏脚 k 接驱动板 PIN(7-k)。
// 2026-10-24 实测(docs/images/debug_*_20261024): 按"同号相接"映射发出" 60%"+BT1 的帧
// 00 AA 00 48 FD 1F 00 FF 9D, 屏上亮的是十位 B~G、个位 A~E+G、图标全灭, 与"反接"推算完全一致。
// 反接后屏上 a-b 两脚的段, 用驱动板 (7-a)-(7-b) 的灯号, 正好是 灯n -> 灯(31-n):
//   屏 DIG1  A~H = 灯30 29 28 27 26 25 24 23
//   屏 DIG2  A~G = 灯22 21 20 19 18 17 16       (十位)
//   屏 DIG3  A~G = 灯15 14 13 12 11 10  9       (个位)
//   灯1~8 屏上无对应。
//
#define CMD_HEARDER_0           0x00
#define CMD_HEARDER_1           0xAA
#define CMD_CS_SUM              0x00
#define CMD_DIG5                0xFF            // DIG5: 驱动板 PB1~PB5/PA0/PA1 本板未接, 固定输出高

extern uint8_t                  exled_ctrl;     // 置 0x02 再 exLogo_send() 发关机帧, 发完自动回 0x00
extern bool                     bdisp_off;      // true: 最后发出的是关机帧(屏灭)

extern uint32_t                 exled;
//{
    #define bled_24G            (1<<29)         // 灯30 A1 "2.4G"
    #define bled_1_B            (1<<28)         // 灯29 百位"1"
    #define bled_1_C            (1<<27)         // 灯28 百位"1"
    #define bled_BT1            (1<<26)         // 灯27 D1 "BT1"
    #define bled_BT2            (1<<25)         // 灯26 E1 "BT2"
    #define bled_WIR            (1<<24)         // 灯25 F1 "Wir"

    #define bled_PERCENT        (1<<23)         // 灯24 G1 "%"
    #define bled_BAT            (1<<22)         // 灯23 H1 电池图标(右上)
    #define bled_2_A            (1<<21)         // 灯22 十位(屏DIG2)
    #define bled_2_B            (1<<20)
    #define bled_2_C            (1<<19)
    #define bled_2_D            (1<<18)
    #define bled_2_E            (1<<17)
    #define bled_2_F            (1<<16)

    #define bled_2_G            (1<<15)         // 灯16
    #define bled_3_A            (1<<14)         // 灯15 个位(屏DIG3)
    #define bled_3_B            (1<<13)
    #define bled_3_C            (1<<12)
    #define bled_3_D            (1<<11)
    #define bled_3_E            (1<<10)
    #define bled_3_F            (1<<9)
    #define bled_3_G            (1<<8)          // 灯9
    //  bit0~7 (灯1~8)                          // 屏上无对应

    #define bled_DIGIT_ALL      (bled_1_B|bled_1_C| \
                                 bled_2_A|bled_2_B|bled_2_C|bled_2_D|bled_2_E|bled_2_F|bled_2_G| \
                                 bled_3_A|bled_3_B|bled_3_C|bled_3_D|bled_3_E|bled_3_F|bled_3_G)
    #define bled_DIGIT_100      (bled_1_B|bled_1_C| \
                                 bled_2_A|bled_2_B|bled_2_C|bled_2_D|bled_2_E|bled_2_F| \
                                 bled_3_A|bled_3_B|bled_3_C|bled_3_D|bled_3_E|bled_3_F)   // "1 0 0"
//}

//-----------------------------------------------------------------------------;
// EXTERN FUNCTION DECLARATION
//
extern void exLogo_send(void);

#endif  /* __EX_LED_ */
