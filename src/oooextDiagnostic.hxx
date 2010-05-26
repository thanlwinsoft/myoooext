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
#ifndef oooextDiagnostic_hxx
#define oooextDiagnostic_hxx

#include "sal/config.h"
#include "rtl/string.hxx"

#include "com/sun/star/beans/XPropertySet.hpp"
#include "com/sun/star/uno/XInterface.hpp"
#include "com/sun/star/uno/Any.hxx"


namespace org { namespace thanlwinsoft { namespace ooo {

void printStringSequence(::com::sun::star::uno::Sequence< ::rtl::OUString > stringSeq);
void printPropertyNames(::com::sun::star::uno::Reference<
    ::com::sun::star::beans::XPropertySet > propSet);
void printServiceNames(::com::sun::star::uno::Reference<
    ::com::sun::star::uno::XInterface> xInterface);

}}}

#endif
