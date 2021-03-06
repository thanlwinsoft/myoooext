#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2010 ThanLwinSoft.org
#
# This file is part of the ThanLwinSoft Myanmar extension for OpenOffice.org.
#
# The ThanLwinSoft Myanmar Extension is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# The GraphiteOOo Extension is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)
project (openoffice.org-my)
enable_language(CXX C)

set(EXTENSION_NAME thanlwinsoft-ooo-myanmar)
set(EXTENSION_ID org.thanlwinsoft.ooo.my)
set(EXT_VERSION_MAJOR 0)
set(EXT_VERSION_MINOR 1)
set(EXT_VERSION_PATCH 0)
if (EXISTS ${PROJECT_SOURCE_DIR}/.hg)
	execute_process(COMMAND hg id -n OUTPUT_VARIABLE MERCURIAL_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "+" "" MERCURIAL_VERSION ${MERCURIAL_VERSION})
    set(EXT_VERSION_PATCH ${MERCURIAL_VERSION})
endif (EXISTS ${PROJECT_SOURCE_DIR}/.hg)

# add the binary tree to the search path for include files
include_directories("${PROJECT_BINARY_DIR}")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-DUNX=1 -DLINUX=1 -DCPPU_ENV=gcc3 -Wall 
        -Wno-unknown-pragmas -Wparentheses -Wextra -Wendif-labels -Wshadow
        -Wno-ctor-dtor-privacy -Wno-non-virtual-dtor)
    execute_process(COMMAND uname -m
        OUTPUT_VARIABLE SYSTEM_MACHINE OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (${SYSTEM_MACHINE} STREQUAL "i686")
        set(EXTENSION_PLATFORM linux_x86)
    elseif (${SYSTEM_MACHINE} STREQUAL "x86_64")
        set(EXTENSION_PLATFORM linux_x86_64)
    endif (${SYSTEM_MACHINE} STREQUAL "i686")
    set(PATH_SEPARATOR ":")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(CMAKE_CXX_FLAGS_DEBUG /MTd)
    set(CMAKE_CXX_FLAGS_RELEASE /MT)
    set(CMAKE_CXX_FLAGS_MINSIZEREL /MT)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO /MT)
    add_definitions(-DWNT=1 -DUNICODE -DCPPU_ENV=msci /wd4290 /wd4800 /W4)
    set(EXTENSION_PLATFORM windows_${CMAKE_SYSTEM_PROCESSOR})
    set(PATH_SEPARATOR ";")
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

set(OOO_VERSION 3.2) # used for basis directory

# OO_SDK_URE_HOME is setup using the setsdkenv_* scripts
find_path(OO_SDK_IDL com/sun/star/udk-modules.idl
    PATHS ENV OO_SDK_HOME /usr/lib/openoffice/basis${OOO_VERSION}/sdk
    PATH_SUFFIXES idl DOC "OO IDL directory")
find_path(OO_SDK_INCLUDE udkversion.mk
    PATHS ENV OO_SDK_HOME /usr/lib/openoffice/basis${OOO_VERSION}/sdk
    PATH_SUFFIXES include DOC "OO SDK include directory")
find_library(OO_SDK_UNO_SAL NAMES uno_sal isal 
    PATHS ENV OO_SDK_HOME /usr/lib/openoffice/basis${OOO_VERSION}/sdk
    PATH_SUFFIXES lib DOC "SAL library")
find_library(OO_SDK_UNO_CPPU NAMES uno_cppu icppu 
    PATHS ENV OO_SDK_HOME /usr/lib/openoffice/basis${OOO_VERSION}/sdk
    PATH_SUFFIXES lib DOC "CPPU library")
find_library(OO_SDK_UNO_CPPUHELPER NAMES uno_cppuhelpergcc3 icppuhelper 
    PATHS ENV OO_SDK_HOME /usr/lib/openoffice/basis${OOO_VERSION}/sdk
    PATH_SUFFIXES lib DOC "CPPU helper library")

find_program(OO_SDK_CPPUMAKER cppumaker 
    PATHS ENV OO_SDK_HOME /usr/lib/openoffice/basis${OOO_VERSION}/sdk
    PATH_SUFFIXES bin DOC "IDL compiler")
find_path(OO_SDK_RDB types.rdb 
    PATHS ENV OO_SDK_URE_HOME ENV OO_SDK_HOME /usr/lib/ure 
    PATH_SUFFIXES share/misc misc DOC "URE RDB directory")
find_path(OO_BASIS_PROGRAM offapi.rdb
    PATHS ENV OO_SDK_HOME /usr/lib/openoffice/basis${OOO_VERSION}/program
    PATH_SUFFIXES ../program DOC "Basis Program directory")
find_path(OO_SDK_URE_BIN version.ini versionrc 
    PATHS ENV OO_SDK_URE_HOME /usr/lib/ure 
    PATH_SUFFIXES bin DOC "URE bin directory")
find_file(OO_SDK_VERSION version.ini versionrc
    PATHS ENV OO_SDK_URE_HOME /usr/lib/ure
    PATH_SUFFIXES bin DOC "URE version file")
find_program(UNOPKG unopkg PATHS ENV OFFICE_PROGRAM_PATH /usr/bin
    DOC "OpenOffice.org Extension Manager")
find_program(ZIP zip)

# check if unopkg supports the -s option
file(STRINGS ${OO_SDK_VERSION} OO_VERSION_STRING REGEX "OOOBaseVersion=[0-9.]+")
file(STRINGS ${OO_SDK_VERSION} OO_PRODUCT_MAJOR_STRING REGEX "ProductMajor=[0-9.]+")
file(STRINGS ${OO_SDK_VERSION} OO_PRODUCT_MINOR_STRING REGEX "ProductMinor=[0-9.]+")
string(REPLACE "OOOBaseVersion=" "" OO_VERSION ${OO_VERSION_STRING})
string(REPLACE "ProductMajor=" "" OO_MAJOR_VERSION ${OO_PRODUCT_MAJOR_STRING})
string(REPLACE "ProductMinor=" "" OO_MINOR_VERSION ${OO_PRODUCT_MINOR_STRING})
if (${OO_VERSION} GREATER 3.2)
    set(UNOPKG_OPTIONS -s)
else (${OO_VERSION} GREATER 3.2) # not sure about the exact minor version to use
	if ((${OO_MAJOR_VERSION} EQUAL 320) AND (${OO_MINOR_VERSION} GREATER 16))
		set(UNOPKG_OPTIONS -s)
	else ((${OO_MAJOR_VERSION} EQUAL 320) AND (${OO_MINOR_VERSION} GREATER 16))
		set(UNOPKG_OPTIONS "")
	endif ((${OO_MAJOR_VERSION} EQUAL 320) AND (${OO_MINOR_VERSION} GREATER 16))
endif (${OO_VERSION} GREATER 3.2)

include_directories ("${OO_SDK_INCLUDE}")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-DUNX=1 -DLINUX=1 -DCPPU_ENV=gcc3 -Wall)
    set(BUILD_SHARED_LIBS on)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

set_directory_properties(PROPERTY COMPILE_DEFINITIONS_DEBUG GROOO_DEBUG=1)

# add the library
add_library(mybreak SHARED src/MyanmarBreak.cpp
    src/MyanmarBreakIterator.cxx
    src/oooextDiagnostic.cxx
    )

set_source_files_properties(src/MyanmarBreakIterator.cxx OBJECT_DEPENDS
    ${PROJECT_BINARY_DIR}/com/sun/star/lang/XInitialization.hpp)

# test MyanmarBreak
add_executable(mybreaktest src/test_MyanmarBreak.cpp src/MyanmarBreak.cpp)
add_test(NAME test_mybreak COMMAND "$<TARGET_FILE:mybreaktest>")

add_executable(mybreakitertest src/test_MyanmarBreakIterator.cxx src/oooextDiagnostic.cxx)
add_test(NAME test_mybreakiter COMMAND "$<TARGET_FILE:mybreakitertest>")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set_tests_properties(test_mybreak test_mybreakiter PROPERTIES 
	  ENVIRONMENT PATH=${OO_SDK_URE_BIN}${PATH_SEPARATOR}$ENV{PATH})
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	file(TO_CMAKE_PATH "${OO_SDK_URE_BIN}" TEST_PATH)
	set_tests_properties(test_mybreak test_mybreakiter PROPERTIES 
		ENVIRONMENT PATH=${TEST_PATH})
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

set_tests_properties(test_mybreak PROPERTIES
	LABELS "Myanmar Syllable Break Positions")
set_tests_properties(test_mybreakiter PROPERTIES
	LABELS "Myanmar Break Iterator for OpenOffice.org (extension needs to be installed)")

enable_testing()

file(RELATIVE_PATH OO_SDK_CPPUMAKER_NATIVE ${OO_SDK_URE_BIN} ${OO_SDK_CPPUMAKER} )

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/com/sun/star/lang/XInitialization.hpp
    COMMAND ${OO_SDK_CPPUMAKER_NATIVE} -O${PROJECT_BINARY_DIR} -Gc -BUCR ${OO_SDK_RDB}/types.rdb ${OO_SDK_RDB}/services.rdb ${OO_BASIS_PROGRAM}/offapi.rdb
    WORKING_DIRECTORY ${OO_SDK_URE_BIN}
    COMMENT "Generate the OOo API header files from the IDL files - may be slow"
    VERBATIM)

target_link_libraries(mybreak ${OO_SDK_UNO_SAL} ${OO_SDK_UNO_CPPU} ${OO_SDK_UNO_CPPUHELPER} )
target_link_libraries(mybreaktest ${OO_SDK_UNO_SAL})
target_link_libraries(mybreakitertest mybreak ${OO_SDK_UNO_SAL} ${OO_SDK_UNO_CPPU}
    ${OO_SDK_UNO_CPPUHELPER})

set(UNO_SHARED_PREFIX ${CMAKE_SHARED_LIBRARY_PREFIX})
set(UNO_SHARED_EXTENSION ${CMAKE_SHARED_LIBRARY_SUFFIX})
configure_file(uno-extension-manifest.xml ${PROJECT_BINARY_DIR}/META-INF/manifest.xml)
configure_file(description.xml ${PROJECT_BINARY_DIR}/description.xml)

file(GLOB L10N_FILES RELATIVE ${PROJECT_SOURCE_DIR} l10n/*.txt )
foreach (l10nfile ${L10N_FILES} )
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PROJECT_SOURCE_DIR}/${l10nfile}
            ${PROJECT_BINARY_DIR}/${l10nfile})
endforeach (l10nfile ${L10NFILES})

file(GLOB DICT_FILES RELATIVE ${PROJECT_SOURCE_DIR}/dict dict/*.dic dict/*.aff dict/*.xcu )
foreach (dictFile ${DICT_FILES} )
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${PROJECT_SOURCE_DIR}/dict/${dictFile}
            ${PROJECT_BINARY_DIR}/${dictFile})
endforeach (dictFile ${DICT_FILES})

set(ZIP_FILES META-INF/manifest.xml description.xml
    ${DICT_FILES}
    ${L10N_FILES}
    ${UNO_SHARED_PREFIX}mybreak${UNO_SHARED_EXTENSION})

add_custom_target(copyfiles ALL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${UNO_SHARED_PREFIX}mybreak${UNO_SHARED_EXTENSION} ${PROJECT_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/dict/my_MM.dic ${PROJECT_BINARY_DIR}/my_MM.dic
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/dict/my_MM.aff ${PROJECT_BINARY_DIR}/my_MM.aff
	)

add_custom_target(mybreakextension ALL COMMAND ${ZIP} ${PROJECT_BINARY_DIR}/${EXTENSION_NAME}_${EXTENSION_PLATFORM}.oxt ${ZIP_FILES}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_BINARY_DIR}/${EXTENSION_NAME}_${EXTENSION_PLATFORM}.oxt)
set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_BINARY_DIR}/my_MM.dic)
set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_BINARY_DIR}/my_MM.aff)

add_dependencies(copyfiles mybreak)
add_dependencies(mybreakextension mybreak copyfiles)

# unopkg -s option is only available with OOo 3.3

add_custom_target(install
    COMMAND ${UNOPKG} add ${UNOPKG_OPTIONS} --shared ${PROJECT_BINARY_DIR}/${EXTENSION_NAME}_${EXTENSION_PLATFORM}.oxt
    # COMMAND ${UNOPKG} add -s --shared ${PROJECT_BINARY_DIR}/${EXTENSION_NAME}_${EXTENSION_PLATFORM}.oxt
    WORKING_DIRECTORY ${OO_SDK_URE_BIN}
    COMMENT install the extension system wide - requires admin permissions)

add_custom_target(uninstall
    COMMAND ${UNOPKG} remove --shared ${EXTENSION_ID}
    WORKING_DIRECTORY ${OO_SDK_URE_BIN}
    COMMENT install the extension system wide - requires admin permissions)

add_custom_target(install_user
#    COMMAND ${UNOPKG} add -s ${PROJECT_BINARY_DIR}/${EXTENSION_NAME}_${EXTENSION_PLATFORM}.oxt
    COMMAND ${UNOPKG} add ${UNOPKG_OPTIONS} ${PROJECT_BINARY_DIR}/${EXTENSION_NAME}_${EXTENSION_PLATFORM}.oxt
    WORKING_DIRECTORY ${OO_SDK_URE_BIN}
    COMMENT install the extension for current user)

add_custom_target(uninstall_user
    COMMAND ${UNOPKG} remove ${EXTENSION_ID}
    WORKING_DIRECTORY ${OO_SDK_URE_BIN}
    COMMENT uninstall the extension for current user)

add_custom_target(reinstall_user
    COMMAND ${UNOPKG} remove ${EXTENSION_ID}
    COMMAND ${UNOPKG} add ${UNOPKG_OPTIONS} ${PROJECT_BINARY_DIR}/${EXTENSION_NAME}_${EXTENSION_PLATFORM}.oxt
    # COMMAND ${UNOPKG} add -s ${PROJECT_BINARY_DIR}/${EXTENSION_NAME}_${EXTENSION_PLATFORM}.oxt
    WORKING_DIRECTORY ${OO_SDK_URE_BIN}
    COMMENT reinstall the extension for current user)
