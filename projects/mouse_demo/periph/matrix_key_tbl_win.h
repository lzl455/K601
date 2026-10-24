/*******************************************************************************
* Copyright (c) 2013, SHENZHEN SEMITEK Co.,LTD.
* All rights reserved.
*
* 文件名称: _MATRIX_KEY_TBL_
*
* 功能说明:
* 修改说明:
*******************************************************************************/
#ifndef _MATRIX_KEY_WIN_TBL_
#define _MATRIX_KEY_WIN_TBL_

//=============================================================================;
// 配置引用区                                                                  ;
//=============================================================================;

//-----------------------------------------------------------------------------;
// 正常矩阵表
//

// WIN键值表
unsigned char           const   matrixkey_win_tbl[] =
{
// C0
    iSTD_PAUSE,          iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iLEFT_SHIFT,      // C15 R1
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_SHIFT,     // C15 R2
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_CTRL,
    iMMK_CALCULATOR_L,   iKEY_TYPE_MMK | iMMK_CALCULATOR_H,         // C17 R5 计算器(Calculator)
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iLEFT_CTRL,
    iSTD_F5,             iKEY_TYPE_STD|iKEY_PRESS,
// C1
    iSTD_Q,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_TAB,            iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_A,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_ESCAPE,         iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_Z,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_WAVE,           iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_1,              iKEY_TYPE_STD|iKEY_PRESS,
// C2
    iSTD_W,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_CAPITAL,        iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_S,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_X,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_F1,             iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_2,              iKEY_TYPE_STD|iKEY_PRESS,
// C3
    iSTD_E,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F3,             iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_D,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F4,             iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_C,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_F2,             iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_3,              iKEY_TYPE_STD|iKEY_PRESS,
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
    iSTD_F6,             iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_COMMA,          iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_KEYBOARD_RO,    iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_ADD,            iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_8,              iKEY_TYPE_STD|iKEY_PRESS,
// C7
    iSTD_O,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F7,             iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_L,              iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_FULLSTOP,       iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_APP,            iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F8,             iKEY_TYPE_STD|iKEY_PRESS,
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
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iLEFT_GUI,        // C16 R1
    iMODE_KEY_FN,        iKEY_TYPE_SPECIAL,                         // iKEY_INDEX_FN
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iLEFT_ALT,
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_GUI,       // C17 R2
    iSTD_NULL,           iKEY_TYPE_STD|iKEY_PRESS|iRIGHT_ALT,
    iMMK_WWW_SEARCH_L,   iKEY_TYPE_MMK | iMMK_WWW_SEARCH_H,         // C16 R7 网页搜索(WWW Search)
    iSTD_SNAPSHOT,       iKEY_TYPE_STD|iKEY_PRESS,
// C10
    iSTD_NULL,           iKEY_TYPE_INVALID,
    iSTD_BACKSPACE,      iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_BACKSLASH,      iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F11,            iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_ENTER,          iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F12,            iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F9,             iKEY_TYPE_STD|iKEY_PRESS,
    iSTD_F10,            iKEY_TYPE_STD|iKEY_PRESS,
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
