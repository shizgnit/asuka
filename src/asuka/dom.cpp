/*
================================================================================
  $RCSfile: dom.cpp,v $ $Revision: 1.7 $ $State: Exp $
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


#include "dom.hpp"

__ASUKA_BEGIN_NAMESPACE

__ASUKA_EXPORT DOM::Node &DOM::Node::appendChild(DOM::Node &child) {
  child.m_node["parentNode"] = &m_node;
  m_node["childNodes"].push(&child.m_node);
  m_node["firstChild"] = &m_node["childNodes"][0];
  m_node["lastChild"] = &m_node["childNodes"][m_node["childNodes"].count()-1];

  return(*this);
}

__ASUKA_EXPORT DOM::Node &DOM::Node::setAttribute(AT::Field name, AT::Field value) {
  DOM::Document document;
  
  document.convert(m_node["ownerDocument"]);

  DOM::Node attribute = document.createAttribute(name);
  DOM::Node text = document.createTextNode(value);

  attribute.appendChild(text);

  m_node["attributes"][name] = &attribute.m_node;

  return(*this);
}

__ASUKA_EXPORT DOM::Node &DOM::Node::setAttributeNode(DOM::Node &attribute) {
  m_node["attributes"][attribute["nodeName"]] = &attribute.m_node;
  return(*this);
}

__ASUKA_EXPORT AT::Boolean DOM::Node::hasChildNodes(void) {
  return(m_node["childNodes"].count() ? true : false);
}


__ASUKA_EXPORT AT::Field DOM::Document::loadXML(AT::Field xml) {
  AT::Field results;

  if(!xml.defined()) {
    return(results);
  }

  AT::Character *parse_string = xml;
  AT::Character *reference;

  AT::Integer32 length;
  AT::Character buffer[1024];

  AT::Character *dynamic = NULL;

  AT::DOM::Node current_node;

  while(*parse_string) {
    reference = parse_string;
    while(*parse_string && *parse_string != '<') {
      parse_string++;
    }
    length = parse_string - reference;
    if(length) {
      AT::Integer32 i=0;
      while(i < length && __DOM_CHARSET_TRIM[reference[i]]) { i++; }
      if(i < length) {
        dynamic = (AT::Character *)malloc((length-i+1) * sizeof(AT::Character));
        memcpy(dynamic, reference+i, length-i);

        dynamic[length-i++] = '\0';
        while(i <= length && __DOM_CHARSET_TRIM[dynamic[length-i]]) {
          dynamic[length-i++] = '\0';
        }

        current_node.appendChild(createTextNode(dynamic));

        free(dynamic);
        dynamic = NULL;
      }
    }

    if(*parse_string == '<') {
      parse_string++;

      AT::Boolean close_stack = false;
      AT::Boolean close_current = false;

      while(__DOM_CHARSET_SPACE[*parse_string]) {
        parse_string++;
      }

      if(*parse_string == '/') {
        parse_string++;
        close_stack = true;
        while(__DOM_CHARSET_SPACE[*parse_string]) {
          parse_string++;
        }
      }

      if(*parse_string == '!') {
        parse_string++;
        if(strncmp(parse_string, "[CDATA[", 7) == 0) {
          reference = parse_string;
          while(*parse_string && *parse_string != ']') {
            parse_string++;
          }
          length = parse_string - reference;
          if(length) {
            if(length > 8 && strncmp(reference, "[CDATA[", 7) == 0) {
              reference += 7;
              length -= 8;
              dynamic = (char *)malloc((length+1) * sizeof(char));
              memcpy(dynamic, reference, length);
              dynamic[length] = '\0';

              current_node.appendChild(createCDATASection(dynamic));

              free(dynamic);
              dynamic = NULL;
            }
          }
        }
      }

      reference = parse_string;
      while(__DOM_CHARSET_ELEMENT_CLOSE[*parse_string]) {
        parse_string++;
      }

      length = parse_string - reference;
      memcpy(buffer, reference, length);
      buffer[length] = '\0';

      if(close_stack) {
        if(current_node["parentNode"].defined()) {
          current_node = current_node["parentNode"];
        }
      }
      else {
        AT::DOM::Node element_node = createElement(buffer);
        if(current_node.defined()) {
          current_node.appendChild(element_node);
        }
        current_node = element_node;
      }

      while(*parse_string && *parse_string != '>') {
        while(__DOM_CHARSET_SPACE[*parse_string]) {
          parse_string++;
        }
        if(*parse_string == '/') {
          close_current = true;
          if(current_node["parentNode"].defined()) {
            current_node = current_node["parentNode"];
          }
          parse_string++;
          while(__DOM_CHARSET_SPACE[*parse_string]) {
            parse_string++;
          }
        }
        if(*parse_string == '>') {
          break;
        }

        reference = parse_string;
        while(__DOM_CHARSET_ATTRIBUTE_NAME[*parse_string]) {
          parse_string++;
        }
        if(!*parse_string) {
          std::cerr<<"attribute definition incomplete"<<std::endl;
        }

        length = parse_string - reference;
        memcpy(buffer, reference, length);
        buffer[length] = '\0';

        AT::DOM::Node attribute_node = createAttribute(buffer);

        while(__DOM_CHARSET_ATTRIBUTE_VALUE[*parse_string]) {
          parse_string++;
        }

        reference = parse_string;
        while(*parse_string && *parse_string != '"') {
          parse_string++;
        }

        length = parse_string - reference;
        memcpy(buffer, reference, length);
        buffer[length] = '\0';

        attribute_node.appendChild(createTextNode(buffer));

        current_node.setAttributeNode(attribute_node);

        parse_string++;
      }

      parse_string++;
    }

  }

  m_node["documentElement"] = &current_node.m_node;
  m_node["childNodes"][0] = &current_node.m_node;

  return(results);
}


__ASUKA_EXPORT AT::Field DOM::serialize(AT::Field &node, AT::Boolean format, AT::Field options, AT::Integer32 level) {
  AT::Field results = "";

  AT::Integer spacing = options["spacing"].defined() ? (AT::Integer)options["spacing"] : 2;
  AT::Boolean colapse = options["colapse"].defined() ? true : false;

  switch((AT::Integer32)node["nodeType"]) {
    case(ELEMENT_NODE): {
      if(format) {
        for(AT::Integer32 i=0; i<level*spacing; i++) {
          results.append(" ");
        }
      }
      results.append("<");
      results.append(node["nodeName"]);
      AT::Field key;
      foreach(key, node["attributes"].keys()) {
        AT::Field attribute = &node["attributes"][key];
        results.append(" ");
        results.append(attribute["nodeName"]);
        results.append("=\"");
        AT::Field attr;
        foreach(attr, &attribute["childNodes"]) {
          results.append(attr["nodeValue"]);
        }
        results.append("\"");
      }

      if(node["childNodes"].defined() || !options["colapse"].defined()) {
        results.append(">");

        AT::Boolean simple_text = false;
        if(node["childNodes"].size() == 1 && node["firstChild"]["nodeType"] == TEXT_NODE) {
          simple_text = true;
        }

        if(format && !simple_text) {
          results.append("\n");
        }

        AT::Field child;
        foreach(child, &node["childNodes"]) {
          results.append(serialize(child, format && !simple_text, options, level+1));
        }
        if(format && !simple_text) {
          for(AT::Integer32 i=0; i<level*spacing; i++) {
            results.append(" ");
          }
        }
        results.append("</");
        results.append(node["nodeName"]);
        results.append(">");
        if(format) {
          results.append("\n");
        }
      }
      else {
        results.append("/>\n");
      }

      break;
    }
    case(TEXT_NODE): {
      if(format) {
        for(AT::Integer32 i=0; i<level*spacing; i++) {
          results.append(" ");
        }
      }
      results.append(node["nodeValue"]);
      if(format) {
        results.append("\n");
      }
      break;
    }
    case(CDATA_SECTION_NODE): {
      results.append(node["nodeValue"]);
      results.append("\n");
      break;
    }
  };

  return(results);
}


__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:

