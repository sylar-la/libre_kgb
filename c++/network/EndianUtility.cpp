#include "EndianUtility.h"

#include <QDebug>

bool EndianUtility::isPlatformBigEndian = false;
bool EndianUtility::isPlatformBigEndianComputed = false;

bool EndianUtility::IsPlatformBigEndian()
{
    if (!isPlatformBigEndianComputed)
    {
        union {
            uint32_t i;
            char c[4];
        } endiancheck = {0x01020304};

        isPlatformBigEndian = endiancheck.c[0] == 1;
        isPlatformBigEndianComputed = true;
    }

    //return isPlatformBigEndian;

    return true;
}
