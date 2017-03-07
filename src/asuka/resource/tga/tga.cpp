/*
================================================================================
  $RCSfile: tga.hpp,v $ $Revision: 1.4 $ $State: Exp $
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

class __ASUKA_MODULE_EXPORT tga : public AT::Resource::Type {
  public:
    tga(void) : AT::Resource::Type("tga", "image", "tga", "TARGA TRUEVISION") {
      std::cerr<<"created the tga parser"<<std::endl;
    }

    ~tga(void) {
      std::cerr<<"released the tga parser"<<std::endl;
    }

    AT::Boolean _release(AT::Field &image) {
      if((void *)image["buffer"][0]) {
        std::cerr<<"releasing image buffer"<<std::endl;
        free((AT::Byte *)(void *)image["buffer"][0]);
      }
      return(true);
    }

    AT::Boolean _validate(AT::File &source) {
      return(true);
    }

    AT::Boolean _input(AT::File &source, AT::Field &results, AT::Field &cache, AT::Field &options) {
	  std::ofstream output("C:\\Logs\\tga.txt", std::ios_base::app);

	  output<<"============================================="<<std::endl;
	  output<<"source: "<<source<<std::endl;

      AT::Field header;

      AT::Byte byte;
      AT::Integer32 header_binary[18];

      source.setpos(0);

      for(AT::Integer32 i=0; i<18; i++) {
        source.extract((char *)&byte, sizeof(byte));
        header_binary[i] = byte;
      }

      output<< "read the header" << std::endl;

      //make sense of the bytes gathered
      header["format"]     = "BGR";
      header["type"]       = (AT::Byte) header_binary[2];
      header["width"]      = header_binary[12] | (header_binary[13]<<8);
      header["height"]     = header_binary[14] | (header_binary[15]<<8);
      header["bpp"]        = (AT::Byte) header_binary[16];
      header["id_length"]  = (AT::Byte) header_binary[0];
      header["x_offset"]   = header_binary[8]  | (header_binary[9]<<8);
      header["y_offset"]   = header_binary[10] | (header_binary[11]<<8);
      header["descriptor"] = (AT::Byte) header_binary[17];

      header["color_map"]["type"]   = (AT::Byte) header_binary[1];
      header["color_map"]["start"]  = (header_binary[3]<<8) | header_binary[4];
      header["color_map"]["length"] = (header_binary[5]<<8) | header_binary[6];
      header["color_map"]["depth"]  = (AT::Byte) header_binary[7];

      header["frames"] = 1;

      header["Bpp"] = header["bpp"] / 8;
      header["pixel_count"] = header["width"] * header["height"];
      header["buffer_size"] = header["pixel_count"] * header["Bpp"];

      output<<"*** width:  "<<header["width"]<<std::endl;
      output<<"*** height: "<<header["height"]<<std::endl;
      output<<"*** Bpp:    "<<header["Bpp"]<<std::endl;
      output<<"*** buffer: "<<header["buffer_size"]<<" bytes"<<std::endl;

      if(header["bpp"] == 32) {
		header["format"] = "BGRA";
	  }

      //we've got everything to figure out what the offset of the image is
      AT::Integer32 offset = 18 + header["id_length"] + header["color_map"]["length"];
      source.setpos(offset);

      AT::Byte order = (((AT::Byte)header["descriptor"]) & 0x30)>>4;

      //allocate memory for the image dest
      AT::Byte *temporary = (AT::Byte *)malloc(header["buffer_size"]);
      AT::Byte *buffer = NULL;

      //
      // Pull out the pixel data, unencode RLE if necessary
      //
      if(header["type"] == 9 || header["type"] == 10) {
	      AT::Integer32 target = 0;
	      AT::Byte *pixel = (AT::Byte *)malloc(header["Bpp"]);
	      while(target < header["buffer_size"]) {
          source.extract((AT::Byte *)&byte, sizeof(byte));
          AT::Integer32 count = (byte & 0x7F) + 1;
	        if(byte & 0x80) {
	          source.extract(pixel, header["Bpp"]);
	          for(AT::Integer32 i=0; i<count; i++) {
	            memcpy(&temporary[target], pixel, header["Bpp"]);
                target += (AT::Integer32)header["Bpp"];
	          }
	        }
	        else {
            AT::Integer32 size = count * (AT::Integer32)header["Bpp"];
	          source.extract(&temporary[target], size);
	          target += size;
	        }
	      }
        free(pixel);
      }
      else {
	      source.extract(temporary, header["buffer_size"]);
      }

      //
      // Normalize the pixel order so clients don't have to worry about it
      //
      if(((AT::Byte)header["descriptor"]) & 0x20) {
        buffer = (AT::Byte *)malloc(header["buffer_size"]);
        AT::Integer32 back = header["buffer_size"];
        for(AT::Integer32 i=0; i < header["buffer_size"]; i++) {
          buffer[--back] = temporary[i];
        }
        free(temporary);
      }
      else {
        buffer = temporary;
      }

      output<<"going to set result values..."<<std::endl;

      results["header"] = header;
      results["buffer"][0] = (void *)buffer;
      results = AT::Raster::format(results, "RGBA");
      free(buffer);

      output<<"done reformatting..."<<std::endl;

	  output.close();

      return(true);
    }
};

extern "C" __ASUKA_MODULE_EXPORT AT::Resource::Type *handle(void) {
  static tga *reference = new tga;
  return(reference);
}


// Local Variables:
// mode:C++
// End:
