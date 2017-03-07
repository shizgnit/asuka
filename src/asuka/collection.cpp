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


#include "collection.hpp"

__ASUKA_BEGIN_NAMESPACE

//
// Initialize the reference copy information
//
#if defined(__pthread__)
AT::Boolean AT::Collection::m_init = true;
pthread_mutex_t AT::Collection::m_mutex;
#endif
AT::Boolean AT::Collection::m_ref_mode = false;
AT::Integer32 AT::Collection::m_ref_alloc = 0;
AT::Integer32 AT::Collection::m_ref_index = 0;
AT::Field **AT::Collection::m_ref_undo = NULL;

__ASUKA_EXPORT AT::Collection &Collection::init() {
  m_vector_buffer  = NULL;

  m_vector_offset  = 0;
  m_vector_size    = 0;
  m_vector_top     = -1;
  m_vector_current = 0;
  m_vector_next    = 0;

  m_terminator = new AT::Field;
  m_terminator->terminate();

  m_hash_root = NULL;

  m_sync = false;

  return(*this);
}

__ASUKA_EXPORT AT::Collection &Collection::reset() {
  if(m_vector_buffer) {
    for(int index = 0; index <= m_vector_top; index++) {
      if(m_vector_buffer[index]) {
        delete(m_vector_buffer[index]);
        m_vector_buffer[index] = NULL;
      }
    }
    free(m_vector_buffer);
    m_vector_buffer = NULL;
  }

  if(m_terminator) {
    delete(m_terminator);
    m_terminator = NULL;
  }

  if(m_hash_root) {
    delete(m_hash_root);
    m_hash_root = NULL;
  }

  return(*this);
}

__ASUKA_EXPORT Collection::Collection(void) {
  init();
}

__ASUKA_EXPORT Collection::Collection(AT::Integer32 nodes) {
  init();

  AT::Integer allocation_size = nodes * sizeof(AT::Field *);
  m_vector_buffer = (AT::Field **)malloc(allocation_size);
  memset(m_vector_buffer, 0, allocation_size);
  m_vector_size = nodes;
}

__ASUKA_EXPORT Collection::Collection(const AT::Collection &in) {
  init();
  copy(in);
}

__ASUKA_EXPORT AT::Collection &Collection::operator=(const AT::Collection *in) {
  copy(*in);
  return(*this);
}

__ASUKA_EXPORT AT::Collection &Collection::operator=(const AT::Collection &in) {
  copy(in);
  return(*this);
}

__ASUKA_EXPORT void Collection::copy(const AT::Collection &in) {
  if(in.m_vector_size > 0) {
    if(!m_vector_buffer) {
      m_vector_buffer = (AT::Field **)malloc(in.m_vector_size * sizeof(AT::Field *));
      m_vector_offset = in.m_vector_offset;
      m_vector_size   = in.m_vector_size;
      memset(m_vector_buffer, 0, m_vector_size * sizeof(AT::Field *));
    }
    else {
      (*this)[in.m_vector_offset];
      (*this)[in.m_vector_offset + in.m_vector_size];
    }
  
    if(m_ref_mode) {
      m_ref_alloc += in.m_vector_size * sizeof(AT::Field *);
      m_ref_undo = (AT::Field **)realloc(m_ref_undo, m_ref_alloc);
      for(AT::Integer32 index = 0; index < in.m_vector_size; index++) {
        if(in.m_vector_buffer[index]) {
          if(!m_vector_buffer[index]) {
            m_vector_buffer[index] = new AT::Field;
          }
          if(in.m_vector_buffer[index]->reference()) {
            *(m_vector_buffer[index]) = in.m_vector_buffer[index]->reference();
          }
          else {
            m_ref_undo[++m_ref_index-1] = in.m_vector_buffer[index];
            in.m_vector_buffer[index]->reference(m_vector_buffer[index]);
            *(m_vector_buffer[index]) = *(in.m_vector_buffer[index]);
          }
        }
      }
    }
    else {
      for(AT::Integer32 index = 0; index < in.m_vector_size; index++) {
        if(in.m_vector_buffer[index]) {
          if(!m_vector_buffer[index]) {
            m_vector_buffer[index] = new AT::Field;
          }
          *(m_vector_buffer[index]) = *(in.m_vector_buffer[index]);
        }
      }
    }

    m_vector_top = in.m_vector_top;
  }

  if(in.m_hash_root) {
    if(m_hash_root) {
      delete(m_hash_root);
    }
    m_hash_root = new AT::Field(*in.m_hash_root);
  }
}

__ASUKA_EXPORT Collection::~Collection(void) {
  reset();
}

__ASUKA_EXPORT AT::Field &Collection::operator[](const AT::Field &key) {
  switch(key.type()) {
  case(AT::TYPE_I32):
    return((*this)[(AT::Integer)(AT::Field)key]);
  case(AT::TYPE_U32):
    return((*this)[(AT::Integer)(AT::Field)key]);
  default:
    return((*this)[(AT::Character *)(AT::Field)key]);
  };
}

__ASUKA_EXPORT AT::Field &Collection::operator[](const AT::Character *key) {
  AT::Integer32 index = 0;

  if(!m_hash_root) {
    m_hash_root = new AT::Field;
  }

  AT::Field *current_node = m_hash_root;
  while(key[index]) {
    if(!current_node) {
      current_node = new AT::Field;
      if(m_sync && !key[index+1]) {
        this->push(current_node);
      }
    }
    current_node = &((*current_node->collection_nodes())[(AT::Integer)key[index]]);
    index++;
  }

  return(*current_node);
}

//==============================================================================
//
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Collection::index_integer(const AT::Integer32 index) {
  AT::Integer32 allocation = 0;
  AT::Integer32 move       = 0;

  AT::Integer32 delta = abs(index - m_vector_offset);

  if(index - m_vector_offset > m_vector_top) {
    m_vector_top = delta;
  }

  if(!m_vector_buffer) {
    m_vector_top = 0;
    m_vector_offset = index;
    allocation = 1;
  }
  else {
    if(index < m_vector_offset) {
      m_vector_top += delta;
      m_vector_offset = index;
      allocation = delta + m_vector_size;
      move = 1;
    }
    if(index > m_vector_offset && delta+1 > m_vector_size) {
      allocation = m_vector_size + (delta+1 - m_vector_size);
    }
  }

  if(allocation) {
    AT::Unsigned32 field_size = sizeof(AT::Field *);

    AT::Field **new_buffer = (AT::Field **)malloc(allocation * field_size);
    memset(new_buffer, 0, allocation * field_size);
    if(m_vector_buffer) {
      memcpy(new_buffer + (delta * move) , m_vector_buffer, m_vector_size * field_size);
      free(m_vector_buffer);
    }
    m_vector_size   = allocation;
    m_vector_buffer = new_buffer;
  }

  AT::Unsigned32 target = index - m_vector_offset;

  if(!m_vector_buffer[target]) {
    m_vector_buffer[target] = new AT::Field;
  }
  return(*m_vector_buffer[target]);
}

__ASUKA_EXPORT AT::Field &Collection::index_unsigned(const AT::Unsigned32 index) {
  AT::Integer32 allocation = 0;
  AT::Integer32 move       = 0;

  AT::Integer32 delta = abs((AT::Integer32)(index - m_vector_offset));

  if(index - m_vector_offset > m_vector_top) {
    m_vector_top = delta;
  }

  if(!m_vector_buffer) {
    m_vector_top = 0;
    m_vector_offset = index;
    allocation = 1;
  }
  else {
    if(index < m_vector_offset) {
      m_vector_top += delta;
      m_vector_offset = index;
      allocation = delta + m_vector_size;
      move = 1;
    }
    if(index > m_vector_offset && delta+1 > m_vector_size) {
      allocation = m_vector_size + (delta+1 - m_vector_size);
    }
  }

  if(allocation) {
    AT::Unsigned32 field_size = sizeof(AT::Field *);

    AT::Field **new_buffer = (AT::Field **)malloc(allocation * field_size);
    memset(new_buffer, 0, allocation * field_size);
    if(m_vector_buffer) {
      memcpy(new_buffer + (delta * move) , m_vector_buffer, m_vector_size * field_size);
      free(m_vector_buffer);
    }
    m_vector_size   = allocation;
    m_vector_buffer = new_buffer;
  }

  AT::Unsigned32 target = index - m_vector_offset;

  if(!m_vector_buffer[target]) {
    m_vector_buffer[target] = new AT::Field;
  }
  return(*m_vector_buffer[target]);
}


__ASUKA_EXPORT AT::Integer32 Collection::size() {
  return(m_vector_size);
}

__ASUKA_EXPORT AT::Integer32 Collection::offset() {
  return(m_vector_offset);
}

__ASUKA_EXPORT AT::Integer32 Collection::top() {
  return(m_vector_top+m_vector_offset);
}

__ASUKA_EXPORT AT::Integer32 Collection::count() {
  return(m_vector_top+1);
}

__ASUKA_EXPORT AT::Collection Collection::sort() {
  //
  // TODO: ...not until It is needed.
  //
  AT::Collection target;
  std::cerr<<"sorting not currently supported"<<std::endl;
  return(target);
}

__ASUKA_EXPORT AT::Collection Collection::reverse() {
  AT::Collection target;
  AT::Field value;

  AT::Integer32 current = index();

  last();
  while(value = prior()) {
    target.push(value);
  }
  index(true, current);

  return(target);
}

__ASUKA_EXPORT AT::Collection Collection::keys() {
  AT::CollectionLevel i;

  if(m_vector_buffer && m_vector_top >= 0) {
    for(AT::Integer32 index=0; index <= m_vector_top; index++) {
      if(m_vector_buffer[index] != NULL) {
        i.list.push(index + m_vector_offset);
      }
    }
  }

  if(m_hash_root) {
    i.current = 0;
    i.label = (AT::Character *)malloc(1024);
    i.label[0] = '\0';

    traverse(m_hash_root, &i);

    free(i.label);
  }

  return(i.list);
}

__ASUKA_EXPORT AT::Collection Collection::hkeys() {
  AT::CollectionLevel i;

  if(m_hash_root) {
    i.current = 0;
    i.label = (AT::Character *)malloc(1024);
    i.label[0] = '\0';

    traverse(m_hash_root, &i);

    free(i.label);
  }

  return(i.list);
}

__ASUKA_EXPORT AT::Collection Collection::ikeys() {
  AT::CollectionLevel i;

  if(m_vector_buffer && m_vector_top >= 0) {
    for(AT::Integer32 index=0; index <= m_vector_top; index++) {
      if(m_vector_buffer[index] != NULL) {
        i.list.push(index + m_vector_offset);
      }
    }
  }

  return(i.list);
}

__ASUKA_EXPORT AT::Collection Collection::values() {
  AT::Collection results;

  AT::Field key;
  foreach(key, keys()) {
    results.push(&(*this)[key]);
  }

  return(results);
}

__ASUKA_EXPORT void Collection::traverse(AT::Field *in, CollectionLevel *lvl) const {
  AT::Collection *list = in->collection_nodes();
  if(list) {
    for(AT::Integer32 i=0; i <= list->m_vector_top; i++) {
      AT::Field *current_node = list->m_vector_buffer[i];
      if(current_node) {
        lvl->label[lvl->current] = i + list->m_vector_offset;
        lvl->label[lvl->current + 1] = '\0';

        if(current_node->defined()) {
          lvl->list.push(lvl->label);
        }

        if(current_node->collection_nodes()) {
          ++lvl->current;
          traverse(current_node, lvl);
        }
      }
    }
    --lvl->current;
  }
}

__ASUKA_EXPORT AT::Field &Collection::push(const AT::Field value) {
  if(value.type() == AT::TYPE_COL) {
    AT::Field temp;
    foreach(temp, value) {
      (*this).push(temp);
    }
  }
  else {
    (*this)[m_vector_offset + m_vector_top + 1] = value;
  }
  return((*this)[m_vector_top]);
}

__ASUKA_EXPORT AT::Field &Collection::push(const AT::Field *value) {
  (*this)[m_vector_offset + m_vector_top + 1] = (AT::Field *)value;
  return((*this)[m_vector_top]);
}

__ASUKA_EXPORT AT::Field Collection::pop() {
  AT::Field value;

  if(m_vector_top >= 0) {
    if(m_vector_buffer[m_vector_top]) {
      value = *m_vector_buffer[m_vector_top];
      delete(m_vector_buffer[m_vector_top]);
      m_vector_buffer[m_vector_top] = NULL;
    }
    m_vector_top--;
  }

  return(value);
}

__ASUKA_EXPORT AT::Field &Collection::unshift(const AT::Field &value) {
  AT::Integer32 target = m_vector_offset;

  if(value.type() == AT::TYPE_COL) {
    AT::Integer32 offset = value.m_data->col->m_vector_offset;
    AT::Integer32 top = value.m_data->col->m_vector_top;
    for(AT::Integer32 i=top; i >= 0; i--) {
      (*this).unshift(value[i+offset]);
    }
  }
  else {
    m_vector_offset++;
    (*this)[target] = (AT::Field *)&value;
  }

  return((*this)[target]);
}

__ASUKA_EXPORT AT::Field &Collection::unshift(const AT::Field *value) {
  AT::Integer32 target = m_vector_offset++;
  (*this)[target] = (AT::Field *)value;
  return((*this)[target]);
}

__ASUKA_EXPORT AT::Field Collection::shift() {
  AT::Field value;

  if(m_vector_size > 0) {
    if(m_vector_buffer[0]) {
      value = *m_vector_buffer[0];
      delete(m_vector_buffer[0]);
      m_vector_buffer[0] = NULL;
    }

    AT::Field **new_buffer = (AT::Field **)malloc(m_vector_size * sizeof(AT::Field *));
    memcpy(new_buffer, m_vector_buffer+1, (m_vector_size - 1) * sizeof(AT::Field *));

    --m_vector_size;
    --m_vector_top;

    free(m_vector_buffer);
    m_vector_buffer = new_buffer;
  }

  return(value);
}

__ASUKA_EXPORT AT::Field Collection::splice(AT::Integer32 offset, AT::Integer32 length, const AT::Field &list) {
  AT::Field removed;

  AT::Integer32 relative_offset;
  AT::Integer32 relative_length;

  AT::Integer32 diff;

  if(offset < 0) {
    offset = 0;
  }

  if(offset < m_vector_offset) {
    (*this)[offset];
    relative_offset = offset;
  }
  else {
    relative_offset = offset - m_vector_offset;
  }

  if(length >= 0) {
    if((relative_offset + length) > m_vector_top) {
      relative_length = m_vector_top + 1 - relative_offset;
    }
    else {
      relative_length = length;
    }
  }
  else {
    relative_length = m_vector_top + 1 - relative_offset;
  }

  if(list.defined()) {
    if(list.type() == TYPE_COL) {
      diff = list.m_data->col->count() - relative_length;
    }
    else {
      diff = 1 - relative_length;
    }
  }
  else {
    diff = relative_length * -1;
  }

  for(AT::Unsigned32 i = relative_offset; i < relative_offset + relative_length; i++) {
    if(m_vector_buffer[i]) {
      removed.push(*m_vector_buffer[i]);
      delete(m_vector_buffer[i]);
      m_vector_buffer[i] = NULL;
    }
    else {
      removed.push(undef);
    }
  }

  AT::Integer32 adjustment_size = m_vector_top + 1 + diff;

  if(adjustment_size <= 0) {
    free(m_vector_buffer);
    init();
    return(removed);
  }

  AT::Field **new_buffer = (AT::Field **)malloc(adjustment_size * sizeof(AT::Field *));

  AT::Unsigned32 current_offset = 0;

  if(relative_offset) {
    memcpy(new_buffer + current_offset, m_vector_buffer, relative_offset * sizeof(AT::Field *));
    current_offset = relative_offset;
  }

  if(list.defined()) {
    if(list.type() == TYPE_COL) {
      for(AT::Unsigned32 j=0; j <= list.m_data->col->m_vector_top; j++) {
        if(list.m_data->col->m_vector_buffer[j]) {
          new_buffer[current_offset] = new AT::Field;
          *new_buffer[current_offset] = *(list.m_data->col->m_vector_buffer[j]);
        }
        else {
          new_buffer[current_offset] = NULL;
        }
        current_offset += 1;
      }
    }
    else {
      new_buffer[current_offset] = new AT::Field;
      *new_buffer[current_offset] = list;
      current_offset += 1;
    }
  }

  AT::Unsigned32 last = relative_offset + relative_length;
  if(last < m_vector_top + 1) {
    memcpy(new_buffer + current_offset, m_vector_buffer + last, (m_vector_top + 1 - last) * sizeof(AT::Field *));
  }

  m_vector_size = adjustment_size;
  m_vector_top = adjustment_size - 1;

  free(m_vector_buffer);
  m_vector_buffer = new_buffer;

  return(removed);
}


__ASUKA_EXPORT AT::Boolean Collection::exists(const AT::Integer index) {
  if(!m_vector_buffer) {
    return(false);
  } 
  if(index - m_vector_offset > m_vector_top) {
    return(false);
  }
  if(m_vector_buffer[index - m_vector_offset] == NULL) {
    return(false);
  }
  return(true);
}

__ASUKA_EXPORT AT::Boolean Collection::exists(const AT::Character *key) {
  if(!m_vector_size && !m_hash_root) {
    return(false);
  }

  if(!key || strlen(key) <= 0) {
    return(false);
  }

  AT::Integer32 index = 0;
  if(!m_hash_root) { return(false); }
  AT::Field *current_node = m_hash_root;
  while(key[index]) {
    if(!current_node) { return(false); }
    if(!current_node->m_data) {
      return(false);
    }
    if(!current_node->m_data->collection_nodes) {
      return(false);
    }
    if(!current_node->m_data->collection_nodes->exists((AT::Integer)key[index])) {
      return(false);
    }

    current_node = &((*(*current_node).collection_nodes())[(AT::Integer)key[index]]);
    index++;
  }
  return(true);
}

__ASUKA_EXPORT AT::Integer32 Collection::index(AT::Boolean reposition, AT::Integer32 value) {
  AT::Integer32 current = m_vector_current + m_vector_offset;
  if(reposition) { m_vector_current = value - m_vector_offset; }
  return(current);
}

__ASUKA_EXPORT AT::Field &Collection::current() {
  if(!m_vector_buffer || m_vector_current > m_vector_top) {
    return(*m_terminator);
  }
  else {
    return(*m_vector_buffer[m_vector_current]);
  }
}

__ASUKA_EXPORT AT::Field &Collection::iterate(AT::Boolean restart) {
  if(restart) { m_vector_next = 0; }

  if(!m_vector_buffer || m_vector_next > m_vector_top) {
    m_vector_current = 0;
    m_vector_next = 0;
    return(*m_terminator);
  }
  else {
    m_vector_current = m_vector_next++;
    if(!m_vector_buffer[m_vector_current]) {
      m_vector_buffer[m_vector_current] = new AT::Field;
    }
    return(*m_vector_buffer[m_vector_current]);
  }
}

__ASUKA_EXPORT AT::Field &Collection::first(AT::Boolean reposition) {
  if(!m_vector_buffer) {
    return(*m_terminator);
  }
  else {
    if(reposition) { m_vector_current = 0; }
    return(*m_vector_buffer[0]);
  }
}

__ASUKA_EXPORT AT::Field &Collection::last(AT::Boolean reposition) {
  if(!m_vector_buffer) {
    return(*m_terminator);
  }
  else {
    if(reposition) { m_vector_current = m_vector_top; }
    return(*m_vector_buffer[m_vector_top]);
  }
}

__ASUKA_EXPORT AT::Field &Collection::next(AT::Boolean reposition) {
  if(!m_vector_buffer || m_vector_current >= m_vector_top) {
    return(*m_terminator);
  }
  else {
    if(reposition) {
      return(*m_vector_buffer[++m_vector_current]);
    }
    else {
      return(*m_vector_buffer[m_vector_current+1]);
    }
  }
}

__ASUKA_EXPORT AT::Field &Collection::prior(AT::Boolean reposition) {
  if(m_vector_buffer || m_vector_current > 0) {
    if(reposition) {
      return(*m_vector_buffer[--m_vector_current]);
    }
    else {
      return(*m_vector_buffer[m_vector_current-1]);
    }
  }
  else {
    return(*m_terminator);
  }
}

__ASUKA_EXPORT void Collection::begin_referencing(void) {
#if defined(__pthread__)
  if(m_init) {
    m_init = false;
    pthread_mutex_init(&m_mutex, NULL);
  }
  pthread_mutex_lock(&m_mutex);
#endif
  m_ref_mode = true;
  m_ref_alloc = 0;
  m_ref_index = 0;
  m_ref_undo = NULL;
}

__ASUKA_EXPORT void Collection::end_referencing(void) {
  if(m_ref_undo != NULL) {
    for(AT::Unsigned32 i=0; i<m_ref_index; i++) {
      m_ref_undo[i]->reference(&undef);
    }
    free(m_ref_undo);
  }
  m_ref_mode = false;
  m_ref_alloc = 0;
  m_ref_index = 0;
  m_ref_undo = NULL;
#if defined(__pthread__)
  pthread_mutex_unlock(&m_mutex);
#endif
}

__ASUKA_END_NAMESPACE


// Local Variables:
// mode:C++
// End:

