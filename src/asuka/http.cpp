/*
================================================================================
  $RCSfile: uri.cpp,v $ $Revision: 1.2 $ $State: Exp $
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


#include "http.hpp"

__ASUKA_BEGIN_NAMESPACE


__ASUKA_EXPORT AT::Field URI::set(const AT::Character *reference) {
  if(m_sections = AT::rx(URI_PATTERN, reference)) {
    //    AT::Field part;
    //    AT::Integer32 index = 0;
    //    foreach(AT::Field, part, m_sections) {
    //      cerr<<"["<<index++<<"]: "<<part<<endl;
    //    }
  }
  return(m_sections);
}

__ASUKA_EXPORT AT::Field URI::locator() {
  AT::Field list;

  if(m_sections[idx_scheme]) {
    list.push(m_sections[idx_scheme]);
    list.push(":");
  }

  if(m_sections[idx_authority]) {
    list.push("//");
    list.push(m_sections[idx_authority]);
  }

  if(m_sections[idx_path]) {
    list.push(m_sections[idx_path]);
  }

  if(m_sections[idx_query]) {
    list.push("?");
    list.push(m_sections[idx_query]);
  }

  if(m_sections[idx_fragment]) {
    list.push("#");
    list.push(m_sections[idx_fragment]);
  }

  std::cerr<<"attempting to join the list"<<std::endl;

  return(AT::join("", list));
}

AT::Field URI::encode(AT::Field source) {
  AT::Field result;

  result = source;

  return(result);
}

AT::Field URI::decode(AT::Field source) {
  AT::Field result = source;

  AT::Field hex;
  AT::Field target;

  while(1) {
    hex = undef;
    target = undef;
    (target, hex) = AT::rx("(\\%(..))", result);
    if(!hex.defined()) {
      break;
    }

    AT::Character string[2];

    string[0] = hex.base(10, 16);
    string[1] = '\0';

    result = AT::substitute(result, target, string);
  }

  return(result);
}

__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
