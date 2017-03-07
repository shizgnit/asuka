/*
================================================================================
  $RCSfile: xpath.cpp,v $ $Revision: 1.7 $ $State: Exp $
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


#include "json.hpp"

__ASUKA_BEGIN_NAMESPACE

__ASUKA_EXPORT AT::Field JSON::lencode(AT::Field &data, AT::Boolean format) {
  AT::Field results;

  AT::Field label;
  foreach(label, data.keys()) {
    results.append("var ");
    results.append(label);
    results.append(" = ");
    results.append(encode(data[label], format ? 0 : -1));
    results.append("; ");
  }

  return(results);
}

AT::Field JSON::encode(AT::Field &data, AT::Integer32 level) {
  AT::Field results;

  switch(data.type()) {
    case(TYPE_UND):
      results = "null";
      break;
    case(TYPE_I32):
    case(TYPE_U32):
    case(TYPE_F32):
      results = data;
      break;
    case(TYPE_STR):
      results.append("\"");
      results.append(substitute(data, "\"", "\\\"", true));
      results.append("\"");
      break;
    case(TYPE_COL):
      if(data.vector() && data.hash()) {
        if(level > 0) {
          results.append("\n");
          for(int i=0; i<level; i++) {
            results.append("  ");
          }
        }
        results.append("{");
        if(level > 0) {
          results.append("\n");
          for(int i=0; i<=level; i++) {
            results.append("  ");
          }
        }
        results.append("\"asuka-auto-nested\": ");
      }
      if(data.vector()) {
        if(data.hash()) {
          if(level >= 0 && data.hash()) {
            level++;
          }
        }
        if(level > 0) {
          results.append("\n");
          for(int i=0; i<level; i++) {
            results.append("  ");
          }
        }
        results.append("[");
        AT::Field entry;
        AT::Boolean first = true;
        for(AT::Unsigned32 i=data.offset(); i<=data.top(); i++) {
          if(!first) {
            results.append(", ");
          }
          first = false;
          results.append(encode(data[i], level>=0 ? level+1 : level));
        }
        if(level >= 0) {
          results.append("\n");
          for(int i=0; i<level; i++) {
            results.append("  ");
          }
        }
        results.append("]");
        if(level >= 0 && data.hash()) {
          level--;
        }
      }
      if(data.vector() && data.hash()) {
        results.append(", ");
      }
      if(data.hash()) {
        if(!data.vector()) {
          if(level > 0) {
            results.append("\n");
            for(int i=0; i<level; i++) {
              results.append("  ");
            }
          }
          results.append("{");
        }
        AT::Field key;
        AT::Boolean first = true;
        foreach(key, data.hkeys()) {
          if(!first) {
            results.append(", ");
          }
          first = false;
          if(level >= 0) {
            results.append("\n");
            for(int i=0; i<=level; i++) {
              results.append("  ");
            }
          }
          results.append("\"");
          results.append(key);
          results.append("\": ");
          results.append(encode(data[key], level>=0 ? level+1 : level));
        }
        if(level >= 0) {
          results.append("\n");
          for(int i=0; i<level; i++) {
            results.append("  ");
          }
        }
        results.append("}");
      }
      if(data.vector() && data.hash()) {
      }
      break;
  };

  return(results);
}

__ASUKA_EXPORT AT::Field JSON::decode(AT::Field data) {
  AT::Character *buffer = data;
  
  AT::Field results;

  while(*buffer) {
    buffer += whitespace(buffer);
    if(strncmp(buffer, "var", 3) == 0) {
      AT::Field value;
      
      buffer += 3;
      buffer += whitespace(buffer);

      AT::Character *offset = buffer;
      while(*buffer != ' ' && *buffer != '=') {
        buffer += 1;
      }

      int length = buffer - offset;
      if(length > 0) {
        char *temp = (char *)malloc(length + 1);
        memset(temp, 0, length + 1);
        strncpy(temp, offset, length);
        results[temp] = &value;
        free(temp);
      }
      else {
        std::cerr<<"uh... not a clue what happened"<<std::endl;
      }

      buffer += whitespace(buffer);
      if(*buffer != '=') {
        std::cerr<<"expected an assignment operator '='"<<std::endl;
      }
      buffer += 1;
      buffer += parse_token(buffer, value);
      buffer += whitespace(buffer);
      if(*buffer != ';') {
        std::cerr<<"expected ';'"<<std::endl;
      }
    }
    buffer += 1;
  }
  
  return(results);
}

AT::Integer JSON::whitespace(AT::Character *buffer) {
  AT::Character *begin = buffer;

  while(*buffer) {
    if(*buffer == '/' && *(buffer+1) == '*') {
      buffer += 2;
      while(*buffer != '*' && *(buffer+1) != '/') {
        buffer += 1;
      }
      buffer += 2;
    }
    if(__JSON_CHARSET_SPACE[*buffer] == __JSON_CHARSET_UNSET) {
      break;
    }
    buffer += 1;
  }

  return(buffer - begin);
}

AT::Integer JSON::parse_token(AT::Character *buffer, AT::Field &results) {  
  AT::Character *begin = buffer;
  AT::Integer mode = UNSET;
  
  buffer += whitespace(buffer);
  
  if(*buffer == '"') {
    mode = STRING;
    buffer += 1;
  }
  else if(__JSON_CHARSET_NUMBER[*buffer] == __JSON_CHARSET_VALUE) {
    mode = NUMBER;
  }
  else if(*buffer == '{') {
    mode = OBJECT;
  }
  else if(*buffer == '[') {
    mode = ARRAY;
  }
  else if(*buffer == 'n' && strncmp(buffer, "null", 4) == 0) {
    buffer += 4;
    mode = UNDEF;
  }
  else {
    std::cerr<<"token parse error"<<std::endl;
    exit(1);
  }
  
  if(mode == UNDEF) {
    return(buffer - begin);
  }
  
  if(mode == OBJECT || mode == ARRAY) {
    return(buffer - begin + parse_type(buffer, results));
  }
  
  AT::Character *offset = buffer;
  AT::Boolean fp = false;
  if(mode == NUMBER) {
    while(__JSON_CHARSET_NUMBER[*buffer] == __JSON_CHARSET_VALUE) {
      if(*buffer == '.') {
        fp = true;
      }
      buffer += 1;
    }
  }
  if(mode == STRING) {
    while(*buffer != '"' && *(buffer-1) != '\\') {
      buffer += 1;
    }
  }
  int length = buffer - offset;
  if(length > 0) {
    char *temp = (char *)malloc(length + 1);
    memset(temp, 0, length + 1);
    strncpy(temp, offset, length);
    results = temp;
    free(temp);
    if(mode == NUMBER) {
      if(fp) {
        results = (AT::Float32)results;
      }
      else {
        results = (AT::Integer32)results;
      }
    }
  }
  else {
    std::cerr<<"uh... not a clue what happened"<<std::endl;
  }
  
  return(mode == STRING ? buffer - begin + 1 : buffer - begin);
}

AT::Integer JSON::parse_type(AT::Character *buffer, AT::Field &results) {
  AT::Character *begin = buffer;
  AT::Integer mode = UNSET;
  
  while(1) {
    buffer += whitespace(buffer);
    
    if(mode == UNSET) {
      if(*buffer == '{') {
        mode = OBJECT;
        buffer += 1;
      }
      else if(*buffer == '[') {
        mode = ARRAY;
        buffer += 1;
      }
      else {
        std::cerr<<"type parse error"<<std::endl;
        exit(1);
      }
    }

    AT::Field token;

    buffer += parse_token(buffer, token);
    buffer += whitespace(buffer);
    if(mode == OBJECT) {
      if(*buffer != ':') {
        std::cerr<<"expected the label/value delimiter ':'"<<std::endl;
        exit(1);
      }
      AT::Field value;
      buffer += parse_token(++buffer, value);
      results[token] = &value;
      buffer += whitespace(buffer);
      if(*buffer == '}') {
        return(++buffer - begin);
      }
      if(*buffer != ',') {
        std::cerr<<"expected the label/value list delimiter ','"<<std::endl;
        exit(1);
      }
      buffer += 1;
    }
    if(mode == ARRAY) {
      results.push(&token);
      if(*buffer == ']') {
        return(++buffer - begin);
      }
      if(*buffer != ',') {
        std::cerr<<"expected the list delimiter ','"<<std::endl;
        exit(1);
      }
      buffer += 1;
    }
  }
}


__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:

