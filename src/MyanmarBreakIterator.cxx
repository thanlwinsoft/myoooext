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
//#include "com/sun/star/linguistic2/XDictionary.hpp"
//#include "com/sun/star/linguistic2/XDictionaryEntry.hpp"
#include "com/sun/star/linguistic2/XSpellChecker.hpp"
#include "com/sun/star/linguistic2/XLinguServiceManager.hpp"
#include "com/sun/star/frame/XFrame.hpp"

#include "MyanmarBreak.hxx"
#include "oooextDiagnostic.hxx"

// component helper namespace
namespace org { namespace thanlwinsoft { namespace ooo { namespace my { namespace myanmarbreakiterator {

namespace css = ::com::sun::star;

// component and service helper functions:
::rtl::OUString SAL_CALL _getImplementationName();
css::uno::Sequence< ::rtl::OUString > SAL_CALL _getSupportedServiceNames();
css::uno::Reference< css::uno::XInterface > SAL_CALL _create( css::uno::Reference< css::uno::XComponentContext > const & context );

}}}}} // closing component helper namespace



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
    fprintf(stderr, "MyanmarBreakIterator\n");
    assert(context.is());
    css::uno::Reference<css::lang::XMultiComponentFactory>
        xFactory(m_xContext->getServiceManager());
    if (xFactory.is())
    {
        printServiceNames(xFactory);

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
            fprintf(stderr, "have LinguServiceManager\n");
            ::rtl::OUString spellChecker = ::rtl::OUString::createFromAscii("com.sun.star.linguistic2.SpellChecker");
            css::uno::Sequence< ::rtl::OUString> mySpellCheckers =
                xLinguServiceManager->getConfiguredServices(spellChecker, m_locale);
            printStringSequence(mySpellCheckers);
            if (mySpellCheckers.getLength() > 0)
            {
                m_xSpellChecker.set(xLinguServiceManager->getSpellChecker());
                if (m_xSpellChecker.is())
                {
                    fprintf(stderr, "have spellchecker\n");
                }
            }
        }
    }
    css::uno::Reference<css::lang::XMultiServiceFactory>
        xServiceFactory(m_xContext->getServiceManager(), css::uno::UNO_QUERY);
    if (xServiceFactory.is())
    {
        fprintf(stderr, "have multi service factory\n");
        // printStringSequence(xServiceFactory->getAvailableServiceNames());
        ::rtl::OUString biCtl =
            ::rtl::OUString::createFromAscii("com.sun.star.i18n.BreakIterator_Unicode");
        m_xBreakIteratorDelegate.set(xFactory->createInstanceWithContext(biCtl, context),
                                     css::uno::UNO_QUERY);
        if (m_xBreakIteratorDelegate.is())
            fprintf(stderr, "have default Unicode iterator\n");
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
    return m_xBreakIteratorDelegate->nextCharacters(aText, nStartPos, aLocale,
                                                    nCharacterIteratorMode,
                                                    nCount, nDone);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousCharacters(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharacterIteratorMode, ::sal_Int32 nCount, ::sal_Int32 & nDone) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->previousCharacters(aText, nStartPos, aLocale,
                                                        nCharacterIteratorMode, nCount, nDone);
}

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextWord(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    css::i18n::Boundary wordBoundary;
    // if nStartPos is the start of a word, we need to move to the next word start
    // otherwise Ctrl+Right doesn't move the cursor
    if (nStartPos < aText.getLength() &&
        ((m_xCharClassification->getType(aText, nStartPos) ==
         css::i18n::UnicodeType::SPACE_SEPARATOR) ||
        otm::MyanmarBreak::isMyanmar(aText[nStartPos])))
    {
        int32_t i = nStartPos;
        // skip over spaces to next word
        if (nWordType == css::i18n::WordType::ANYWORD_IGNOREWHITESPACES ||
            nWordType == css::i18n::WordType::WORD_COUNT)
        {
            while (i < aText.getLength() &&
                m_xCharClassification->getType(aText, i) ==
                css::i18n::UnicodeType::SPACE_SEPARATOR)
                ++i;
            if (i > nStartPos)
                wordBoundary.startPos = i;
        }
        if (i == nStartPos)
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
                    // check dictionary for multisyllable word
                    ::rtl::OUString testWord(aText.getStr() + wordBoundary.startPos,
                                             i - wordBoundary.startPos);
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
            }
        }
        i = wordBoundary.startPos;
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
                wordBoundary.endPos = i;
            }
            else if (m_xSpellChecker.is())
            {
                if (i == wordBoundary.endPos)
                    break; // syllable hasn't changed
                // check dictionary for multisyllable word
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
        }
        if (nWordType == css::i18n::WordType::WORD_COUNT)
        {
            while ((wordBoundary.endPos < aText.getLength())
                && (m_xCharClassification->getType(aText, wordBoundary.endPos) ==
                    css::i18n::UnicodeType::SPACE_SEPARATOR))
            {
                wordBoundary.endPos++;
            }
        }
        rtl::OString utf8String;
        aText.convertToString(&utf8String, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
        fprintf(stderr, "nextWord %d -> %d-%d type %d\n%s\n", nStartPos,
                wordBoundary.startPos, wordBoundary.endPos,
                nWordType, utf8String.getStr());
        return wordBoundary;
    }
    wordBoundary = m_xBreakIteratorDelegate->nextWord(aText, nStartPos, aLocale, nWordType);
    fprintf(stderr, "default nextWord %d -> %d-%d type %d\n", nStartPos,
                wordBoundary.startPos, wordBoundary.endPos,
                nWordType);
    return wordBoundary;
}

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousWord(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    if (nStartPos > 0 &&
        otm::MyanmarBreak::isMyanmar(aText[nStartPos-1]))
    {
        css::i18n::Boundary wordBoundary;
        wordBoundary.endPos = nStartPos;
        int32_t i = nStartPos - 1;
        for (size_t syl = 0; syl < MAX_SYLLABLE_PER_WORD; syl++)
        {
            --i;
            while (i >= 0 &&
                otm::MyanmarBreak::isMyanmar(aText[i]) &&
                !otm::MyanmarBreak::isBreak(aText, i+1))
            {
                --i;
            }
            if (syl == 0)
            {
                wordBoundary.startPos = i + 1;
            }
            else if (m_xSpellChecker.is())
            {
                if (i == wordBoundary.startPos)
                    break; // syllable hasn't changed
                // check dictionary for multisyllable word
                ::rtl::OUString testWord(aText.getStr() + i + 1,
                                         wordBoundary.endPos - i - 1);
                css::beans::PropertyValues defaultProps;
                if (m_xSpellChecker->isValid(testWord, m_locale, defaultProps))
                {
                    wordBoundary.startPos = i + 1;
                }
            }
            else
            {
                break;
            }
        }
        if (nWordType == css::i18n::WordType::WORD_COUNT)
        {
            while ((wordBoundary.endPos < aText.getLength())
                && (m_xCharClassification->getType(aText, wordBoundary.endPos) ==
                    css::i18n::UnicodeType::SPACE_SEPARATOR))
            {
                wordBoundary.endPos++;
            }
        }
        rtl::OString utf8String;
        aText.convertToString(&utf8String, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
        fprintf(stderr, "previousWord %d -> %d-%d type %d\n%s\n", nStartPos,
                wordBoundary.startPos, wordBoundary.endPos,
                nWordType, utf8String.getStr());
        return wordBoundary;
    }
    return m_xBreakIteratorDelegate->previousWord(aText, nStartPos, aLocale, nWordType);
}

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getWordBoundary(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType, ::sal_Bool bPreferForward) throw (css::uno::RuntimeException)
{
    css::i18n::Boundary wordBoundary;
    if (nPos >= 0 && nPos < aText.getLength())
    {
        sal_Int16 cType = m_xCharClassification->getType(aText, nPos);
        sal_Int16 prevType = (nPos > 0)? m_xCharClassification->getType(aText, nPos-1)
            : css::i18n::UnicodeType::UNASSIGNED;

        if ((nPos > 0 && otm::MyanmarBreak::isMyanmar(aText[nPos-1])) ||
            otm::MyanmarBreak::isMyanmar(aText[nPos]))
        {
            if (nPos > 0 && (otm::MyanmarBreak::isBreak(aText, nPos) ||
                cType == css::i18n::UnicodeType::SPACE_SEPARATOR ||
                prevType == css::i18n::UnicodeType::SPACE_SEPARATOR))
            {
                if (bPreferForward)
                {
                    wordBoundary = nextWord(aText, nPos-1, aLocale, nWordType);
                }
                else
                {
                    wordBoundary = previousWord(aText, nPos, aLocale, nWordType);
                }
            }
            else
            {
                // We need to search in both directions for a boundary, but we don't know where
                // the previous word starts, so to be safe, search from beginning of the
                // nearest continuous sequence of continuous Myanmar characters
                int32_t mmStart = nPos;
                while ((mmStart > 0) &&
                    otm::MyanmarBreak::isMyanmar(aText[mmStart-1])) --mmStart;
                // the first word boundary would be skipped if nextword was used
                // so find the first word directly here
                wordBoundary.startPos = mmStart;
                int32_t i = mmStart;
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
                        wordBoundary.endPos = i;
                    }
                    else if (m_xSpellChecker.is())
                    {
                        if (i == wordBoundary.endPos)
                            break; // syllable hasn't changed
                        // check dictionary for multisyllable word
                        ::rtl::OUString testWord(aText.getStr() + mmStart, i - mmStart);
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
                }
                while (wordBoundary.endPos < nPos)
                {
                    fprintf(stderr, "wb %d-%d ", wordBoundary.startPos, wordBoundary.endPos);
                    // use start pos here, because nextWord always moves to start of
                    // next word after startPos
                    wordBoundary = nextWord(aText, wordBoundary.startPos, aLocale, nWordType);
                }
            }
            rtl::OString utf8String;
            aText.convertToString(&utf8String, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
            fprintf(stderr, "getWordBoundary %d -> %d-%d type %d\n%s\n", nPos,
                    wordBoundary.startPos, wordBoundary.endPos, 
                    nWordType, utf8String.getStr());
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
    if (otm::MyanmarBreak::isMyanmar(aText[nPos]))
    {
        if (nPos == 0) return sal_True;
        if (nPos > 0 && otm::MyanmarBreak::isMyanmar(aText[nPos-1]))
        {
            if (otm::MyanmarBreak::isBreak(aText, nPos))
            {
                // TODO Check dictionary?
                fprintf(stderr, "Begin Myanmar BeginWord at %d\n", nPos);
                return sal_True;
            }
            else
            {
                fprintf(stderr, "Begin Myanmar BeginWord at %d - no\n", nPos);
                return sal_False;
            }
        }
    }
    return m_xBreakIteratorDelegate->isBeginWord(aText, nPos, aLocale, nWordType);
}

::sal_Bool SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::isEndWord(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    if (otm::MyanmarBreak::isMyanmar(aText[nPos]))
    {
        if (nPos == 0) return sal_True;
        if (nPos > 0 && otm::MyanmarBreak::isMyanmar(aText[nPos-1]))
        {
            if (otm::MyanmarBreak::isBreak(aText, nPos))
            {
                // TODO Check dictionary?
                fprintf(stderr, "Myanmar EndWord at %d\n", nPos);
                return sal_True;
            }
            else
            {
                fprintf(stderr, "Myanmar EndWord at %d - no\n", nPos);
                return sal_False;
            }
        }
    }
    return m_xBreakIteratorDelegate->isEndWord(aText, nPos, aLocale, nWordType);
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
    // hyphenator isn't relevant for Myanmar

    // the normalbreakiterator will be earlier than a non-space word break
    result = m_xBreakIteratorDelegate->getLineBreak(aText, nStartPos, aLocale,
                                                  nMinBreakPos, aHyphOptions, aUserOptions);

    // now try to see if there is a break nearer to the start position
    if (result.breakIndex > -1 && otm::MyanmarBreak::isMyanmar(aText[result.breakIndex]))
    {
        css::i18n::Boundary b = nextWord(aText, result.breakIndex, aLocale,
                                         css::i18n::WordType::DICTIONARY_WORD);
        if (b.endPos <= nStartPos)
        {
            result.breakIndex = b.endPos;
            result.breakType = css::i18n::BreakType::WORDBOUNDARY;
        }
        rtl::OString utf8String;
        aText.convertToString(&utf8String, RTL_TEXTENCODING_UTF8, OUSTRING_TO_OSTRING_CVTFLAGS);
        fprintf(stderr, "getLineBreak start %d min break %d break-pos %d\n%s\n",
            nStartPos, nMinBreakPos, result.breakIndex, utf8String.getStr());
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
    return m_xBreakIteratorDelegate->beginOfCharBlock(aText, nStartPos, aLocale, nCharType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::endOfCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->endOfCharBlock(aText, nStartPos, aLocale, nCharType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->nextCharBlock(aText, nStartPos, aLocale, nCharType);
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
    return m_xBreakIteratorDelegate->previousCharBlock(aText, nStartPos, aLocale, nCharType);
}

}}}} // closing anonymous implementation namespace



// component helper namespace
namespace org { namespace thanlwinsoft { namespace ooo { namespace my { namespace myanmarbreakiterator {

::rtl::OUString SAL_CALL _getImplementationName() {
    return ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
        "org::thanlwinsoft::ooo::my::MyanmarBreakIterator"));
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

extern "C" void SAL_CALL component_getImplementationEnvironment(
    const char ** envTypeName, uno_Environment **)
{
    *envTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

extern "C" void * SAL_CALL component_getFactory(
    const char * implName, void * serviceManager, void * registryKey)
{
    return ::cppu::component_getFactoryHelper(
        implName, serviceManager, registryKey, entries);
}

extern "C" sal_Bool SAL_CALL component_writeInfo(
    void * serviceManager, void * registryKey)
{
    return ::cppu::component_writeInfoHelper(serviceManager, registryKey, entries);
}
