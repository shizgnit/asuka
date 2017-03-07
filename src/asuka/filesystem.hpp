/*
================================================================================
  $RCSfile: filesystem.hpp,v $ $Revision: 1.1.1.1 $ $State: Exp $
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

#ifndef __ASUKA_FILESYSTEM_HPP
#define __ASUKA_FILESYSTEM_HPP

__ASUKA_BEGIN_NAMESPACE

#define BUFFER_SIZE 72536

__ASUKA_EXPORT AT::Boolean rm(const AT::Character *filename);
__ASUKA_EXPORT AT::Boolean mv(const AT::Character *src, const AT::Character *dest);
__ASUKA_EXPORT AT::Boolean cp(const AT::Character *src, const AT::Character *dest);

__ASUKA_EXPORT AT::Boolean mkdir(const AT::Character *path, AT::Field mask);
__ASUKA_EXPORT AT::Boolean rmdir(const AT::Character *path);

__ASUKA_EXPORT AT::Field pwd(const AT::Character *path=NULL);

__ASUKA_EXPORT AT::Field stat(const AT::Character *path);
__ASUKA_EXPORT AT::Field lstat(const AT::Character *path);

__ASUKA_EXPORT AT::Field filetype(const AT::Character *path);


class DHAbstract;

class __ASUKA_EXPORT Directory {
public:
  Directory(const AT::Character *path=NULL);
  ~Directory(void);

  AT::Boolean open(const AT::Character *path);
  AT::Boolean close();

  AT::Field next();

  operator AT::Field (void);

private:
  AT::Character *m_path;

  DHAbstract *m_dha;
};


class __ASUKA_EXPORT File {
public:
  void init(void);

  File(void);
  File(const File &copy);
  File(AT::Character *src, AT::Boolean load_cache=false);

  ~File(void);

  void copy(const AT::File &source);
  AT::File &operator=(const AT::File &source);

  AT::Character *source(AT::Character *src=NULL, AT::Boolean load_cache=false);

  AT::Byte *cache(AT::Byte *contents=NULL, AT::Unsigned32 size=0, AT::Boolean alloc=false, AT::Character *src=NULL);
  AT::Boolean release(void);

  AT::Unsigned32 extract(AT::Unknown *data, size_t bytes, AT::Boolean swap_byte_order=false);
  AT::Field extract(AT::Byte delimiter='\n');
  AT::Field extract(enum AT::storage_types type, AT::Boolean swap_byte_order=false);

  operator AT::Field (void);

  AT::Unsigned32 size(void);

  AT::Unsigned32 getpos(void);
  AT::Unsigned32 setpos(AT::Unsigned32 ptr);

  AT::Boolean defined(void) {
    return(m_data != NULL);
  }

private:
  class node {
    public:
    node(void) { init(); }
    void init(void) {
      src = NULL;
      stream = NULL;
      contents = NULL;
      alloc = false;
      ptr = 0;
      count = 1;
    }

    AT::Character *src;

    FILE *stream;

    AT::Byte *contents;
    AT::Boolean alloc;
    size_t size;

    AT::Unsigned32 ptr;

    AT::Unsigned32 count;
  };
  node *m_data;
};


class __ASUKA_EXPORT Archive {
  public:
  enum {
    idx_signature = 0,
    idx_version   = 1,
    idx_general   = 2,
    idx_method    = 3,
    idx_mtime     = 4,
    idx_mdate     = 5,
    idx_crc32     = 6,
    idx_csize     = 7,
    idx_usize     = 8,
    idx_flength   = 9,
    idx_elength   = 10,
    idx_fpos      = 11,
    idx_filename  = 12,
    idx_extra     = 13,
  };

  Archive(void) { /*NULL*/ }
  Archive(AT::Character *src) {
    m_file.source(src);
  }

  AT::Boolean source(AT::Character *src) {
    return(m_file.source(src) != NULL);
  }

  AT::Boolean source(const AT::File &file) {
    m_file = file;
    return(true);
  }

  AT::Field &list(void);
  AT::Field locator(const AT::Character *name);

  AT::File extract(AT::Field &header);

  private:
  AT::File  m_file;
  AT::Field m_listing;
};

namespace Path {
  __ASUKA_EXPORT AT::Boolean relative(const AT::Character *path);

  __ASUKA_EXPORT AT::File open(const AT::Character *path);

  __ASUKA_EXPORT AT::Field split(const AT::Character *path);
  __ASUKA_EXPORT AT::Field delimiter(const AT::Character *path);
  __ASUKA_EXPORT AT::Field basename(const AT::Character *path);
  __ASUKA_EXPORT AT::Field dirname(const AT::Character *path);
  __ASUKA_EXPORT AT::Field resolve(AT::Field relative, AT::Field current=undef);
};


__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:
