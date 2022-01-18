#include <windows.h>
#include "WonAlphaBlend.h"

BOOL APIENTRY
WonAlphaBlend(HDC hdc, INT x, INT y, INT cx, INT cy,
              HDC hdcSrc, INT xSrc, INT ySrc, INT cxSrc, INT cySrc,
              WONBLENDFUNCTION bf)
{
    BITMAPINFO bi;
    HBITMAP hbm, hbmSrc;
    LPBYTE pbBits, pbBitsSrc;
    LPBYTE pb, pbSrc;
    HGDIOBJ hbmOld, hbmSrcOld;
    HDC hdcMem, hdcMemSrc;
    INT i;
    BYTE b, g, r, a, alpha;

    if (cx < 0 || cy < 0 || cxSrc < 0 || cySrc < 0)
        return FALSE;

    if (bf.BlendOp != WON_AC_SRC_OVER)
        return FALSE;

    if (bf.SourceConstantAlpha == 0)
        return TRUE;

    SetStretchBltMode(hdc, COLORONCOLOR);
    if (bf.SourceConstantAlpha == 0xFF && bf.AlphaFormat == 0)
        return StretchBlt(hdc, x, y, cx, cy,
                          hdcSrc, xSrc, ySrc, cxSrc, cySrc, SRCCOPY);

    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));
    bi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth    = cx;
    bi.bmiHeader.biHeight   = cy;
    bi.bmiHeader.biPlanes   = 1;
    bi.bmiHeader.biBitCount = 32;
    hbm = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (VOID **)&pbBits,
                           NULL, 0);
    if (hbm != NULL)
    {
        hbmSrc = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS,
                                  (VOID **)&pbBitsSrc, NULL, 0);
        if (hbmSrc != NULL)
        {
            hdcMem = CreateCompatibleDC(NULL);
            if (hdcMem != NULL)
            {
                hdcMemSrc = CreateCompatibleDC(NULL);
                if (hdcMemSrc != NULL)
                {
                    goto success;
                }
                DeleteDC(hdcMem);
            }
            DeleteObject(hbmSrc);
        }
        DeleteObject(hbm);
    }
    return FALSE;

success:
    SetStretchBltMode(hdcMem, COLORONCOLOR);
    hbmOld = SelectObject(hdcMem, hbm);
    BitBlt(hdcMem, 0, 0, cx, cy, hdc, x, y, SRCCOPY);

    SetStretchBltMode(hdcMemSrc, COLORONCOLOR);
    hbmSrcOld = SelectObject(hdcMemSrc, hbmSrc);
    StretchBlt(hdcMemSrc, 0, 0, cx, cy,
               hdcSrc, xSrc, ySrc, cxSrc, cySrc, SRCCOPY);

    pb = pbBits;
    pbSrc = pbBitsSrc;
    if (bf.AlphaFormat == WON_AC_SRC_ALPHA)
    {
        if (bf.SourceConstantAlpha == 0xFF)
        {
            // SCA equals 0xFF
            for(i = 0; i < cx * cy; i++)
            {
                a = pbSrc[3];
                *pb++ = *pbSrc++ + ((UINT)(255 - a) * *pb) / 255;
                *pb++ = *pbSrc++ + ((UINT)(255 - a) * *pb) / 255;
                *pb++ = *pbSrc++ + ((UINT)(255 - a) * *pb) / 255;
                *pb++ = *pbSrc++ + ((UINT)(255 - a) * *pb) / 255;
            }
        }
        else
        {
            // the source has both the SCA and per-pixel alpha
            for(i = 0; i < cx * cy; i++)
            {
                b = (UINT)*pbSrc++ * bf.SourceConstantAlpha / 255;
                g = (UINT)*pbSrc++ * bf.SourceConstantAlpha / 255;
                r = (UINT)*pbSrc++ * bf.SourceConstantAlpha / 255;
                a = (UINT)*pbSrc++ * bf.SourceConstantAlpha / 255;
                *pb++ = (UINT)b + (255 - a) * *pb / 255;
                *pb++ = (UINT)g + (255 - a) * *pb / 255;
                *pb++ = (UINT)r + (255 - a) * *pb / 255;
                *pb++ = (UINT)a + (255 - a) * *pb / 255;
            }
        }
    }
    else
    {
        // AC_SRC_ALPHA is not set
        for(i = 0; i < cx * cy; i++)
        {
            b = (UINT)*pbSrc++ * bf.SourceConstantAlpha / 255;
            *pb++ = (UINT)b + (255 - bf.SourceConstantAlpha) * *pb / 255;
            g = (UINT)*pbSrc++ * bf.SourceConstantAlpha / 255;
            *pb++ = (UINT)g + (255 - bf.SourceConstantAlpha) * *pb / 255;
            r = (UINT)*pbSrc++ * bf.SourceConstantAlpha / 255;
            *pb++ = (UINT)r + (255 - bf.SourceConstantAlpha) * *pb / 255;
            a = (UINT)*pbSrc++ * bf.SourceConstantAlpha / 255;
            *pb++ = (UINT)a + (255 - bf.SourceConstantAlpha) * *pb / 255;
        }
    }
    SelectObject(hdcMemSrc, hbmSrcOld);
    DeleteObject(hbmSrc);
    DeleteDC(hdcMemSrc);

    BitBlt(hdc, x, y, cx, cy, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbm);
    DeleteDC(hdcMem);

    return TRUE;
}
