/*
================================================================================
  $RCSfile: proxy.hpp,v $ $Revision: 1.1.1.1 $ $State: Exp $
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

#ifndef __ASUKA_PROXY_HPP
#define __ASUKA_PROXY_HPP

__ASUKA_BEGIN_NAMESPACE

template<class type> class __ASUKA_EXPORT Proxy {
public:
  Proxy(void)                  : m_reference(NULL) { /*NULL*/ }
  Proxy(Proxy<type> &in_Proxy) : m_reference(NULL) { attach(in_Proxy.m_reference); }
  Proxy(type *in_reference)    : m_reference(NULL) { attach(in_reference); }

  ~Proxy(void) { detach(); }

  Proxy<type> &operator=(const Proxy<type> &in_Proxy) { 
    attach(in_Proxy.m_reference);
    return(*this);
  }
  Proxy<type> &operator=(type *in_reference)    {
    attach(in_reference);
    return(*this);
  }

  operator type *(void)  { return(m_reference);  }
  type &operator*(void)  { return(*m_reference); }
  type *operator->(void) { return(m_reference);  }

  type *pointer(void) { return(m_reference); }

  class __ASUKA_EXPORT Reference {
    friend class Proxy<type>;
  protected:
    Reference(): __ref_count(0) { /*NULL*/ }
  private:
    AT::Integer32 __ref_count;
    void increment(void) { __ref_count++; }
    void decrement(void) { __ref_count--; }
  };

  int references(void) { if(m_reference) return(m_reference->__ref_count); return(0); }

  void terminate(void) { }

private:
  type *m_reference;

  void attach(type *);
  void detach(void);
};

template<class type> void Proxy<type>::attach(type *in_reference) {
  if(in_reference && m_reference != in_reference) {
    detach();
    m_reference = in_reference;
    m_reference->increment();
  }
}

template<class type> void Proxy<type>::detach(void) {
  if(m_reference != NULL) {
    m_reference->decrement();
    if(m_reference->__ref_count == 0) {
      delete(m_reference);
    }
    m_reference = NULL;
  }
}

__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:
