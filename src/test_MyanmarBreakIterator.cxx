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
#include <cstdlib>
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
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"

#include "MyanmarBreakIterator.hxx"

namespace css = ::com::sun::star;

css::lang::Locale locale(::rtl::OUString::createFromAscii("my"), ::rtl::OUString(), ::rtl::OUString());

bool showNextBreaks(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,
    ::rtl::OString utf8Text, ::sal_Int16 nWordType)
{
    css::uno::Reference<css::lang::XServiceInfo> xSI(xBreak, css::uno::UNO_QUERY);
    rtl::OUString breakIteratorName = xSI->getImplementationName();
    rtl::OString breakIteratorUtf8Name;
    breakIteratorName.convertToString(&breakIteratorUtf8Name,
        RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    int32_t pos = -1;
    css::i18n::Boundary b = xBreak->nextWord(utf16text, pos, locale, nWordType);
    while (b.endPos <= utf16text.getLength() && b.startPos != b.endPos)
    {
        ::rtl::OUString word(utf16text.getStr() + b.startPos, b.endPos - b.startPos);
        ::rtl::OString utf8Word;
        word.convertToString(&utf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
        fprintf(stderr, "%s::nextWord %d-%d '%s' type %d\n",
                breakIteratorUtf8Name.getStr(), b.startPos,
                b.endPos, utf8Word.getStr(), nWordType);
        b = xBreak->nextWord(utf16text, b.endPos-1, locale, nWordType);
    }

    return true;
}

bool showPreviousBreaks(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,
    ::rtl::OString utf8Text, ::sal_Int16 nWordType)
{
    css::uno::Reference<css::lang::XServiceInfo> xSI(xBreak, css::uno::UNO_QUERY);
    rtl::OUString breakIteratorName = xSI->getImplementationName();
    rtl::OString breakIteratorUtf8Name;
    breakIteratorName.convertToString(&breakIteratorUtf8Name,
        RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    int32_t pos = utf16text.getLength();
    css::i18n::Boundary b = xBreak->previousWord(utf16text, pos, locale, nWordType);
    while (b.startPos >= 0 && b.startPos != b.endPos)
    {
        ::rtl::OUString word(utf16text.getStr() + b.startPos, b.endPos - b.startPos);
        ::rtl::OString utf8Word;
        word.convertToString(&utf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
        fprintf(stderr, "%s::prevWord %d-%d '%s' type %d\n",
                breakIteratorUtf8Name.getStr(), b.startPos,
                b.endPos, utf8Word.getStr(), nWordType);
        b = xBreak->previousWord(utf16text, b.startPos, locale, nWordType);
    }

    return true;
}

bool breakPointsCorrect(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,
    ::rtl::OString utf8Text, int32_t expectedBreaks[][2], size_t numBreaks,
    ::sal_Int16 nWordType)
{
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    bool valid = true;
    int32_t pos = -1;
    for (size_t i = 0; i < numBreaks; i++)
    {
        css::i18n::Boundary b = xBreak->nextWord(utf16text, pos, locale, nWordType);
        valid &= (b.startPos == expectedBreaks[i][0]);
        valid &= (b.endPos == expectedBreaks[i][1]);
        if (!valid)
        {
            ::rtl::OUString word(utf16text.getStr() + b.startPos, b.endPos - b.startPos);
            ::rtl::OString utf8Word;
            word.convertToString(&utf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
            ::rtl::OUString expectedWord(utf16text.getStr() + expectedBreaks[i][0],
                                         expectedBreaks[i][1] - expectedBreaks[i][0]);
            ::rtl::OString expectedUtf8Word;
            expectedWord.convertToString(&expectedUtf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);

            fprintf(stderr, "breakPoint unexpected '%s' %d-%d expected '%s' %d-%d\n",
                    utf8Word.getStr(), b.startPos, b.endPos,
                    expectedUtf8Word.getStr(), expectedBreaks[i][0], expectedBreaks[i][1]);
            break;
        }
        pos = b.endPos - 1;
    }
    pos = utf16text.getLength();
    for (int i = numBreaks-1; i >= 0; i--)
    {
        css::i18n::Boundary b = xBreak->previousWord(utf16text, pos, locale, nWordType);
        valid &= (b.startPos == expectedBreaks[i][0]);
        valid &= (b.endPos == expectedBreaks[i][1]);
        // hack for double space
        if (!valid && utf16text[b.startPos] == 0x20 &&
            b.startPos + 1 < utf16text.getLength() &&
            utf16text[b.startPos+1] == 0x20 &&
            (nWordType == css::i18n::WordType::ANYWORD_IGNOREWHITESPACES ||
             nWordType == css::i18n::WordType::DICTIONARY_WORD))
        {
            // previous and next chooses different space
            valid = true;
        }
        if (!valid)
        {
            ::rtl::OUString word(utf16text.getStr() + b.startPos, b.endPos - b.startPos);
            ::rtl::OString utf8Word;
            word.convertToString(&utf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
            ::rtl::OUString expectedWord(utf16text.getStr() + expectedBreaks[i][0],
                                         expectedBreaks[i][1] - expectedBreaks[i][0]);
            ::rtl::OString expectedUtf8Word;
            expectedWord.convertToString(&expectedUtf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);

            fprintf(stderr, "previous breakPoint unexpected '%s' %d-%d expected '%s' %d-%d\n",
                    utf8Word.getStr(), b.startPos, b.endPos,
                    expectedUtf8Word.getStr(), expectedBreaks[i][0], expectedBreaks[i][1]);
            break;
        }
        pos = b.startPos;
    }
    return valid;
}


int main(int /*argc*/, char ** argv)
{
    
    css::uno::Reference<css::uno::XComponentContext> xContext = cppu::bootstrap();
    if (!xContext.is())
    {
        fprintf(stderr, "Failed to bootstrap context\n");
        return -1;
    }
    ::rtl::OUString myIterName =
        ::rtl::OUString::createFromAscii("com.sun.star.i18n.BreakIterator_my");
    //css::uno::Reference<css::i18n::XBreakIterator> xMMBreak(xContext->
    //    getServiceManager()->createInstanceWithContext(myIterName, xContext),
    //     css::uno::UNO_QUERY);
    css::uno::Reference<css::i18n::XBreakIterator> xMMBreak(
        org::thanlwinsoft::ooo::my::myanmarbreakiterator::_create(xContext), css::uno::UNO_QUERY);
    if (!xMMBreak.is())
    {
        fprintf(stderr, "Failed to create BreakIterator_my\n");
        return -1;
    }
    ::rtl::OUString uniIterName =
        ::rtl::OUString::createFromAscii("com.sun.star.i18n.BreakIterator_Unicode");
    css::uno::Reference<css::i18n::XBreakIterator> xUnicodeBreak(xContext->
        getServiceManager()->createInstanceWithContext(uniIterName, xContext),
         css::uno::UNO_QUERY);
    if (!xUnicodeBreak.is())
    {
        fprintf(stderr, "Failed to create BreakIterator_Unicode\n");
        return -1;
    }

    bool status = true;

    ::rtl::OString testEnglish("Find the breaks.  Second sentence is for O.O.O., but what are the “correct” answers?");
    // Words, spaces, punctuation returned as separate 'words'
    showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANY_WORD);
    showPreviousBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANY_WORD);
    int32_t testEnglishABreaks0[][2] = {{0,4},{4,5},{5,8},{8,9},{9,15},{15,16},
        {16,17},{17,18},{18,24},{24,25},{25,33},{33,34},{34,36},{36,37},
        {37,40},{40,41},{41,46},{46,47},{47,48},{48,49},{49,52},{52,53},
        {53,57},{57,58},{58,61},{61,62},{62,65},{65,66},{66,67},{67,74},{74,75},
        {75,76},{76,83},{83,84}};

    status &= breakPointsCorrect(xMMBreak, testEnglish, testEnglishABreaks0,
                                 sizeof(testEnglishABreaks0)/sizeof(testEnglishABreaks0[0]),
                                 css::i18n::WordType::ANY_WORD);
    // The next option seems to give stand alone words without punctuation, spaces skipped
    showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    showPreviousBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    int32_t testEnglishABreaks1[][2] = {{0,4},{5,8},{9,15},{15,16},
        {17,18},{18,24},{25,33},{34,36},
        {37,40},{41,42},{42,43},{43,44},{44,45},{45,46},{46,47},{47,48},{49,52},
        {53,57},{58,61},{62,65},{66,67},{67,74},{74,75},
        {76,83},{83,84}};
    status &= breakPointsCorrect(xMMBreak, testEnglish, testEnglishABreaks1,
                                 sizeof(testEnglishABreaks1)/sizeof(testEnglishABreaks1[0]),
                                 css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    // Dictionary Word gives words with fullstops appended for abbreviations
    showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::DICTIONARY_WORD);
    showPreviousBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::DICTIONARY_WORD);
    int32_t testEnglishABreaks2[][2] = {{0,4},{5,8},{9,16},
        {17,18},{18,24},{25,33},{34,36},
        {37,40},{41,47},{47,48},{49,52},
        {53,57},{58,61},{62,65},{66,67},{67,74},{74,75},
        {76,83},{83,84}};
    status &= breakPointsCorrect(xMMBreak, testEnglish, testEnglishABreaks2,
                                 sizeof(testEnglishABreaks2)/sizeof(testEnglishABreaks2[0]),
                                 css::i18n::WordType::DICTIONARY_WORD);
    // punctuation is joined to 'words', spaces are separate 'words'
    showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::WORD_COUNT);
    showPreviousBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::WORD_COUNT);
    int32_t testEnglishABreaks3[][2] = {{0,4},{4,5},{5,8},{8,9},{9,16},
        {16,17},{17,18},{18,24},{24,25},{25,33},{33,34},{34,36},{36,37},
        {37,40},{40,41},{41,48},{48,49},{49,52},{52,53},
        {53,57},{57,58},{58,61},{61,62},{62,65},{65,66},{66,75},
        {75,76},{76,84}};
    status &= breakPointsCorrect(xMMBreak, testEnglish, testEnglishABreaks3,
                                 sizeof(testEnglishABreaks3)/sizeof(testEnglishABreaks3[0]),
                                 css::i18n::WordType::WORD_COUNT);

    ::rtl::OString testEnglishB(" initial space");
    showNextBreaks(xUnicodeBreak, testEnglishB, css::i18n::WordType::ANY_WORD);
    // The next option seems to give stand alone words without punctuation, spaces skipped
    showNextBreaks(xUnicodeBreak, testEnglishB, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    // Dictionary Word gives words with fullstops appended for abbreviations
    showNextBreaks(xUnicodeBreak, testEnglishB, css::i18n::WordType::DICTIONARY_WORD);
    // punctuation is joined to 'words', spaces are separate 'words'
    showNextBreaks(xUnicodeBreak, testEnglishB, css::i18n::WordType::WORD_COUNT);

    ::rtl::OString testA("ကောင်းလား။");
    showNextBreaks(xUnicodeBreak, testA, css::i18n::WordType::WORD_COUNT);
    showNextBreaks(xMMBreak, testA, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    showNextBreaks(xMMBreak, testA, css::i18n::WordType::DICTIONARY_WORD);
    
    int32_t testABreaks0[][2] = {{0,6},{6,9},{9,10}};
    status &= breakPointsCorrect(xMMBreak, testA, testABreaks0,
                                 sizeof(testABreaks0)/sizeof(testABreaks0[0]),
                                 css::i18n::WordType::ANY_WORD);

    status &= breakPointsCorrect(xMMBreak, testA, testABreaks0,
                                 sizeof(testABreaks0)/sizeof(testABreaks0[0]),
                                 css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    status &= breakPointsCorrect(xMMBreak, testA, testABreaks0,
                                 sizeof(testABreaks0)/sizeof(testABreaks0[0]),
                                 css::i18n::WordType::DICTIONARY_WORD);
    int32_t testABreaks3[][2] = {{0,6},{6,10}};
    status &= breakPointsCorrect(xMMBreak, testA, testABreaks3,
                                 sizeof(testABreaks3)/sizeof(testABreaks3[0]),
                                 css::i18n::WordType::WORD_COUNT);

    ::rtl::OString testB("နေရာ အလွတ်ရှိရင် ဘာ “စကားလုံး” တွေမလဲ။ ");
    // in this test mode the dictionary doesn't seem to work properly
    int32_t testBBreaks0[][2] = {{0,4},{4,5},{5,10},{10,13},{13,16},{16,17},{17,19},{19,20},{20,21},{21,29},{29,30},{30,31},{31,34},{34,35},{35,37},{37,38},{38,39}};
    status &= breakPointsCorrect(xMMBreak, testB, testBBreaks0,
                                 sizeof(testBBreaks0)/sizeof(testBBreaks0[0]),
                                 css::i18n::WordType::ANY_WORD);
    int32_t testBBreaks1[][2] = {{0,4},{5,10},{10,13},{13,16},{17,19},{20,21},{21,29},{29,30},{31,34},{34,35},{35,37},{37,38}};
    status &= breakPointsCorrect(xMMBreak, testB, testBBreaks1,
                                 sizeof(testBBreaks1)/sizeof(testBBreaks1[0]),
                                 css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    status &= breakPointsCorrect(xMMBreak, testB, testBBreaks1,
                                 sizeof(testBBreaks1)/sizeof(testBBreaks1[0]),
                                 css::i18n::WordType::DICTIONARY_WORD);
    int32_t testBBreaks3[][2] = {{0,4},{4,5},{5,10},{10,13},{13,16},{16,17},{17,19},{19,20},{20,30},{30,31},{31,34},{34,35},{35,38},{38,39}};
    status &= breakPointsCorrect(xMMBreak, testB, testBBreaks3,
                                 sizeof(testBBreaks3)/sizeof(testBBreaks3[0]),
                                 css::i18n::WordType::WORD_COUNT);

    ::rtl::OString testC(" အရှေ့မှာ");
    int32_t testCBreaks0[][2] = {{0,1},{1,6},{6,9}};
    status &= breakPointsCorrect(xMMBreak, testC, testCBreaks0,
                                 sizeof(testCBreaks0)/sizeof(testCBreaks0[0]),
                                 css::i18n::WordType::ANY_WORD);
    int32_t testCBreaks1[][2] = {{1,6},{6,9}};
    status &= breakPointsCorrect(xMMBreak, testC, testCBreaks1,
                                 sizeof(testCBreaks1)/sizeof(testCBreaks1[0]),
                                 css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
    status &= breakPointsCorrect(xMMBreak, testC, testCBreaks1,
                                 sizeof(testCBreaks1)/sizeof(testCBreaks1[0]),
                                 css::i18n::WordType::DICTIONARY_WORD);
    status &= breakPointsCorrect(xMMBreak, testC, testCBreaks0,
                                 sizeof(testCBreaks0)/sizeof(testCBreaks0[0]),
                                 css::i18n::WordType::WORD_COUNT);

    if (!status)
        fprintf(stderr, "%s test failed!", argv[0]);

    return !status;
}