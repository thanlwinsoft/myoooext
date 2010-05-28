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
#include "cppuhelper/bootstrap.hxx"

#include "com/sun/star/i18n/WordType.hpp"
#include "com/sun/star/i18n/Boundary.hpp"
#include "com/sun/star/i18n/XBreakIterator.hpp"
#include "com/sun/star/lang/Locale.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"


namespace css = ::com::sun::star;

css::lang::Locale locale(::rtl::OUString::createFromAscii("my"), ::rtl::OUString(), ::rtl::OUString());

bool showNextBreaks(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,
    ::rtl::OString utf8Text, ::sal_Int16 nWordType)
{
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    int32_t pos = -1;
    css::i18n::Boundary b = xBreak->nextWord(utf16text, pos, locale, nWordType);
    while (b.endPos <= utf16text.getLength() && b.startPos > -1)
    {
        ::rtl::OUString word(utf16text.getStr() + b.startPos, b.endPos - b.startPos);
        ::rtl::OString utf8Word;
        word.convertToString(&utf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
        fprintf(stderr, "nextWord %d-%d '%s' type %d\n", b.startPos,
                b.endPos, utf8Word.getStr(), nWordType);
        b = xBreak->nextWord(utf16text, b.startPos, locale, nWordType);
    }

    return true;
}

bool breakPointsCorrect(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,
    ::rtl::OString utf8Text, int32_t * expectedBreaks, size_t numBreaks,
    ::sal_Int16 nWordType)
{
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    bool valid = true;
    int32_t pos = 0;
    for (size_t i = 0; i < numBreaks; i++)
    {
        css::i18n::Boundary b = xBreak->nextWord(utf16text, pos, locale, nWordType);
        if (i > 0)
            valid &= (b.startPos == expectedBreaks[i-1]);
        else
            valid &= (b.startPos == 0);
        valid &= (b.endPos == expectedBreaks[i]);
        if (!valid)
        {
            fprintf(stderr, "breakPoint unexpected %d-%d expected %d\n%s\n",
                    b.startPos, b.endPos, expectedBreaks[i], utf8Text.getStr());
            break;
        }
        pos = b.endPos - 1;
    }
    return valid;
}


int main(int /*argc*/, char ** /*argv*/)
{
    
    css::uno::Reference<css::uno::XComponentContext> xContext = cppu::bootstrap();
    if (!xContext.is())
    {
        fprintf(stderr, "Failed to bootstrap context\n");
        return -1;
    }
    ::rtl::OUString myIterName =
        ::rtl::OUString::createFromAscii("com.sun.star.i18n.BreakIterator_my");
    css::uno::Reference<css::i18n::XBreakIterator> xMMBreak(xContext->
        getServiceManager()->createInstanceWithContext(myIterName, xContext),
         css::uno::UNO_QUERY);
    if (!xMMBreak.is())
    {
        fprintf(stderr, "Failed to create BreakIterator_my\n");
        return -1;
    }
    ::rtl::OUString uniIterName =
        ::rtl::OUString::createFromAscii("com.sun.star.i18n.BreakIterator_Unicode");
    css::uno::Reference<css::i18n::XBreakIterator> xUnicodeBreak(xContext->
        getServiceManager()->createInstanceWithContext(myIterName, xContext),
         css::uno::UNO_QUERY);
    if (!xUnicodeBreak.is())
    {
        fprintf(stderr, "Failed to create BreakIterator_Unicode\n");
        return -1;
    }

    ::rtl::OString testEnglish("Find the breaks. Second sentence is for O.O.O..");
    // This gives strange results after first word
    //showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANY_WORD);
    // The next option seems to give stand alone words without punctuation
    showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    // Dictionary Word gives words with punctuation appended
    showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::DICTIONARY_WORD);
    // This only seems to show first char of word
    showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::WORD_COUNT);

    ::rtl::OString testA("ကောင်းလား။");
    int32_t testABreaks[] = {6,9,10};
    bool status = true;
    showNextBreaks(xMMBreak, testA, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    showNextBreaks(xMMBreak, testA, css::i18n::WordType::DICTIONARY_WORD);
    status &= breakPointsCorrect(xMMBreak, testA, testABreaks, sizeof(testABreaks)/sizeof(int32_t),
                                 css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);


    return !status;
}
