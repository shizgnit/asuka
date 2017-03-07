/*
================================================================================
  $RCSfile: resource.cpp,v $ $Revision: 1.3 $ $State: Exp $
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


#include "resource.hpp"

#if defined(__posix__)
#  include <dlfcn.h>
#endif

__ASUKA_BEGIN_NAMESPACE

namespace Resource {

//==============================================================================
// Limiter - Constructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Limiter::Limiter(void) {
#if defined(__pthread__)
  pthread_mutex_init(&m_mutex, NULL);
#endif
  m_current = 0;
  m_active = NULL;
  m_available = NULL;
}

//==============================================================================
// Limiter - Destructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Limiter::~Limiter(void) {
#if defined(__pthread__)
  pthread_mutex_destroy(&m_mutex);
#endif
}

//==============================================================================
// Limiter - Resource reservation
//------------------------------------------------------------------------------
__ASUKA_EXPORT Limited *Limiter::reserve(void) {
#if defined(__pthread__)
  pthread_mutex_lock(&m_mutex);
#endif
  Limited *result = NULL;
  if(m_available) {
    result = m_available;
    if(m_available->m_right) {
      m_available->m_right->m_left = NULL;
    }
    m_available = m_available->m_right;
  }
  if(result) {
    if(!m_active) {
      m_active = result;
    }
    else {
      result->m_right = m_active;
      result->m_left = NULL;
      m_active->m_left = result;
      m_active = result;
    }
  }
#if defined(__pthread__)
  pthread_mutex_unlock(&m_mutex);
#endif
  return(result);
}

//==============================================================================
// Limiter - Resource release
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Limiter::release(Limited *ref) {
  if(ref) {
#if defined(__pthread__)
    pthread_mutex_lock(&m_mutex);
#endif
    if(!ref->m_right && !ref->m_left) {
      m_current++;
    }
    else {
      if(ref == m_active) {
        m_active = ref->m_right;
      }
      if(ref->m_right) {
        ref->m_right->m_left = ref->m_left;
      }
      if(ref->m_left) {
        ref->m_left->m_right = ref->m_right;
      }
    }
    if(!m_available) {
      m_available = ref;
    }
    else {
      ref->m_right = m_available;
      ref->m_left = NULL;
      m_available->m_left = ref;
      m_available = ref;
    }
#if defined(__pthread__)
    pthread_mutex_unlock(&m_mutex);
#endif
  }
}

//==============================================================================
// Type - Constructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Type::Type(AT::Field ext, AT::Field type, AT::Field name, AT::Field desc) {
  m_details["ext"] = ext;
  m_details["type"] = type;
  m_details["name"] = name;
  m_details["desc"] = desc;
  m_parent = NULL;
}

//==============================================================================
// Manager - Constructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Manager::Manager(void) {
  m_pool = NULL;
  m_count = 0;
}

//==============================================================================
// Manager - Destructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Manager::~Manager(void) {
  if(m_pool) {
    for(AT::Integer32 i=0; i<m_count; i++) {
      delete(m_pool[i]);
      m_pool[i] = NULL;
    }
    free(m_pool);
    m_pool = NULL;
  }
}

//==============================================================================
// Manager - Dynamic library loading
//------------------------------------------------------------------------------
#if defined(__windows__)
__ASUKA_EXPORT void Manager::accept(AT::Field src) {
  AT::Field mask = "*.dll";

  AT::Field files;
  if(AT::filetype(src) == "directory") {
    files = AT::Directory(AT::join("\\", (src, mask)));
  }
  else {
    files.push(AT::Directory(src));
  }

  AT::Field file;
  foreach(file, files) {
    if(!AT::rx("\\.dll$", file)) {
      continue;
    }

    AT::Field library = AT::join("\\", (src, file));

    HINSTANCE ref = LoadLibrary(library);
    if (!ref) {
      std::cerr << "failed to load '" << library << "'" << std::endl;
      continue;
    }

    handle_t *handle_i = (handle_t*)GetProcAddress(ref, "handle");
    if (!handle_i) {
      std::cerr << "failed to load interface 'handle'" << std::endl;
      continue;
    }

    accept(handle_i());
  }
}
#endif

#if defined(__posix__)
__ASUKA_EXPORT void Manager::accept(AT::Field src) {
  AT::Field files, file;
  if(AT::filetype(src) == "directory") {
    //std::cerr<<"  src '"<<src<<"' is a directory"<<std::endl;
    files = AT::Directory(src);
  }
  else {
    //std::cerr<<"  src '"<<src<<"' is *not* a directory"<<std::endl;
    files.push(src);
  }

  foreach(file, files) {
    if(AT::rx("^\\.", file)) {
      continue;
    }

    AT::Field library = AT::join("/", (src, file));

    //std::cerr<<"  loading library '"<<library<<"'"<<std::endl;

    void *ref  = dlopen(library, RTLD_LAZY);
    if (!ref) {
      std::cerr << "failed to load '" << library << "' => " << dlerror() << std::endl;
      continue;
    }

    handle_t *handle_i  = (handle_t*)dlsym(ref, "handle");
    if (!handle_i) {
      std::cerr << "failed to load interface 'handle' => " << dlerror() << std::endl;
      continue;
    }

    accept(handle_i());
  }
}
#endif

//==============================================================================
// Manager - Type loader
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Manager::accept(Type *type) {
  if(type) {
    type->_parent(this);
    m_pool = (Type **)realloc(m_pool, (m_count + 1) * sizeof(Type *));
    m_pool[m_count] = type;
    m_count++;
  }
}

//==============================================================================
// Manager - Input
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Manager::input(AT::Character *src, AT::Field &cache, AT::Field &options) {
  AT::File temporary = AT::Path::open(src);
  return(input(temporary, cache, options));
}

//==============================================================================
// Manager - Input
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Manager::input(AT::File &source, AT::Field &cache, AT::Field &options) {
  //std::cout<<"Manager::input"<<std::endl;
  AT::Field tmp = AT::lc(source.source());
  //std::cout<<"  loading file  '" << tmp <<"'"<<std::endl;
  for(AT::Integer32 i=0; i<m_count; i++) {
    if(AT::rx("\\.(\\w+)$", tmp)[0] != m_pool[i]->m_details["ext"]) {
      //std::cout<<"*** pool("<<i<<") extention doesn't match"<<std::endl;
      continue;
    }
    if(!m_pool[i]->_validate(source)) {
      //std::cout<<"*** pool("<<i<<") didn't validate"<<std::endl;
      continue;
    }
    AT::Field results;
    AT::Field type = m_pool[i]->m_details["type"];
    if(cache.defined() && cache[type][source.source()].defined()) {
      results = &cache[type][source.source()];
    }
    else {
      m_pool[i]->_input(source, results, cache, options);
      AT::Field key;
      foreach(key, m_pool[i]->m_details.keys()) {
        //std::cout << "*** setting internal '" << key << "' as '" << m_pool[i]->m_details[key] << "'" <<std::endl;
        results["_internal"][key] = m_pool[i]->m_details[key];
      }
      results["_internal"]["parser"] = (void *)m_pool[i];
      if(cache.defined()) {
        cache[type][source.source()] = &results;
      }
      //std::cout<<"  done, with results... returning (makes a copy)"<<std::endl;
    }
    return(results);
  }
  //std::cout<<"  done, without results"<<std::endl;
  return(undef);
}

//==============================================================================
// Manager - Output
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Manager::output(AT::Field &data, AT::Field target, AT::Field type) {
  return(undef);
}

//==============================================================================
// Manager - Release
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Boolean Manager::release(AT::Field &data) {
  if((void *)data["_internal"]["parser"]) {
    AT::Resource::Type *allocator = (AT::Resource::Type *)(void *)data["_internal"]["parser"];
    return(allocator->_release(data));
  }
  return(true);
}

//==============================================================================
// Manager - Support list
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Manager::list(AT::Field type) {
  AT::Field results;
  for(AT::Integer32 i=0; i<m_count; i++) {
    if(!type.defined() || m_pool[i]->m_details["type"] == type) {
      results.push(m_pool[i]->m_details);
    }
  }
  return(results);
}

};

__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
