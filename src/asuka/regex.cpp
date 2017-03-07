/*
================================================================================
  $RCSfile: regex.cpp,v $ $Revision: 1.1.1.1 $ $State: Exp $
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


#include "regex.hpp"

__ASUKA_BEGIN_NAMESPACE

AT::Field rx_cache;

//==============================================================================
// Regular expression helper routine
//------------------------------------------------------------------------------
__ASUKA_EXPORT AT::Field rx(const AT::Character *expression, const AT::Character *reference, AT::Integer32 offset) {
  if(expression == NULL || strlen(expression) <= 0) {
    return(undef);
  }
  if(reference == NULL) {
    return(undef);
  }
  int length = strlen(reference);
  if(length <= 0) {
    return(undef);
  }
  if(length == 1 && reference[length-1] == '\n') {
    return(undef);
  }

  if(!rx_cache[expression].defined()) {
    rx_cache[expression] = (AT::Unknown *)new AT::RegEx(expression);
  }
  AT::RegEx *current = (AT::RegEx *)(AT::Unknown *)rx_cache[expression];
  return(current->match(reference, offset));
}

//==============================================================================
// RegEx - State - Constructor
//------------------------------------------------------------------------------
RegEx::State::State(enum types type) {
  init();
  m_type = type;
  m_literal = 0;
  m_charset = NULL;
  m_max = 1;
  m_min = 1;
}

//==============================================================================
// RegEx - State - Constructor
//------------------------------------------------------------------------------
RegEx::State::State(char literal, bool negate) {
  init();
  m_type = LITERAL;
  m_literal = literal;
  m_charset = NULL;
  m_negate = negate;
}

//==============================================================================
// RegEx - State - Constructor
//------------------------------------------------------------------------------
RegEx::State::State(AT::Character *charset, bool negate) {
  init();
  m_type = CHARSET;
  m_literal = 0;
  if(m_type == CHARSET) {
    m_charset = 
		_strdup(charset);
  }
  m_negate = negate;
}

//==============================================================================
// RegEx - State - Destructor
//------------------------------------------------------------------------------
RegEx::State::~State(void) {
  if(m_type == CHARSET && m_charset) {
    free(m_charset);
  }
}

//==============================================================================
// RegEx - State - Init
//------------------------------------------------------------------------------
void RegEx::State::init(void) {
  m_root = NULL;
  m_count = 0;
  m_transitions = NULL;
  m_negate = false;
  m_max = 0;
  m_min = 0;
  m_greedy = true;
  m_subexp_id = 0;
}

//==============================================================================
// RegEx - State - Transition
//------------------------------------------------------------------------------
RegEx::State *RegEx::State::transition(RegEx::State *next) {
  AT::Integer32 size = (m_count + 2) * sizeof(State *);
  m_transitions = (State **)realloc(m_transitions, (++m_count + 1) * sizeof(State *));
  m_transitions[m_count-1] = next;
  return(next);
}

//==============================================================================
// RegEx - State - Multiplicity bounds
//------------------------------------------------------------------------------
void RegEx::State::boundary(bool greedy, AT::Integer32 max, AT::Integer32 min) {
  m_greedy = greedy;
  m_max = max;
  m_min = min;
}


RegEx::Stack::Stack(void) {
  m_data = NULL;
  m_size = 0;
  m_count = 0;
}

RegEx::Stack::~Stack(void) {
  for(int i=0; i < m_count; i++) {
    delete m_data[i];
  }
  if(m_data) {
    free(m_data);
  }
}

RegEx::StackNode *RegEx::Stack::push(RegEx::StackNode *value) {
  m_size += 1;
  m_count += 1;
  m_data = (StackNode **)realloc(m_data, (m_count+1) * sizeof(StackNode *));
  m_data[m_count-1] = value;
  m_data[m_count] = NULL;
  return(value);
}

RegEx::StackNode *RegEx::Stack::pop(void) {
  if(m_count > 0) {
    StackNode *top = m_data[--m_count];
    m_data[m_count] = NULL;
    return(top);
  }
  else {
    return(NULL);
  }
}

RegEx::StackNode *RegEx::Stack::last(void) {
  if(m_count > 0) {
    return(m_data[m_count-1]);
  }
  else {
    return(NULL);
  }
}

__ASUKA_EXPORT RegEx::RegEx(const AT::Character *parameter) {
  m_root = new State(START);

  m_subexp_count = 0;

  m_anchor_front = false;
  m_anchor_back = false;

  if(parameter != NULL) {
    compile(parameter);
  }
}

__ASUKA_EXPORT void RegEx::cleanup(State *start) {
  if(start == NULL) {
    start = m_root;
  }

  State *current = start;

  if(current->m_type == SUBEXP) {
    cleanup(current->m_root);
  }
  for(AT::Integer32 i=0; i<current->m_count; i++) {
    cleanup(current->m_transitions[i]);
  }
  free(current->m_transitions);
  current->m_transitions = NULL;
  delete current;
}

__ASUKA_EXPORT AT::Integer32 RegEx::compile(const AT::Character *parameter, State *root) {
  if(root == NULL) {
    root = m_root;
  }
  
  State *start = root;
  State *current = root;
  
  start->m_subexp_id = ++m_subexp_count - 1;
  
  AT::Integer32 index = 0;
  char charset[__RE_BUFFERSIZE];
  
  while(parameter[index]) {
    if(__RE_CHARSET_ESCAPE[parameter[index]] == __RE_CHARSET_VALUE && __RE_CHARSET_CONTROL[parameter[index+1]] == __RE_CHARSET_VALUE) {
      switch(parameter[index+1]) {
      case('w'):   current = current->transition(new State(__RE_CHARSET_ALPHANUMERIC));        break;
      case('W'):   current = current->transition(new State(__RE_CHARSET_ALPHANUMERIC, true));  break;
      case('d'):   current = current->transition(new State(__RE_CHARSET_NUMERIC));             break;
      case('D'):   current = current->transition(new State(__RE_CHARSET_NUMERIC, true));       break;
      case('s'):   current = current->transition(new State(__RE_CHARSET_SPACE));               break;
      case('S'):   current = current->transition(new State(__RE_CHARSET_SPACE, true));         break;
      };
      index += 1;
    }
    
    else if(__RE_CHARSET_ESCAPE[parameter[index]] == __RE_CHARSET_VALUE) {
      current = current->transition(new State(parameter[++index]));
    }
    
    else if(parameter[index] == '.') {
      current = current->transition(new State(__RE_CHARSET_ANY));
    }
    
    else if(parameter[index] == '[') {
      bool negate  = false;
      bool collate = false;
      bool equivalence = false;
      bool cc = false;
      switch(parameter[index+1]) {
      case('^'):
        negate = true;
        index++;
        break;
      case('.'):
        collate = true;
        index++;
        break;
      case('='):
        equivalence = true;
        index++;
        break;
      case(':'):
        cc = true;
        index++;
        break;
      };
      memset(charset, __RE_CHARSET_UNSET, __RE_BUFFERSIZE);
      while(parameter[index] != ']') {
        charset[parameter[index++]] = __RE_CHARSET_VALUE;
      }
      charset[__RE_BUFFERSIZE-1] = '\0';
      current = current->transition(new State(charset, negate));
    }

    else if(parameter[index] == '(') {
      current = current->transition(new State(SUBEXP));
      current->m_root = new State(START);
      current->m_root->m_root = current;
      ++index += compile(parameter + index, current->m_root);
    }

    else if(parameter[index] == ')') {
      current = current->transition(new State(STOP));
      current->m_subexp_id = start->m_subexp_id;
      current->m_root = start->m_root;
      return(index);
    }

    else if(parameter[index] == '|') {
      current = current->transition(new State(STOP));
      current->m_subexp_id = start->m_subexp_id;
      current->m_root = start->m_root;
      current = start;
    }

    else if(parameter[index] == '^' && index == 0) {
      m_anchor_front = true;
      index++;
      continue;
    }

    else if(parameter[index] == '$' && parameter[index+1] == '\0') {
      m_anchor_back = true;
      index++;
      continue;
    }

    else {
      current = current->transition(new State(parameter[index]));
    }

    AT::Integer32 min = 1;
    AT::Integer32 max = 1;
    char min_str[6];
    char max_str[6];
    bool next;
    AT::Character *endptr;
    bool greedy = true;

    switch(parameter[index+1]) {
    case('+'):
      min = 1;
      max = -1;
      index++;
      if(parameter[index+1] == '?') {
        greedy = false;
        index++;
      }
      break;

    case('*'):
      min = 0;
      max = -1;
      index++;
      if(parameter[index+1] == '?') {
        greedy = false;
        index++;
      }
      break;

    case('?'):
      min = 0;
      max = 1;
      index++;
      if(parameter[index+1] == '?') {
        greedy = false;
        index++;
      }
      break;

    case('{'):
      index += 2;
      next = false;
      memset(min_str, 0, 6);
      memset(max_str, 0, 6);
      min = -1;
      max = -1;
      while(parameter[index] != '}') {
        if(parameter[index] == ',') {
          next = true;
        }
        else {
          next ? max_str[strlen(max_str)] = parameter[index] : min_str[strlen(min_str)] = parameter[index];
        }
        index++;
      }
      if(strlen(min_str)) {
        min = strtoul(min_str, &endptr, 10);
        if(strlen(max_str)) {
          max = strtoul(max_str, &endptr, 10);
        }
        else {
          if(next) { max = min; }
        }
      }
      if(parameter[index+1] == '?') {
        greedy = false;
        index++;
      }
      break;
    };
    current->boundary(greedy, max, min);
    if(parameter[index]) { index++; }
  }

  current->transition(new State(STOP));

  return(index);
}

__ASUKA_EXPORT RegEx::Stack *RegEx::execute(const AT::Character *reference) {
  AT::Integer32 index=0;
  AT::Integer32 attempt=0;
  State *current = m_root;

  Stack *history_stack = new Stack;
  Stack *entry_stack   = new Stack;

  while(1) {
    for(AT::Integer32 i=0; i<history_stack->m_count; i++) {
      if(history_stack->m_data[i]->m_current->m_type == START) {
        continue;
      }
      if(history_stack->m_data[i]->m_current->m_type == STOP) {
        continue;
      }
      if(history_stack->m_data[i]->m_current->m_type == LITERAL) {
        continue;
      }
      if(history_stack->m_data[i]->m_current->m_type == CHARSET) {
        continue;
      }
      if(history_stack->m_data[i]->m_current->m_type == SUBEXP) {
        continue;
      }
    }
      
    bool match = false;
    switch(current->m_type) {
      case(START):
        match = true;
        break;
        
      case(STOP):
        if(current->m_root) {
          history_stack->push(new StackNode(current, index));
          current = current->m_root;
          match = true;
        }
        else {
          history_stack->push(new StackNode(current, index));
          if(m_anchor_back) {
            if(reference[index] == '\0') {
              delete entry_stack;
              return(history_stack);
            }
          }
          else {
            delete entry_stack;
            return(history_stack);
          }
        }
        break;
        
      case(LITERAL):
        if(reference[index] == current->m_literal) {
          match = true;
          index++;
        }
        break;
        
      case(CHARSET):
        if(current->m_charset[reference[index]] == __RE_CHARSET_VALUE) {
          match = !current->m_negate;
        }
        else {
          if(reference[index]) {
            match = current->m_negate;
          }
        }
        if(match) {
          index++;
        }
        break;
        
      case(SUBEXP):
        current = current->m_root;
        continue;
    };
    
    StackNode *last_entry = entry_stack->last();
    AT::Integer32 count = 0;
    AT::Integer32 transition = 0;
    if(last_entry && last_entry->m_current == current) {
      count = last_entry->m_count;
      transition = last_entry->m_transition;
    }
    entry_stack->push(new StackNode(current, index, count+1, transition));
    
    if(match) {
      history_stack->push(new StackNode(entry_stack->last()));
      
      if(current->m_greedy) {
        if(current->m_max != -1 && count+1 >= current->m_max) {
          while(entry_stack->last() && entry_stack->last()->m_current == current) {
            delete entry_stack->pop();
          }
          current = current->m_transitions[0];
        }
      }
      else {
        if(count+1 >= current->m_min) {
          while(entry_stack->last() && entry_stack->last()->m_current == current) {
            delete entry_stack->pop();
          }
          current = current->m_transitions[0];
        }
      }
    }
    else {
      while(entry_stack->last() && entry_stack->last()->m_current == current) {
        delete entry_stack->pop();
      }
      
      if(count >= current->m_min) {
        current = current->m_transitions[0];
        continue;
      }
      
      current = NULL;
      StackNode *backtrack = history_stack->last();
      while(backtrack) {
        if(backtrack == entry_stack->last()) {
          if(backtrack->m_count >= backtrack->m_current->m_min) {
            current = backtrack->m_current->m_transitions[0];
            index = backtrack->m_index;
            delete entry_stack->pop();
            break;
          }
        }
        if(backtrack->m_transition+1 < backtrack->m_current->m_count) {
          backtrack->m_transition += 1;
          current = backtrack->m_current->m_transitions[backtrack->m_transition];
          index = backtrack->m_index;
          break;
        }
        if(backtrack->m_current->m_type == LITERAL || backtrack->m_current->m_type == CHARSET) {
          if(backtrack->m_current->m_greedy) {
            if(backtrack->m_count-1 >= backtrack->m_current->m_min) {
              delete history_stack->pop();
              backtrack = history_stack->last();
              if(backtrack->m_current->m_type != START) {
                current = backtrack->m_current->m_transitions[backtrack->m_transition];
                index = backtrack->m_index;
                break;
              }
            }
          }
          else {
            if(backtrack->m_current->m_max == -1 || backtrack->m_count-1 < backtrack->m_current->m_max) {
              if(reference[backtrack->m_index]) {
                current = backtrack->m_current;
                index = backtrack->m_index;
                break;
              }
              else {
                while(history_stack->last() && history_stack->last()->m_current == backtrack->m_current) {
                  delete history_stack->pop();
                }
                if(!history_stack->last()) {
                  current = NULL;
                }
                else {
                  current = history_stack->last()->m_current;
                }
              }
            }
          }
        }
        if(backtrack->m_current->m_type == START &&
           backtrack->m_current->m_root != NULL &&
           backtrack->m_current->m_root->m_min == 0)
        {
          current = backtrack->m_current->m_root->m_transitions[0];
          index = backtrack->m_index;
          delete history_stack->pop();
          break;
        }

        delete history_stack->pop();
        backtrack = history_stack->last();
      }
      if(!current) {
        if(reference[++attempt] && !m_anchor_front) {
          index = attempt;
          current = m_root;
          while(history_stack->last()) {
            delete history_stack->pop();
          }
          while(entry_stack->last()) {
            delete entry_stack->pop();
          }
        }
        else {
          delete entry_stack;
          delete history_stack;
          return(NULL);
        }
      }
    }
  }

}

__ASUKA_EXPORT AT::Field RegEx::match(const AT::Character *reference, AT::Integer32 offset) {
  AT::Field list;

  if(reference == NULL) {
    return(list);
  }

  AT::Integer32 *starts = (AT::Integer32 *)malloc((m_subexp_count+1) * sizeof(AT::Integer32));

  Stack *history_stack = execute(reference);
  if(history_stack) {
    for(AT::Integer32 i=0; i<history_stack->m_count; i++) {
      if(history_stack->m_data[i]->m_current->m_type == START) {
        AT::Integer32 index = history_stack->m_data[i]->m_current->m_subexp_id;
        starts[index] = history_stack->m_data[i]->m_index;
      }
      if(history_stack->m_data[i]->m_current->m_type == STOP) {
        AT::Integer32 index = history_stack->m_data[i]->m_current->m_subexp_id;
        AT::Integer32 front = starts[index];
        AT::Integer32 back  = history_stack->m_data[i]->m_index;
        AT::Integer32 size  = back - front;

        AT::Character *buffer = (AT::Character *)malloc((size + 1) * sizeof(AT::Character));
        buffer[0] = '\0';
        if(size) {
          memcpy(buffer, reference + front, size);
          buffer[size] = '\0';
        }
        list[index] = buffer;
        
        free(buffer);
      }
    }
    delete history_stack;
  }

  if(list.defined()) {
    while(offset-- && list.shift()) { }
  }
  
  free(starts);

  return(list);
}

__ASUKA_EXPORT AT::Field RegEx::detailed_match(const AT::Character *reference) {
  AT::Field list;

  if(reference == NULL) {
    return(list);
  }

  AT::Integer32 *starts = (AT::Integer32 *)malloc((m_subexp_count+1) * sizeof(AT::Integer32));

  Stack *history_stack = execute(reference);
  if(history_stack) {
    for(AT::Integer32 i=0; i<history_stack->m_count; i++) {
      if(history_stack->m_data[i]->m_current->m_type == START) {
        AT::Integer32 index = history_stack->m_data[i]->m_current->m_subexp_id;
        starts[index] = history_stack->m_data[i]->m_index;
      }
      if(history_stack->m_data[i]->m_current->m_type == STOP) {
        AT::Integer32 index = history_stack->m_data[i]->m_current->m_subexp_id;
        AT::Integer32 front = starts[index];
        AT::Integer32 back  = history_stack->m_data[i]->m_index;
        AT::Integer32 size  = back - front;

        AT::Character *buffer = (AT::Character *)malloc((size + 1) * sizeof(AT::Character));
        if(size) {
          memcpy(buffer, reference + front, size);
          buffer[size] = '\0';
        }
        
        list[index][0] = buffer;
        list[index][1] = front;
        list[index][2] = back;

        free(buffer);
      }
    }
    delete history_stack;
  }

  free(starts);

  return(list);
}

__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
