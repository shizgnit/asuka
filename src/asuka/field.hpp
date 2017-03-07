/*
================================================================================
  $RCSfile: field.hpp,v $ $Revision: 1.4 $ $State: Exp $
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

#ifndef __ASUKA_FIELD_HPP
#define __ASUKA_FIELD_HPP

__ASUKA_BEGIN_NAMESPACE

//==============================================================================
// MULTI-MODE COMMON
//------------------------------------------------------------------------------
#define __ASUKA_MULTI_MODE_COMMON(sx, mt)	\
  public:	\
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, const AT::Field &r) { \
    switch(r.type()) { \
    case(TYPE_UND):  return(l.mt##_und(r));	\
    case(TYPE_I32):  return(l.mt##_i32(r.m_data->i32));	\
    case(TYPE_U32):  return(l.mt##_u32(r.m_data->u32));	\
    case(TYPE_F32):  return(l.mt##_f32(r.m_data->f32));	\
    case(TYPE_STR):  return(l.mt##_str(r.m_data->str));	\
    case(TYPE_COL):  return(l.mt##_str(r.cast_str())); \
    }; \
    return(undef); \
  }	\
\
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Integer r)    { return(l.mt##_i32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Integer l, const AT::Field &r)    { AT::Field v = l; return(v sx r); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Integer16 r)  { return(l.mt##_i32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Integer16 l, const AT::Field &r)  { AT::Field v = l; return(v sx r); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Integer32 r)  { return(l.mt##_i32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Integer32 l, const AT::Field &r)  { AT::Field v = l; return(v sx r); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Integer64 r)  { return(l.mt##_i32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Integer64 l, const AT::Field &r)  { AT::Field v = l; return(v sx r); } \
\
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Unsigned r)   { return(l.mt##_u32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Unsigned l, const AT::Field &r)   { AT::Field v = l; return(v sx r); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Unsigned16 r) { return(l.mt##_u32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Unsigned16 l, const AT::Field &r) { AT::Field v = l; return(v sx r); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Unsigned32 r) { return(l.mt##_u32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Unsigned32 l, const AT::Field &r) { AT::Field v = l; return(v sx r); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Unsigned64 r) { return(l.mt##_u32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Unsigned64 l, const AT::Field &r) { AT::Field v = l; return(v sx r); } \
\
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Float32 r)    { return(l.mt##_f32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Float32 l, const AT::Field &r)    { AT::Field v = l; return(v sx r); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Float64 r)    { return(l.mt##_f32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Float64 l, const AT::Field &r)    { AT::Field v = l; return(v sx r); } \
\
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Byte r)       { return(l.mt##_u32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Byte l, const AT::Field &r)       { AT::Field v = l; return(v sx r); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Character r)  { return(l.mt##_u32(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Character l, const AT::Field &r)  { AT::Field v = l; return(v sx r); } \
\
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Character *r)    { return(l.mt##_str(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Character* l, const AT::Field &r)    { AT::Field v = l; return(v sx r); } \
\
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, const AT::Character *r)    { return(l.mt##_str(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Character *l, const AT::Field &r)    { AT::Field v = l; return(v sx r); } \
\
  private: \
  AT::Field mt##_i32(AT::Integer32 in) const { \
    AT::Field v; \
    switch(type()) { \
    case(TYPE_I32):  v = m_data->i32 sx in;  break; \
    case(TYPE_U32):  v = m_data->u32 sx in;  break; \
    case(TYPE_F32):  v = m_data->f32 sx in;  break; \
    case(TYPE_STR):  v = cast_f32() sx in;  break; \
    }; \
    return(v); \
  } \
\
  AT::Field mt##_u32(AT::Unsigned32 in) const { \
    AT::Field v; \
    switch(type()) { \
    case(TYPE_I32):  v = m_data->i32 sx in;  break; \
    case(TYPE_U32):  v = m_data->u32 sx in;  break; \
    case(TYPE_F32):  v = m_data->f32 sx in;  break; \
    case(TYPE_STR):  v = cast_f32() sx in;  break; \
    }; \
    return(v); \
  } \
\
  AT::Field mt##_f32(AT::Float32 in) const { \
    AT::Field v; \
    switch(type()) { \
    case(TYPE_I32):  v = cast_f32() sx in;  break; \
    case(TYPE_U32):  v = cast_f32() sx in;  break; \
    case(TYPE_F32):  v = m_data->f32 sx in;  break; \
    case(TYPE_STR):  v = cast_f32() sx in;  break; \
    }; \
    return(v); \
  }


//==============================================================================
// MULTI-MODE ARITHMETIC
//------------------------------------------------------------------------------
#define __ASUKA_MULTI_MODE_ARITHMETIC_INLINE(sx, mt) \
  __ASUKA_MULTI_MODE_COMMON(sx, mt) \
  private: \
  AT::Field mt##_und(const AT::Field &in) const { \
    return(*this); \
  } \
\
  AT::Field mt##_str(const AT::Character *in) const { \
    AT::Field v = in; \
    switch(type()) { \
    case(TYPE_I32):  v = cast_f32() sx v.cast_f32();  break; \
    case(TYPE_U32):  v = cast_f32() sx v.cast_f32();  break; \
    case(TYPE_F32):  v = m_data->f32 sx v.cast_f32();  break; \
    case(TYPE_STR):  v = cast_f32() sx v.cast_f32();  break; \
    }; \
    return(v); \
  } \
\
  public: \
  AT::Field operator sx##= (const AT::Field &r) { (*this) = (*this) sx r; return((*this)); } \
\
  AT::Field operator sx##= (AT::Integer r)     { (*this) = (*this) sx r; return((*this)); } \
  AT::Field operator sx##= (AT::Integer16 r)   { (*this) = (*this) sx r; return((*this)); } \
  AT::Field operator sx##= (AT::Integer32 r)   { (*this) = (*this) sx r; return((*this)); } \
  AT::Field operator sx##= (AT::Integer64 r)   { (*this) = (*this) sx r; return((*this)); } \
\
  AT::Field operator sx##= (AT::Unsigned r)    { (*this) = (*this) sx r; return((*this)); } \
  AT::Field operator sx##= (AT::Unsigned16 r)  { (*this) = (*this) sx r; return((*this)); } \
  AT::Field operator sx##= (AT::Unsigned32 r)  { (*this) = (*this) sx r; return((*this)); } \
  AT::Field operator sx##= (AT::Unsigned64 r)  { (*this) = (*this) sx r; return((*this)); } \
\
  AT::Field operator sx##= (AT::Float32 r)     { (*this) = (*this) sx r; return((*this)); } \
  AT::Field operator sx##= (AT::Float64 r)     { (*this) = (*this) sx r; return((*this)); } \
\
  AT::Field operator sx##= (AT::Byte r)        { (*this) = (*this) sx r; return((*this)); } \
  AT::Field operator sx##= (AT::Character r)   { (*this) = (*this) sx r; return((*this)); } \
\
  AT::Field operator sx##= (AT::Character *r)     { (*this) = (*this) sx r; return((*this)); }


#define __ASUKA_MULTI_MODE_ARITHMETIC(sx, mt) \
  __ASUKA_MULTI_MODE_COMMON(sx, mt) \
  private: \
  AT::Field mt##_und(const AT::Field &in) const { \
    return(*this); \
  } \
\
  AT::Field mt##_str(const AT::Character *in) const { \
    AT::Field v = in; \
    switch(type()) { \
    case(TYPE_I32):  v = cast_f32() sx v.cast_f32();  break; \
    case(TYPE_U32):  v = cast_f32() sx v.cast_f32();  break; \
    case(TYPE_F32):  v = m_data->f32 sx v.cast_f32();  break; \
    case(TYPE_STR):  v = cast_f32() sx v.cast_f32();  break; \
    }; \
    return(v); \
  }

//==============================================================================
// MULTI-MODE COMPARISON
//------------------------------------------------------------------------------
#define __ASUKA_MULTI_MODE_COMPARISON(sx, mt) \
  __ASUKA_MULTI_MODE_COMMON(sx, mt) \
\
  private: \
  AT::Field mt##_und(const AT::Field &in) const { \
    AT::Field v = type() sx in.type(); \
    return(v); \
  } \
\
  AT::Field mt##_str(const AT::Character *in) const { \
    AT::Field v = in; \
    switch(type()) { \
    case(TYPE_I32):  v = cast_f32() sx v.cast_f32();  break; \
    case(TYPE_U32):  v = cast_f32() sx v.cast_f32();  break; \
    case(TYPE_F32):  v = m_data->f32 sx v.cast_f32(); break; \
    case(TYPE_STR):  v = (strcmp(m_data->str, in) sx 0);    break; \
    case(TYPE_COL):  v = (strcmp(cast_str(), in) sx 0);     break; \
    default: v = 1 sx 0; break; \
    }; \
    return(v); \
  }

//==============================================================================
// MULTI-MODE LOGICAL
//------------------------------------------------------------------------------
#define __ASUKA_MULTI_MODE_LOGICAL(sx, mt) \
  __ASUKA_MULTI_MODE_COMMON(sx, mt) \
  public: \
  friend __ASUKA_EXPORT AT::Field operator sx (const AT::Field &l, AT::Boolean r)    { return(l.mt##_bool(r)); } \
  friend __ASUKA_EXPORT AT::Field operator sx (AT::Boolean l, const AT::Field &r)    { AT::Field v = l; return(v sx r); } \
\
  private: \
  AT::Field mt##_und(const AT::Field &in) const { \
    AT::Field v; \
    switch(type()) { \
    case(TYPE_UND):  v = false sx false;  break; \
    case(TYPE_I32):  v = m_data->i32 sx false;  break; \
    case(TYPE_U32):  v = m_data->u32 sx false;  break; \
    case(TYPE_F32):  v = m_data->f32 sx false;  break; \
    case(TYPE_STR):  v = cast_f32() sx false;  break; \
    }; \
    return(v); \
  } \
\
  AT::Field mt##_str(const AT::Character *in) const { \
    AT::Field v = in; \
    switch(type()) { \
    case(TYPE_I32):  v = cast_f32() sx v.cast_f32();  break; \
    case(TYPE_U32):  v = cast_f32() sx v.cast_f32();  break; \
    case(TYPE_F32):  v = m_data->f32 sx v.cast_f32();  break; \
    case(TYPE_STR):  v = (strcmp(m_data->str, in) sx 0); break; \
    }; \
    return(v); \
  } \
\
  AT::Field mt##_bool(AT::Boolean in) const { \
    AT::Field v; \
    switch(type()) { \
    case(TYPE_I32):  v = m_data->i32 sx in;  break; \
    case(TYPE_U32):  v = m_data->u32 sx in;  break; \
    case(TYPE_F32):  v = m_data->f32 sx in;  break; \
    case(TYPE_STR):  v = cast_f32() sx in;  break; \
    }; \
    return(v); \
  }

class __ASUKA_EXPORT Field {
  friend class AT::Collection;

  //
  // Stream Functions
  //
  friend __ASUKA_EXPORT std::ostream &operator<<(std::ostream &, const AT::Field &);
  
public:
  //
  // Multimode
  //
  __ASUKA_MULTI_MODE_ARITHMETIC_INLINE(+,add);
  __ASUKA_MULTI_MODE_ARITHMETIC_INLINE(-,sub);

  // subset for VC.NET
  __ASUKA_MULTI_MODE_ARITHMETIC(*,mul);
  __ASUKA_MULTI_MODE_ARITHMETIC(/,div);

  __ASUKA_MULTI_MODE_COMPARISON(==,eq);
  __ASUKA_MULTI_MODE_COMPARISON(!=,ne);
  __ASUKA_MULTI_MODE_COMPARISON(<,lt);
  __ASUKA_MULTI_MODE_COMPARISON(>,gt);
  __ASUKA_MULTI_MODE_COMPARISON(<=,le);
  __ASUKA_MULTI_MODE_COMPARISON(>=,ge);

  __ASUKA_MULTI_MODE_LOGICAL(&&,and);
  __ASUKA_MULTI_MODE_LOGICAL(||,or);

public:
  AT::Field &init();
  AT::Field &reset();

  //
  // Constructors
  //
  Field(void);
  Field(const AT::Field &in);

  Field(const AT::Integer    &in)  { init(); assign_i32(in); }
  Field(const AT::Integer16  &in)  { init(); assign_i32(in); }
  Field(const AT::Integer32  &in)  { init(); assign_i32(in); }
  Field(const AT::Integer64  &in)  { init(); assign_i32(in); }
  Field(const AT::Unsigned16 &in)  { init(); assign_u32(in); }
  Field(const AT::Unsigned   &in)  { init(); assign_u32(in); }
  Field(const AT::Unsigned32 &in)  { init(); assign_u32(in); }
  Field(const AT::Unsigned64 &in)  { init(); assign_u32(in); }
  Field(const AT::Float32    &in)  { init(); assign_f32(in); }
  Field(const AT::Float64    &in)  { init(); assign_f32(in); }
  Field(const AT::Byte       &in)  { init(); assign_u32(in); }
  Field(const AT::Character  &in)  { init(); assign_u32(in); }
  Field(const AT::Character  *in)  { init(); assign_str(in); }
  Field(const AT::Unknown    *in)  { init(); assign_usr(in); }

  Field(const AT::Collection  &in) { init(); assign_col(in); }

  Field(AT::Field *in)                        { init(); assign_ref(in); }
  Field(AT::Collection *in)        { init(); assign_col(*in); }

  //
  // Destructors
  //
  ~Field(void);

  //
  // Type queries
  //
  AT::Integer type() const    { return(m_data ? m_data->type : TYPE_UND); }
  AT::Boolean defined() const { return(m_data ? (m_data->type != TYPE_UND) : 0); }

  //
  // Assigment
  //
  void copy(const AT::Field &);
  AT::Field &operator=(const AT::Field &);

  AT::Field &operator=(AT::Integer    in) { assign_i32(in); return(*this); }
  AT::Field &operator=(AT::Integer16  in) { assign_i32(in); return(*this); }
  AT::Field &operator=(AT::Integer32  in) { assign_i32(in); return(*this); }
  AT::Field &operator=(AT::Integer64  in) { assign_i32(in); return(*this); }
  AT::Field &operator=(AT::Unsigned   in) { assign_u32(in); return(*this); }
  AT::Field &operator=(AT::Unsigned16 in) { assign_u32(in); return(*this); }
  AT::Field &operator=(AT::Unsigned32 in) { assign_u32(in); return(*this); }
  AT::Field &operator=(AT::Unsigned64 in) { assign_u32(in); return(*this); }
  AT::Field &operator=(AT::Float32    in) { assign_f32(in); return(*this); }
  AT::Field &operator=(AT::Float64    in) { assign_f32(in); return(*this); }
  AT::Field &operator=(AT::Byte       in) { assign_u32(in); return(*this); }
  AT::Field &operator=(AT::Character  in) { assign_u32(in); return(*this); }
  AT::Field &operator=(AT::Character *in) { assign_str(in); return(*this); }
  AT::Field &operator=(AT::Unknown   *in) { assign_usr(in); return(*this); }

  AT::Field &operator=(const AT::Collection &in) { assign_col(in); return(*this); }

  AT::Field &operator=(AT::Field *in)      { assign_ref(in); return(*this); }
  AT::Field &operator=(AT::Collection *in) { assign_col(*in); return(*this); }


  //
  // Cast
  //
  operator AT::Integer    (void) { return(cast_i32()); }
  operator AT::Integer16  (void) { return(cast_i32()); }
  operator AT::Integer32  (void) { return(cast_i32()); }
  operator AT::Integer64  (void) { return(cast_i32()); }
  operator AT::Unsigned   (void) { return(cast_u32()); }
  operator AT::Unsigned16 (void) { return(cast_u32()); }
  operator AT::Unsigned32 (void) { return(cast_u32()); }
  operator AT::Unsigned64 (void) { return(cast_u32()); }
  operator AT::Float32    (void) { return(cast_f32()); }
  operator AT::Float64    (void) { return(cast_f32()); }
  operator AT::Byte       (void) { return(cast_u32()); }
  operator AT::Character  (void) { return(cast_u32()); }
  operator AT::Character *(void) { return(cast_str()); }
  operator AT::Unknown   *(void) { return(cast_usr()); }

  operator AT::Collection &(void) const  { return(cast_col()); }

  operator AT::Field *(void) { return(this); }

  // TODO: not necessary yet...
  //operator AT::Collection *(void) { __ASUKA_RETURN_TYPE_COL; }

  //
  // lua_State*
  //
#if defined(__lua__)
  Field(lua_State *in)  { init(); assign_lua(in); }  
  AT::Field &operator=(lua_State *in) { assign_lua(in); return(*this); }
#endif

  //
  // Increment operators
  //
  AT::Field &operator++(void);
  AT::Field operator++(AT::Integer);

  AT::Field &operator--(void);
  AT::Field operator--(AT::Integer);

  //
  // Logical tests
  //
  operator AT::Boolean(void);
  AT::Boolean operator!(void);

  //
  // Index operators
  //
  AT::Field &operator[](const AT::Field &key) const;
  AT::Field &operator[](const AT::Integer index) const;
  AT::Field &operator[](const AT::Integer16 index) const;
  AT::Field &operator[](const AT::Integer32 index) const;
  AT::Field &operator[](const AT::Integer64 index) const;
  AT::Field &operator[](const AT::Unsigned index) const;
  AT::Field &operator[](const AT::Unsigned16 index) const;
  AT::Field &operator[](const AT::Unsigned32 index) const;
  AT::Field &operator[](const AT::Unsigned64 index) const;
  AT::Field &operator[](const AT::Character index) const;
  AT::Field &operator[](const AT::Character *key) const;

  // NOTE: This is necessary for the new gcc 3.3 anal ambiguity checking.
  AT::Field &operator[](AT::Character *key) const { return((*this)[(const AT::Character *)key]); }

  //
  // Input stream support
  //
  AT::Field &operator << (AT::Field &);
  AT::Field &operator << (const AT::Character *);
  AT::Field &operator << (AT::Character *);
  

  // NOTE: Delegate to the type specific collection.
  AT::Integer32 size()                     { return(create_col().size()); }
  AT::Integer32 offset()                   { return(create_col().offset()); }
  AT::Integer32 top()                      { return(create_col().top()); }
  AT::Integer32 count()                    { return(create_col().count()); }

  AT::Field sort()                         { return(create_col().sort()); }
  AT::Field reverse()                      { return(create_col().reverse()); }
  AT::Field keys() const                   { return(create_col().keys()); }
  AT::Field hkeys() const                  { return(create_col().hkeys()); }
  AT::Field ikeys() const                  { return(create_col().ikeys()); }
  AT::Field values() const                   { return(create_col().values()); }

  AT::Field &push(const AT::Field in)      { return(create_col().push(in)); }
  AT::Field &push(const AT::Field *in)     { return(create_col().push(in)); }
  AT::Field &unshift(const AT::Field &in)  { return(create_col().unshift(in)); }
  AT::Field &unshift(const AT::Field *in)  { return(create_col().unshift(in)); }
  AT::Field pop()                          { return(create_col().pop()); }
  AT::Field shift()                        { return(create_col().shift()); }
  AT::Field splice(AT::Integer32 o, AT::Integer32 l=-1, const AT::Field &list=undef) { return(create_col().splice(o, l, list)); }

  AT::Boolean exists(const AT::Character *key) { return(create_col().exists(key)); }

  AT::Integer32 index(AT::Boolean pos = false, AT::Integer32 value = 0) { return(create_col().index(pos, value)); }

  AT::Field &current()                     { return(create_col().current()); }
  AT::Field &iterate(AT::Boolean restart = false) { return(create_col().iterate(restart)); }
  AT::Field &first(AT::Boolean pos = true) { return(create_col().first(pos)); }
  AT::Field &last(AT::Boolean pos = true)  { return(create_col().last(pos)); }
  AT::Field &next(AT::Boolean pos = true)  { return(create_col().next(pos)); }
  AT::Field &prior(AT::Boolean pos = true) { return(create_col().prior(pos)); }

  //
  // Miscellaneous
  //
  void sync(AT::Boolean value) { create_col().sync(value); }

  AT::Field base(AT::Integer32 target_ordinal, AT::Integer32 current_ordinal=10);

  void append(AT::Character *in);
  void uc();
  void lc();
  void chomp(AT::Boolean whitespace=false);
  AT::Unsigned32 length();

  AT::Boolean vector(void) {
    if(type() == TYPE_COL) {
      return(m_data->col->vector());
    }
    return(false);
  }
  AT::Boolean hash(void) {
    if(type() == TYPE_COL) {
      return(m_data->col->hash());
    }
    return(false);
  }

  //
  // TODO
  //
  //AT::Byte byte();
  //AT::Field substr(AT::Unsigned32 i, AT::Unsigned j=0);

  AT::Field *operator&(void) { return(this); }
  AT::Field &operator*(void) { return(*this); }

  AT::Field *reference(AT::Field *pointer=NULL) {
    if(!m_data) {
      m_data = new node;
    }
    if(pointer) {
      m_data->reference = pointer == &undef ? NULL : pointer;
    }
    return(m_data->reference);
  }

  //
  // Collection
  //
  AT::Collection *collection_nodes() {
    if(!m_data) {
      m_data = new node;
    }
    if(!m_data->collection_nodes) {
      m_data->collection_nodes = new AT::Collection;
    }
    return(m_data->collection_nodes);
  }

  void terminate(void) {
    reset();
    m_data = new node;
    m_data->type = TYPE_TRM;
  }

protected:
  class node {
    public:
    node(void) { init(); }
    void init(void) {
      type = TYPE_UND;

      i32 = 0;
      u32 = 0;
      f32 = 0;
      str = NULL;

      col = NULL;

      cast = NULL;
      count = 0;

      collection_nodes = NULL;
      reference = 0;
    }

    AT::Byte type;
    union {
      AT::Unknown    *usr;

      AT::Integer32  i32;
      AT::Unsigned32 u32;
      AT::Float32    f32;
      AT::Character  *str;

      AT::Collection *col;
    };

    AT::Character *cast;
    AT::Integer32 count;

    AT::Collection *collection_nodes;

    AT::Field *reference;
  };
  node *m_data;

  void assign_i32(const AT::Integer32 in);
  void assign_u32(const AT::Unsigned32 in);
  void assign_f32(const AT::Float32 in);
  void assign_str(const AT::Character *in, AT::Boolean alloc=true);
  void assign_col(const AT::Collection &in);
  void assign_ref(const AT::Field *in);
  void assign_usr(const AT::Unknown *in);

  AT::Collection &create_col(void) const;

  AT::Integer32  cast_i32(void) const;
  AT::Unsigned32 cast_u32(void) const;
  AT::Float32    cast_f32(void) const;
  AT::Character  *cast_str(void) const;
  AT::Collection &cast_col(void) const;
  AT::Unknown    *cast_usr(void) const;

#if defined(__lua__)
  void assign_lua(lua_State *in);
  AT::Field assign_lua_resolve(lua_State *state, AT::Integer32 index);
#endif

};

//
// WARNING: not thread safe... yet.
//
__ASUKA_EXPORT AT::Field &operator,(AT::Field &l, AT::Field &r);

__ASUKA_END_NAMESPACE


__ASUKA_EXPORT extern AT::Field undef;

#if defined(__lua__)
__ASUKA_EXPORT void lua_setfield(lua_State *state, AT::Field &data);
#endif

#endif

// Local Variables:
// mode:C++
// End:

