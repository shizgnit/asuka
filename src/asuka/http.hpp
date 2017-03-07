/*
================================================================================
  $RCSfile: uri.hpp,v $ $Revision: 1.1.1.1 $ $State: Exp $
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

#ifndef __ASUKA_HTTP_HPP
#define __ASUKA_HTTP_HPP

__ASUKA_BEGIN_NAMESPACE


enum {
  URI_UNDEFINED  = 0x00,
  URI_ABSOLUTE   = 0x01,
  URI_RELATIVE   = 0x02,
  URI_CURRENT    = 0x04,
  URI_NETWORK    = 0x08,
};

#define URI_PATTERN "^(([^:/?#]*):)?(//([^/?#]*))?(/[^?#]*)?(\\?([^#]*))?(#(.*))?$"

class __ASUKA_EXPORT URI {
  enum {
    idx_scheme    = 0,
    idx_authority = 2,
    idx_path      = 4,
    idx_query     = 5,
    idx_fragment  = 7,
  };

public:
  URI(void) { /*NULL*/  }
  URI(const AT::Character *reference) {
    set(reference);
  }

  AT::Field operator() (const AT::Character *reference) { return(set(reference)); }

  AT::Field scheme()    { return(m_sections[idx_scheme]); }
  AT::Field authority() { return(m_sections[idx_authority]); }
  AT::Field path()      { return(m_sections[idx_path]); }
  AT::Field query()     { return(m_sections[idx_query]); }
  AT::Field fragment()  { return(m_sections[idx_fragment]); }

  AT::Field locator();

  static AT::Field encode(AT::Field source);
  static AT::Field decode(AT::Field source);

private:
  AT::Field set(const AT::Character *reference);

  AT::Field m_sections;
};

__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:
