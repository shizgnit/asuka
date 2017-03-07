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


#include "xpath.hpp"

__ASUKA_BEGIN_NAMESPACE

__ASUKA_EXPORT AT::Field XPath::resolve(AT::Field segment) {
  AT::Field results;

  //
  // Resolve any shortcuts
  //
  if(segment.length() == 0) {
    results["axis"] = "descendant";
    results["type"] = "node";
    results["name"] = "*";
  }
  if(segment == "*") {
    results["axis"] = "child";
    results["type"] = "node";
    results["name"] = "*";
  }
  if(segment == ".") {
    results["axis"] = "self";
    results["type"] = "node";
    results["name"] = "*";
  }
  if(segment == "..") {
    results["axis"] = "parent";
    results["type"] = "node";
    results["name"] = "*";
  }

  if(results["axis"].defined()) {
    return(results);
  }

  //
  // Parse the fully qualified segments
  //  
  AT::Field parts = AT::rx("^((`w+)::)?(.+)$", segment);
  AT::Field match;

  results["axis"] = parts[0].defined() ? parts[0] : "child";
  results["axis"].lc();

  if(match = AT::rx("(`w+)`((.*)`)", parts[2])) {
    results["type"] = "function";
    results["name"] = match[0];
    results["args"] = match[1];
  }
  if(match = AT::rx("(`w+)`[(.+)`]", parts[2])) {
    results["type"] = "node";
    results["name"] = match[0];
    results["filter"] = split("`s+", match[1]);
  }

  if(!results["name"].defined()) {
    results["type"] = "node";
    results["name"] = parts[2];
  }
  
  return(results);
}

__ASUKA_EXPORT AT::Field XPath::execute(AT::Field &context, AT::Field path) {
  AT::Field segments = AT::split("/", path);

  if(AT::rx("^/", path)) {
    if(context["nodeName"] != "#document") {
      if(!context["ownerDocument"].defined()) {
        std::cerr<<"*** unable to find the owner document"<<std::endl;
        return(undef);
      }
      context = &context["ownerDocument"];
    }
    segments.shift();
  }

  //
  // Make sure only unique nodes are returned in document order
  //
  AT::Field set = traverse(context, segments);
  AT::Field results;

  AT::Field order;
  if(context["nodeName"] != "#document") {
    order = list(context["ownerDocument"], "descendant", "node");
  }
  else {
    order = list(context, "descendant", "node");
  }

  AT::Field node;
  foreach(node, order) {
    if(set[node["nodeId"]].defined()) {
      results.push(&node);
    }
  }

  return(results);
}

__ASUKA_EXPORT AT::Field XPath::traverse(AT::Field &context, AT::Field segments) {
  AT::Field results;

  AT::Field segment = segments.shift();
  AT::Field resolved = resolve(segment);
  
  if(resolved["type"] == "function") {
    std::cerr<<"*** function '"<<resolved["name"]<<"' not supported"<<std::endl;
    return(results);
  }
  
  AT::Field node;
  foreach(node, list(context, resolved["axis"], resolved["type"])) {
    if(resolved["name"] != "*" && node["nodeName"] != resolved["name"]) {
      continue;
    }
    if(resolved["filter"].defined() && !match(node, resolved["filter"])) {
      continue;
    }
    if(segments.size() == 0) {
      results[node["nodeId"]] = &node;
    }
    else {
      AT::Field entry;
      foreach(entry, traverse(node, segments).values()) {
        results[entry["nodeId"]] = &entry;
      }
    }
  }

  return(results);
}

__ASUKA_EXPORT AT::Field XPath::list(AT::Field &node, AT::Field axis, AT::Field type) {
  AT::Field results;

  if(axis == "descendant-or-self") {
    results.push(&node);
    axis = "descendant";
  }
  if(axis == "descendant") {
    if(node["childNodes"].defined()) {
      AT::Field child;
      foreach(child, node["childNodes"]) {
        if(type == "node" && child["nodeType"] == 1) {
          results.push(&child);
        }
        AT::Field result;
        foreach(result, list(child, axis, type)) {
          results.push(&result);
        }
      }
    }
  }

  if(axis == "self") {
    results.push(&node);
  }

  if(axis == "child") {
    AT::Field child;
    foreach(child, node["childNodes"]) {
      if(type == "node" && child["nodeType"] == 1) {
        results.push(&child);
      }
    }
  }

  if(axis == "parent") {
    if(node["parentNode"].defined()) {
      results.push(&node["parentNode"]);
    }
  }

  if(axis == "ancestor-or-self") {
    results.push(&node);
    axis = "ancestor";
  }
  if(axis == "ancestor") {
    AT::Field current = &node;
    while(current["parentNode"].defined()) {
      results.push(&current["parentNode"]);
    }
  }

  return(results);
}

__ASUKA_EXPORT AT::Boolean XPath::match(AT::Field &node, AT::Field &filter) {
  AT::Boolean result = false;
  for(AT::Integer i=0; i < filter.size(); i++) {
    if(i%2) { continue; }

    AT::Boolean iteration = false;

    AT::Field entry = AT::rx("(@)?(`w+)(`s*(!=|=)`s*[\"'](.+)[\"'])?", filter[i]);

    //
    // Evaluate the children
    //
    if(!entry[0].defined() && node["childNodes"].defined()) {
      AT::Field child;
      foreach(child, &node["childNodes"]) {
        if(child["nodeName"] == entry[1]) {
          iteration = true;
          break;
        }
      }
    }

    //
    // Evaluate the attributes
    //
    if(entry[0] == "@" && node["attributes"][entry[1]].defined()) {
      if(!entry[2].defined()) {
        iteration = true;
      }
      else {
        AT::Field attribute = &node["attributes"][entry[1]];
        if(attribute["childNodes"][0]["nodeValue"] == entry[4]) {
          iteration = entry[3] == "=" ? true : false;
        }
        else {
          iteration = entry[3] == "=" ? false : true;
        }
      }
    }

    //
    // Determine how this iteration effects the result
    //
    if(i > 0) {
      if(filter[i-1] == "or") {
        result = result || iteration;
      }
      if(filter[i-1] == "and") {
        result = result && iteration;
      }
    }
    else {
      result = iteration;
    }
  }
  return(result);
}


__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:

