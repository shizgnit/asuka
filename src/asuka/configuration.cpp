/*
================================================================================
  $RCSfile: configuration.cpp,v $ $Revision: 1.3 $ $State: Exp $
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


#include "configuration.hpp"

#define MAX_BUFFER 2048

__ASUKA_BEGIN_NAMESPACE

__ASUKA_EXPORT void Configuration::expect(AT::Character *spec, AT::Field value) {
  AT::Field list;
  AT::Field optional;
  AT::Field requirement;
  AT::Field type;

  (list, optional, requirement, type) = AT::rx("^(.+?)(([=:])(\\w))?$", spec);

  AT::Field aliases = AT::split("\\|", list);
  AT::Field unmodified = aliases.shift();
  AT::Field key = AT::lc(unmodified);

  if(!type.defined()) {
    type = "b";
  }
  else {
    //validate the type
  }

  m_specification[key]["TYPE"]  = type;
  m_specification[key]["ALIAS"] = aliases;
  m_specification[key]["UNMOD"] = unmodified;

  AT::Field alias;
  foreach(alias, &m_specification[key]["ALIAS"]) {
    alias.lc();
  }

  if(value.defined()) {
    m_specification[key]["DATA"] = value;
  }
}

__ASUKA_EXPORT AT::Field Configuration::cmdln(AT::Integer argc, AT::Character **argv, AT::Boolean passthrough) {
  AT::Field data;
  AT::Boolean extract = true;
  AT::Integer32 offset = 0;

  AT::Field result;

  for(AT::Integer i=0; i < argc; i++) {
    if(extract && strlen(argv[i]) > 1 && argv[i][0] == '-') {
      offset = 1;
      if(argv[i][1] == '-') {
        offset = 2;
      }

      if(strlen(argv[i] + offset) == 0) {
        extract = false;
        continue;
      }

      AT::Field key;
      AT::Field negate = false;
      AT::Field optional;
      AT::Field value;

      (key, optional, value) = AT::rx("^(.+?)(=(.+))?$", argv[i] + offset);

      AT::Field list;
      if(offset == 1) {
        AT::Character option[2];
        option[1] = '\0';

        AT::Character *ref = key;
        while(option[0] = *ref++) {
          list.push(option);
        }
      }
      else {
        list.push(key);
      }

      AT::Field entry;
      while(entry = list.shift()) {
        (key, negate) = lookup(entry, true);

        if(!key.defined()) {
          result.push(argv[i]);
          continue;
        }

        if(m_specification[key]["TYPE"] == "b") {
          data[key] = !negate;
        }
        else {
          if(list.count()) {
            value = join("", list);
            break;
          }
        }
      }

      if(!m_specification.exists(key)) {
        continue;
      }

      if(m_specification[key]["TYPE"] != "b") {
        if(negate) {
          data[key].terminate();
        }
        else {
          if(!value.defined() && (i+1) < argc) {
            value = argv[++i];
          }
          data[key].push(value);
        }
      }
    }
    else {
      result.push(argv[i]);
    }
  }

  AT::Field set;
  foreach(set, data.keys()) {
    m_specification[set]["DATA"] = data[set];
  }

  m_results = undef;

  return(result);
}

__ASUKA_EXPORT AT::Field Configuration::file(AT::Character *filename) {
  AT::Field data;

  AT::Field result;

  AT::Field line;
  foreach(line, AT::File(filename)) {
    if(AT::rx("^\\s*#", line)) {
      continue;
    }

    AT::Field key;
    AT::Field negate = false;
    AT::Field optional;
    AT::Field value;
    
    (key, optional, value) = AT::rx("^\\s*(.+?)\\s*(=\\s*\"([^\"]+)\")?;\n?", line);
    (key, negate) = lookup(key);
    
    if(!key.defined()) {
      continue;
    }

    if(!m_specification.exists(key)) {
      continue;
    }

    if(value.defined()) {
      if(m_specification[key]["TYPE"] == "b") {
        if(value == 0 || value == "false") {
          data[key] = false;
        }
        else {
          data[key] = true;
        }
      }
      else {
        data[key].push(value);
      }
    }
    else {
      data[key].terminate();
    }
  }

  AT::Field set;
  foreach(set, data.keys()) {
    m_specification[set]["DATA"] = data[set];
  }

  m_results = undef;

  return(result);
}

__ASUKA_EXPORT AT::Field Configuration::registry(AT::Character *path, AT::Boolean traverse) {
  AT::Field data;

  AT::Field result;
  
#if defined(__windows__)
  
  AT::Character key[MAX_BUFFER];

  HKEY read;
  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0L, KEY_READ , &read) == ERROR_SUCCESS) {
    AT::Integer32 index;
    AT::Integer32 status;

    if(traverse) {
      index = 0;
      status = RegEnumKey(read, index, key, MAX_BUFFER);
      while(status == ERROR_SUCCESS) {
        registry(AT::Field() << path << "\\" << key, traverse);
        status = RegEnumKey(read, ++index, key, MAX_BUFFER);
      }
    }

    DWORD maximum = MAX_BUFFER;
    DWORD type = 0;

    index = 0;
    status = RegEnumValue(read, index, key, &maximum, NULL, &type, NULL, NULL);
    while(status == ERROR_SUCCESS) {
      AT::Field value = "???";
      HKEY access;
      if(type == REG_DWORD) {
        DWORD contents;
        DWORD size = 4;
        if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0L, KEY_ALL_ACCESS, &access) == ERROR_SUCCESS) {
          if(RegQueryValueEx(access, key, NULL, &type, (LPBYTE)&contents, &size) == ERROR_SUCCESS) {
            value = contents;
          }
        }
        RegCloseKey(access);
      }
      if(type == REG_SZ || type == REG_EXPAND_SZ) {
        AT::Character contents[MAX_BUFFER];
        if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0L, KEY_ALL_ACCESS, &access) == ERROR_SUCCESS) {
		  maximum = MAX_BUFFER;
          if(RegQueryValueEx(access, key, NULL, &type, (LPBYTE)&contents, &maximum) == ERROR_SUCCESS) {
            value = contents;
          }
        }
        RegCloseKey(access);
      }
      if(type == REG_BINARY) {
        value = "binary";
      }
      
      data[AT::lc(key)].push(value);

      maximum = MAX_BUFFER;
      status = RegEnumValue(read, ++index, key, &maximum, NULL, &type, NULL, NULL);
    }
  }
  RegCloseKey(read);

  AT::Field set;
  foreach(set, data.keys()) {
    m_specification[set]["DATA"] = data[set];
  }

#endif

  m_results = undef;

  return(result);
}

__ASUKA_EXPORT AT::Field Configuration::cgi(void) {
  AT::Field data;

  AT::Field result;

  AT::Field method = AT::uc(getenv("REQUEST_METHOD"));
  if(!method.defined()) {
    return(result);
  }

  AT::Field source;

  if (method == "GET") {
    source = getenv("QUERY_STRING");
  }

  if (method == "POST") {
    AT::Character *buffer;
    AT::Field length = getenv("CONTENT_LENGTH");

    buffer = (AT::Character *)malloc((length+1) * sizeof(AT::Character));
    memset(buffer, 0, (length+1) * sizeof(AT::Character));
    fread(buffer, length, 1, stdin);
    source = buffer;
    free(buffer);
  }

  AT::Field part, parts = AT::split("\\&", source);
  foreach(part, parts) {
    AT::Field key;
    AT::Field negate = false;
    AT::Field value;

    (key, value) = AT::split("\\=", part, 2);

    (key, negate) = lookup(key);
    if(!key.defined()) {
      continue;
    }

    if(!m_specification.exists(key)) {
      continue;
    }

    if(value.defined()) {
      if(m_specification[key]["TYPE"] == "b") {
        if(value == 0 || value == "false") {
          data[key] = false;
        }
        else {
          data[key] = true;
        }
      }
      else {
        data[key].push(AT::URI::decode(value));
      }
    }
    else {
      data[key].terminate();
    }
  }

  AT::Field set;
  foreach(set, data.keys()) {
    m_specification[set]["DATA"] = data[set];
  }

  m_results = undef;

  return(result);
}

__ASUKA_EXPORT AT::Field &Configuration::operator[](const AT::Field &key) {
  if(!m_results.defined()) {
    m_results = *this;
  }
  return(m_results[key]);
}

__ASUKA_EXPORT AT::Field &Configuration::operator[](const AT::Character *key) {
  if(!m_results.defined()) {
    m_results = *this;
  }
  return(m_results[key]);
}

__ASUKA_EXPORT AT::Field &Configuration::operator[](AT::Character *key) {
  if(!m_results.defined()) {
    m_results = *this;
  }
  return(m_results[key]);
}

__ASUKA_EXPORT Configuration::operator AT::Field(void) {
  AT::Field collect, key;
  foreach(key, m_specification.keys()) {
    if(!m_specification[key]["UNMOD"].defined()) {
      continue;
    }
    if(m_specification[key]["DATA"].defined()) {
      collect[m_specification[key]["UNMOD"]] = m_specification[key]["DATA"];
    }
  }
  return(collect);
}

__ASUKA_EXPORT AT::Field Configuration::undefined(void) {
  AT::Field collect, key;
  foreach(key, m_specification.keys()) {
    if(!m_specification[key]["DATA"].defined()) {
      collect.push(m_specification[key]["UNMOD"]);
    }
  }
  return(collect);
}

__ASUKA_EXPORT AT::Field Configuration::lookup(AT::Field key, AT::Field negatable) {
  AT::Field lookfor = AT::lc(key);

  AT::Field results;

  if(m_specification.exists(lookfor)) {
    results[0] = lookfor;
    results[1] = false;
  }
  else {
    AT::Field alt, alias;
    foreach(alt, m_specification.keys()) {
      foreach(alias, m_specification[alt]["ALIAS"]) {
        if(lookfor == alias) {
          results[0] = *alt;
          break;
        }
      }
      if(results[0].defined()) {
        break;
      }
    }
    if(!results[0].defined() && negatable) {
      AT::Character *string = lookfor;
      if(strlen(string) > 2 && string[0] == 'n' && string[1] == 'o') {
        if(string[2] == '-') {
          results = lookup(string+3);
        }
        else {
          results = lookup(string+2);
	}
        if(results[0].defined()) {
          results[1] = true;
        }
      }
    }
  }

  return(results);
}

__ASUKA_EXPORT AT::Field Configuration::list(void) {
  AT::Field results;

  AT::Field name;
  foreach(name, m_specification.keys()) {
    results.push(AT::Field() << "  name: " << name);
    if(m_specification[name]["ALIAS"].count()) {
      AT::Field alias;
      foreach(alias, m_specification[name]["ALIAS"]) {
        results.push(AT::Field() << "* alias: " << alias);
      }
    }
    if(m_specification[name]["TYPE"]) {
      results.push(AT::Field() << "  type: " << m_specification[name]["TYPE"]);
    }
    if(m_specification[name]["DATA"].defined()) {
      if(m_specification[name]["TYPE"] == "b") {
        if(m_specification[name]["DATA"]) {
          results.push(AT::Field() << "  flag: true");
        }
        else {
          results.push(AT::Field() << "  flag: false");
        }
      }
      else {
        if(m_specification[name]["DATA"].count()) {
          AT::Field value;
          foreach(value, m_specification[name]["DATA"]) {
            results.push(AT::Field() << "  value: " << value);
          }
        }
        else {
          results.push(AT::Field() << "  <<unset>>");
        }
      }
    }
    else {
      results.push(AT::Field() << "  <<undefined>>");
    }
  }

  return(results);
}


__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
