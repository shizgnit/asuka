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

#ifndef __ASUKA_JSON_HPP
#define __ASUKA_JSON_HPP

__ASUKA_BEGIN_NAMESPACE


#define __JSON_BUFFERSIZE 129

#define __JSON_CHARSET_UNSET ' '
#define __JSON_CHARSET_VALUE 'X'

#define ___ __JSON_CHARSET_UNSET
#define _X_ __JSON_CHARSET_VALUE

/*
    0   1   2   3   4   5   6   7   8   9
 0 NUL SOH STX ETX EOT ENQ ACK BEL BS  HT
 1  LF  VT  FF  CR  SO  SI DLE DC1 DC2 DC3
 2 DC4 NAK SYN ETB CAN EM  SUB ESC  FS  GS
 3  RS  US  SP  !   "   #   $   %   &   '
 4  (   )   *   +   ,   -   .   /   0   1
 5  2   3   4   5   6   7   8   9   :   ;
 6  <   =   >   ?   @   A   B   C   D   E
 7  F   G   H   I   J   K   L   M   N   O
 8  P   Q   R   S   T   U   V   W   X   Y
 9  Z   [   \   ]   ^   _   `   a   b   c
10  d   e   f   g   h   i   j   k   l   m
11  n   o   p   q   r   s   t   u   v   w
12  x   y   z   {   |   }   ~  DEL
*/

static char __JSON_CHARSET_SPACE[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,_X_,
   _X_,___,___,_X_,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,_X_,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,'\0'
};

static char __JSON_CHARSET_NUMBER[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,_X_,_X_,___,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,___,___,
   ___,___,___,___,___,___,___,___,___,_X_,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,_X_,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,'\0'
};

#undef ___
#undef _X_


namespace JSON {

  enum types {
    UNSET = 0x0,
    UNDEF,
    STRING,
    NUMBER,
    OBJECT,
    ARRAY
  };

  __ASUKA_EXPORT AT::Field lencode(AT::Field &data, AT::Boolean format=false);

  __ASUKA_EXPORT AT::Field encode(AT::Field &data, AT::Integer32 level=0);
  __ASUKA_EXPORT AT::Field decode(AT::Field data);

  AT::Integer whitespace(AT::Character *buffer);

  AT::Integer parse_token(AT::Character *buffer, AT::Field &results);
  AT::Integer parse_type(AT::Character *buffer, AT::Field &results);

}


__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:

