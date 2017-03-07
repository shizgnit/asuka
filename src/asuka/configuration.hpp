/*
================================================================================
  $RCSfile: configuration.hpp,v $ $Revision: 1.1.1.1 $ $State: Exp $
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


#include "asuka.hpp"

#ifndef __ASUKA_CONFIGURATION_HPP
#define __ASUKA_CONFIGURATION_HPP

__ASUKA_BEGIN_NAMESPACE

class __ASUKA_EXPORT Configuration {
public:
  Configuration(void) { /*NULL*/ }
  ~Configuration(void) { /*NULL*/ }

  void expect(AT::Character *spec, AT::Field value=undef);

  //
  // The Open Group Base Specifications Issue 6
  // IEEE Std 1003.1, 2004 Edition
  // Copyright (C) 2001-2004 The IEEE and The Open Group, All Rights reserved.
  //
  // 12.1 Utility Argument Syntax
  //
  AT::Field cmdln(AT::Integer argc, AT::Character **argv, AT::Boolean passthrough=true);
  AT::Field file(AT::Character *filename);
  AT::Field registry(AT::Character *path, AT::Boolean traverse = false);

  //
  // All characters above 127 (7F hex) or below 33 (21 hex) are escaped.
  // This includes the space character, which is escaped as %20.
  // Also, the plus sign (+) needs to be interpreted as a space character.
  //
  AT::Field cgi(void);

  AT::Field list(void);

  AT::Field &operator[](const AT::Field &key);
  AT::Field &operator[](const AT::Character *key);
  AT::Field &operator[](AT::Character *key);

  operator AT::Field(void);
  AT::Field undefined(void);

private:
  AT::Field lookup(AT::Field key, AT::Field negatable=false);

  AT::Field m_specification;
  AT::Field m_results;
};

__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:
