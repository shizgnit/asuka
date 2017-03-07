/*
================================================================================
  $RCSfile: string_manip.hpp,v $ $Revision: 1.1.1.1 $ $State: Exp $
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

#ifndef __ASUKA_MANIP_HPP
#define __ASUKA_MANIP_HPP

__ASUKA_BEGIN_NAMESPACE

__ASUKA_EXPORT AT::Field substitute(AT::Field in_buffer, const AT::Character *expression, const AT::Character *replacement, AT::Boolean global=false);
__ASUKA_EXPORT AT::Field substitute(AT::Field in_buffer, AT::Field &replacement);

__ASUKA_EXPORT AT::Field split(const AT::Character *expression, AT::Field buffer, AT::Integer32 limit=0);
__ASUKA_EXPORT AT::Field join(const AT::Character *delimiter, AT::Field list);

__ASUKA_EXPORT AT::Field lc(const AT::Character *string);
__ASUKA_EXPORT AT::Field uc(const AT::Character *string);

__ASUKA_EXPORT AT::Field chomp(const AT::Character *string, AT::Boolean whitespace=false);

__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:
