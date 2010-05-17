/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
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

#include "sal/config.h"
#include "cppuhelper/factory.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "cppuhelper/implbase5.hxx"
#include "com/sun/star/lang/XInitialization.hpp"
#include "com/sun/star/frame/XDispatch.hpp"
#include "com/sun/star/frame/XDispatchProvider.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/i18n/XBreakIterator.hpp"
#include "com/sun/star/frame/XFrame.hpp"


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
};

MyanmarBreakIterator::MyanmarBreakIterator(css::uno::Reference< css::uno::XComponentContext > const & context) :
    m_xContext(context)
{}

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
void SAL_CALL MyanmarBreakIterator::dispatch( const css::util::URL& aURL, const css::uno::Sequence< css::beans::PropertyValue >& aArguments ) throw(css::uno::RuntimeException)
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

void SAL_CALL MyanmarBreakIterator::addStatusListener( const css::uno::Reference< css::frame::XStatusListener >& xControl, const css::util::URL& aURL ) throw (css::uno::RuntimeException)
{
    // add your own code here
}

void SAL_CALL MyanmarBreakIterator::removeStatusListener( const css::uno::Reference< css::frame::XStatusListener >& xControl, const css::util::URL& aURL ) throw (css::uno::RuntimeException)
{
    // add your own code here
}

// ::com::sun::star::frame::XDispatchProvider:
css::uno::Reference< css::frame::XDispatch > SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::queryDispatch( const css::util::URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags ) throw(css::uno::RuntimeException)
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
    // TODO: Exchange the default return implementation for "nextCharacters" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousCharacters(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharacterIteratorMode, ::sal_Int32 nCount, ::sal_Int32 & nDone) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "previousCharacters" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextWord(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "nextWord" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return css::i18n::Boundary();
}

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousWord(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "previousWord" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return css::i18n::Boundary();
}

css::i18n::Boundary SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getWordBoundary(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType, ::sal_Bool bPreferForward) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "getWordBoundary" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return css::i18n::Boundary();
}

::sal_Int16 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getWordType(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "getWordType" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Bool SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::isBeginWord(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "isBeginWord" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return sal_False;
}

::sal_Bool SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::isEndWord(const ::rtl::OUString & aText, ::sal_Int32 nPos, const css::lang::Locale & aLocale, ::sal_Int16 nWordType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "isEndWord" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return sal_False;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::beginOfSentence(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "beginOfSentence" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::endOfSentence(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "endOfSentence" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

css::i18n::LineBreakResults SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getLineBreak(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int32 nMinBreakPos, const css::i18n::LineBreakHyphenationOptions & aHyphOptions, const css::i18n::LineBreakUserOptions & aUserOptions) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "getLineBreak" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return css::i18n::LineBreakResults();
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::beginOfScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "beginOfScript" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::endOfScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "endOfScript" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "nextScript" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousScript(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, ::sal_Int16 nScriptType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "previousScript" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int16 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::getScriptType(const ::rtl::OUString & aText, ::sal_Int32 nPos) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "getScriptType" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::beginOfCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "beginOfCharBlock" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::endOfCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "endOfCharBlock" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::nextCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "nextCharBlock" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
}

::sal_Int32 SAL_CALL org::thanlwinsoft::ooo::my::MyanmarBreakIterator::previousCharBlock(const ::rtl::OUString & aText, ::sal_Int32 nStartPos, const css::lang::Locale & aLocale, ::sal_Int16 nCharType) throw (css::uno::RuntimeException)
{
    // TODO: Exchange the default return implementation for "previousCharBlock" !!!
    // Exchange the default return implementation.
    // NOTE: Default initialized polymorphic structs can cause problems because of
    // missing default initialization of primitive types of some C++ compilers or
    // different Any initialization in Java and C++ polymorphic structs.
    return 0;
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
