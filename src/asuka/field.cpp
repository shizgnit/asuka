/*
================================================================================
  $RCSfile: field.cpp,v $ $Revision: 1.4 $ $State: Exp $
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


#include "field.hpp"

__ASUKA_EXPORT AT::Field undef;

__ASUKA_BEGIN_NAMESPACE


const AT::Character digits[63] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

//==============================================================================
// Sequence operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &operator,(AT::Field &l, AT::Field &r)  {
  static AT::Field x;

  if(l.type() == AT::TYPE_COL) {
    l.push(&r);
    return(l);
  }
  else {
    x.reset();
    x.push(&l);
    x.push(&r);
    return(x);
  }
}


//==============================================================================
// Stream operators
//------------------------------------------------------------------------------
__ASUKA_EXPORT std::ostream &operator<<(std::ostream &output, const AT::Field &value) {
  output << (AT::Character *)(AT::Field)value;
  return(output);
}

__ASUKA_EXPORT AT::Field &Field::operator<<(AT::Field &value) {
  this->append(value);
  return(*this);
}

__ASUKA_EXPORT AT::Field &Field::operator<<(const AT::Character *value) {
  this->append((AT::Character *)value);
  return(*this);
}

__ASUKA_EXPORT AT::Field &Field::operator<<(AT::Character *value) {
  this->append(value);
  return(*this);
}

//==============================================================================
// Field - Init
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::init() {
  m_data = NULL;

  return(*this);
}

//==============================================================================
// Field - Reset
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::reset() {
  if(!m_data) {
    return(*this);
  }

  switch(type()) {
  case(TYPE_STR):
    free(m_data->str);
    m_data->str = NULL;
    break;
  case(TYPE_COL):
    delete(m_data->col);
    m_data->col = NULL;
    break;
  };

  m_data->type = TYPE_UND;

  if(m_data->cast) {
    free(m_data->cast);
    m_data->cast = NULL;
  }

  if(m_data->collection_nodes) {
    delete(m_data->collection_nodes);
    m_data->collection_nodes = NULL;
  }

  if(m_data->count < 0) {
    delete(m_data);
    m_data = NULL;
  }

  return(*this);
}

//==============================================================================
// Field - Constructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Field::Field(void) {
  init();
}

//==============================================================================
// Field - Constructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Field::Field(const AT::Field &in) {
  init();
  copy(in);
}

//==============================================================================
// Field - Assignment operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator=(const AT::Field &in) {
  copy(in);
  return(*this);
}

//==============================================================================
// Field - Copy
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::copy(const AT::Field &in) {
  if(!in.m_data) {
    reset();
    return;
  }
  switch(in.m_data->type) {
  case(TYPE_TRM): reset(); return;
  case(TYPE_I32): assign_i32(in.m_data->i32);  break;
  case(TYPE_U32): assign_u32(in.m_data->u32);  break;
  case(TYPE_F32): assign_f32(in.m_data->f32);  break;
  case(TYPE_STR): assign_str(in.m_data->str);  break;
  case(TYPE_COL): assign_col(*in.m_data->col); break;
  case(TYPE_USR): assign_usr(in.m_data->usr);  break;
  case(TYPE_UND): reset(); break;
  };

  if(!m_data) {
    m_data = new node;
  }
  if(in.m_data->collection_nodes) {
    if(!m_data->collection_nodes) {
      m_data->collection_nodes = new AT::Collection;
    }
    *(m_data->collection_nodes) = *(in.m_data->collection_nodes);
  }
}

//==============================================================================
// Field - Destructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Field::~Field(void) {
  if(m_data && --m_data->count < 0) {
    reset();
  }
}

//==============================================================================
// Field - Post increment operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator++(void) {
  (*this) = (*this) + 1;
  return(*this);
}

//==============================================================================
// Field - Pre increment operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Field::operator++(AT::Integer) {
  AT::Field temporary = (*this);
  (*this) = (*this) + 1;
  return(temporary);
}

//==============================================================================
// Field - Post decrement operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator--(void) {
  (*this) = (*this) - 1;
  return(*this);
}

//==============================================================================
// Field - Pre decement operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Field::operator--(AT::Integer) {
  AT::Field temporary = (*this);
  (*this) = (*this) - 1;
  return(temporary);
}

//==============================================================================
// Field - Type conversion
//------------------------------------------------------------------------------
__ASUKA_EXPORT Field::operator AT::Boolean(void) {
  switch(type()) {
  case(TYPE_I32):  return(m_data->i32?true:false);
  case(TYPE_U32):  return(m_data->u32?true:false);
  case(TYPE_F32):  return(m_data->f32?true:false);
  case(TYPE_STR):  return(m_data->str?true:false);
  case(TYPE_COL):  return(m_data->col?true:false);
  };
  return(false);
}

//==============================================================================
// Field - Negation operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Boolean Field::operator!(void) {
  switch(type()) {
  case(TYPE_I32):  return(!m_data->i32);
  case(TYPE_U32):  return(!m_data->u32);
  case(TYPE_F32):  return(!m_data->f32);
  case(TYPE_STR):  return(!m_data->str);
  case(TYPE_COL):  return(!m_data->col);
  };
  return(true);
}

//==============================================================================
// Field - Assignment
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::assign_i32(const AT::Integer32 in) {
  reset();
  if(!m_data) {
    m_data = new node;
  }
  m_data->i32  = in;
  m_data->type = TYPE_I32;
}

//==============================================================================
// Field - Assignment
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::assign_u32(const AT::Unsigned32 in) {
  reset();
  if(!m_data) {
    m_data = new node;
  }
  m_data->u32  = in;
  m_data->type = TYPE_U32;
}

//==============================================================================
// Field - Assignment
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::assign_f32(const AT::Float32 in) {
  reset();
  if(!m_data) {
    m_data = new node;
  }
  m_data->f32  = in;
  m_data->type = TYPE_F32;
}

//==============================================================================
// Field - Assignment
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::assign_str(const AT::Character *in, AT::Boolean alloc) {
  reset();
  if(!in) {
    return;
  }
  if(!m_data) {
    m_data = new node;
  }
  if(alloc) {
    m_data->str = _strdup((AT::Character *)in);
  }
  else {
    m_data->str = (AT::Character *)in;
  }
  m_data->type = TYPE_STR;
}

//==============================================================================
// Field - Assignment
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::assign_col(const AT::Collection &in) {
  if(!m_data || m_data->type != TYPE_COL) {
    reset();
    if(!m_data) {
      m_data = new node;
    }
    m_data->col = new AT::Collection;
    m_data->type = TYPE_COL;
  }
  if(m_data->col->referencing()) {
    *(m_data->col) = in;
  }
  else {
    m_data->col->begin_referencing();
    *(m_data->col) = in;
    m_data->col->end_referencing();
  }
}

//==============================================================================
// Field - Assignment
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::assign_ref(const AT::Field *in) {
  if(m_data) {
    if(--m_data->count < 0) {
      reset();
    }
    m_data = NULL;
  }
  if(in && in->type() != TYPE_TRM) {
    if(!in->m_data) {
      AT::Field *reference = (AT::Field *)in;
      reference->m_data = new node;
    }
    m_data = in->m_data;
    m_data->count++;
  }
}

//==============================================================================
// Field - Assignment
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::assign_usr(const AT::Unknown *in) {
  reset();
  if(!m_data) {
    m_data = new node;
  }
  m_data->usr  = (AT::Unknown *)in;
  m_data->type = TYPE_USR;
}

//==============================================================================
// Field - Create collection
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Collection &Field::create_col(void) const {
  AT::Field *reference = (AT::Field *)this;

  if(type() != TYPE_COL) {
    reference->reset();
    if(!m_data) {
      reference->m_data = new node;
    }
    reference->m_data->col = new AT::Collection;
    reference->m_data->type = TYPE_COL;
  }
  return(*(m_data->col));
}

//==============================================================================
// Field - Cast
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Integer32 Field::cast_i32(void) const {
  char *endptr;
  switch(type()) {
  case(TYPE_I32):  return(m_data->i32);
  case(TYPE_U32):  return((AT::Integer32)m_data->u32);
  case(TYPE_F32):  return((AT::Integer32)m_data->f32);
  case(TYPE_STR):
    if(m_data->str) {
      return(strtol(m_data->str, &endptr, 10));
    }
    else {
      return(0);
    }
  };
  return(0);
}

//==============================================================================
// Field - Cast
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Unsigned32 Field::cast_u32(void) const {
  char *endptr;
  switch(type()) {
  case(TYPE_I32):  return((AT::Unsigned32)m_data->i32);
  case(TYPE_U32):  return(m_data->u32);
  case(TYPE_F32):  return((AT::Unsigned32)m_data->f32);
  case(TYPE_STR):
    if(m_data->str) {
      return(strtoul(m_data->str, &endptr, 10));
    }
    else {
      return(0);
    }
  };
  return(0);
}

//==============================================================================
// Field - Cast
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Float32 Field::cast_f32(void) const {
  char *endptr;
  switch(type()) {
  case(TYPE_I32):  return((AT::Float32)m_data->i32);
  case(TYPE_U32):  return((AT::Float32)m_data->u32);
  case(TYPE_F32):  return(m_data->f32);
  case(TYPE_STR):
    if(m_data->str) {
      return(strtod(m_data->str, &endptr));
    }
    else {
      return(0);
    }
  };
  return(0);
}

//==============================================================================
// Field - Cast
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Character *Field::cast_str(void) const {
  AT::Field *reference = (AT::Field *)this;

  AT::Unsigned32 allocate = 0;
  AT::Unsigned32 size = 0;

  AT::Field key;
  AT::Field list;

  AT::Character *temporary;

  switch(type()) {
  case(TYPE_I32):
    if(!m_data->cast) {
      reference->m_data->cast = (AT::Character *)malloc(32 * sizeof(AT::Character));
    }
    m_data->cast[0] = '\0';
    sprintf(m_data->cast, "%d", m_data->i32);
    return(m_data->cast);
  case(TYPE_U32):
    if(!m_data->cast) {
      reference->m_data->cast = (AT::Character *)malloc(32 * sizeof(AT::Character));
    }
    m_data->cast[0] = '\0';
    sprintf(m_data->cast, "%u", m_data->u32);
    return(m_data->cast);
  case(TYPE_F32):
    if(!m_data->cast) {
      reference->m_data->cast = (AT::Character *)malloc(32 * sizeof(AT::Character));
    }
    m_data->cast[0] = '\0';
    sprintf(m_data->cast, "%g", m_data->f32);
    return(m_data->cast);
  case(TYPE_STR):
    return(m_data->str);
  case(TYPE_COL):
    if(m_data->cast) {
      free(m_data->cast);
      reference->m_data->cast = NULL;
    }
    for(list = m_data->col->keys(); (key = list.iterate()).defined();) {
      if((*this)[key].defined()) {
        temporary = (AT::Character *)(*this)[key];
        size = strlen(temporary);
        if(size) {
          allocate += size;
          reference->m_data->cast = (AT::Character *)realloc(m_data->cast, (allocate + 1) * sizeof(AT::Character));
          memcpy(m_data->cast + (allocate - size), temporary, size * sizeof(AT::Character));
          m_data->cast[allocate] = '\0';
        }
      }
    }
    if(!m_data->cast) {
      reference->m_data->cast = (AT::Character *)malloc(1 * sizeof(AT::Character));
      m_data->cast[0] = '\0';
    }
    return(m_data->cast);
  };

  return("");
}

//==============================================================================
// Field - Cast
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Collection &Field::cast_col(void) const  {
  AT::Field *reference = (AT::Field *)this;

  static AT::Collection cast;

  switch(type()) {
  case(TYPE_COL): return(*m_data->col);
  };

  return(cast);
}

//==============================================================================
// Field - Cast
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Unknown *Field::cast_usr(void) const  {
  AT::Field *reference = (AT::Field *)this;

  switch(type()) {
  case(TYPE_USR): return(m_data->usr);
  };

  return(NULL);
}

#if defined(__lua__)
//==============================================================================
// Field - Assign
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::assign_lua(lua_State *state) {
  while(lua_gettop(state) > 0) {
    this->unshift(&assign_lua_resolve(state, lua_gettop(state)));
    lua_pop(state, 1);
  }
}
__ASUKA_EXPORT AT::Field Field::assign_lua_resolve(lua_State *state, AT::Integer32 index) {
  AT::Field results;
  switch(lua_type(state, index)) {
    case LUA_TNUMBER: {
        results = lua_tointeger(state, index);
        if(results == 0) {
          results = lua_tonumber(state, index);
        }
      }
      break;
    case LUA_TSTRING:  results = lua_tostring(state, index);  break;
    case LUA_TBOOLEAN: results = lua_toboolean(state, index); break;
    case LUA_TTABLE:
      lua_pushnil(state);
      while(lua_next(state, index) != 0) {
        AT::Field key;
        switch(lua_type(state, lua_gettop(state)-1)) {
          case LUA_TNUMBER:
            key = (AT::Integer32)lua_tonumber(state, lua_gettop(state)-1);
            break;
          case LUA_TSTRING:
            key = (AT::Character *)lua_tostring(state, lua_gettop(state)-1);
            break;
        }
        AT::Field value = assign_lua_resolve(state, lua_gettop(state));
        results[key] = &value;
        lua_pop(state, 1);
      }
      break;
    default: break;
  }
  return(results);
}

#endif

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Field &key) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[key]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Integer index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Integer16 index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Integer32 index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Integer64 index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Unsigned index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Unsigned16 index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Unsigned32 index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Unsigned64 index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Character index) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[(AT::Unsigned32)index]);
}

//==============================================================================
// Field - Index operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Field::operator[](const AT::Character *key) const {
  AT::Field *reference = (AT::Field *)this;

  if(!m_data || m_data->type != TYPE_COL) {
    reference->create_col();
  }
  return((*m_data->col)[key]);
}

//==============================================================================
// Field - Base conversion
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Field::base(AT::Integer32 target_ordinal, AT::Integer32 current_ordinal) {
  AT::Field value;

  if(target_ordinal < 2 || current_ordinal < 2) {
    return(value);
  }

  if(target_ordinal > 62 || current_ordinal > 62) {
    return(value);
  }

  if(current_ordinal == 10) {
    AT::Unsigned32 decimal = (*this);
    AT::Character buffer[256];
    AT::Character *index = buffer;
    while(decimal > 0) {
      *index++ = digits[decimal % target_ordinal];
      decimal /= target_ordinal;
    }
    *index = '\0';
    if(index != buffer) {
      AT::Character reverse[256];
      index = reverse;
      AT::Unsigned32 i=strlen(buffer);
      while(1) {
        *index++ = buffer[--i];
        if(i == 0) {
          break;
        }
      }
      *index = '\0';
      value = reverse;
    }
    else {
      value = 0;
    }
    return(value);
  }
  else {
    AT::Character *source = (*this);
    AT::Unsigned32 pos = (AT::Unsigned32)pow((float)current_ordinal, (int)(strlen(source)-1));

    AT::Unsigned32 decimal = 0;

    while(*source) {
      AT::Character *index = (AT::Character *)digits;
      while(*index) {
        if(*index++ == *source) {
          break;
        }
      }
      decimal += (index - digits - 1) * pos;
      source++;
      pos /= current_ordinal;
    }

    if(target_ordinal != 10) {
      value = AT::Field(decimal).base(target_ordinal);
    }
    else {
      value = decimal;
    }
    return(value);
  }
}

//==============================================================================
// Field - String append
//------------------------------------------------------------------------------
__ASUKA_EXPORT void Field::append(AT::Character *in) {
  if(in == NULL) {
    return;
  }

  if(type()) {
    if(type() != TYPE_STR) {
      this->assign_str(this->cast_str());
    }
  }
  else {
    *this = "";
  }

  AT::Unsigned32 length  = strlen(in);
  if(length <= 0) {
    return;
  }
  
  if(m_data->str && strlen(m_data->str)) {
    AT::Unsigned32 current = strlen(m_data->str);
    AT::Unsigned32 total = current + length;
    if(total <= 0) {
      return;
    }
    
    m_data->str = (AT::Character *)realloc(m_data->str, (total+1) * sizeof(AT::Character));
    memcpy(m_data->str+current, in, length);
    m_data->str[total] = '\0';
  }
  else {
    if(m_data->str) {
      free(m_data->str);
    }
    m_data->str = (AT::Character *)malloc((length+1) * sizeof(AT::Character));
    memcpy(m_data->str, in, length);
    m_data->str[length] = '\0';
  }
}

__ASUKA_EXPORT void Field::uc() {
  if(type() != TYPE_STR) {
    return;
  }
  for(AT::Integer32 i=0; i<strlen(m_data->str); i++) {
    m_data->str[i] = _toupper(m_data->str[i]);
  }
}

__ASUKA_EXPORT void Field::lc() {
  if(type() != TYPE_STR) {
    return;
  }
  AT::Character *ref = m_data->str;
  for(AT::Integer32 i=0; i<strlen(ref); i++) {
    ref[i] = _tolower(ref[i]);
  }
}

__ASUKA_EXPORT void Field::chomp(AT::Boolean whitespace) {
  if(type() != TYPE_STR) {
    return;
  }
  AT::Character *ref = m_data->str;
  AT::Character prior = '\0';
  for(AT::Integer32 i=strlen(ref)-1; i>=0; i--) {
    AT::Character current = ref[i];
    AT::Boolean match = false;
    
    if(whitespace && (current == ' ' || current == '\t')) {
      match = true;
    }
    if(current == '\n') {
      match = true;
    }
    if(prior == '\n' && current == '\r') {
      match = true;
    }
    
    if(!match) {
      break;
    }
    prior = current;
    ref[i] = '\0';
  }
}

__ASUKA_EXPORT AT::Unsigned32 Field::length() {
  if(type() != TYPE_STR || !m_data->str) {
    return(0);
  }
  return(strlen(m_data->str));
}


__ASUKA_END_NAMESPACE

#if defined(__lua__)
//==============================================================================
// Field - Cast
//------------------------------------------------------------------------------
__ASUKA_EXPORT void lua_setfield(lua_State *state, AT::Field &data) {
  lua_newtable(state);
  for(AT::Unsigned32 i=data.offset(); i<=data.top(); i++) {
    if(!data[i].defined()) {
      continue;
    }
    lua_pushnumber(state, i);
    switch(data[i].type()) {
      case AT::TYPE_COL: lua_setfield(state, data[i]); break;
      case AT::TYPE_I32: lua_pushnumber(state, data[i]); break;
      case AT::TYPE_U32: lua_pushnumber(state, data[i]); break;
      case AT::TYPE_F32: lua_pushnumber(state, data[i]); break;
      case AT::TYPE_STR: lua_pushstring(state, data[i]); break;
    }
    lua_rawset(state, -3);
  }
  AT::Field key;
  foreach(key, data.keys()) {
    lua_pushstring(state, key);
    switch(data[key].type()) {
      case AT::TYPE_COL: lua_setfield(state, data[key]); break;
      case AT::TYPE_I32: lua_pushnumber(state, data[key]); break;
      case AT::TYPE_U32: lua_pushnumber(state, data[key]); break;
      case AT::TYPE_F32: lua_pushnumber(state, data[key]); break;
      case AT::TYPE_STR: lua_pushstring(state, data[key]); break;
    }
    lua_rawset(state, -3);
  }
}
#endif

// Local Variables:
// mode:C++
// End:

