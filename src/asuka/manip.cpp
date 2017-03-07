/*
================================================================================
  $RCSfile: string_manip.cpp,v $ $Revision: 1.2 $ $State: Exp $
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


#include "manip.hpp"

__ASUKA_BEGIN_NAMESPACE

__ASUKA_EXPORT AT::Field substitute(AT::Field buffer, const AT::Character *expression, const AT::Character *replacement, AT::Boolean global) {
  AT::Character *cbuffer = _strdup(buffer);
  AT::Integer32 length   = (AT::Integer32)strlen(replacement);
  AT::RegEx regex(expression);
  
  AT::Unsigned32 offset = 0;
  
  do {
    if(cbuffer && strlen(cbuffer) <= offset) {
      break;
    }
    AT::Field results = regex.detailed_match(cbuffer + offset);

    if(!regex.detailed_match(cbuffer)) { break; }

    AT::Integer32 match_start  = results[0][1] + offset;
    AT::Integer32 match_end    = results[0][2] + offset;
    AT::Integer32 match_length = results[0][2] - results[0][1];

    if(match_length <= 0) { break; }

    if(match_length < length) {
      AT::Integer32 buffer_size = (AT::Integer32)(strlen(cbuffer) + (length - match_length) + 1) * sizeof(AT::Character);
      cbuffer = (AT::Character *)realloc(cbuffer, buffer_size);
    }

    AT::Character *temp         = _strdup(cbuffer + match_end);
    AT::Character *target_start = cbuffer + match_start;
    AT::Character *target_end   = cbuffer + match_start + length;
    AT::Character *buffer_end   = target_end + strlen(temp);

    memcpy(target_start, replacement, length);
    memcpy(target_end, temp, strlen(temp));

    (*buffer_end) = '\0';

    free(temp);
    
    offset = target_end - cbuffer;
  } while(global);

  AT::Field modified_buffer = cbuffer;
  free(cbuffer);

  return(modified_buffer);
}

__ASUKA_EXPORT AT::Field substitute(AT::Field buffer, AT::Field &replacement) {
  AT::Field results = buffer;

  if(replacement.type() == TYPE_COL) {
    AT::Field expression = "%";
    for(AT::Field i=0; i < replacement.count(); i++) {
      AT::Field key = AT::join("", (expression, i));
      results = AT::substitute(results, key, replacement[i], true);
    }
  }
  else {
    results = AT::substitute(results, "%0", replacement, true);
  }

  return(results);
}

__ASUKA_EXPORT AT::Field split(const AT::Character *expression, AT::Field buffer, AT::Integer32 limit) {
  AT::Field list;
  AT::Character *cbuffer = buffer;
  AT::RegEx regex(expression);

  AT::Integer32 iteration = 1;

  do {
    AT::Field results = regex.detailed_match(cbuffer);

    if(!regex.detailed_match(cbuffer)) { break; }

    AT::Integer32 match_start  = results[0][1];
    AT::Integer32 match_end    = results[0][2];
    AT::Integer32 match_length = results[0][2] - results[0][1];

    if(list.count()) {
      list.pop();
    }

    AT::Character *temp = (AT::Character *)malloc((match_start + 1) * sizeof(AT::Character) );
    memcpy(temp, cbuffer, match_start);
    temp[match_start] = '\0';

    list.push(temp);

    cbuffer += match_end;

    list.push(cbuffer);

    free(temp);
  } while(++iteration != limit);

  if(!list.count()) {
    list.push(buffer);
  }

  return(list);
}

__ASUKA_EXPORT AT::Field join(const AT::Character *delimiter, AT::Field list) {
  AT::Field result;
  if(!list.size()) {
    return(result);
  }

  AT::Integer32 delimiter_size = (AT::Integer32)strlen(delimiter);
  if(delimiter_size == 0) {
    result = (AT::Character *)list;
    return(result);
  }

  AT::Integer32 allocate = (AT::Integer32)strlen(list) + (delimiter_size * list.count()) + 1;
  AT::Character *cbuffer = (AT::Character *)malloc(allocate * sizeof(AT::Character));

  AT::Integer32 target = 0;

  AT::Field string;
  foreach(string, list) {
    AT::Integer32 string_size = (AT::Integer32)strlen(string);

    memcpy(cbuffer + target, (AT::Character *)string, string_size);
    target += string_size;

    memcpy(cbuffer + target, (AT::Character *)delimiter, delimiter_size);
    target += delimiter_size;
  }

  if(target > delimiter_size) {
    target -= delimiter_size;
    cbuffer[target] = '\0';
  }

  result = cbuffer;
  free(cbuffer);

  return(result);
}

__ASUKA_EXPORT AT::Field lc(const AT::Character *string) {
  AT::Field result;
  if(string) {
    AT::Character *ref = _strdup(string);
    for(AT::Integer32 i=0; i<strlen(ref); i++) {
#if defined(__posix__)
      ref[i] = _tolower(ref[i]);
#else
      ref[i] = tolower(ref[i]);
#endif
    }
    result = ref;
    free(ref);
  }
  return(result);
}

__ASUKA_EXPORT AT::Field uc(const AT::Character *string) {
  AT::Field result;
  if(string) {
    AT::Character *ref = _strdup(string);
    for(AT::Integer32 i=0; i<strlen(ref); i++) {
      ref[i] = _toupper(ref[i]);
    }
    result = ref;
    free(ref);
  }
  return(result);
}

__ASUKA_EXPORT AT::Field chomp(const AT::Character *string, AT::Boolean whitespace) {
  AT::Field result;
  if(string) {
    AT::Character *ref = _strdup(string);
    for(AT::Integer32 i=(AT::Integer32)strlen(ref)-1; i>=0; i--) {
      AT::Character current = ref[i];
      AT::Boolean match = false;
  
      if(whitespace && (current == ' ' || current == '\t')) {
        match = true;
      }
      if(current == '\n') {
        match = true;
      }
  
      if(!match) {
        break;
      }
      
      ref[i] = '\0';
    }
    result = ref;
    free(ref);
  }
  return(result);
}


__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
