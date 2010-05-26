/*************************************************************************
 *
 * Copyright 2010 ThanLwinSoft.org
 *
 * This file is part of the ThanLwinSoft Myanmar Extension for OpenOffice.org.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This extension is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * This extension is distributed in the hope that it will be useful,
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


#ifndef MYANMARBREAK_HXX
#define MYANMARBREAK_HXX

#include "sal/config.h"
#include "sal/types.h"
#include "rtl/ustring.hxx"

class MyanmarBreak
{
public:
    typedef sal_Unicode MMChar;

    typedef enum MySyllablePart {
        MY_SYLLABLE_UNKNOWN = 0,
        MY_SYLLABLE_CONSONANT = 1,
        MY_SYLLABLE_MEDIAL = 2,
        MY_SYLLABLE_VOWEL = 3,
        MY_SYLLABLE_TONE = 4,
        MY_SYLLABLE_1039 = 5,
        MY_SYLLABLE_103A = 6,
        MY_SYLLABLE_NUMBER = 7,
        MY_SYLLABLE_SECTION = 8,
        MY_SYLLABLE_NUM_PARTS = 9
    } MySyllablePart;

    typedef enum MyPairStatus {
        MY_PAIR_ILLEGAL = 0,
        MY_PAIR_NO_BREAK = 1,
        MY_PAIR_SYL_BREAK = 2,
        MY_PAIR_WORD_BREAK = 3,
        MY_PAIR_PUNCTUATION = 4,
        MY_PAIR_CONTEXT = 5
    } MyPairStatus;

    static bool isMyanmar(const MMChar c);
    static MyPairStatus getPairStatus(const MMChar a, const MMChar b);
    static bool isBreak(::rtl::OUString & text, int32_t position);
private:
    static MySyllablePart getSyllablePart(const MMChar mmChar);
    static const int PAIR_TABLE[MY_SYLLABLE_NUM_PARTS][MY_SYLLABLE_NUM_PARTS];
    static const MySyllablePart CHAR_PART[0xA0];
    static const int MY_BASE;

};

#endif // MYANMARBREAK_HXX
