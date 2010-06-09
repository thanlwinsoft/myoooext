/*************************************************************************
 *
 * Copyright 2010 ThanLwinSoft.org
 *
 * This file is part of the ThanLwinSoft Myanmar extension for OpenOffice.org
 *
 * This Extension is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * This Extension is distributed in the hope that it will be useful,
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

#include "com/sun/star/frame/XDesktop.hpp"
#include "com/sun/star/frame/XFrame.hpp"
#include "com/sun/star/i18n/WordType.hpp"
#include "com/sun/star/i18n/Boundary.hpp"
#include "com/sun/star/i18n/XBreakIterator.hpp"
#include "com/sun/star/lang/DisposedException.hpp"
#include "com/sun/star/lang/Locale.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "com/sun/star/uno/XComponentContext.hpp"

#include "MyanmarBreakIterator.hxx"

namespace css = ::com::sun::star;
#ifdef _MSC_VER
typedef long int32_t;
#endif

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

void showBeginEnd(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,
    ::rtl::OString utf8Text, ::sal_Int16 nWordType)
{
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    for (sal_Int32 i = 0; i < utf16text.getLength(); i++)
    {
        sal_Bool begin = xBreak->isBeginWord(utf16text, i, locale, nWordType);
        sal_Bool end = xBreak->isEndWord(utf16text, i, locale, nWordType);
        char c = ' ';
        if (begin && end) c = '|';
        else if (begin) c = '<';
        else if (end) c = '>';
        fprintf(stderr, "%c", c);
    }
    fprintf(stderr, ".%d\n%s\n", nWordType, utf8Text.getStr());
}

bool compareBreaks(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,
    css::uno::Reference<css::i18n::XBreakIterator> & xMMBreak,
    ::rtl::OString utf8Text, ::sal_Int16 nWordType,
    int32_t * extraBreaks, size_t numExtraBreaks)
{
    bool ok = true;
    size_t extra = 0;
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    for (sal_Int32 i = 0; i < utf16text.getLength(); i++)
    {
        sal_Bool begin = xBreak->isBeginWord(utf16text, i, locale, nWordType);
        sal_Bool end = xBreak->isEndWord(utf16text, i, locale, nWordType);
        sal_Bool beginMM = xMMBreak->isBeginWord(utf16text, i, locale, nWordType);
        sal_Bool endMM = xMMBreak->isEndWord(utf16text, i, locale, nWordType);
        if ((begin != beginMM) && (end != endMM))
        {
            if (extra < numExtraBreaks && extraBreaks[extra] == i)
                ++extra;
            else
            {
                fprintf(stderr, "unexpected begin/end word %d type %d\n", i, nWordType);
                ok = false;
            }
        }
        else if (begin != beginMM)
        {
            fprintf(stderr, "unexpected begin word %d type %d\n", i, nWordType);
            ok = false;
        }
        else if (end != endMM)
        {
            fprintf(stderr, "unexpected begin word %d type %d\n", i, nWordType);
            ok = false;
        }
    }
    return ok;
}

void showLineBreaks(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,::rtl::OString utf8Text)
{
    css::i18n::LineBreakHyphenationOptions hyphenationOptions;
    css::i18n::LineBreakUserOptions userOptions;
    css::uno::Reference<css::lang::XServiceInfo> xSI(xBreak, css::uno::UNO_QUERY);
    rtl::OUString breakIteratorName = xSI->getImplementationName();
    rtl::OString breakIteratorUtf8Name;
    breakIteratorName.convertToString(&breakIteratorUtf8Name,
        RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
    fprintf(stderr, "LineBreaks using %s\n", breakIteratorUtf8Name.getStr());
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    int32_t prevBreak = utf16text.getLength();
    for (int32_t targetBreak = utf16text.getLength(); targetBreak > 0; targetBreak--)
    {
        css::i18n::LineBreakResults lbResult =
            xBreak->getLineBreak(utf16text, targetBreak, locale, 0, hyphenationOptions, userOptions);
        ::rtl::OString utf8LineFit;
        if (lbResult.breakIndex < prevBreak && lbResult.breakIndex >= 0)
        {
            ::rtl::OUString utf16LineFit(utf16text.getStr(), lbResult.breakIndex);
            utf16LineFit.convertToString(&utf8LineFit, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
            fprintf(stderr, "%s|%d type %d\n", utf8LineFit.getStr(), lbResult.breakIndex, lbResult.breakType);
            prevBreak = lbResult.breakIndex;
        }
    }
}

bool checkLineBreaks(css::uno::Reference<css::i18n::XBreakIterator> & xBreak,::rtl::OString utf8Text,
                     int32_t * lineBreaks, size_t numBreaks)
{
    bool valid = true;
    css::i18n::LineBreakHyphenationOptions hyphenationOptions;
    css::i18n::LineBreakUserOptions userOptions;
    ::rtl::OUString utf16text(utf8Text.getStr(), utf8Text.getLength(), RTL_TEXTENCODING_UTF8);
    int32_t prevBreak = utf16text.getLength();
    int32_t iLB = numBreaks - 1;
    for (int32_t targetBreak = utf16text.getLength(); targetBreak > 0; targetBreak--)
    {
        css::i18n::LineBreakResults lbResult =
            xBreak->getLineBreak(utf16text, targetBreak, locale, 0, hyphenationOptions, userOptions);
        ::rtl::OString utf8LineFit;
        if (lbResult.breakIndex < prevBreak && lbResult.breakIndex >= 0)
        {
            if ((iLB >= 0 && lineBreaks[iLB] == lbResult.breakIndex) || lbResult.breakIndex == 0)
            {
                --iLB;
            }
            else
            {
                ::rtl::OUString utf16LineFit(utf16text.getStr(), lbResult.breakIndex);
                utf16LineFit.convertToString(&utf8LineFit, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
                fprintf(stderr, "%s|%d type %d but expected %d\n", utf8LineFit.getStr(), lbResult.breakIndex, lbResult.breakType, (iLB < 0)? -1 : lineBreaks[iLB]);
                prevBreak = lbResult.breakIndex;
                valid = false;
                break;
            }
            prevBreak = lbResult.breakIndex;
        }
    }
    return valid;
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

            fprintf(stderr, "next breakPoint unexpected '%s' %d-%d expected '%s' %d-%d type %d\n",
                    utf8Word.getStr(), b.startPos, b.endPos,
                    expectedUtf8Word.getStr(), expectedBreaks[i][0],
                    expectedBreaks[i][1], nWordType);
            valid = false;
            return false;
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

            fprintf(stderr, "previous breakPoint unexpected '%s' %d-%d expected '%s' %d-%d type %d\n",
                    utf8Word.getStr(), b.startPos, b.endPos,
                    expectedUtf8Word.getStr(), expectedBreaks[i][0],
                    expectedBreaks[i][1], nWordType);
            valid = false;
            return false;;
        }
        pos = b.startPos;
    }
    int32_t searchPos = 0;
    //if (nWordType != css::i18n::WordType::DICTIONARY_WORD)
    //    return valid;
    for (size_t i = 0; i < numBreaks; i++)
    {
        for (; searchPos < expectedBreaks[i][1]; searchPos++)
        {
            if ((utf16text[searchPos] == 0x20 || utf16text[searchPos] == 0x09 ||
                 utf16text[searchPos] == 0x0A)
                && searchPos < expectedBreaks[i][0])
            {
                // skip space words
                continue;
            }
            css::i18n::Boundary b = xBreak->getWordBoundary(utf16text,
                searchPos, locale, nWordType, sal_True);
            if (b.startPos != expectedBreaks[i][0] ||
                b.endPos != expectedBreaks[i][1])
            {
                ::rtl::OUString word(utf16text.getStr() + b.startPos, b.endPos - b.startPos);
                ::rtl::OString utf8Word;
                word.convertToString(&utf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
                ::rtl::OUString expectedWord(utf16text.getStr() + expectedBreaks[i][0],
                                            expectedBreaks[i][1] - expectedBreaks[i][0]);
                ::rtl::OString expectedUtf8Word;
                expectedWord.convertToString(&expectedUtf8Word, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
                fprintf(stderr, "getWordBoundary %d gave %d-%d '%s' expected %d-%d '%s' type %d\n",
                        searchPos, b.startPos, b.endPos, utf8Word.getStr(),
                        expectedBreaks[i][0], expectedBreaks[i][1],
                        expectedUtf8Word.getStr(), nWordType);
                valid = false;
            }
        }
    }
    return valid;
}

#define ARRAY_WITH_LEN(A) A, sizeof(A)/sizeof(A[0])

int main(int argc, char ** argv)
{
    bool verbose = false;
    if (argc > 1 && ((strcmp(argv[1],"-v") == 0) || (strcmp(argv[1],"--verbose") == 0)))
    {
        verbose = true;
    }
    if (argc > 1 && ((strcmp(argv[1],"-h") == 0) || (strcmp(argv[1],"--help") == 0)))
    {
        fprintf(stderr, "Usage: %s [-v|--verbose]\nRuns the MyanmarBreakIterator Tests",
                argv[0]);
        return 1;
    }
    bool status = true;

    css::uno::Reference<css::uno::XComponentContext> xContext;
    try
    {
      xContext.set(cppu::bootstrap());
      if (!xContext.is())
      {
          fprintf(stderr, "Failed to bootstrap context\n");
          return -1;
      }
    }
    catch (cppu::BootstrapException be)
    {

        fprintf(stderr, "Exception in bootstrap context\n");
        return -1;
    }
    // create a container block for the references
    {
        ::rtl::OUString myIterName =
            ::rtl::OUString::createFromAscii("com.sun.star.i18n.BreakIterator_my");
        //css::uno::Reference<css::i18n::XBreakIterator> xMMBreak(xContext->
        //    getServiceManager()->createInstanceWithContext(myIterName, xContext),
        //     css::uno::UNO_QUERY);
        // Calling create directly, enables the debugger to see the break iterator properly
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

        ::rtl::OString testEnglish("Find the breaks.  Second sentence is for O.O.O., but what are the “correct” answers?");
        // Using WordType::ANY_WORD seems to give unpredictable results, because
        // breakiterator_unicode does not include in the case statement for
        // loadICUBreakIterator. Therefore, don't test this case for now.
        // Words, spaces, punctuation returned as separate 'words'
    //     if (verbose)
    //     {
    //         showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANY_WORD);
    //         showPreviousBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANY_WORD);
    //     }
    //     int32_t testEnglishABreaks0[][2] = {{0,4},{4,5},{5,8},{8,9},{9,15},{15,16},
    //         {16,17},{17,18},{18,24},{24,25},{25,33},{33,34},{34,36},{36,37},
    //         {37,40},{40,41},{41,46},{46,47},{47,48},{48,49},{49,52},{52,53},
    //         {53,57},{57,58},{58,61},{61,62},{62,65},{65,66},{66,67},{67,74},{74,75},
    //         {75,76},{76,83},{83,84}};
    // 
    //     status &= breakPointsCorrect(xMMBreak, testEnglish, testEnglishABreaks0,
    //                                  sizeof(testEnglishABreaks0)/sizeof(testEnglishABreaks0[0]),
    //                                  css::i18n::WordType::ANY_WORD);
        // The next option seems to give stand alone words without punctuation, spaces skipped
        if (verbose)
        {
            showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
            showPreviousBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
        }
        int32_t testEnglishABreaks1[][2] = {{0,4},{5,8},{9,15},{15,16},
            {17,18},{18,24},{25,33},{34,36},
            {37,40},{41,42},{42,43},{43,44},{44,45},{45,46},{46,47},{47,48},{49,52},
            {53,57},{58,61},{62,65},{66,67},{67,74},{74,75},
            {76,83},{83,84}};
        status &= breakPointsCorrect(xMMBreak, testEnglish, testEnglishABreaks1,
                                    sizeof(testEnglishABreaks1)/sizeof(testEnglishABreaks1[0]),
                                    css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
        // Dictionary Word gives words with fullstops appended for abbreviations
        if (verbose)
        {
            showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::DICTIONARY_WORD);
            showPreviousBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::DICTIONARY_WORD);
        }
        int32_t testEnglishABreaks2[][2] = {{0,4},{5,8},{9,16},
            {17,18},{18,24},{25,33},{34,36},
            {37,40},{41,47},{47,48},{49,52},
            {53,57},{58,61},{62,65},{66,67},{67,74},{74,75},
            {76,83},{83,84}};
        status &= breakPointsCorrect(xMMBreak, testEnglish, testEnglishABreaks2,
                                    sizeof(testEnglishABreaks2)/sizeof(testEnglishABreaks2[0]),
                                    css::i18n::WordType::DICTIONARY_WORD);
        // punctuation is joined to 'words', spaces are separate 'words'
        if (verbose)
        {
            showNextBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::WORD_COUNT);
            showPreviousBreaks(xUnicodeBreak, testEnglish, css::i18n::WordType::WORD_COUNT);
        }
        int32_t testEnglishABreaks3[][2] = {{0,4},{4,5},{5,8},{8,9},{9,16},
            {16,17},{17,18},{18,24},{24,25},{25,33},{33,34},{34,36},{36,37},
            {37,40},{40,41},{41,48},{48,49},{49,52},{52,53},
            {53,57},{57,58},{58,61},{61,62},{62,65},{65,66},{66,75},
            {75,76},{76,84}};
        status &= breakPointsCorrect(xMMBreak, testEnglish, testEnglishABreaks3,
                                    sizeof(testEnglishABreaks3)/sizeof(testEnglishABreaks3[0]),
                                    css::i18n::WordType::WORD_COUNT);

        if (verbose)
        {
            //showBeginEnd(xUnicodeBreak, testEnglish, css::i18n::WordType::ANY_WORD);
            //showBeginEnd(xMMBreak, testEnglish, css::i18n::WordType::ANY_WORD);
            showBeginEnd(xUnicodeBreak, testEnglish, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
            showBeginEnd(xMMBreak, testEnglish, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
            showBeginEnd(xUnicodeBreak, testEnglish, css::i18n::WordType::DICTIONARY_WORD);
            showBeginEnd(xMMBreak, testEnglish, css::i18n::WordType::DICTIONARY_WORD);
            showBeginEnd(xUnicodeBreak, testEnglish, css::i18n::WordType::WORD_COUNT);
            showBeginEnd(xMMBreak, testEnglish, css::i18n::WordType::WORD_COUNT);

            showLineBreaks(xUnicodeBreak, testEnglish);
            showLineBreaks(xMMBreak, testEnglish);
        }
        //status &= compareBreaks(xUnicodeBreak, xMMBreak, testEnglish,
        //                        css::i18n::WordType::ANY_WORD, NULL, 0);
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testEnglish,
                                css::i18n::WordType::ANYWORD_IGNOREWHITESPACES, NULL, 0);
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testEnglish,
                                css::i18n::WordType::DICTIONARY_WORD, NULL, 0);
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testEnglish,
                                css::i18n::WordType::WORD_COUNT, NULL, 0);

        int32_t expectedLBEnglish[] = { 5,9,18,25,34,37,41,49,53,58,62,66,76 };
        status &= checkLineBreaks(xMMBreak, testEnglish, ARRAY_WITH_LEN(expectedLBEnglish) );

        ::rtl::OString testEnglishB(" initial space, comma");
        if (verbose)
        {
            //showNextBreaks(xUnicodeBreak, testEnglishB, css::i18n::WordType::ANY_WORD);
            // The next option seems to give stand alone words without punctuation, spaces skipped
            showNextBreaks(xUnicodeBreak, testEnglishB, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
            // Dictionary Word gives words with fullstops appended for abbreviations
            showNextBreaks(xUnicodeBreak, testEnglishB, css::i18n::WordType::DICTIONARY_WORD);
            // punctuation is joined to 'words', spaces are separate 'words'
            showNextBreaks(xUnicodeBreak, testEnglishB, css::i18n::WordType::WORD_COUNT);
        }
        //int32_t testEnglishBBreaks0[][2] = {{0,1},{1,8},{8,9},{9,14},{14,15},{15,16},{16,21}};
        //status &= breakPointsCorrect(xMMBreak, testEnglishB, testEnglishBBreaks0,
        //                             sizeof(testEnglishBBreaks0)/sizeof(testEnglishBBreaks0[0]),
        //                             css::i18n::WordType::ANY_WORD);
        int32_t testEnglishBBreaks1[][2] = {{1,8},{9,14},{14,15},{16,21}};
        status &= breakPointsCorrect(xMMBreak, testEnglishB, testEnglishBBreaks1,
                                    sizeof(testEnglishBBreaks1)/sizeof(testEnglishBBreaks1[0]),
                                    css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
        status &= breakPointsCorrect(xMMBreak, testEnglishB, testEnglishBBreaks1,
                                    sizeof(testEnglishBBreaks1)/sizeof(testEnglishBBreaks1[0]),
                                    css::i18n::WordType::DICTIONARY_WORD);
        int32_t testEnglishBBreaks3[][2] = {{0,1},{1,8},{8,9},{9,15},{15,16},{16,21}};
        status &= breakPointsCorrect(xMMBreak, testEnglishB, testEnglishBBreaks3,
                                    sizeof(testEnglishBBreaks3)/sizeof(testEnglishBBreaks3[0]),
                                    css::i18n::WordType::WORD_COUNT);

        //status &= compareBreaks(xUnicodeBreak, xMMBreak, testEnglishB,
        //                        css::i18n::WordType::ANY_WORD, NULL, 0);
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testEnglishB,
                                css::i18n::WordType::ANYWORD_IGNOREWHITESPACES, NULL, 0);
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testEnglishB,
                                css::i18n::WordType::DICTIONARY_WORD, NULL, 0);
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testEnglishB,
                                css::i18n::WordType::WORD_COUNT, NULL, 0);

        ::rtl::OString testA("ကောင်းလား။");
        if (verbose)
        {
            showNextBreaks(xUnicodeBreak, testA, css::i18n::WordType::WORD_COUNT);
            showNextBreaks(xMMBreak, testA, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
            showNextBreaks(xMMBreak, testA, css::i18n::WordType::DICTIONARY_WORD);
            showLineBreaks(xUnicodeBreak, testA);
            showLineBreaks(xMMBreak, testA);
        }


        int32_t testABreaks0[][2] = {{0,6},{6,9},{9,10}};
        //status &= breakPointsCorrect(xMMBreak, testA, testABreaks0,
        //                             sizeof(testABreaks0)/sizeof(testABreaks0[0]),
        //                             css::i18n::WordType::ANY_WORD);

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

        int32_t testAExtraBreaks[] = {6,9};
        //status &= compareBreaks(xUnicodeBreak, xMMBreak, testA,
        //                        css::i18n::WordType::ANY_WORD, testAExtraBreaks,
        //                        sizeof(testAExtraBreaks)/sizeof(testAExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testA,
                                css::i18n::WordType::ANYWORD_IGNOREWHITESPACES, testAExtraBreaks,
                                sizeof(testAExtraBreaks)/sizeof(testAExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testA,
                                css::i18n::WordType::DICTIONARY_WORD, testAExtraBreaks,
                                sizeof(testAExtraBreaks)/sizeof(testAExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testA,
                                css::i18n::WordType::WORD_COUNT, testAExtraBreaks,
                                sizeof(testAExtraBreaks)/sizeof(testAExtraBreaks[0]));

        ::rtl::OString testB("နေရာ အလွတ်ရှိရင် ဘာ “စကားလုံး” တွေမလဲ။ ");
        // in this test mode the dictionary doesn't seem to work properly
        //int32_t testBBreaks0[][2] = {{0,4},{4,5},{5,10},{10,13},{13,16},{16,17},{17,19},{19,20},{20,21},{21,29},{29,30},{30,31},{31,34},{34,35},{35,37},{37,38},{38,39}};
        //status &= breakPointsCorrect(xMMBreak, testB, testBBreaks0,
        //                             sizeof(testBBreaks0)/sizeof(testBBreaks0[0]),
        //                             css::i18n::WordType::ANY_WORD);
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

        int32_t testBExtraBreaks [] = { 10,13,34,35,37 };
        //status &= compareBreaks(xUnicodeBreak, xMMBreak, testB,
        //                        css::i18n::WordType::ANY_WORD, testBExtraBreaks,
        //                        sizeof(testBExtraBreaks)/sizeof(testBExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testB,
                                css::i18n::WordType::ANYWORD_IGNOREWHITESPACES, testBExtraBreaks,
                                sizeof(testBExtraBreaks)/sizeof(testBExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testB,
                                css::i18n::WordType::DICTIONARY_WORD, testBExtraBreaks,
                                sizeof(testBExtraBreaks)/sizeof(testBExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testB,
                                css::i18n::WordType::WORD_COUNT, testBExtraBreaks,
                                sizeof(testBExtraBreaks)/sizeof(testBExtraBreaks[0]));

        if (verbose)
        {
            showLineBreaks(xUnicodeBreak, testB);
            showLineBreaks(xMMBreak, testB);
        }

        ::rtl::OString testC(" အရှေ့မှာ");
        int32_t testCBreaks0[][2] = {{0,1},{1,6},{6,9}};
        //status &= breakPointsCorrect(xMMBreak, testC, testCBreaks0,
        //                             sizeof(testCBreaks0)/sizeof(testCBreaks0[0]),
        //                             css::i18n::WordType::ANY_WORD);
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

        int32_t testCExtraBreaks [] = { 6 };
        //status &= compareBreaks(xUnicodeBreak, xMMBreak, testC,
        //                        css::i18n::WordType::ANY_WORD, testCExtraBreaks,
        //                        sizeof(testCExtraBreaks)/sizeof(testCExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testC,
                                css::i18n::WordType::ANYWORD_IGNOREWHITESPACES, testCExtraBreaks,
                                sizeof(testCExtraBreaks)/sizeof(testCExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testC,
                                css::i18n::WordType::DICTIONARY_WORD, testCExtraBreaks,
                                sizeof(testCExtraBreaks)/sizeof(testCExtraBreaks[0]));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testC,
                                css::i18n::WordType::WORD_COUNT, testCExtraBreaks,
                                sizeof(testCExtraBreaks)/sizeof(testCExtraBreaks[0]));

        ::rtl::OString testMixedLang("မြန်မာ English မြန်မာစကား (ကွင်း)");
        int32_t testMixedLangBreaks1[][2] = {{0,6},{7,14},{15,21},{21,25},{26,27},{27,32},{32,33}};
        status &= breakPointsCorrect(xMMBreak, testMixedLang,
                                ARRAY_WITH_LEN(testMixedLangBreaks1),
                                css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
        status &= breakPointsCorrect(xMMBreak, testMixedLang,
                                ARRAY_WITH_LEN(testMixedLangBreaks1),
                                css::i18n::WordType::DICTIONARY_WORD);
        int32_t testMixedLangBreaks3[][2] =
            {{0,6},{6,7},{7,14},{14,15},{15,21},{21,25},{25,26},{26,33}};
        status &= breakPointsCorrect(xMMBreak, testMixedLang,
                                ARRAY_WITH_LEN(testMixedLangBreaks3),
                                css::i18n::WordType::WORD_COUNT);

        int32_t testMixedExtraBreaks [] = { 21 };
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testMixedLang,
                                css::i18n::WordType::ANYWORD_IGNOREWHITESPACES,
                                ARRAY_WITH_LEN(testMixedExtraBreaks));
        status &= compareBreaks(xUnicodeBreak, xMMBreak, testMixedLang,
                                css::i18n::WordType::DICTIONARY_WORD,
                                ARRAY_WITH_LEN(testMixedExtraBreaks));

        status &= compareBreaks(xUnicodeBreak, xMMBreak, testMixedLang,
                                css::i18n::WordType::WORD_COUNT,
                                ARRAY_WITH_LEN(testMixedExtraBreaks));

        ::rtl::OString testNumTag("12.\tဒေသများ");
        if (verbose)
        {
            showNextBreaks(xUnicodeBreak, testNumTag, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
            showNextBreaks(xMMBreak, testNumTag, css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
        }
        int32_t numTabBreaks1[][2] = {{0,2},{2,3},{4,7},{7,11}};
        status &= breakPointsCorrect(xMMBreak, testNumTag,
                                ARRAY_WITH_LEN(numTabBreaks1),
                                css::i18n::WordType::ANYWORD_IGNOREWHITESPACES);
        status &= breakPointsCorrect(xMMBreak, testNumTag,
                                ARRAY_WITH_LEN(numTabBreaks1),
                                css::i18n::WordType::DICTIONARY_WORD);
        int32_t numTabBreaks3[][2] = {{0,3},{3,4},{4,7},{7,11}};
        status &= breakPointsCorrect(xMMBreak, testNumTag,
                                ARRAY_WITH_LEN(numTabBreaks3),
                                css::i18n::WordType::WORD_COUNT);

        ::rtl::OString testD("ဌာန");
        int32_t testDBreaks1[][2] = {{0,3}};
        status &= breakPointsCorrect(xMMBreak, testD,
                                ARRAY_WITH_LEN(testDBreaks1),
                                css::i18n::WordType::DICTIONARY_WORD);

        ::rtl::OString testE("ဖက်စ်");
        int32_t testEBreaks1[][2] = {{0,5}};
        status &= breakPointsCorrect(xMMBreak, testE,
                                ARRAY_WITH_LEN(testEBreaks1),
                                css::i18n::WordType::DICTIONARY_WORD);

        ::rtl::OString testF("CMYIမှ ရယူနိုင်ပါသည်၊ ");
        int32_t testFBreaks1[][2] = {{0,4},{4,6},{7,8},{8,10},{10,15},{15,17},{17,20},{20,21}};
        status &= breakPointsCorrect(xMMBreak, testF,
                                ARRAY_WITH_LEN(testFBreaks1),
                                css::i18n::WordType::DICTIONARY_WORD);



        if (!status)
            fprintf(stderr, "\n*** %s test failed! ***\n\n", argv[0]);

    }
    // The bootstrap may have started an instance of OpenOffice, which will
    // cause ctest to hang unless we terminate it.
    ::rtl::OUString desktopService =
            ::rtl::OUString::createFromAscii("com.sun.star.frame.Desktop");
    css::uno::Reference<css::frame::XDesktop> xDesktop(xContext->
        getServiceManager()->createInstanceWithContext(desktopService, xContext),
                                                       css::uno::UNO_QUERY);
    if (xDesktop.is())
    {
        try
        {
            css::uno::Reference <css::frame::XFrame > xFrame = xDesktop->getCurrentFrame();
            // if there is a frame, then it probably wasn't created by this
            //  test, so don't terminate
            if (!xFrame.is())
            {
                bool terminate = xDesktop->terminate();
                if (!terminate)
                {
                    printf("Desktop terminate vetoed\n");
                }
            }
        }
        catch (css::lang::DisposedException e)
        {
            ::rtl::OString msg;
            e.Message.convertToString(&msg, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
            printf("Caught DisposedException %s\n", msg.getStr());
        }
    }

    return !status;
}
