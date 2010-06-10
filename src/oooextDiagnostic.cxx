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

#include <cstring>
#include <cstdio>

#include "oooextDiagnostic.hxx"

#include "uno/lbnames.h"
#include "rtl/strbuf.hxx"

#include "cppuhelper/implementationentry.hxx"
#include "com/sun/star/deployment/XPackageInformationProvider.hpp"
#include "com/sun/star/deployment/PackageInformationProvider.hpp"
#include "com/sun/star/resource/StringResourceWithLocation.hpp"
#include "com/sun/star/text/XTextViewCursor.hpp"
#include "com/sun/star/text/XTextViewCursorSupplier.hpp"
#include "com/sun/star/lang/XComponent.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"

namespace css = ::com::sun::star;
namespace oto = ::org::thanlwinsoft::ooo;

::rtl::OString oto::unicodeToHex(::rtl::OUString unicode)
{
    ::rtl::OStringBuffer buffer(unicode.getLength() * 6);
    for (int i = 0; i < unicode.getLength(); i++)
    {
        if (unicode[i] > 32 && unicode[i] < 128)
            buffer.append(static_cast<char>(unicode[i]));
        else
        {
            buffer.append("\\u");
            buffer.append(static_cast<sal_Int32>(unicode[i]), 16);
        }
    }
    return buffer.makeStringAndClear();
}

void oto::printStringSequence(::com::sun::star::uno::Sequence< ::rtl::OUString> stringSeq)
{
    ::rtl::OString aServiceName;
    for (int i = 0; i < stringSeq.getLength(); i++)
    {
        stringSeq[i].convertToString(&aServiceName, RTL_TEXTENCODING_UTF8, 128);
        fprintf(stderr, "%s\n", aServiceName.getStr());
    }
}

void oto::printPropertyNames(css::uno::Reference<css::beans::XPropertySet > propSet)
{
    css::uno::Reference< css::beans::XPropertySetInfo>xPropSetInfo(propSet.get()->getPropertySetInfo());
    css::uno::Sequence< css::beans::Property> properties = xPropSetInfo.get()->getProperties();
    for (int i = 0; i < properties.getLength(); i++)
    {
        ::rtl::OString propName;
        ::rtl::OString propValue;
        properties[i].Name.convertToString(&propName, RTL_TEXTENCODING_UTF8, 128);
        try
        {
            ::css::uno::Any aValue = propSet.get()->getPropertyValue(properties[i].Name);
            if (aValue.hasValue() && aValue.has< ::rtl::OUString>())
            {
                ::rtl::OUString value = aValue.get< ::rtl::OUString>();
                value.convertToString(&propValue, RTL_TEXTENCODING_UTF8, propValue.getLength());
            }
            else
            {
                propValue = "?";
            }
        }
        catch (::com::sun::star::lang::WrappedTargetException we)
        {
            css::uno::Exception e = we.TargetException.get<css::uno::Exception>();
            e.Message.convertToString(&propValue, RTL_TEXTENCODING_UTF8, propValue.getLength());
        }
        catch (::com::sun::star::uno::Exception e)
        {
            e.Message.convertToString(&propValue, RTL_TEXTENCODING_UTF8, propValue.getLength());
        }
        fprintf(stderr, "Property name:%s value:%s\n", propName.getStr(), propValue.getStr());
    }
}

void oto::printServiceNames(::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> xInterface)
{
    css::uno::Reference<css::lang::XServiceInfo> xServiceInfo(xInterface, css::uno::UNO_QUERY);
    if (xServiceInfo.is())
    {
        ::rtl::OString aServiceName;
        css::uno::Sequence< ::rtl::OUString> serviceNames =
            xServiceInfo->getSupportedServiceNames();
        for (int i = 0; i < serviceNames.getLength(); i++)
        {
            serviceNames[i].convertToString(&aServiceName, RTL_TEXTENCODING_UTF8, 128);
            fprintf(stderr, "service: %s\n", aServiceName.getStr());
        }
    }
}
