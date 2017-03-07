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

class __ASUKA_MODULE_EXPORT obj : public AT::Resource::Type {
  public:
    obj(void) : AT::Resource::Type("obj", "mesh", "obj", "Wavefront Object Format") {
      std::cerr<<"created the obj parser"<<std::endl;
    }

    ~obj(void) {
      std::cerr<<"released the obj parser"<<std::endl;
    }

    AT::Boolean _release(AT::Field &mesh) {
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
        
        if(command == "v") {
          AT::Field vertex;
          if(elements.size() == 3) {
            vertex[0] = (AT::Float32)elements[0];
            vertex[1] = (AT::Float32)elements[1];
            vertex[2] = (AT::Float32)elements[2];
          }
          else {
            std::cerr<<"*** failed to parse vertex"<<std::endl;
          }
          results["mesh"]["vertex"].push(&vertex);
          continue;
        }
        
        if(command == "vn") {
          AT::Field normal;
          if(elements.size() == 3) {
            normal[0] = (AT::Float32)elements[0];
            normal[1] = (AT::Float32)elements[1];
            normal[2] = (AT::Float32)elements[2];
          }
          else {
            std::cerr<<"*** failed to parse vertex normal"<<std::endl;
          }
          results["mesh"]["normal"].push(&normal);
          continue;
        }
        
        if(command == "vt") {
          AT::Field texture;
          if(elements.size() == 2) {
            texture[0] = (AT::Float32)elements[0];
            texture[1] = (AT::Float32)elements[1];
          }
          else {
            std::cerr<<"*** failed to parse texture vertex"<<std::endl;
          }
          results["mesh"]["texture"].push(&texture);
          continue;
        }
        
        if(command == "f") {
          AT::Integer32 edges = elements.size() <= 3 ? elements.size() : 4;
          AT::Integer32 faces = elements.size() / edges;
          for(AT::Integer32 f=0; f<faces; f++) {
            AT::Field face;
            for(AT::Integer32 i=0; i<edges; i++) {
              AT::Field vertex, texture, normal;
              (vertex, texture, normal) = split("/", elements[i]);
              if(vertex.defined()) {
                face[i][0] = &results["mesh"]["vertex"][(int)vertex-1];
              }
              if(texture.defined()) {
                face[i][1] = &results["mesh"]["texture"][(int)texture-1];
              }
              if(normal.defined()) {
                face[i][2] = &results["mesh"]["normal"][(int)normal-1];
              }
            }
            results["mesh"]["face"].push(&face);
            AT::Field group;
            foreach(group, current_groups) {
              results["mesh"]["group"][group].push(&face);
            }
            if(current_material.defined()) {
              face["material"] = current_material;
              results["mesh"]["material"][current_material].push(&face);
            }
          }
          continue;
        }
        
        if(command == "g") {
          current_groups = elements;
          continue;
        }
        
        if(command == "usemtl") {
          current_material = elements[0];
          continue;
        }
        
        if(command == "mtllib") {
          if(m_parent) {
            AT::Field dependency = elements[0];
            if(AT::Path::relative(dependency)) {
              dependency = AT::Path::resolve(dependency, AT::Path::dirname(source.source()));
            }
            AT::Field materials = m_parent->input(dependency, cache, options);
            results["material"] = &materials["material"];
          }
          continue;
        }
        
        std::cerr<<"*** unrecognized command '"<<command<<"'"<<std::endl;
      }
      
      return(true);
    }
};

extern "C" __ASUKA_MODULE_EXPORT AT::Resource::Type *handle(void) {
  static obj *reference = new obj;
  return(reference);
}


// Local Variables:
// mode:C++
// End:
