/*
================================================================================
  $RCSfile: filesystem.cpp,v $ $Revision: 1.3 $ $State: Exp $
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


#include "filesystem.hpp"

#if defined(__windows__)

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <lmerr.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(__zlib__)
#  define ZLIB_WINAPI
#  include <zlib.h>
#endif

#endif

#if defined(__posix__)

#include <sys/types.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>
#include <utime.h>
#if defined(__zlib__)
#  include <zlib.h>
#endif
#ifndef MAP_FILE
#  define MAP_FILE 0
#endif

#endif

#ifdef __ASUKA_X86
#define SWAP_BYTES false
#endif

#ifndef SWAP_BYTES
#define SWAP_BYTES true
#endif

__ASUKA_BEGIN_NAMESPACE

//==============================================================================
// Database handle abstraction class
//------------------------------------------------------------------------------
class DHAbstract {
  public:

#if defined(__windows__)
  HANDLE handle;
  WIN32_FIND_DATA *last;
#endif

#if defined(__posix__)
  DIR *handle;
  struct dirent *last;
#endif

};

//==============================================================================
// Copy file
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Boolean cp(const char *srcfile, const char *dest) {
  DWORD dwAttrs;
  if (CopyFile(srcfile, dest, FALSE)) {
    dwAttrs = GetFileAttributes(srcfile);
    if (!(dwAttrs & FILE_ATTRIBUTE_READONLY)) {
      SetFileAttributes(dest, dwAttrs | FILE_ATTRIBUTE_READONLY);
    }
  }
  else {
    //std::cerr<<" *** unable to copy the file!"<<std::endl;
    return(false);
  }
  return(true);
}

#endif

#if defined(__posix__)

AT::Boolean cp(const AT::Character *src, const AT::Character *dest) {
  AT::Integer32 fdin, fdout;
  AT::Unknown *sbuf, *dbuf;
  struct stat sst;

  if((fdin = open(src, O_RDONLY)) < 0) {
    printf("error, unable to open source file\n");
    return(false);
  }

  if(fstat(fdin, &sst) < 0) {
    printf("error, unable to stat source file\n");
    return(false);
  }

  if( (fdout = open(dest, O_RDWR|O_CREAT|O_TRUNC, sst.st_mode)) < 0) {
    printf("error, unable to open target file.\n");
    return(false);
  }

  if(lseek(fdout, sst.st_size - 1, SEEK_SET) == -1) {
    printf("error\n");
    return(false);
  }

  if(write(fdout, "", 1) != 1) {
    printf("error\n");
    return(false);
  }

  if( (sbuf = mmap(0, sst.st_size, PROT_READ, MAP_FILE|MAP_SHARED, fdin, 0)) == (caddr_t) -1) {
    printf("error\n");
    return(false);
  }

  if( (dbuf = mmap(0, sst.st_size, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fdout, 0)) == (caddr_t) -1) {
    printf("error\n");
    return(false);
  }

  memcpy(dbuf, sbuf, sst.st_size);

  return(true);
}

#endif

//==============================================================================
// Remove file
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Boolean rm(const char *filename) {
  DeleteFile(filename);
  return(true);
}

#endif

#if defined(__posix__)

AT::Boolean rm(const AT::Character *filename) {
  remove(filename);
}

#endif

//==============================================================================
// Move file
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Boolean mv(const char *srcfile, const char *dest) {
  MoveFile(srcfile, dest);
  return(true);
}

#endif

#if defined(__posix__)

AT::Boolean mv(const AT::Character *src, const AT::Character *dest) {
  rename(src, dest);
}

#endif

//==============================================================================
// Make directory
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Boolean mkdir(const AT::Character *path) {
  return(CreateDirectory(path, NULL) ? true : false);
}

#endif

#if defined(__posix__)

mode_t _mask(AT::Field &mask) {
  mode_t bytes = 0;

  if(mask.defined()) {
    AT::Character *cmask = mask;
    AT::Integer32 offset=strlen(cmask);

    AT::Boolean valid = false;

    AT::Field octal;
    if(offset == 3 && mask <= 511 && mask > 0) {
      octal  = mask.base(8);
      cmask  = octal;
      offset = strlen(cmask);
      valid  = true;
    }
    if(offset == 4 && mask <= 7777 && mask > 0) {
      valid = true;
    }

    if(valid) {
      AT::Byte oth = (AT::Integer32)(cmask[offset-1] - 48);
      AT::Byte grp = (AT::Integer32)(cmask[offset-2] - 48);
      AT::Byte usr = (AT::Integer32)(cmask[offset-3] - 48);
      if(offset == 4) {
        AT::Byte stk = (AT::Integer32)(cmask[offset-4] - 48);
      }
      bytes |= (oth);
      bytes |= (grp << 3);
      bytes |= (usr << 6);
    }
  }

  return(bytes);
}

AT::Boolean mkdir(const AT::Character *path, AT::Field mask) {
  return(::mkdir(path, _mask(mask)));
}

#endif

//==============================================================================
// Remove directory
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Boolean rmdir(const AT::Character *path) {
  return(RemoveDirectory(path) ? true : false);
}

#endif

#if defined(__posix__)

AT::Boolean rmdir(const AT::Character *path) {
  return(::rmdir(path));
}

#endif

//==============================================================================
// Present working directory
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Field pwd(const AT::Character *path) {
  AT::Field result;
  static char current[2048];

  if(path) {
    SetCurrentDirectory(path);
  }

  GetCurrentDirectory(2048, current);
  result = current;
  return(result);
}

#endif

#if defined(__posix__)

AT::Field pwd(const AT::Character *path) {
  AT::Field result;
  static char current[2048];

  if(path) {
    chdir(path);
  }
  getcwd(current, 2048);
  result = current;
  return(result);
}

#endif

//==============================================================================
// Stats
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Field stat(const AT::Character *path) {
  AT::Field results;

  struct _stat sst;

  if(_stat(path, &sst) != 0) {
    return(results);
  }

  results[12] = "";
  results[11] = "";
  results[10] = sst.st_ctime;
  results[9]  = sst.st_mtime;
  results[8]  = sst.st_atime;
  results[7]  = sst.st_size;
  results[6]  = "";
  results[5]  = "";
  results[4]  = "";
  results[3]  = "";
  results[2]  = sst.st_mode;
  results[1]  = sst.st_dev + 'A';
  results[0]  = "";

  return(results);
}

__ASUKA_EXPORT AT::Field lstat(const AT::Character *path) {
  return(stat(path));
}

#endif

#if defined(__posix__)

AT::Field stat(const AT::Character *path) {
  AT::Field results;
  struct stat sst;

  if(!path) {
    return(results);
  }

  if(stat(path, &sst) < 0) {
    return(results);
  }

  results[12] = sst.st_blocks;
  results[11] = sst.st_blksize;
  results[10] = sst.st_ctime;
  results[9]  = sst.st_mtime;
  results[8]  = sst.st_atime;
  results[7]  = sst.st_size;
  results[6]  = sst.st_rdev;
  results[5]  = sst.st_gid;
  results[4]  = sst.st_uid;
  results[3]  = sst.st_nlink;
  results[2]  = sst.st_mode;
  results[1]  = sst.st_dev;
  results[0]  = sst.st_ino;

  return(results);
}

AT::Field lstat(const AT::Character *path) {
  AT::Field results;
  struct stat sst;

  if(!path) {
    return(results);
  }

  if(lstat(path, &sst) < 0) {
    return(results);
  }

  results[12] = sst.st_blocks;
  results[11] = sst.st_blksize;
  results[10] = sst.st_ctime;
  results[9]  = sst.st_mtime;
  results[8]  = sst.st_atime;
  results[7]  = sst.st_size;
  results[6]  = sst.st_rdev;
  results[5]  = sst.st_gid;
  results[4]  = sst.st_uid;
  results[3]  = sst.st_nlink;
  results[2]  = sst.st_mode;
  results[1]  = sst.st_dev;
  results[0]  = sst.st_ino;

  return(results);
}

#endif

//==============================================================================
// Filetype
//------------------------------------------------------------------------------
#if defined(__windows__)
__ASUKA_EXPORT AT::Field filetype(const AT::Character *path) {
  AT::Field result = "unknown";

  AT::Field stats = stat(path);
  if(stats.size() && stats[2].defined()) {
    AT::Unsigned32 mode = stats[2];

    if(mode & _S_IFREG)  { result = "regular"; }
    if(mode & _S_IFDIR)  { result = "directory"; }
  }

  return(result);
}
#endif

#if defined(__posix__)
__ASUKA_EXPORT AT::Field filetype(const AT::Character *path) {
  AT::Field result;

  AT::Field stats = stat(path);
  if(stats.size() && stats[2].defined()) {
    AT::Unsigned32 mode = stats[2];

    if(S_ISREG(mode))  { result = "regular"; }
    if(S_ISDIR(mode))  { result = "directory"; }
    if(S_ISCHR(mode))  { result = "character"; }
    if(S_ISBLK(mode))  { result = "block"; }
    if(S_ISFIFO(mode)) { result = "fifo"; }

#if defined(S_ISLNK)
    if(S_ISLNK(mode))  { result = "link"; }
#endif

#if defined(S_ISSOCK)
    if(S_ISSOCK(mode)) { result = "socket"; }
#endif
  }

  return(result);
}
#endif

//==============================================================================
// Directory - Constructor by path
//------------------------------------------------------------------------------
__ASUKA_EXPORT Directory::Directory(const AT::Character *path) {
  m_path = NULL;

  m_dha = new DHAbstract;
  m_dha->last   = NULL;
  m_dha->handle = NULL;
  if(path) {
    this->open(path);
  }
}

//==============================================================================
// Directory - Destructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT Directory::~Directory(void) {
  this->close();
  if(m_dha) {
    delete(m_dha);
    m_dha = NULL;
  }
}

//==============================================================================
//  Directory - Type conversion
//------------------------------------------------------------------------------
__ASUKA_EXPORT Directory::operator AT::Field (void) {
  AT::Field results;
  AT::Field entry;
  while(1) {
    AT::Field entry = this->next();
    if(!entry) {
      break;
    }
    results.push(entry);
  }
  return(results);
}

//==============================================================================
// Directory - Open handle
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Boolean Directory::open(const AT::Character *path) {
  this->close();
  if(path) {
    AT::Unsigned32 length = strlen(path);
    if(path[length-1] == '\\') {
      m_path = (AT::Character *)malloc((length + 2) * sizeof(AT::Character));
      memcpy(m_path, path, length);
      m_path[length] = '*';
      m_path[length+1] = '\0';
      //std::cerr<<"setting path to '"<<m_path<<"'"<<std::endl;
    }
    else {
      m_path = _strdup(path);
    }
  }
  if(!m_path) {
    return(false);
  }

  return(true);
}

#endif

#if defined(__posix__)

AT::Boolean Directory::open(const AT::Character *path) {
  this->close();
  if(!path) {
    return(false);
  }

  m_path = strdup(path);
  if(m_path) {
    m_dha->handle = opendir(m_path);
    if(m_dha->handle == NULL) {
      //std::cerr<<" *** unable to open directory"<<std::endl;
      return(false);
    }
  }
  else {
    return(false);
  }

  return(true);
}

#endif

//==============================================================================
// Directory - Close handle
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Boolean Directory::close() {
  if(m_path) {
    free(m_path);
    m_path = NULL;
  }
  if(m_dha->last) {
    delete m_dha->last;
  }
  if(m_dha->handle) {
    FindClose(m_dha->handle);
  }

  return(true);
}

#endif

#if defined(__posix__)

AT::Boolean Directory::close() {
  if(m_path) {
    free(m_path);
    m_path = NULL;
  }
  if(m_dha->last) {
    delete m_dha->last;
  }
  if(m_dha->handle) {
    closedir(m_dha->handle);
  }

  return(true);
}

#endif


//==============================================================================
// Directory - Next entry
//------------------------------------------------------------------------------
#if defined(__windows__)

__ASUKA_EXPORT AT::Field Directory::next() {
  AT::Field value;

  if(!m_dha->last) {
    m_dha->last = new WIN32_FIND_DATA;

    m_dha->handle = FindFirstFile(m_path, m_dha->last);
    if (m_dha->handle == INVALID_HANDLE_VALUE) {
      std::cerr<<" *** unable to locate the first file!"<<std::endl;
      return(value);
    }
    value = m_dha->last->cFileName;
  }
  else {
    if(FindNextFile(m_dha->handle, m_dha->last)) {
      value = m_dha->last->cFileName;
    }
    else {
      if (GetLastError() == ERROR_NO_MORE_FILES) {
        return(value);
      }
      else {
        std::cerr<<" *** unable to locate the next file!"<<std::endl;
        return(value);
      }
    }
  }

  return(value);
}

#endif

#if defined(__posix__)

AT::Field Directory::next() {
  AT::Field value;

  if(m_dha->handle) {
    if(m_dha->last = readdir(m_dha->handle)) {
      value = m_dha->last->d_name;
    }
  }

  return(value);
}

#endif


//==============================================================================
// File - Init
//------------------------------------------------------------------------------
__ASUKA_EXPORT void File::init(void) {
  m_data = NULL;
}

//==============================================================================
// File - Constructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT File::File(void) {
  init();
}

//==============================================================================
// File - Constructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT File::File(const File &source) {
  init();
  copy(source);
}

//==============================================================================
// File - Constructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT File::File(AT::Character *src, AT::Boolean load_cache) {
  init();
  source(src, load_cache);
}

//==============================================================================
// File - Destructor
//------------------------------------------------------------------------------
__ASUKA_EXPORT File::~File(void) {
  release();
}

//==============================================================================
// File - Assignment operator
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::File &File::operator=(const AT::File &source) {
  copy(source);
  return(*this);
}

//==============================================================================
// File - Copy
//------------------------------------------------------------------------------
__ASUKA_EXPORT void File::copy(const AT::File &source) {
  if(source.m_data) {
    release();
    m_data = source.m_data;
    m_data->count++;
  }
}

//==============================================================================
// File - Set source
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Character *File::source(AT::Character *src, AT::Boolean load_cache) {
  if(src != NULL) {
    release();
    m_data = new node;
    m_data->src = _strdup(src);
    if(load_cache) {
      cache();
    }
  }
  return(m_data->src);
}

//==============================================================================
// File - Cache
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Byte *File::cache(AT::Byte *contents, AT::Unsigned32 size, AT::Boolean alloc, AT::Character *src) {
  if(contents) {
    if(m_data) {
      release();
    }
    if(!m_data) {
      m_data = new node;
    }
    m_data->contents = contents;
    m_data->size = size;
    m_data->alloc = alloc;
    if(src != NULL) {
      m_data->src = _strdup(src);
    }
    return(m_data->contents);
  }

  if(m_data) {
    if(m_data->contents) {
      return(m_data->contents);
    }
    if(!m_data->src) {
      return(NULL);
    }
  }
  else {
    return(NULL);
  }

  AT::Unsigned32 current = getpos();

  if(m_data->stream == NULL) {
    std::cout<<"*** fs: loading file "<<m_data->src<<std::endl;
    if((m_data->stream = fopen(m_data->src, "rb")) == NULL) {
      std::cout<<"*** fs: failed"<<m_data->src<<std::endl;
      return(NULL);
    }
  }
  else {
    setpos(0);
  }

  m_data->contents = (AT::Byte *)malloc(BUFFER_SIZE);
  AT::Byte *read_contents = (AT::Byte *)malloc(BUFFER_SIZE);

  size_t end_ptr = 0;
  size_t bytes_read;

  while(!feof(m_data->stream)) {
    bytes_read = fread(read_contents, sizeof(AT::Character), BUFFER_SIZE - 1, m_data->stream);
    if(ferror(m_data->stream)) {
      std::cout<<"*** fs: error reading stream"<<std::endl;
      return(NULL);
    }
    std::cout<<"*** fs: read "<<bytes_read<<" bytes"<<std::endl;

    memcpy((void *)(m_data->contents + end_ptr), (void *)read_contents, bytes_read);

    end_ptr += bytes_read;
    std::cout<<"*** fs: current total "<<end_ptr<<" bytes"<<std::endl;
    m_data->contents = (AT::Byte *)realloc(m_data->contents, end_ptr + BUFFER_SIZE);
  }
  if(m_data->stream) {
    fclose(m_data->stream);
    m_data->stream = NULL;
  }
  m_data->size = end_ptr;
  m_data->alloc = true;

  std::cout<<"*** fs: total "<<end_ptr<<" bytes"<<std::endl;

  if(read_contents) {
    free(read_contents);
    read_contents = NULL;
  }

  setpos(current);

  return(m_data->contents);
}

//==============================================================================
// File - Release cache
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Boolean File::release(void) {
  if(m_data && --m_data->count <= 0) {
    if(m_data->src) {
      free(m_data->src);
      m_data->src = NULL;
    }
    if(m_data->stream) {
      fclose(m_data->stream);
      m_data->stream = NULL;
    }
    if(m_data->alloc && m_data->contents) {
      free(m_data->contents);
      m_data->contents = NULL;
      m_data->alloc = false;
    }
    delete(m_data);
  }
  m_data = NULL;
  return(true);
}

//==============================================================================
// File - Extract
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Unsigned32 File::extract(void *data, size_t bytes, AT::Boolean swap_byte_order) {
  if(!data) {
    return(0);
  }

  if(m_data->contents) {
    //std::cout<<"*** pulling "<<bytes<<" from the memory buffer..."<<std::endl;
    if( (bytes + m_data->ptr) > m_data->size ) {
      bytes = m_data->size - m_data->ptr;
    }
    memcpy(data, (void *)(m_data->contents + m_data->ptr), bytes);
    m_data->ptr += bytes;
  }
  else {
    //std::cout<<"*** pulling "<<bytes<<" from the file stream..."<<std::endl;
    if(m_data->stream == NULL) {
      if(m_data->src == NULL) {
        std::cerr<<"no source set!"<<std::endl;
        return(0);
      }
      if((m_data->stream = fopen(m_data->src, "rb")) == NULL) {
        std::cerr<<"failed to open the source '"<<m_data->src<<"'"<<std::endl;
        return(0);
      }
      setpos(m_data->ptr);
    }
    bytes = fread(data, sizeof(AT::Byte), bytes, m_data->stream);
    if(ferror(m_data->stream)) {
      std::cerr<<"failed to read the data!"<<std::endl;
      return(0);
    }
  }

  if(swap_byte_order) {
    AT::Byte *data_ptr = (AT::Byte *)data;
    AT::Byte *backup = (AT::Byte *)malloc(bytes);
    memcpy(backup, data, bytes);

    for(AT::Unsigned32 address = 0; address < bytes; address++) {
      AT::Byte *target_address = data_ptr + (bytes - 1 - address);
      (*target_address) = (*(backup + address));
    }

    if(backup) {
      free(backup);
    }
  }

  return(bytes);
}

//==============================================================================
// File - Extract
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field File::extract(AT::Byte delimiter) {
  AT::Field value;

  AT::Byte *collect = NULL;
  AT::Byte *buffer = (AT::Byte *)malloc(BUFFER_SIZE * sizeof(AT::Byte));

  AT::Unsigned32 bytes = 0;
  AT::Unsigned32 read;

  register AT::Unsigned32 i;

  AT::Unsigned32 current = getpos();
  while(1) {
    if(read = extract(buffer, BUFFER_SIZE)) {
      for(i=0; i < read; i++) {
        if(buffer[i] == delimiter) {
          i++;
          break;
        }
      }
      collect = (AT::Byte *)realloc(collect, (bytes+i+1) * sizeof(AT::Byte));
      memcpy(collect + bytes, buffer, i);
      bytes += i;
      collect[bytes] = '\0';
      if(delimiter != '\r' && collect[bytes-1] == '\r') {
        collect[bytes-1] = '\0';
      }
      if(collect[bytes-1] == delimiter) {
        break;
      }
    }
    else {
      break;
    }
  }

  free(buffer);

  if(!bytes) {
    return(value);
  }

  if(collect) {
    value = (AT::Character *)collect;
    free(collect);
  }

  setpos(current + bytes);

  return(value);
}

//==============================================================================
// File - Extract
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field File::extract(enum AT::storage_types type, AT::Boolean swap_byte_order) {
  AT::Field value;

  Unsigned32 size;
  void *data;

  switch(type) {
    case(AT::TYPE_I08):  size = sizeof(AT::Integer08);  break;
    case(AT::TYPE_I16):  size = sizeof(AT::Integer16);  break;
    case(AT::TYPE_I32):  size = sizeof(AT::Integer32);  break;
    case(AT::TYPE_U08):  size = sizeof(AT::Unsigned08); break;
    case(AT::TYPE_U16):  size = sizeof(AT::Unsigned16); break;
    case(AT::TYPE_U32):  size = sizeof(AT::Unsigned32); break;
    case(AT::TYPE_F32):  size = sizeof(AT::Float32);    break;
    case(AT::TYPE_F64):  size = sizeof(AT::Float64);    break;
  }

  if(!size) {
    return(value);
  }

  data = malloc(size);

  extract(data, size, swap_byte_order);

  switch(type) {
    case(AT::TYPE_I08):  value = *(AT::Integer08 *)data;  break;
    case(AT::TYPE_I16):  value = *(AT::Integer16 *)data;  break;
    case(AT::TYPE_I32):  value = *(AT::Integer32 *)data;  break;
    case(AT::TYPE_U08):  value = *(AT::Unsigned08 *)data; break;
    case(AT::TYPE_U16):  value = *(AT::Unsigned16 *)data; break;
    case(AT::TYPE_U32):  value = *(AT::Unsigned32 *)data; break;
    case(AT::TYPE_F32):  value = *(AT::Float32 *)data;    break;
    case(AT::TYPE_F64):  value = *(AT::Float64 *)data;    break;
  }

  if(data) {
    free(data);
  }

  return(value);
}

//==============================================================================
// File - Type conversion
//------------------------------------------------------------------------------
__ASUKA_EXPORT File::operator AT::Field (void) {
  AT::Field list;
  while(1) {
    AT::Field value = extract();
    if(!value) {
      break;
    }
    list.push(value);
  }
  return(list);
}

//==============================================================================
// File - Get size
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Unsigned32 File::size(void) {
  if(m_data && m_data->contents) {
    return(m_data->size);
  }
  return(0);
}

//==============================================================================
// File - Get position
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Unsigned32 File::getpos(void) {
//
// Some implementations do not support unsigned int assignments to fpos_t
// which renders these functions rather useless... it's okay since this
// library is very 32 bit bound.
//
//  if(m_stream) {
//    fgetpos(m_stream, &m_ptr);
//  }
//  return(m_ptr);
  if(m_data == NULL) {
    return(0);
  }
  if(m_data->stream) {
    m_data->ptr = ftell(m_data->stream);
  }
  return(m_data->ptr);
}

//==============================================================================
// File - Set position
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Unsigned32 File::setpos(AT::Unsigned32 ptr) {
//  m_ptr = ptr;
//  if(m_stream) {
//    fsetpos(m_stream, &m_ptr);
//  }
//  return(m_ptr);

  if(m_data == NULL) {
    return(0);
  }
  m_data->ptr = ptr;
  if(m_data->stream) {
    fseek(m_data->stream, m_data->ptr, SEEK_SET);
  }
  return(m_data->ptr);
}

//==============================================================================
// Archive - Contents list
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field &Archive::list(void) {
  if(!m_listing.defined() && m_file.defined()) {
    AT::Boolean done = false;
    while(!done) {
      AT::Field header;

      header[idx_fpos] = m_file.getpos();
      
      AT::Boolean swap = false;
      
      header[idx_signature] = m_file.extract(AT::TYPE_U32, swap);
      if(header[idx_signature] != 0x04034b50 && header[idx_signature] != 0x4b500403 ) {
        m_file.setpos(header[idx_fpos]);
        done = true;
        std::cerr<<"*** done extracting header data"<<std::endl;
        continue;
      }

      header[idx_version] = m_file.extract(AT::TYPE_U16, swap);
      header[idx_general] = m_file.extract(AT::TYPE_U16, swap);
      header[idx_method]  = m_file.extract(AT::TYPE_U16, swap);
      header[idx_mtime]   = m_file.extract(AT::TYPE_U16, swap);
      header[idx_mdate]   = m_file.extract(AT::TYPE_U16, swap);
      header[idx_crc32]   = m_file.extract(AT::TYPE_U32, swap);
      header[idx_csize]   = m_file.extract(AT::TYPE_U32, swap);
      header[idx_usize]   = m_file.extract(AT::TYPE_U32, swap);
      header[idx_flength] = m_file.extract(AT::TYPE_U16, swap);
      header[idx_elength] = m_file.extract(AT::TYPE_U16, swap);

      AT::Character *filename = (AT::Character *)malloc((header[idx_flength] + 1) * sizeof(AT::Character));
      AT::Character *extra    = (AT::Character *)malloc((header[idx_elength] + 1) * sizeof(AT::Character));

      memset(filename, 0, (header[idx_flength] + 1) * sizeof(AT::Character));
      memset(extra, 0, (header[idx_elength] + 1) * sizeof(AT::Character));

      m_file.extract(filename, header[idx_flength], false);
      m_file.extract(extra, header[idx_elength], false);

      header[idx_filename] = filename;
      header[idx_extra] = extra;

      std::cerr<<"*** listing for '"<<filename<<"'"<<std::endl;
      
      free(filename);
      free(extra);

      m_file.setpos(m_file.getpos() + header[idx_csize]);

      AT::Unsigned16 general = header[idx_general];
      if(general & 4) {
        std::cerr<<"extracting the data descriptor!!!"<<std::endl;
        header[idx_crc32] = m_file.extract(AT::TYPE_U32, swap);
        header[idx_csize] = m_file.extract(AT::TYPE_U32, swap);
        header[idx_usize] = m_file.extract(AT::TYPE_U32, swap);
      }

      m_listing.push(&header);
    }
  }
  return(m_listing);
}

//==============================================================================
// Archive - Contents list
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Archive::locator(const AT::Character *name) {
  AT::Field header;
  foreach(header, list()) {
    if(header[idx_filename] == name) {
      return(header);
    }
  }
  return(header);
}

//==============================================================================
// Archive - Extract
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::File Archive::extract(AT::Field &header) {
  AT::File results;
  
  //
  // Do some simply validations
  //
  if(!header.defined()) {
    return(results);
  }
  if(header.size() != 14) {
    return(results);
  }
  if(header[idx_csize] == 0) {
    return(results);
  }

  //
  // Make sure dependencies have the complete path to the archive
  //
  AT::Field path = header[idx_filename];
  if(AT::Path::relative(header[idx_filename])) {
    AT::Field archive = m_file.source();
    archive = AT::substitute(archive, "[\\/]*[^\\/]+.zip$", "");
    path = AT::Path::resolve(header[idx_filename], archive);
    std::cerr<<"*** resolved path as '"<<path<<"'"<<std::endl;
  }

  m_file.setpos(header[idx_fpos] + 30 + header[idx_flength] + header[idx_elength]);

  AT::Byte *src  = (AT::Byte *)malloc(header[idx_csize] + 2);
  AT::Byte *dest = (AT::Byte *)malloc(header[idx_usize] + 2);

  memset(src,  0, header[idx_csize] + 2);
  memset(dest, 0, header[idx_usize] + 2);

#if defined(__zlib__)
  z_stream stream;
  if (header[idx_method] == Z_DEFLATED) {
    stream.zalloc = NULL;
    stream.zfree  = NULL;
    stream.opaque = NULL;

    stream.next_in = NULL;
    stream.avail_in = 0;

    int err;
    if((err=inflateInit2(&stream, -MAX_WBITS)) == Z_OK) {
      m_file.extract(src, header[idx_csize]);

      stream.next_in = src;
      stream.avail_in = header[idx_csize];

      stream.next_out = dest;
      stream.avail_out = header[idx_usize];

      int rval;
      if((rval = inflate(&stream, Z_FINISH)) != Z_STREAM_END) {
        std::cerr<<"!!! failed to uncompress the buffer: ";
        switch(rval) {
          case(Z_STREAM_ERROR):
            std::cerr<<"Z_STREAM_ERROR"<<std::endl;
            break;
          case(Z_MEM_ERROR):
            std::cerr<<"Z_MEM_ERROR"<<std::endl;
            break;
          case(Z_BUF_ERROR):
            std::cerr<<"Z_BUF_ERROR"<<std::endl;
            break;
          case(Z_DATA_ERROR):
            std::cerr<<"Z_DATA_ERROR"<<std::endl;
            break;
          default:
            std::cerr<<"UNKNOWN"<<std::endl;
            break;
        };
      }
      else {
        inflateEnd(&stream);
      }
    }
    else {
      std::cerr<<"!!! failed init"<<std::endl;
    }
  }
  if(header[idx_method] == 0) {
    std::cerr<<"*** attempting to extract a non-compressed buffer"<<std::endl;
    m_file.extract(dest, header[idx_csize]);
    std::cerr<<"*** done"<<std::endl;
  }
#else
  std::cerr<<"library built without zlib..."<<std::endl;
#endif

  free(src);
  results.cache(dest, header[idx_usize], true, path);

//  std::cerr<<"*** cached data in '"<<results.source()<<"'"<<std::endl;
  
  return(results);
}

//==============================================================================
// Path - Delimiter
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Path::delimiter(const AT::Character *path) {
  AT::Field results;
  AT::Field search = AT::rx("([\\/])", path);
  results = search[0].defined() ? search[0] : "/";
  return(results);
}

//==============================================================================
// Path - Relative
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Boolean Path::relative(const AT::Character *path) {
  if(AT::rx("^[\\/]", path) || AT::rx("^`w:[\\/]", path)) {
    return(false);
  }
  return(true);
}

//==============================================================================
// Path - Base name
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Path::basename(const AT::Character *path) {
  AT::Field results = AT::rx("([^\\/]+)$", path);
  return(results[0]);
}

//==============================================================================
// Path - Dir name
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Path::dirname(const AT::Character *path) {
  AT::Field results = AT::rx("^(.+)[\\/][^\\/]+$", path);
  return(results[0]);
}

//==============================================================================
// Path - Resolve paths
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Path::resolve(AT::Field relative, AT::Field current) {
  if(!current) {
    current = AT::pwd();
    std::cerr<<"set current to: "<<current<<std::endl;
  }
  AT::Field search = AT::rx("([\\/])", current);
  AT::Field delimiter = search[0].defined() ? search[0] : "/";

  AT::Field result = AT::join(delimiter, (current, relative));

  if(delimiter == "/") {
    result = AT::substitute(result, "//", "/", true);
    result = AT::substitute(result, "^`./", "");
    result = AT::substitute(result, "/`./", "/", true);
    result = AT::substitute(result, "/`.$", "/");
    result = AT::substitute(result, "[^/]*/`.`./", "", true);
    result = AT::substitute(result, "[^/]*/`.`.$", "");
    result = AT::substitute(result, "/`.`./", "/", true);
  }

  if(delimiter == "\\") {
    result = AT::substitute(result, "`\\`\\", "\\", true);
    result = AT::substitute(result, "^`.`\\", "");
    result = AT::substitute(result, "`\\`.`\\", "\\", true);
    result = AT::substitute(result, "`\\`.$", "\\");
    result = AT::substitute(result, "[^\\]*`\\`.`.`\\", "", true);
    result = AT::substitute(result, "[^\\]*`\\`.`.$", "");
    result = AT::substitute(result, "`\\`.`.`\\", "\\", true);
  }
  
  return(result);
}

//==============================================================================
// Path - Split
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field Path::split(const AT::Character *path) {
  AT::Field list;
  
  AT::Field patterns;
  patterns[0] = "^(`w:[\\/])(.+)$";
  patterns[1] = "^([\\/]+)(.+)$";
  patterns[2] = "^(`.{1,2}[\\/])(.+)$";
  
  AT::Field results;
  for(AT::Unsigned32 i=0; i<patterns.size(); i++) {
    results = AT::rx(patterns[i], path);
    if(results.defined() && results.size() == 2) {
      break;
    }
  }
  if(results) {
    list.push(results[0]);
    list.push(AT::split("[\\/]+", results[1]));
  }
  else {
    list.push(undef);
    list.push(AT::split("[\\/]+", path));
  }
  
  return(list);
}

//==============================================================================
// Path - Open
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::File Path::open(const AT::Character *path) {
  AT::Field character = AT::Path::delimiter(path);
  
  AT::File results;
  AT::Field stack;
  
  AT::Field list = AT::Path::split(path);
  if(!list.defined() || list.size() == 0) {
    std::cerr<<"failed to split path list!"<<std::endl;
    return(results);
  }
  
  AT::Field device = list.shift();
  if(!device.defined()) {
    stack.push(list.shift());
  }

  AT::Field current;
  AT::Field backtrack = false;
  
  while(1) {
    current = device;
    current.append(join(character, stack));
    
    if(!backtrack) {
      AT::Field type = AT::filetype(current);
      //std::cerr<<"*** current '"<<current<<"' ("<<type<<")"<<std::endl;
      if(type.length()) {
        if(type == "regular") {
          //std::cerr<<"*** file"<<std::endl;
          results.source(current);
          return(results);
        }
        if(type == "directory") {
          //std::cerr<<"*** directory"<<std::endl;
        }
      }
      else {
        AT::Field entry;
        while(entry = list.shift()) {
          stack.push(entry);
        }
      }
      if(list.size() == 0) {
        backtrack = true;
        continue;
      }
      stack.push(list.shift());
    }
    else {
      list.unshift(stack.pop());
      AT::Field archive = current;
      archive.append(".zip");
      //std::cout<<"*** looking for archive  '" << archive <<"'"<<std::endl;
      if(AT::filetype(archive) == "regular") {
        AT::Field relative = AT::join(character, list);
        AT::Archive temporary(archive);
        AT::Field header = temporary.locator(relative);
        results = temporary.extract(header);
        return(results);
      }
      if(stack.size() == 0) {
        std::cerr<<"*** invalid source file specified '"<<path<<"'"<<std::endl;
        break;
      }
    }
  }
  return(results);
}


__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
