/*
================================================================================
  $RCSfile: dom.hpp,v $ $Revision: 1.8 $ $State: Exp $
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

#ifndef __ASUKA_DOM_HPP
#define __ASUKA_DOM_HPP

__ASUKA_BEGIN_NAMESPACE

#define __DOM_BUFFERSIZE 129

#define __DOM_CHARSET_UNSET '\0'
#define __DOM_CHARSET_VALUE 'X'

#define ___ __DOM_CHARSET_UNSET
#define _X_ __DOM_CHARSET_VALUE

/*
    0   1   2   3   4   5   6   7   8   9
 0 NUL SOH STX ETX EOT ENQ ACK BEL BS  HT
 1  LF  VT  FF  CR  SO  SI DLE DC1 DC2 DC3
 2 DC4 NAK SYN ETB CAN EM  SUB ESC  FS  GS
 3  RS  US  SP  !   "   #   $   %   &   '
 4  (   )   *   +   ,   -   .   /   0   1
 5  2   3   4   5   6   7   8   9   :   ;
 6  <   =   >   ?   @   A   B   C   D   E
 7  F   G   H   I   J   K   L   M   N   O
 8  P   Q   R   S   T   U   V   W   X   Y
 9  Z   [   \   ]   ^   _   `   a   b   c
10  d   e   f   g   h   i   j   k   l   m
11  n   o   p   q   r   s   t   u   v   w
12  x   y   z   {   |   }   ~  DEL
*/

static char __DOM_CHARSET_ATTRIBUTE_VALUE[__DOM_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,_X_,___,___,___,___,___,___,
   ___,___,_X_,___,_X_,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,_X_,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,'\0'
};

static char __DOM_CHARSET_ATTRIBUTE_NAME[__DOM_BUFFERSIZE] = {
   ___,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,___,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,___,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,___,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,'\0'
};

static char __DOM_CHARSET_ELEMENT_CLOSE[__DOM_BUFFERSIZE] = {
   ___,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,___,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,___,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,___,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,___,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,
   _X_,_X_,_X_,_X_,_X_,_X_,_X_,_X_,'\0'
};

static char __DOM_CHARSET_SPACE[__DOM_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,_X_,___,___,___,___,___,___,
   ___,___,_X_,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,'\0'
};

static char __DOM_CHARSET_TRIM[__DOM_BUFFERSIZE] = {
   ___,___,___,___,___,___,___,___,___,_X_,
   _X_,___,___,_X_,___,___,___,___,___,___,
   ___,___,___,_X_,___,___,___,___,___,___,
   ___,___,_X_,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,___,___,
   ___,___,___,___,___,___,___,___,'\0'
};


#undef ___
#undef _X_

namespace DOM {

  enum type {
    ELEMENT_NODE = 1,
    ATTRIBUTE_NODE = 2,
    TEXT_NODE = 3,
    CDATA_SECTION_NODE = 4,
    ENTITY_REFERENCE_NODE = 5,
    ENTITY_NODE = 6,
    PROCESSING_INSTRUCTION_NODE = 7,
    COMMENT_NODE = 8,
    DOCUMENT_NODE = 9,
    DOCUMENT_TYPE_NODE = 10,
    DOCUMENT_FRAGMENT_NODE = 11,
    NOTATION_NODE = 12
  };

  __ASUKA_EXPORT AT::Field serialize(AT::Field &node, AT::Boolean format=false, AT::Field options=undef, AT::Integer32 level=0);

  class __ASUKA_EXPORT Node {
    friend class Document;

    public:
      Node(void) { /*NULL*/ }
      Node(const Node &instance) {
        m_node = (AT::Field *)instance.reference();
      }

      Node operator[](const AT::Character *key) {
        Node temporary;
        temporary.convert(m_node[key]);
        return(temporary);
      }

      operator AT::Field &(void) {
        return(m_node);
      }

      Node &operator=(const Node &instance) {
        m_node = (AT::Field *)instance.reference();
        return(*this);
      }

      Node &appendChild(Node &child);
      Node &appendChild(AT::Field &child) {
        Node temporary;
        temporary.convert(child);
        appendChild(temporary);

        return(*this);
      }

      Node &setAttribute(AT::Field name, AT::Field value);

      Node &setAttributeNode(Node &attribute);

      AT::Boolean hasChildNodes(void);

      AT::Field keys(void) {
        return(m_node.keys());
      }

      AT::Boolean defined() {
        return(m_node.defined());
      }

      const AT::Field *reference(void) const {
        return(&m_node);
      }

    AT::Field selectSingleNode(AT::Field path) {
      return(AT::XPath::execute(m_node, path)[0]);
    }

    AT::Field selectNodes(AT::Field path) {
      return(AT::XPath::execute(m_node, path));
    }

    protected:
      Node &convert(AT::Field &reference) {
        m_node = &reference;
        return(*this);
      }

      AT::Field m_node;
  };

  class __ASUKA_EXPORT Document : public Node {
    public:
      Document(void) {
        m_node["nodeName"] = "#document";
      }

      Document(AT::Field &documentElement) {
	m_node["nodeId"] = AT::UUID();
        m_node["documentElement"] = &documentElement;
	m_node["childNodes"][0] = &documentElement;
      }

      Node createAttribute(AT::Field name) {
        AT::Field node;
        node["nodeType"] = ATTRIBUTE_NODE;
        node["nodeName"] = name;
	node["nodeId"] = AT::UUID();
        node["ownerDocument"] = &m_node;

        Node temporary;
        temporary.convert(node);
        return(temporary);
      }
      Node createCDATASection(AT::Field data) {
        AT::Field node;
        node["nodeType"] = CDATA_SECTION_NODE;
        node["nodeName"] = "#cdata-section";
	node["nodeId"] = AT::UUID();
        node["nodeValue"] = data;
        node["ownerDocument"] = &m_node;

        Node temporary;
        temporary.convert(node);
        return(temporary);
      }
      Node createComment(AT::Field data) {
        AT::Field node;
        node["nodeType"] = COMMENT_NODE;
        node["nodeName"] = "#comment";
	node["nodeId"] = AT::UUID();
        node["nodeValue"] = data;
        node["ownerDocument"] = &m_node;

        AT::DOM::Node temporary;
        temporary.convert(node);
        return(temporary);
      }
      Node createElement(AT::Field name) {
        AT::Field node;
        node["nodeType"] = ELEMENT_NODE;
        node["nodeName"] = name;
	node["nodeId"] = AT::UUID();
        node["ownerDocument"] = &m_node;

        Node temporary;
        temporary.convert(node);
        return(temporary);
      }
      Node createTextNode(AT::Field data) {
        AT::Field node;
        node["nodeType"] = TEXT_NODE;
        node["nodeName"] = "#text";
	node["nodeId"] = AT::UUID();
        node["nodeValue"] = data;
        node["ownerDocument"] = &m_node;

        Node temporary;
        temporary.convert(node);
        return(temporary);
      }

      AT::Field loadXML(AT::Field xml);

      AT::Field serialize(AT::Boolean format=false, AT::Field options=undef) {
        return(AT::DOM::serialize(m_node["documentElement"], format, options));
      }

  };

}


__ASUKA_END_NAMESPACE

#endif

// Local Variables:
// mode:C++
// End:

