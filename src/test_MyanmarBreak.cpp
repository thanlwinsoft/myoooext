/*************************************************************************
 *
 * Copyright 2010 ThanLwinSoft.org
 *
 * This file is part of the ThanLwinSoft Myanmar extension for OpenOffice.org
 *
 * The ThanLwinSoft Myanmar Extension is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * The ThanLwinSoft Myanmar Extension is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
#include <cstdio>
#include <cstring>

#include "sal/config.h"
#include "rtl/string.h"
#include "rtl/string.hxx"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"

#include <vector>

#include "MyanmarBreak.hxx"

using namespace ::org::thanlwinsoft::myanmar;

bool breakPointsCorrect(::rtl::OString utf8Text, int32_t * expectedBreaks, size_t numBreaks)
{
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    bool valid = true;
    size_t iBreak = 0;
    for (int i = 1; i < utf16text.getLength(); i++)
    {
        if (MyanmarBreak::isBreak(utf16text, i))
        {
            if (i == expectedBreaks[iBreak])
            {
                ++iBreak;
                if (numBreaks == iBreak)
                    break; // all breaks found
            }
            else
            {
                fprintf(stderr, "broke at %d but expected %d in %s\n",
                        i, expectedBreaks[iBreak], utf8Text.getStr());
                valid = false;
            }
        }
        else if (i == expectedBreaks[iBreak])
        {
            fprintf(stderr, "expected break at %d not found in %s\n", i, utf8Text.getStr());
            valid = false;
        }
    }
    return valid;
}

int main(int /*argc*/, char ** /*argv*/)
{
    bool status = true;
    ::rtl::OString testA("ကောင်းလား။");
    int32_t testABreaks[] = {6,9,10};
    status &= breakPointsCorrect(testA, testABreaks, sizeof(testABreaks)/sizeof(int32_t));

    ::rtl::OString testB("ဘယ်မှာခွေဲမလဲ။");
    int32_t testBBreaks[] = {3,6,10,11,13,14};
    status &= breakPointsCorrect(testB, testBBreaks, sizeof(testBBreaks)/sizeof(int32_t));

    ::rtl::OString testC("သင်္ချိုင်းကိုမသွားနဲ့။");
    int32_t testCBreaks[] = {11,14,15,19,22,23};
    status &= breakPointsCorrect(testC, testCBreaks, sizeof(testCBreaks)/sizeof(int32_t));

    ::rtl::OString testD("သင်္ဘောနဲ့သွေးရင်ခေါင်မှုလား။");
    int32_t testDBreaks[] = {7,10,14,17,22,25,28,29};
    status &= breakPointsCorrect(testD, testDBreaks, sizeof(testDBreaks)/sizeof(int32_t));
    return !status;
}
