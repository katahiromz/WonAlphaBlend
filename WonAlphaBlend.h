#pragma once

#ifndef _INC_WINDOWS
    #include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _WONBLENDFUNCTION {
    BYTE BlendOp;
    BYTE BlendFlags;
    BYTE SourceConstantAlpha;
    BYTE AlphaFormat;
} WONBLENDFUNCTION, *PWONBLENDFUNCTION;

#define WON_AC_SRC_OVER 0x00
#define WON_AC_SRC_ALPHA 0x01

BOOL APIENTRY
WonAlphaBlend(HDC hdc, INT x, INT y, INT cx, INT cy,
              HDC hdcSrc, INT xSrc, INT ySrc, INT cxSrc, INT cySrc,
              WONBLENDFUNCTION bf);

#if defined(_WONVER) && (_WONVER < 0x0500)
    #define AlphaBlend WonAlphaBlend
    #define GdiAlphaBlend WonAlphaBlend
    #define AC_SRC_OVER WON_AC_SRC_OVER
    #define AC_SRC_ALPHA WON_AC_SRC_ALPHA
#endif

#ifdef __cplusplus
} // extern "C"
#endif
