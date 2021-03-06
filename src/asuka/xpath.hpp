/*
================================================================================
  $RCSfile: dom.hpp,v $ $Revision: 1.8 $ $State: Exp $
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

#ifndef __ASUKA_XPATH_HPP
#define __ASUKA_XPATH_HPP

__ASUKA_BEGIN_NAMESPACE

/*

  http://www.w3.org/TR/xpath

  This implementation is incomplete.  Node index specification is not
  supported... maybe if I ever write an xpath that uses one I'll finish
  it.  Also, not all axes have been tested.

*/



namespace XPath {

  __ASUKA_EXPORT AT::Field resolve(AT::Field segment);
  __ASUKA_EXPORT AT::Field execute(AT::Field &context, AT::Field path);
  __ASUKA_EXPORT AT::Field traverse(AT::Field &context, AT::Field segments);
  __ASUKA_EXPORT AT::Field list(AT::Field &node, AT::Field axis, AT::Field type);
  __ASUKA_EXPORT AT::Boolean match(AT::Field &node, AT::Field &filter);

}


__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:

