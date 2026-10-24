/*******************************************************************************
* Copyright (c) 2013, SHENZHEN SEMITEK Co.,LTD.
* All rights reserved.
*
* 文件名称: _MATRIX_KEY_TBL_
*
* 功能说明:
* 修改说明:
*******************************************************************************/
#ifndef _MATRIX_KEY_WIN_FN_TBL_
#define _MATRIX_KEY_WIN_FN_TBL_

//=============================================================================;
// 配置引用区                                                                  ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// FN按下，未锁定
//

// fn 键值表
unsigned char           const   matrixkey_win_fn_tbl[] =
{
// C0
    iSTD_PAUSE,          iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iLEFT_SHIFT,      // C15 R1
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_SHIFT,     // C15 R2
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_CTRL,
    iMMK_CALCULATOR_L,   iKEY_TYPE_MMK | iMMK_CALCULATOR_H,    // C17 R5 计算器(Calculator)
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iLEFT_CTRL,
    iMMK_PREV_TRACK_L,  iKEY_TYPE_MMK | iMMK_PREV_TRACK_H,     // FN+F5 上一首
// C1
    iSTD_Q,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_TAB,            iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_A,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_ESCAPE,         iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_Z,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_WAVE,           iKEY_TYPE_STD|iKEY_PRESS,
    iMODE_KEY_BT0,        iKEY_TYPE_SPECIAL,
// C2
    iSTD_W,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_CAPITAL,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_S,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_X,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iMMK_MEDIA_SELECT_L, iKEY_TYPE_MMK | iMMK_MEDIA_SELECT_H,  // FN+F1 音乐
    iMODE_KEY_BT1,        iKEY_TYPE_SPECIAL,
// C3
    iSTD_E,              iKEY_TYPE_STD|iKEY_PRESS,
    iMMK_VOLUMEUP_L,    iKEY_TYPE_MMK | iMMK_VOLUMEUP_H,       // FN+F3 音量+
    iSTD_D,              iKEY_TYPE_STD|iKEY_PRESS,
    iMMK_MUTE_L,        iKEY_TYPE_MMK | iMMK_MUTE_H,           // FN+F4 静音
    iSTD_C,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iMMK_VOLUMEDOWN_L,  iKEY_TYPE_MMK | iMMK_VOLUMEDOWN_H,     // FN+F2 音量-
    iMODE_KEY_24G,        iKEY_TYPE_SPECIAL,
// C4
    iSTD_R,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_T,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_G,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_V,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_B,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_5,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_4,              iKEY_TYPE_STD|iKEY_PRESS,
// C5
    iSTD_U,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_Y,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_J,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_H,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_M,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_N,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_6,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_7,              iKEY_TYPE_STD|iKEY_PRESS,
// C6
    iSTD_I,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_SQUARE_BRACKET_RIGHT, iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_K,              iKEY_TYPE_STD|iKEY_PRESS,
    iMMK_NEXT_TRACK_L,  iKEY_TYPE_MMK | iMMK_NEXT_TRACK_H,     // FN+F6 下一首
    iSTD_COMMA,          iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_KEYBOARD_RO,    iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_ADD,            iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_8,              iKEY_TYPE_STD|iKEY_PRESS,
// C7
    iKEY_MAC,            iKEY_TYPE_SPECIAL,
    iMMK_PLAY_PAUSE_L,  iKEY_TYPE_MMK | iMMK_PLAY_PAUSE_H,     // FN+F7 播放/暂停
    iSTD_L,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_FULLSTOP,       iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_APP,            iKEY_TYPE_STD|iKEY_PRESS,
    iMMK_STOP_L,        iKEY_TYPE_MMK | iMMK_STOP_H,           // FN+F8 停止
    iSTD_9,              iKEY_TYPE_STD|iKEY_PRESS,
// C8
    iSTD_P,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_SQUARE_BRACKET_LEFT, iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_SEMICOLON,      iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_QUOTATION_SIGN, iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_SLASH,          iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_SUBTRACT,       iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_0,              iKEY_TYPE_STD|iKEY_PRESS,
// C9
    iSTD_SCROLL,         iKEY_TYPE_STD|iKEY_PRESS,
    iKEY_WIN_LOCK,      iKEY_TYPE_SPECIAL,                     // FN+WIN 锁WIN (C16 R1 左WIN)
    iMODE_KEY_FN,        iKEY_TYPE_SPECIAL,                         // iKEY_INDEX_FN
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iLEFT_ALT,
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_GUI,       // C17 R2
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_ALT,
    iMMK_WWW_SEARCH_L,   iKEY_TYPE_MMK | iMMK_WWW_SEARCH_H,    // C16 R7 网页搜索(WWW Search)
    iSTD_SNAPSHOT,       iKEY_TYPE_STD|iKEY_PRESS,
// C10
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_BACKSPACE,      iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_BACKSLASH,      iKEY_TYPE_STD|iKEY_PRESS,
    iMMK_MY_COMPUTER_L, iKEY_TYPE_MMK | iMMK_MY_COMPUTER_H,    // FN+F11 我的电脑
    iSTD_ENTER,          iKEY_TYPE_STD|iKEY_PRESS,
    iMMK_WWW_FAVORITES_L, iKEY_TYPE_MMK | iMMK_WWW_FAVORITES_H, // FN+F12 收藏
    iMMK_WWW_HOME_L,    iKEY_TYPE_MMK | iMMK_WWW_HOME_H,       // FN+F9 浏览器
    iMMK_MAIL_L,        iKEY_TYPE_MMK | iMMK_MAIL_H,           // FN+F10 邮件
// C11
    iSTD_NUMPAD7,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD4,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD1,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_SPACE,          iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMLOCK,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_DOWN,           iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_DELETE,         iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
// C12
    iSTD_NUMPAD8,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD5,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD2,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD0,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD_DIVIDE,  iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_RIGHT,          iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_INSERT,         iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
// C13
    iSTD_NUMPAD9,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD6,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD3,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD_FULLSTOP, iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD_MULTIPLY, iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NUMPAD_SUBTRACT, iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_PAGEUP,         iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_PAGEDOWN,       iKEY_TYPE_STD|iKEY_PRESS,
// C14
    iSTD_NUMPAD_ADD,     iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_NUMPAD_ENTER,   iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_UP,             iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_LEFT,           iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_HOME,           iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_END,            iKEY_TYPE_STD|iKEY_PRESS,
};
//-----------------------------------------------------------------------------;

#endif
