/*
================================================================================
  $RCSfile: resource.hpp,v $ $Revision: 1.3 $ $State: Exp $
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

#ifndef __ASUKA_RESOURCE_HPP
#define __ASUKA_RESOURCE_HPP

__ASUKA_BEGIN_NAMESPACE

namespace Resource {

class __ASUKA_EXPORT Limited {
  friend class Limiter;
  public:
    Limited(void) {
      m_right = NULL;
      m_left = NULL;
    }

  private:
    Limited *m_right;
    Limited *m_left;
};

class __ASUKA_EXPORT Limiter {
  public:
    Limiter(void);
    ~Limiter(void);

    Limited *reserve(void);
    void release(Limited *ref);

    int count(void) { return(m_current); }

  private:
#if defined(__pthread__)
    pthread_mutex_t m_mutex;
#endif

    int m_current;

    Limited *m_active;
    Limited *m_available;
};

class __ASUKA_EXPORT Manager;

class __ASUKA_EXPORT Type {
  public:
    Type(AT::Field ext, AT::Field type, AT::Field subtype, AT::Field desc="");

    virtual ~Type() { }

    virtual AT::Boolean _release(AT::Field &data) { return(true); }
    virtual AT::Boolean _validate(AT::File &source) { return(true); }
    virtual AT::Boolean _input(AT::File &source, AT::Field &results, AT::Field &cache, AT::Field &options) { return(undef); }
    virtual AT::Boolean _output(AT::Field &data, AT::Field target) { return(false); }

    AT::Field m_details;
    
    virtual void _parent(Manager *parent) { m_parent = parent; }

  protected:
    Manager *m_parent;
    
  private:
    Type(void);
};
typedef Type *handle_t();

class __ASUKA_EXPORT Manager {
  public:
    Manager(void);
    ~Manager(void);

    void accept(AT::Field source);
    void accept(Type *type);

    AT::Boolean release(AT::Field &data);

    AT::Field input(AT::Character *source, AT::Field &cache=undef, AT::Field &options=undef);
    AT::Field input(AT::File &source, AT::Field &cache=undef, AT::Field &options=undef);

    AT::Field output(AT::Field &data, AT::Field target, AT::Field type=undef);

    AT::Field list(AT::Field type=undef);

  protected:
    Type **m_pool;
    AT::Integer32 m_count;
};

};

__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:
