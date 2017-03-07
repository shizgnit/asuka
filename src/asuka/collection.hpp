/*
================================================================================
  $RCSfile: collection.hpp,v $ $Revision: 1.3 $ $State: Exp $
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

#ifndef __ASUKA_COLLECTION_HPP
#define __ASUKA_COLLECTION_HPP

__ASUKA_BEGIN_NAMESPACE

//
// These macros should really never be used...
//
#define foreach(_variable, _list) for(AT::Field _i##_variable = _list; ( _variable = &_i##_variable.iterate() ).defined();)
#define unless(_expression) if(!( _expression ))

//class Field;

class CollectionLevel;

class __ASUKA_EXPORT Collection {
public:

  AT::Collection &init();
  AT::Collection &reset();

  //
  // Constructors
  //
  Collection(void);
  Collection(AT::Integer32 nodes);
  Collection(const AT::Collection &);

  //
  // Destructors
  //
  ~Collection(void);

  //
  // Assignment
  //
  void copy(const AT::Collection &);
  AT::Collection &operator=(const AT::Collection *);
  AT::Collection &operator=(const AT::Collection &);

  //
  // Index operators
  //
  AT::Field &operator[](const AT::Field &key);
  AT::Field &operator[](const AT::Character *key);
  AT::Field &operator[](const AT::Integer index) { return(index_integer(index)); }
  AT::Field &operator[](const AT::Integer16 index) { return(index_integer(index)); }
  AT::Field &operator[](const AT::Integer32 index) { return(index_integer(index)); }
  AT::Field &operator[](const AT::Integer64 index) { return(index_integer(index)); }
  AT::Field &operator[](const AT::Unsigned index) { return(index_unsigned(index)); }
  AT::Field &operator[](const AT::Unsigned16 index) { return(index_unsigned(index)); }
  AT::Field &operator[](const AT::Unsigned32 index) { return(index_unsigned(index)); }
  AT::Field &operator[](const AT::Unsigned64 index) { return(index_unsigned(index)); }

  AT::Field &index_integer(const AT::Integer32 index);
  AT::Field &index_unsigned(const AT::Unsigned32 index);

  AT::Integer32 size();
  AT::Integer32 offset();
  AT::Integer32 top();
  AT::Integer32 count();

  AT::Collection sort();
  AT::Collection reverse();
  AT::Collection keys();
  AT::Collection hkeys();
  AT::Collection ikeys();
  AT::Collection values();

  AT::Field &push(const AT::Field);
  AT::Field &push(const AT::Field *);

  AT::Field &unshift(const AT::Field &);
  AT::Field &unshift(const AT::Field *);

  AT::Field pop();
  AT::Field shift();

  AT::Field splice(AT::Integer32 offset, AT::Integer32 length, const AT::Field &list);

  //AT::Boolean exists(const AT::Field &key);
  AT::Boolean exists(const AT::Integer key);
  AT::Boolean exists(const AT::Character *key);

  //
  // Iteration Functions
  //
  AT::Integer32 index(AT::Boolean reposition = false, AT::Integer32 value = 0);

  AT::Field &current();
  AT::Field &iterate(AT::Boolean restart = false);
  AT::Field &first(AT::Boolean reposition = true);
  AT::Field &last(AT::Boolean reposition = true);
  AT::Field &next(AT::Boolean reposition = true);
  AT::Field &prior(AT::Boolean reposition = true);

  void sync(AT::Boolean value) {
    m_sync = value;
  }

  void terminate(void) {}

  bool vector(void) {
    return(m_vector_buffer != NULL);
  }
  bool hash(void) {
    return(m_hash_root != NULL);
  }

  //
  // This referencing implementation is not thread safe
  //
  AT::Boolean referencing(void) {
    return(m_ref_mode);
  }

  void begin_referencing(void);
  void end_referencing(void);

protected:
  AT::Field **m_vector_buffer;

  AT::Integer32 m_vector_offset;
  AT::Integer32 m_vector_size;
  AT::Integer32 m_vector_top;
  AT::Integer32 m_vector_current;
  AT::Integer32 m_vector_next;

  AT::Field *m_terminator;

  AT::Field *m_hash_root;

  AT::Boolean m_sync;

  void traverse(AT::Field *node, CollectionLevel *i) const;

#if defined(__pthread__)
  static AT::Boolean m_init;
  static pthread_mutex_t m_mutex;
#endif  
  static AT::Boolean m_ref_mode;
  static AT::Integer32 m_ref_alloc;
  static AT::Integer32 m_ref_index;
  static AT::Field **m_ref_undo;
};

class __ASUKA_EXPORT CollectionLevel {
  public:
    AT::Integer    current;
    AT::Character  *label;
    AT::Collection list;
};

class __ASUKA_EXPORT CollectionNode {
  public:
    CollectionNode(void) {
      m_collection_nodes = NULL;
    }

    AT::Collection *collection_nodes() {
      if(!m_collection_nodes) {
        m_collection_nodes = new AT::Collection;
      }
      return(m_collection_nodes);
    }

    void terminate(void) {
    }

  protected:
    AT::Collection *m_collection_nodes;
};


__ASUKA_END_NAMESPACE


#endif

// Local Variables:
// mode:C++
// End:

