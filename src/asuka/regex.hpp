/*
================================================================================
  $RCSfile: regex.hpp,v $ $Revision: 1.1.1.1 $ $State: Exp $
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

#ifndef __ASUKA_REGEX_HPP
#define __ASUKA_REGEX_HPP

__ASUKA_BEGIN_NAMESPACE


#define __RE_BUFFERSIZE 129

#define __RE_CHARSET_UNSET ' '
#define __RE_CHARSET_VALUE 'X'

#define ___ __RE_CHARSET_UNSET
#define _X_ __RE_CHARSET_VALUE

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

static char __RE_CHARSET_ESCAPE[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,_X_,___,___,___,_X_,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,'\0'
};

static char __RE_CHARSET_ANY[__RE_BUFFERSIZE] = {
   ___,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,'\0'
};

static char __RE_CHARSET_SPACE[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,_X_,___,___,___,___,___,___,
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

static char __RE_CHARSET_CONTROL[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,_X_,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,_X_,___,___,___,_X_,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   _X_,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,_X_,___,___,___,_X_,
   ___,___,___,___,___,___,___,___,'\0'
};

static char __RE_CHARSET_LITERAL[__RE_BUFFERSIZE] = {
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,___,_X_,_X_,_X_,
   ___,___,___,___,___,___,___,___,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,___,___,
   _X_,_X_,_X_,___,___,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,___,___,___,___,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,___,_X_,_X_,_X_,'\0'
};

static char __RE_CHARSET_BLANK[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
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

static char __RE_CHARSET_NUMERIC[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,'\0'
};

static char __RE_CHARSET_ALPHA[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,___,___,___,___,___,___,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,___,___,___,___,___,'\0'
};

static char __RE_CHARSET_ALPHANUMERIC[__RE_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,___,___,
   ___,___,___,___,___,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,___,___,___,___,___,___,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,___,___,___,___,___,'\0'
};

#undef ___
#undef _X_


class __ASUKA_EXPORT RegEx {
  public:
  enum types {
    START   = 0x01,
    STOP    = 0x02,
    SUBEXP  = 0x03,
    LITERAL = 0x04,
    CHARSET = 0x05,
  };

  class State {
    friend class RegEx;

  private:

    State(enum types type);
    State(char literal, bool negate=false);
    State(AT::Character *charset=NULL, bool negate=false);

    ~State(void);

    void init(void);

    State *transition(State *next);
    void boundary(bool greedy, AT::Integer32 max, AT::Integer32 min);

  private:

    AT::Integer32  m_subexp_id;

    AT::Integer32  m_type;
    char m_literal;
    AT::Character *m_charset;

    bool m_negate;

    bool m_greedy;
    AT::Integer32  m_max;
    AT::Integer32  m_min;

    AT::Integer32  m_count;
    State **m_transitions;

    State *m_root;
  };

  class StackNode {
    friend class RegEx;

  public:

    ~StackNode(void) {
//      std::cerr<<"going away"<<std::endl;
    }

  private:

    StackNode(State *current, AT::Integer32 index, AT::Integer32 count=0, AT::Integer32 transition=0) {
      m_current = current;
      m_index = index;
      m_count = count;
      m_transition = transition;
    }

    StackNode(StackNode *instance) {
      m_current = instance->m_current;
      m_index = instance->m_index;
      m_count = instance->m_count;
      m_transition = instance->m_transition;
    }

  private:

    State *m_current;
    AT::Integer32 m_index;
    AT::Integer32 m_count;
    AT::Integer32 m_transition;
  };

  class Stack {
    friend class RegEx;

  private:

    Stack(void);

    ~Stack(void);

    StackNode *push(StackNode *value);
    StackNode *pop(void);
    StackNode *last(void);

  private:
    StackNode **m_data;
    AT::Integer32 m_size;
    AT::Integer32 m_count;
  };

public:

  RegEx(const AT::Character *parameter=NULL);

  ~RegEx(void) { cleanup(m_root); }

  AT::Integer32 compile(const AT::Character *parameter, State *root=NULL);
  Stack *execute(const AT::Character *reference);

  AT::Field match(const AT::Character *reference, AT::Integer32 offset=1);
  AT::Field detailed_match(const AT::Character *reference);

private:

  void cleanup(State *root=NULL);

  int m_subexp_count;

  bool m_anchor_front;
  bool m_anchor_back;

  State *m_root;
};

extern AT::Field rx_cache;
__ASUKA_EXPORT AT::Field rx(const AT::Character *expression, const AT::Character *reference, AT::Integer32 offset=1);

__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:
