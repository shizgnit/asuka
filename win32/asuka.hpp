/*
================================================================================
  $RCSfile: asuka.hpp,v $ $Revision: 1.5 $ $State: Exp $
--------------------------------------------------------------------------------

  Copyright (c) 2005, Dee E. Abbott
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

================================================================================
*/


#ifndef __ASUKA_HPP
#define __ASUKA_HPP

#define BUILD_MAJOR 0
#define BUILD_MINOR 0
#define BUILD_REV 1

#define RELEASE $(BUILD_MAJOR).$(BUILD_MINOR).$(BUILD_REV)

#define __windows__ 1

#define __zlib__ 1
//#define __pthread__ 1
//#define __mysql__ 1

//
//-[ MACROS ]-----------------------------------------------------------
//
#if defined(__windows__)
#  if defined(__static__)
#    define __ASUKA_EXPORT
#    define __ASUKA_MODULE_EXPORT
#  else
#    if defined(__exports__)
#      define __ASUKA_EXPORT __declspec(dllexport)
#    else
#      define __ASUKA_EXPORT __declspec(dllimport)
#    endif
#    if defined(__module_exports__)
#      define __ASUKA_MODULE_EXPORT __declspec(dllexport)
#    endif
#  endif
#endif

#if defined(__posix__)
#  define __ASUKA_EXPORT
#  define __ASUKA_MODULE_EXPORT
#endif

#define __ASUKA_BEGIN_NAMESPACE namespace AT {
#define __ASUKA_END_NAMESPACE }

__ASUKA_BEGIN_NAMESPACE

//
//-[ STORAGE TYPES ]--------------------------------------------------
//
enum storage_types {
  TYPE_UND   = 0x00,

  TYPE_I08,
  TYPE_I16,
  TYPE_I32,
  TYPE_I64,
  TYPE_U08,
  TYPE_U16,
  TYPE_U32,
  TYPE_U64,
  TYPE_F32,
  TYPE_F64,
  TYPE_STR,

  TYPE_COL,
  TYPE_TRM,

  TYPE_USR
};

typedef int                 Integer;
typedef char                Integer08;
typedef short               Integer16;
typedef long                Integer32;
typedef unsigned int        Unsigned;
typedef unsigned char       Unsigned08;
typedef unsigned short      Unsigned16;
typedef unsigned long       Unsigned32;
typedef float               Float32;
typedef double              Float64;
typedef char                Character;
typedef unsigned char       Byte;
typedef bool                Boolean;
typedef void                Unknown;

#if defined(__windows__)
typedef __int64             Integer64;
typedef unsigned __int64    Unsigned64;
#define OMANIP(var) ostream& (__cdecl * var)(ostream&)
#define MANIP(var) ios& (__cdecl * var)(ios&)
#endif

#if defined(__posix__)
typedef long long           Integer64;
typedef unsigned long long  Unsigned64;
#define OMANIP(var) ostream& (*var)(ostream&)
#define MANIP(var) ios& (*var)(ios&)
#endif

__ASUKA_END_NAMESPACE

//
//-[ INCLUDES ]---------------------------------------------------------
//

#if defined(__windows__)
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#endif

#if defined(__posix__)
#include <fstream.h>
#include <iostream.h>
#include <string.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdarg.h>
#include <math.h>

#if defined(__pthread__)
#include <pthread.h>
#endif

#if defined(__mysql__)
#include <mysql/mysql.h>
#endif

//
// type conversion loss of data warning
//
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)

//
// logical signed/unsigned mismatch
//
#pragma warning(disable: 4018)

#include "asuka/collection.hpp"
#include "asuka/field.hpp"
#include "asuka/timer.hpp"
#include "asuka/regex.hpp"
#include "asuka/manip.hpp"
#include "asuka/http.hpp"
#include "asuka/filesystem.hpp"
#include "asuka/configuration.hpp"
#include "asuka/identity.hpp"
#include "asuka/xpath.hpp"
#include "asuka/dom.hpp"
#include "asuka/resource.hpp"
#include "asuka/raster.hpp"
#include "asuka/linear.hpp"
#include "asuka/json.hpp"


#if defined(__mysql__) && defined(__pthread__)
#include "asuka/db.hpp"
#endif

#endif

// Local Variables:
// mode:C++
// End:

