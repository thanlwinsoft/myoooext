/*************************************************************************
 *
 * Copyright 2010 ThanLwinSoft.org
 *
 * This file is part of the ThanLwinSoft Myanmar Extension for OpenOffice.org.
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
#include <cassert>
#include <cstring>
#include <cstdio>

#include "sal/config.h"
#include "rtl/string.hxx"
#include "rtl/ustring.hxx"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "cppuhelper/implbase5.hxx"
#include "com/sun/star/beans/PropertyValue.hpp"
#include "com/sun/star/beans/PropertyValues.hpp"
#include "com/sun/star/lang/XInitialization.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"
#include "com/sun/star/lang/XMultiServiceFactory.hpp"
#include "com/sun/star/frame/XDispatch.hpp"
#include "com/sun/star/frame/XDispatchProvider.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/i18n/WordType.hpp"
#include "com/sun/star/i18n/BreakType.hpp"
#include "com/sun/star/i18n/UnicodeType.hpp"
#include "com/sun/star/i18n/XCharacterClassification.hpp"
#include "com/sun/star/i18n/XBreakIterator.hpp"
#include "com/sun/star/linguistic2/XSpellChecker.hpp"
#include "com/sun/star/linguistic2/XLinguServiceManager.hpp"
#include "com/sun/star/frame/XFrame.hpp"

#include "MyanmarBreak.hxx"
#include "oooextDiagnostic.hxx"
#include "MyanmarBreakIterator.hxx"

#ifndef NDEBUG
#define MMBI_DEBUG
#endif

/// anonymous implementation namespace
namespace org { namespace thanlwinsoft { namespace ooo { namespace my {

namespace css = ::com::sun::star;
namespace otm = ::org::thanlwinsoft::myanmar;

const size_t MAX_SYLLABLE_PER_WORD = 5;

class MyanmarBreakIterator:
    public ::cppu::WeakImplHelper5<
        css::lang::XInitialization,
        css::frame::XDispatch,
        css::frame::XDispatchProvider,
        css::lang::XServiceInfo,
        css::i18n::XBreakIterator>
{
public:
    explicit MyanmarBreakIterator(css::uno::Reference< css::uno::XComponentContext > const & context);

    // ::com::sun::star::lang::XInitialization:
    virtual void SAL_CALL initialize(const css::uno::Sequence< ::com::sun::star::uno::Any > & aArguments) throw (css::uno::RuntimeException, css::uno::Exception);

    // ::com::sun::star::frame::XDispatch:
    virtual void SAL_CALL dispatch(const css::util::URL & URL, const css::uno::Sequence< css::beans::PropertyValue > & Arguments) throw (css::uno::RuntimeException);
    virtual void SAL_CALL addStatusListener(const css::uno::Reference< css::frame::XStatusListener > & Control, const css::util::URL & URL) throw (css::uno::RuntimeException);
    virtual void SAL_CALL removeStatusListener(const css::uno::Reference< css::frame::XStatusListener > & Control, const css::util::URL & URL) throw (css::uno::RuntimeException);

    // ::com::sun::star::frame::XDispatchProvider:
    virtual css::uno::Reference< css::frame::XDispatch > SAL_CALL queryDispatch(const css::util::URL & URL, const ::rtl::OUString & TargetFrameName, ::sal_Int32 SearchFlags) throw (css::uno::RuntimeException);
    virtual css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > SAL_CALL queryDispatches(const css::uno::Sequence< css::frame::DispatchDescriptor > & Requests) throw (css::uno::RuntimeException);

    // ::com::sun::star::lang::XServiceInfo:
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL supportsService(const ::rtl::OUString & ServiceName) throw (css::uno::RuntimeException);
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw (css::uno::RuntimeException);

    // ::com::sun::star::i18n::XBreakIterator:
    virtual ::sal_Int32 SAL_CALL nextCharacters(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharacterIteratorMode, ::sal_Int32 nCount, ::sal_Int32 & nDone) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL previousCharacters(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharacterIteratorMode, ::sal_Int32 nCount, ::sal_Int32 & nDone) throw (css::uno::RuntimeException);
    virtual css::i18n::Boundary SAL_CALL nextWord(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException);
    virtual css::i18n::Boundary SAL_CALL previousWord(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException);
    virtual css::i18n::Boundary SAL_CALL getWordBoundary(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType, ::sal_Bool bPreferForward) throw (css::uno::RuntimeException);
    virtual ::sal_Int16 SAL_CALL getWordType(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL isBeginWord(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL isEndWord(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL beginOfSentence(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL endOfSentence(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException);
    virtual css::i18n::LineBreakResults SAL_CALL getLineBreak(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int32 nMinBreakPos, const css::i18n::LineBreakHyphenationOptions & aHyphOptions, const css::i18n::LineBreakUserOptions & aUserOptions) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL beginOfScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL endOfScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL nextScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL previousScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException);
    virtual ::sal_Int16 SAL_CALL getScriptType(const ::rtl::OUString & aText, ::sal_Int32 nPos) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL beginOfCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL endOfCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL nextCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException);
    virtual ::sal_Int32 SAL_CALL previousCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException);

private:
    MyanmarBreakIterator(const org::thanlwinsoft::ooo::my::MyanmarBreakIterator &); // not defined
    MyanmarBreakIterator& operator=(const org::thanlwinsoft::ooo::my::MyanmarBreakIterator &); // not defined

    bool isWhiteSpace(const ::rtl::OUString & aText, ::sal_Int32 i);
    bool isPunctuation(const ::rtl::OUString & aText, ::sal_Int32 i);

    // destructor is private and will be called indirectly by the release call    virtual ~org::thanlwinsoft::ooo::my::MyanmarBreakIterator() {}

    css::uno::Reference< css::uno::XComponentContext > m_xContext;
    css::uno::Reference< css::frame::XFrame > m_xFrame;
    css::uno::Reference< css::i18n::XBreakIterator> m_xBreakIteratorDelegate;
    css::uno::Reference< css::linguistic2::XSpellChecker> m_xSpellChecker;
    css::uno::Reference< css::i18n::XCharacterClassification> m_xCharClassification;
    css::lang::Locale m_locale;
};

MyanmarBreakIterator::MyanmarBreakIterator(css::uno::Reference< css::uno::XComponentContext > const & context) :
    m_xContext(context),
    m_locale(::rtl::OUString::createFromAscii("my"), ::rtl::OUString(), ::rtl::OUString())
{
#ifdef MMBI_DEBUG
    fprintf(stderr, "MyanmarBreakIterator\n");
#endif
    assert(context.is());
    css::uno::Reference<css::lang::XMultiComponentFactory>
        xFactory(m_xContext->getServiceManager());
    if (xFactory.is())
    {
#ifdef MMBI_DEBUG
        printServiceNames(xFactory);
#endif
        ::rtl::OUString charClassificationServiceName = ::rtl::OUString::createFromAscii(
            "com.sun.star.i18n.CharacterClassification");
        m_xCharClassification.set(
            xFactory->createInstanceWithContext(charClassificationServiceName, context),
                                                css::uno::UNO_QUERY);

        ::rtl::OUString linguServiceManager =
            ::rtl::OUString::createFromAscii("com.sun.star.linguistic2.LinguServiceManager");
        css::uno::Reference<css::linguistic2::XLinguServiceManager> xLinguServiceManager
            (xFactory->createInstanceWithContext(linguServiceManager, context), css::uno::UNO_QUERY);
        if (xLinguServiceManager.is())
        {
#ifdef MMBI_DEBUG
            fprintf(stderr, "have LinguServiceManager\n");
#endif
            ::rtl::OUString spellChecker = ::rtl::OUString::createFromAscii("com.sun.star.linguistic2.SpellChecker");
            css::uno::Sequence< ::rtl::OUString> mySpellCheckers =
                xLinguServiceManager->getConfiguredServices(spellChecker, m_locale);
#ifdef MMBI_DEBUG
            printStringSequence(mySpellCheckers);
#endif
            if (mySpellCheckers.getLength() > 0)
            {
                m_xSpellChecker.set(xLinguServiceManager->getSpellChecker());
                if (m_xSpellChecker.is())
                {
#ifdef MMBI_DEBUG
                    fprintf(stderr, "have spellchecker\n");
#endif
                }
            }
        }
    }
    css::uno::Reference<css::lang::XMultiServiceFactory>
        xServiceFactory(m_xContext->getServiceManager(), css::uno::UNO_QUERY);
    if (xServiceFactory.is())
    {
#ifdef MMBI_DEBUG
        fprintf(stderr, "have multi service factory\n");
#endif
        // printStringSequence(xServiceFactory->getAvailableServiceNames());
        ::rtl::OUString biCtl =
            ::rtl::OUString::createFromAscii("com.sun.star.i18n.BreakIterator_Unicode");
        m_xBreakIteratorDelegate.set(xFactory->createInstanceWithContext(biCtl, context),
                                     css::uno::UNO_QUERY);
        if (m_xBreakIteratorDelegate.is())
        {
#ifdef MMBI_DEBUG
            fprintf(stderr, "have default Unicode iterator\n");
#endif
        }
    }
}

// ::com::sun::star::lang::XInitialization:
void SAL_CALL MyanmarBreakIterator::initialize( const css::uno::Sequence< css::uno::Any >& aArguments ) throw (css::uno::Exception, css::uno::RuntimeException)
{
    css::uno::Reference < css::frame::XFrame > xFrame;
    if ( aArguments.getLength() ) {
        aArguments[0] >>= xFrame;
        m_xFrame = xFrame;
    }
}

// ::com::sun::star::frame::XDispatch:
void SAL_CALL MyanmarBreakIterator::dispatch( const css::util::URL& aURL, const css::uno::Sequence< css::beans::PropertyValue >& /*aArguments*/ ) throw(css::uno::RuntimeException)
{
    if ( aURL.Protocol.equalsAscii("") == 0 )
    {
        if ( aURL.Path.equalsAscii("com.sun.star.i18n.BreakIterator_my") )
        {
                // add your own code here
                return;
        }
    }
}

void SAL_CALL MyanmarBreakIterator::addStatusListener( const css::uno::Reference< css::frame::XStatusListener >& /*xControl*/, const css::util::URL& /*aURL*/ ) throw (css::uno::RuntimeException)
{
    // TODO
}

void SAL_CALL MyanmarBreakIterator::removeStatusListener( const css::uno::Reference< css::frame::XStatusListener >& /*xControl*/, const css::util::URL& /*aURL*/ ) throw (css::uno::RuntimeException)
{
    // add your own code here
}

// ::com::sun::star::frame::XDispatchProvider:
css::uno::Reference< css::frame::XDispatch > SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::queryDispatch( const css::util::URL& aURL, const ::rtl::OUString& /*sTargetFrameName*/, sal_Int32 /*nSearchFlags*/ ) throw(css::uno::RuntimeException)
{
    css::uno::Reference< css::frame::XDispatch > xRet;
    if ( !m_xFrame.is() )
        return 0;

    if ( aURL.Protocol.equalsAscii("") == 0 )
    {
        if ( aURL.Path.equalsAscii("com.sun.star.i18n.BreakIterator_my") == 0 )
            xRet = this;
    }
    return xRet;
}

css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::queryDispatches( const css::uno::Sequence< css::frame::DispatchDescriptor >& seqDescripts ) throw(css::uno::RuntimeException)
{
    sal_Int32 nCount = seqDescripts.getLength();
    css::uno::Sequence< css::uno::Reference< css::frame::XDispatch > > lDispatcher(nCount);

    for( sal_Int32 i=0; i<nCount; ++i ) {
        lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL,
                                        seqDescripts[i].FrameName,
                                        seqDescripts[i].SearchFlags );
    }

    return lDispatcher;
}

// com.sun.star.uno.XServiceInfo:
::rtl::OUString SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getImplementationName() throw (css::uno::RuntimeException)
{
    return org::thanlwinsoft::ooo::my::myanmarbreakiterator::_getImplementationName();
}

::sal_Bool SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::supportsService(::rtl::OUString const & serviceName) throw (css::uno::RuntimeException)
{
    css::uno::Sequence< ::rtl::OUString > serviceNames = org::thanlwinsoft::ooo::my::myanmarbreakiterator::_getSupportedServiceNames();
    for (::sal_Int32 i = 0; i < serviceNames.getLength(); ++i) {
        if (serviceNames[i] == serviceName)
            return sal_True;
    }
    return sal_False;
}

css::uno::Sequence< ::rtl::OUString > SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getSupportedServiceNames() throw (css::uno::RuntimeException)
{
    return org::thanlwinsoft::ooo::my::myanmarbreakiterator::_getSupportedServiceNames();
}

// ::com::sun::star::i18n::XBreakIterator:
::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextCharacters(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharacterIteratorMode, ::sal_Int32 nCount, ::sal_Int32 & nDone) throw (css::uno::RuntimeException)
{
    ::sal_Int32 ret = m_xBreakIteratorDelegate->nextCharacters(aText, nStartPos, aLocale,
                                                    nCharacterIteratorMode,
                                                    nCount, nDone);
#ifdef MMBI_DEBUG
    fprintf(stderr, "nextCharacters from %d count=%d done=%d return=%d mode=%d\n",
            nStartPos, nCount, nDone, ret, nCharacterIteratorMode);
#endif
    return ret;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousCharacters(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharacterIteratorMode, ::sal_Int32 nCount, ::sal_Int32 & nDone) throw (css::uno::RuntimeException)
{
    ::sal_Int32 ret = m_xBreakIteratorDelegate->previousCharacters(aText, nStartPos, aLocale,
                                                        nCharacterIteratorMode, nCount, nDone);
#ifdef MMBI_DEBUG
    fprintf(stderr, "previousCharacters from %d count=%d done=%d return=%d mode=%d\n",
            nStartPos, nCount, nDone, ret, nCharacterIteratorMode);
#endif
    return ret;
}

bool org::thanlwinsoft::ooo::my::MyanmarBreakIterator::isWhiteSpace(const ::rtl::OUString & aText, ::sal_Int32 i)
{
    // see ICU u_isWhitespace
    switch (aText[i])
    {
        case 0x0009:
        case 0x000A:
        case 0x000B:
        case 0x000C:
        case 0x000D:
        case 0x001C:
        case 0x001D:
        case 0x001E:
        case 0x001F:
        case 0x0085:
        case 0x200B:// ZWSP
            return true;
        default:
            break;
    }
    return (m_xCharClassification->getType(aText, i) == css::i18n::UnicodeType::SPACE_SEPARATOR);
}

bool org::thanlwinsoft::ooo::my::MyanmarBreakIterator::isPunctuation(const ::rtl::OUString & aText, ::sal_Int32 i)
{
    sal_Int16 cType = m_xCharClassification->getType(aText, i);
    return (cType == css::i18n::UnicodeType::OTHER_PUNCTUATION ||
            cType == css::i18n::UnicodeType::START_PUNCTUATION ||
            cType == css::i18n::UnicodeType::END_PUNCTUATION ||
            cType == css::i18n::UnicodeType::INITIAL_PUNCTUATION ||
            cType == css::i18n::UnicodeType::FINAL_PUNCTUATION);
}

// nWordType determines what type of boundary is returned
// ANY_WORD - Words, spaces, punctuation returned as separate 'words'
// ANYWORD_IGNOREWHITESPACES - single spaces not returned, words, punctuation separate 'words'
// DICTIONARY_WORD - same as ANYWORD_IGNOREWHITESPACES except acronymns with . are single 'words'
// WORD_COUNT - punctuation is joined to 'words', spaces are separate 'words'

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextWord(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    css::i18n::Boundary wordBoundary;
    int32_t i = (nStartPos < 0)? 0 : nStartPos;
    int32_t punctuationStart = i;
    wordBoundary.startPos = i;
    // The start will be inside an existing word, need to find end of that word, before
    // looking for start of next word


    // if nStartPos is the start of a word, we need to move to the next word start
    // otherwise Ctrl+Right doesn't move the cursor
    if (i < aText.getLength())
    {
        if (i == nStartPos)
        {
            if (otm::MyanmarBreak::isMyanmar(aText[i]))
            {
                // find first word break after start position
                for (size_t syl = 0; syl < MAX_SYLLABLE_PER_WORD; syl++)
                {
                    ++i;
                    while (i < aText.getLength() &&
                        otm::MyanmarBreak::isMyanmar(aText[i]) &&
                        !otm::MyanmarBreak::isBreak(aText, i))
                    {
                        ++i;
                    }
                    if (syl == 0)
                    {
                        wordBoundary.startPos = i;
                    }
                    else if (m_xSpellChecker.is())
                    {
                        if (i == wordBoundary.startPos)
                            break; // syllable hasn't changed
                        // check dictionary for multi-syllable word
                        ::rtl::OUString testWord(aText.getStr() + nStartPos,
                                                i - nStartPos);
                        css::beans::PropertyValues defaultProps;
                        if (m_xSpellChecker->isValid(testWord, m_locale, defaultProps))
                        {
                            wordBoundary.startPos = i;
                        }
                    }
                    else
                    {
                        break;
                    }
                    if (!otm::MyanmarBreak::isMyanmar(aText[i]))
                        break;
                }
            }
            else // the current word is not Myanmar
            {
                if (isWhiteSpace(aText, i))
                {
                    do
                    {
                        ++i;
                    } while (i < aText.getLength() && isWhiteSpace(aText, i));
                    wordBoundary.startPos = i;
                }
                else if (isPunctuation(aText, i))
                {
                    do
                    {
                        ++i;
                    } while (i < aText.getLength() && isPunctuation(aText, i));
                    wordBoundary.startPos = i;
                }
            }
        }
        i = wordBoundary.startPos;
        // skip over spaces to next word
        if (nWordType == css::i18n::WordType::ANYWORD_IGNOREWHITESPACES ||
            nWordType == css::i18n::WordType::DICTIONARY_WORD)
        {
            while (i < aText.getLength() && isWhiteSpace(aText, i))
            {
                ++i;
                wordBoundary.startPos = i;
            }
        }
        else if (nWordType == css::i18n::WordType::WORD_COUNT)
        {
            punctuationStart = wordBoundary.startPos;
            while (i < aText.getLength() && isPunctuation(aText, i))
            {
                ++i;
                wordBoundary.startPos = i;
            }
        }
        wordBoundary.endPos = wordBoundary.startPos;
        if (otm::MyanmarBreak::isMyanmar(aText[i]))
        {
            for (size_t syl = 0; syl < MAX_SYLLABLE_PER_WORD &&
                i < aText.getLength(); syl++)
            {
                ++i;
                while (i < aText.getLength() &&
                    otm::MyanmarBreak::isMyanmar(aText[i]) &&
                    !otm::MyanmarBreak::isBreak(aText, i))
                {
                    ++i;
                }
                if (syl == 0)
                {
                    wordBoundary.endPos = i;
                }
                else if (m_xSpellChecker.is())
                {
                    if (i == wordBoundary.endPos)
                        break; // syllable hasn't changed
                    // check dictionary for multisyllable word
                    assert(i <= aText.getLength());
                    ::rtl::OUString testWord(aText.getStr() + wordBoundary.startPos,
                                            i - wordBoundary.startPos);
                    css::beans::PropertyValues defaultProps;
                    if (m_xSpellChecker->isValid(testWord, m_locale, defaultProps))
                    {
                        wordBoundary.endPos = i;
                    }
                }
                else
                {
                    break;
                }
                if (i == aText.getLength() || !otm::MyanmarBreak::isMyanmar(aText[i+1]))
                    break;
            }
            if (nWordType == css::i18n::WordType::WORD_COUNT)
            {
                wordBoundary.startPos = punctuationStart;
                while ((wordBoundary.endPos < aText.getLength())
                    && isPunctuation(aText, wordBoundary.endPos))
                {
                    wordBoundary.endPos++;
                }
            }
#ifdef MMBI_DEBUG
            rtl::OString utf8String;
            aText.convertToString(&utf8String, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
            fprintf(stderr, "nextWord %d -> %d-%d type %d\n%s\n", nStartPos,
                    wordBoundary.startPos, wordBoundary.endPos,
                    nWordType, utf8String.getStr());
#endif
            return wordBoundary;
        }
    }
    wordBoundary = m_xBreakIteratorDelegate->nextWord(aText, nStartPos, aLocale, nWordType);
#ifdef MMBI_DEBUG
    fprintf(stderr, "default nextWord %d -> %d-%d type %d\n", nStartPos,
                wordBoundary.startPos, wordBoundary.endPos,
                nWordType);
#endif
    return wordBoundary;
}

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousWord(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    css::i18n::Boundary wordBoundary;
    int32_t i = nStartPos;
    if (nStartPos < aText.getLength())
    {
        if (otm::MyanmarBreak::isMyanmar(aText[i]))
        {
            while (!otm::MyanmarBreak::isBreak(aText, i) &&
                i > 0 && otm::MyanmarBreak::isMyanmar(aText[i-1]))
            {
                --i;
            }
            wordBoundary.endPos = i;
        }
        else
        {
            if (isWhiteSpace(aText, i))
            {
                while (i > 0 && isWhiteSpace(aText, i-1))
                    --i;
                wordBoundary.endPos = i;
            }
            else if (isPunctuation(aText, i))
            {
                while (i > 0 && isPunctuation(aText, i-1))
                    --i;
                wordBoundary.endPos = i;
            }
            else // not Myanmar
            {
                wordBoundary.endPos = i;
            }
        }
    }
    else
    {
        i = wordBoundary.endPos = aText.getLength();
    }
    int32_t punctuationEnd = wordBoundary.endPos;
    if (nWordType == css::i18n::WordType::ANYWORD_IGNOREWHITESPACES ||
        nWordType == css::i18n::WordType::DICTIONARY_WORD)
    {
        while (i > 0 && isWhiteSpace(aText, i-1))
        {
            --i;
            wordBoundary.endPos = i;
        }
    }
    else if (nWordType == css::i18n::WordType::WORD_COUNT)
    {
        punctuationEnd = wordBoundary.endPos;
        while (i > 0 && isPunctuation(aText, i-1))
        {
            --i;
            wordBoundary.endPos = i;
        }
    }
    if (i > 0 && otm::MyanmarBreak::isMyanmar(aText[i-1]) &&
        (nWordType == css::i18n::WordType::WORD_COUNT || !isPunctuation(aText, i-1)))
    {
        for (size_t syl = 0; syl < MAX_SYLLABLE_PER_WORD && i > 0; syl++)
        {
            --i;
            while (i > 0 &&
                otm::MyanmarBreak::isMyanmar(aText[i-1]) &&
                !otm::MyanmarBreak::isBreak(aText, i))
            {
                --i;
            }
            if (syl == 0)
            {
                wordBoundary.startPos = i;
            }
            else if (m_xSpellChecker.is())
            {
                if (i == wordBoundary.startPos)
                    break; // syllable hasn't changed
                // check dictionary for multisyllable word
                assert(i >= 0 && i <= wordBoundary.endPos);
                ::rtl::OUString testWord(aText.getStr() + i,
                                         wordBoundary.endPos - i);
                css::beans::PropertyValues defaultProps;
                if (m_xSpellChecker->isValid(testWord, m_locale, defaultProps))
                {
                    wordBoundary.startPos = i;
                }
            }
            else
            {
                break;
            }
            if (i == 0 || !otm::MyanmarBreak::isMyanmar(aText[i-1]))
                break;
        }
        if (nWordType == css::i18n::WordType::WORD_COUNT)
        {
            wordBoundary.endPos = punctuationEnd;
            while ((wordBoundary.startPos > 0)
                && (isPunctuation(aText, wordBoundary.startPos - 1)))
            {
                wordBoundary.startPos--;
            }
        }
#ifdef MMBI_DEBUG
        rtl::OString utf8String;
        aText.convertToString(&utf8String, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
        fprintf(stderr, "previousWord %d -> %d-%d type %d\n%s\n", nStartPos,
                wordBoundary.startPos, wordBoundary.endPos,
                nWordType, utf8String.getStr());
#endif
        return wordBoundary;
    }
    return m_xBreakIteratorDelegate->previousWord(aText, nStartPos, aLocale, nWordType);
}

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getWordBoundary(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType, ::sal_Bool bPreferForward) throw (css::uno::RuntimeException)
{
    css::i18n::Boundary wordBoundary;
    if (nPos >= 0 && nPos < aText.getLength())
    {
        int32_t mmStart = nPos;
        if (mmStart < 0) mmStart = 0;
        if (nWordType == css::i18n::WordType::ANYWORD_IGNOREWHITESPACES ||
            nWordType == css::i18n::WordType::DICTIONARY_WORD)
        {
            // if whitespace skip to nearest wordBoundary
            if (bPreferForward)
            {
                while (isWhiteSpace(aText, mmStart) && mmStart < aText.getLength())
                    ++mmStart;
            }
            else
            {
                while (isWhiteSpace(aText, mmStart) && mmStart > 0)
                    --mmStart;
            }
        }
        if ((mmStart > 0 && otm::MyanmarBreak::isMyanmar(aText[mmStart-1])) ||
            otm::MyanmarBreak::isMyanmar(aText[mmStart]))
        {
            // We need to search in both directions for a boundary, but we don't know where
            // the previous word starts, so to be safe, search from beginning of the
            // nearest continuous sequence of continuous Myanmar characters
            while ((mmStart > 0) && !isPunctuation(aText, mmStart-1) &&
                otm::MyanmarBreak::isMyanmar(aText[mmStart-1])) --mmStart;
            wordBoundary = nextWord(aText, mmStart-1, aLocale, nWordType);
            while (wordBoundary.endPos < nPos ||
                (bPreferForward && wordBoundary.endPos == nPos))
            {
#ifdef MMBI_DEBUG
                fprintf(stderr, "wb %d-%d ", wordBoundary.startPos, wordBoundary.endPos);
#endif
                // use end_pos-1 here, because nextWord always moves to start of
                // next word after specified position
                wordBoundary = nextWord(aText, wordBoundary.endPos-1, aLocale, nWordType);
            }
            if (nWordType == css::i18n::WordType::WORD_COUNT &&
                wordBoundary.startPos > 0 &&
                wordBoundary.startPos < aText.getLength() &&
                !isWhiteSpace(aText, wordBoundary.startPos))
            {
                // if it is a non-whitespace word append leading punctuation
                // in Word Count mode
                while (isPunctuation(aText, wordBoundary.startPos-1))
                    wordBoundary.startPos--;
            }
#ifdef MMBI_DEBUG
            rtl::OString utf8String;
            aText.convertToString(&utf8String, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
            fprintf(stderr, "getWordBoundary %d -> %d-%d type %d\n%s\n", nPos,
                    wordBoundary.startPos, wordBoundary.endPos, 
                    nWordType, utf8String.getStr());
#endif
            return wordBoundary;
        }
    }
    return m_xBreakIteratorDelegate->getWordBoundary(aText, nPos, aLocale, nWordType, bPreferForward);
}

::sal_Int16 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getWordType(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->getWordType(aText, nPos, aLocale);
}

::sal_Bool SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::isBeginWord(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    sal_Bool isBegin = m_xBreakIteratorDelegate->isBeginWord(aText, nPos, aLocale, nWordType);

    if (!isBegin && nPos > 0 && nPos < aText.getLength() &&
        otm::MyanmarBreak::isMyanmar(aText[nPos-1]) &&
        otm::MyanmarBreak::isMyanmar(aText[nPos]) &&
        otm::MyanmarBreak::isBreak(aText, nPos))
    {
        // the position is a syllable break, so check if it is mid-word by going back to
        // start of Myanmar text
        int32_t i = nPos;
        while (i > 0 && otm::MyanmarBreak::isMyanmar(aText[i-1]) &&
            !isPunctuation(aText, i-1)) --i;
        // now use nextWord
        css::i18n::Boundary b = nextWord(aText, i-1, aLocale, nWordType);
        while (b.endPos < nPos && b.startPos != b.endPos)
        {
            b = nextWord(aText, b.endPos-1, aLocale, nWordType);
        }
        if (b.endPos == nPos)
            isBegin = true;
#ifdef MMBI_DEBUG
        fprintf(stderr, "MM BeginWord at %d = %d type %d\n", nPos, isBegin, nWordType);
#endif
    }
#ifdef MMBI_DEBUG
    fprintf(stderr, "BeginWord at %d = %d\n", nPos, isBegin);
#endif
    return isBegin;
}

::sal_Bool SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::isEndWord(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    sal_Bool isEnd = m_xBreakIteratorDelegate->isEndWord(aText, nPos, aLocale, nWordType);
    if (!isEnd && nPos > 0 && nPos < aText.getLength() &&
        otm::MyanmarBreak::isMyanmar(aText[nPos-1]) &&
        otm::MyanmarBreak::isMyanmar(aText[nPos]) &&
        otm::MyanmarBreak::isBreak(aText, nPos))
    {
        // the position is a syllable break, so check if it is mid-word by going back to
        // start of Myanmar text
        int32_t i = nPos;
        while (i > 0 && otm::MyanmarBreak::isMyanmar(aText[i-1]) &&
            !isPunctuation(aText, i-1)) --i;
        // now use nextWord
        css::i18n::Boundary b = nextWord(aText, i-1, aLocale, nWordType);
        while (b.endPos < nPos && b.startPos != b.endPos)
        {
            b = nextWord(aText, b.endPos-1, aLocale, nWordType);
        }
        if (b.endPos == nPos)
            isEnd = true;
    }
#ifdef MMBI_DEBUG
    fprintf(stderr, "EndWord at %d = %d\n", nPos, isEnd);
#endif
    return isEnd;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::beginOfSentence(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->beginOfSentence(aText, nStartPos, aLocale);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::endOfSentence(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->endOfSentence(aText, nStartPos, aLocale);
}

css::i18n::LineBreakResults SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getLineBreak(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int32 nMinBreakPos, const css::i18n::LineBreakHyphenationOptions & aHyphOptions, const css::i18n::LineBreakUserOptions & aUserOptions) throw (css::uno::RuntimeException)
{
    css::i18n::LineBreakResults result;
    css::i18n::LineBreakResults prevResult;
    // hyphenator isn't relevant for Myanmar

    // the normalbreakiterator will be earlier than a non-space word break
    result = m_xBreakIteratorDelegate->getLineBreak(aText, nStartPos, aLocale,
                                                  nMinBreakPos, aHyphOptions, aUserOptions);
    prevResult = result;
    // now try to see if there is a break nearer to the start position
    if (result.breakIndex > -1 && otm::MyanmarBreak::isMyanmar(aText[result.breakIndex]))
    {
        css::i18n::Boundary b = nextWord(aText, result.breakIndex - 1, aLocale,
                                         css::i18n::WordType::DICTIONARY_WORD);
        while (b.endPos <= nStartPos)
        {
            prevResult = result;
            result.breakIndex = b.endPos;
            result.breakType = css::i18n::BreakType::WORDBOUNDARY;
            // use word count, since trailing punctuation should be on previous line
            b = nextWord(aText, b.endPos - 1, aLocale, css::i18n::WordType::WORD_COUNT);
        }
        // append trailing whitespace
        while (result.breakIndex < aText.getLength() &&
            isWhiteSpace(aText, result.breakIndex))
        {
            result.breakIndex++;
            if (result.breakIndex > nStartPos)
            {
                result = prevResult;
                break;
            }
        }
#ifdef MMBI_DEBUG
        rtl::OString utf8String;
        aText.convertToString(&utf8String, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
        fprintf(stderr, "getLineBreak start %d min break %d break-pos %d\n%s\n",
            nStartPos, nMinBreakPos, result.breakIndex, utf8String.getStr());
#endif
    }

    return result;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::beginOfScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->beginOfScript(aText, nStartPos, nScriptType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::endOfScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->endOfScript(aText, nStartPos, nScriptType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->nextScript(aText, nStartPos, nScriptType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->previousScript(aText, nStartPos, nScriptType);
}

::sal_Int16 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getScriptType(const ::rtl::OUString & aText, ::sal_Int32 nPos) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->getScriptType(aText, nPos);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::beginOfCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
#ifdef MMBI_DEBUG
    fprintf(stderr, "beginOfCharBlock %d type %d\n", nStartPos, nCharType);
#endif
    return m_xBreakIteratorDelegate->beginOfCharBlock(aText, nStartPos, aLocale, nCharType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::endOfCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
#ifdef MMBI_DEBUG
    fprintf(stderr, "endOfCharBlock %d type %d\n", nStartPos, nCharType);
#endif
    return m_xBreakIteratorDelegate->endOfCharBlock(aText, nStartPos, aLocale, nCharType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
#ifdef MMBI_DEBUG
    fprintf(stderr, "nextCharBlock %d type %d\n", nStartPos, nCharType);
#endif
    return m_xBreakIteratorDelegate->nextCharBlock(aText, nStartPos, aLocale, nCharType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
#ifdef MMBI_DEBUG
    fprintf(stderr, "previousCharBlock %d type %d\n", nStartPos, nCharType);
#endif
    return m_xBreakIteratorDelegate->previousCharBlock(aText, nStartPos, aLocale, nCharType);
}

}}}} // closing anonymous implementation namespace



// component helper namespace
namespace org { namespace thanlwinsoft { namespace ooo { namespace my { namespace myanmarbreakiterator {

::rtl::OUString SAL_CALL _getImplementationName() {
    return ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
        "org.thanlwinsoft.ooo.my.MyanmarBreakIterator"));
}

css::uno::Sequence< ::rtl::OUString > SAL_CALL _getSupportedServiceNames()
{
    css::uno::Sequence< ::rtl::OUString > s(2);
    s[0] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
        "com.sun.star.frame.ProtocolHandler"));
    s[1] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
        "com.sun.star.i18n.BreakIterator_my"));
    return s;
}

SAL_DLLPUBLIC_EXPORT
css::uno::Reference< css::uno::XInterface > SAL_CALL _create(
    const css::uno::Reference< css::uno::XComponentContext > & context)
        SAL_THROW((css::uno::Exception))
{
    return static_cast< ::cppu::OWeakObject * >(new org::thanlwinsoft::ooo::my::MyanmarBreakIterator(context));
}

}}}}} // closing component helper namespace

static ::cppu::ImplementationEntry const entries[] = {
    { &org::thanlwinsoft::ooo::my::myanmarbreakiterator::_create,
      &org::thanlwinsoft::ooo::my::myanmarbreakiterator::_getImplementationName,
      &org::thanlwinsoft::ooo::my::myanmarbreakiterator::_getSupportedServiceNames,
      &::cppu::createSingleComponentFactory, 0, 0 },
    { 0, 0, 0, 0, 0, 0 }
};

extern "C" {

SAL_DLLPUBLIC_EXPORT
void SAL_CALL component_getImplementationEnvironment(
    const char ** envTypeName, uno_Environment **)
{
    *envTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

SAL_DLLPUBLIC_EXPORT
void * SAL_CALL component_getFactory(
    const char * implName, void * serviceManager, void * registryKey)
{
    return ::cppu::component_getFactoryHelper(
        implName, serviceManager, registryKey, entries);
}

SAL_DLLPUBLIC_EXPORT
sal_Bool SAL_CALL component_writeInfo(
    void * serviceManager, void * registryKey)
{
    return ::cppu::component_writeInfoHelper(serviceManager, registryKey, entries);
}

} // extern "C"
