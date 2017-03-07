/*
================================================================================
  $RCSfile: ttf.hpp,v $ $Revision: 1.4 $ $State: Exp $
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

class __ASUKA_MODULE_EXPORT mtl : public AT::Resource::Type {
  public:
    mtl(void) : AT::Resource::Type("mtl", "material", "mtl", "Wavefront Material Format") {
      std::cerr<<"created the mtl parser"<<std::endl;
    }

    ~mtl(void) {
      std::cerr<<"released the mtl parser"<<std::endl;
    }

    AT::Boolean _release(AT::Field &font) {
      return(true);
    }

    AT::Boolean _validate(AT::File &source) {
      return(true);
    }

    AT::Boolean _input(AT::File &source, AT::Field &results, AT::Field &cache, AT::Field &options) {
      AT::Field line;
      
      AT::Field current_groups;
      AT::Field current_material;
      
      foreach(line, source) {
        line.chomp();
        if(!line.length()) {
          continue;
        }
        if(AT::rx("^\\s*#", line)) {
          std::cerr<<"*** comment: "<<line<<std::endl;
          continue;
        }
        
        AT::Field elements = AT::split("\\s+", line);
        AT::Field command = elements.shift();

        std::cerr<<"*** "<<command<<"("<<elements.size()<<") "<<AT::join(", ", elements)<<std::endl;
        
        if(command == "newmtl") {
          current_material = elements[0];
          continue;
        }
        
        if(command == "Ka") {
          results["material"][current_material]["ambient"][0] = (AT::Float32)elements[0];
          results["material"][current_material]["ambient"][1] = (AT::Float32)elements[1];
          results["material"][current_material]["ambient"][2] = (AT::Float32)elements[2];
          continue;
        }
        if(command == "Kd") {
          results["material"][current_material]["diffuse"][0] = (AT::Float32)elements[0];
          results["material"][current_material]["diffuse"][1] = (AT::Float32)elements[1];
          results["material"][current_material]["diffuse"][2] = (AT::Float32)elements[2];
          continue;
        }
        if(command == "Ks") {
          results["material"][current_material]["specular"][0] = (AT::Float32)elements[0];
          results["material"][current_material]["specular"][1] = (AT::Float32)elements[1];
          results["material"][current_material]["specular"][2] = (AT::Float32)elements[2];
          continue;
        }
        if(command == "Ke") {
          results["material"][current_material]["emission"][0] = (AT::Float32)elements[0];
          results["material"][current_material]["emission"][1] = (AT::Float32)elements[1];
          results["material"][current_material]["emission"][2] = (AT::Float32)elements[2];
          continue;
        }
        if(command == "Ns") {
          results["material"][current_material]["shininess"] = (AT::Float32)elements[0];
          continue;
        }
        if(command == "d") {
          results["material"][current_material]["opacity"] = (AT::Float32)elements[0];
          continue;
        }
        
        if(command == "map_Kd") {
          if(m_parent && current_material.defined()) {
	          AT::Field dependency = elements[0];
            if(AT::Path::relative(dependency)) {
              dependency = AT::Path::resolve(dependency, AT::Path::dirname(source.source()));
            }
            AT::Field map = m_parent->input(dependency, cache, options);
            results["material"][current_material]["diffuse"]["map"] = &map;
          }
          continue;
        }
        
        std::cerr<<"*** unrecognized command '"<<command<<"'"<<std::endl;
      }
      return(true);
    }
};

extern "C" __ASUKA_MODULE_EXPORT AT::Resource::Type *handle(void) {
  static mtl *reference = new mtl;
  return(reference);
}


// Local Variables:
// mode:C++
// End:
