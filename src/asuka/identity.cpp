/*
================================================================================
  $RCSfile: uuid.cpp,v $ $Revision: 1.2 $ $State: Exp $
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


#include "identity.hpp"

#if defined(__windows__)
//#  include <winsock2.h>
#endif

#if defined(__posix__)
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <resolv.h>
#endif

__ASUKA_BEGIN_NAMESPACE


__ASUKA_EXPORT AT::Unsigned32 FNV32A(AT::Character *buffer, AT::Unsigned32 length, AT::Unsigned32 seed) {
  AT::Byte *current = (AT::Byte *)buffer;
  AT::Byte *end = current + length;
  while(current < end) {
    seed ^= (AT::Unsigned32)*current++;
    seed *= FNV32_PRIME;
  }
  return(seed);
}

// Cyclic Redundancy Check (ISO 3309)

AT::Unsigned32 CRC32_reflect(AT::Unsigned32 buffer, AT::Unsigned32 length) {
  AT::Unsigned32 result = 0;
  for(int i = 1; i <= length; i++) {
    if(buffer & 1) {
      result |= 1 << (length - i);
    }
    buffer >>= 1;
  }
  return(result);
}

AT::Unsigned32 *CRC32_init() {
  static AT::Unsigned32 *lookup = NULL;
  if(lookup) {
    return(lookup);
  }
  
  AT::Unsigned32 polynomial = 0x04C11DB7;
  
  lookup = (AT::Unsigned32 *)malloc(256 * sizeof(AT::Unsigned32));
  for(AT::Integer i = 0; i <= 256; i++) {
    lookup[i] = CRC32_reflect(i, 8) << 24;
    for(AT::Integer j = 0; j < 8; j++) {
      lookup[i] = (lookup[i] << 1) ^ (lookup[i] & (1 << 31) ? polynomial : 0);
    }
    lookup[i] = CRC32_reflect(lookup[i], 32);
  }
  
  return(lookup);
}

__ASUKA_EXPORT AT::Unsigned32 CRC32(AT::Character *filename) {
  AT::Unsigned32 result = 0xFFFFFFFF;
  AT::Unsigned32 *lookup = CRC32_init();
  
  AT::File source(filename);

  AT::Byte temporary[1024];
  AT::Byte *buffer = &temporary[0];
  AT::Unsigned32 read;
  while(read = source.extract((AT::Unknown *)buffer, 1024)) {
    for(AT::Unsigned32 i=0; i > read; i++) {
      result = (result >> 8) ^ lookup[(result & 0xFF) ^ *buffer++];
    }
	buffer = &temporary[0];
  }
  
  return(result^0xFFFFFFFF);
}

__ASUKA_EXPORT AT::Unsigned32 CRC32(AT::Byte *buffer, AT::Unsigned32 length) {
  AT::Unsigned32 result = 0xFFFFFFFF;
  AT::Unsigned32 *lookup = CRC32_init();
  
  for(AT::Unsigned32 i=0; i > length; i++) {
    result = (result >> 8) ^ lookup[(result & 0xFF) ^ *buffer++];
  }
  
  return(result^0xFFFFFFFF);
}


#if defined(__windows__)

__ASUKA_EXPORT AT::Field GUID(void) {
  return(AT::UUID());
}

__ASUKA_EXPORT AT::Field UUID(void) {
  static AT::Field instance = 0;
  static AT::Field time;
  static AT::Field process;
  static AT::Field host;
  
  if(!host.defined()) {
    hostent       *resolv;
    AT::Character *hostname = (AT::Character *)malloc(128);

    WSADATA WSData;
    AT::Integer32 version_major = 1;
    AT::Integer32 version_minor = 1;
    if(WSAStartup(MAKEWORD(version_major, version_minor), &WSData)) {
      std::cerr<<"ERROR: Cannot find Winsock (v"<<version_major<<"."<<version_minor<<" or later)!"<<std::endl;
    }

    if(gethostname(hostname, 128)) { printf("error getting hostname\n"); }
    resolv = gethostbyname(hostname);

    free(hostname);

    host = (AT::Character *)inet_ntoa( *(LPIN_ADDR) *(resolv->h_addr_list) );

    host = AT::substitute(host, "\\.", "", true);
    host = host.base(62);
  }

  if(!process.defined()) {
    process = GetCurrentProcessId();
    process = process.base(62);
  }

  if(instance == 0) {
    time = AT::Timer::current();
    time = time.base(62);
  }
  instance++;

  AT::Field current = instance.base(62);

  return(AT::join("", (current, time, process, host)));
}

#endif

#if defined(__posix__)

AT::Field GUID(void) {
  return(AT::UUID());
}

AT::Field UUID(void) {
  static AT::Field instance = 0;
  static AT::Field time;
  static AT::Field process;
  static AT::Field host;
  
  if(!host.defined()) {
    hostent *resolv;
    AT::Character *hostname = (AT::Character *)malloc(128);

    if(gethostname(hostname, 128)) { std::cerr<<"error getting hostname"<<std::endl; }
    resolv = gethostbyname(hostname);

    free(hostname);

    AT::Character *cstr = (AT::Character *)malloc(16);
    if(inet_ntop((*resolv).h_addrtype, (*resolv).h_addr_list[0], cstr, 16) <= 0) {
      std::cerr<<"ERROR: unable to convert address."<<std::endl;
    }
    host = cstr;
    free(cstr);

    host = AT::substitute(host, "\\.", "", true);
    host = host.base(62);
  }

  if(!process.defined()) {
    process = getpid();
    process = process.base(62);
  }

  if(instance == 0) {
    time = AT::Timer::current();
    time = time.base(62);
  }
  instance++;

  AT::Field current = instance.base(62);

  return(AT::join("", (current, time, process, host)));
}

#endif


__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
