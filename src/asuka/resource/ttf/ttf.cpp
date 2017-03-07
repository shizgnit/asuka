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

// http://freetype.sourceforge.net/freetype2/docs/reference/ft2-basic_types.html#FT_Bitmap


#include "asuka.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

class __ASUKA_MODULE_EXPORT ttf : public AT::Resource::Type {
  public:
    ttf(void) : AT::Resource::Type("ttf", "font", "ttf", "TrueType Font") {
      std::cerr<<"created the ttf parser"<<std::endl;
    }

    ~ttf(void) {
      std::cerr<<"released the ttf parser"<<std::endl;
    }

    AT::Boolean _release(AT::Field &font) {

      return(true);
    }

    AT::Boolean _validate(AT::File &source) {
      return(true);
    }

    AT::Boolean _input(AT::File &source, AT::Field &results, AT::Field &cache, AT::Field &options) {
      //
      // Load the library
      //
      static FT_Library library = NULL;
      if(!library && FT_Init_FreeType(&library)) {
        std::cout<<"*** failed to initialize the freetype library"<<std::endl;
        return(false);
      }
      std::cout<<"*** initialized the freetype library"<<std::endl;

      //
      // Load the face
      //
      AT::Unsigned32 n = options["face"].defined() ? (AT::Unsigned32)options["face"] : 0 ;
      std::cout<<"*** loading face, from source cache "<<n<<std::endl;

      FT_Face face;

      AT::Byte *contents = source.cache();
      if(FT_New_Memory_Face(library, contents, source.size(), n, &face)) {
        std::cout<<"*** failed to load the font from memory, "<<source.size()<<" bytes"<<std::endl;
        return(false);
      }

      std::cout<<"*** loaded the font"<<std::endl;

      //
      // Populate some of the header values
      //
      AT::Field header;
      header["glyphs"] = face->num_glyphs;

      std::cout<<"*** font has "<<face->num_glyphs<<" glyphs"<<std::endl;

      header["horizontal"]  = FT_HAS_HORIZONTAL(face);
      header["vertical"]    = FT_HAS_VERTICAL(face);
      header["kerning"]     = FT_HAS_KERNING(face);
      header["scalable"]    = FT_IS_SCALABLE(face);
      header["fixed_width"] = FT_IS_FIXED_WIDTH(face);

      std::cout<<"*** "<<face->num_fixed_sizes<<" fixed sizes"<<std::endl;
      for(AT::Unsigned32 i=0; i<face->num_fixed_sizes; i++) {
        header["sizes"][i] = face->available_sizes[i].size;
      }

      //
      // Set the font size
      //
      AT::Unsigned32 height = options["height"].defined() ? (AT::Unsigned32)options["height"] : 16 ;
      AT::Unsigned32 width = options["width"].defined() ? (AT::Unsigned32)options["width"] : 16 ;
      std::cout<<"*** height "<<height<<std::endl;
      std::cout<<"*** width "<<width<<std::endl;
      if(FT_Set_Pixel_Sizes(face, width, height)) {
        std::cout<<"*** failed to set the font pixel size"<<std::endl;
        return(false);
      }
      std::cout<<"*** set the font size"<<std::endl;

      FT_GlyphSlot slot = face->glyph;

      AT::Unsigned32 min = options["min"].defined() ? (AT::Unsigned32)options["min"] : 0 ;
      AT::Unsigned32 max = options["max"].defined() ? (AT::Unsigned32)options["max"] : 255 ;

      for(AT::Unsigned i=min; i<=max; i++) {
        AT::Field bitmap;

        //if(FT_Load_Char( face, text[i], FT_LOAD_RENDER|FT_LOAD_MONOCHROME)) {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
          std::cout<<i<<" font character not loaded"<<std::endl;
          continue;
        }

        AT::Unsigned32 rows = slot->bitmap.rows;
        AT::Unsigned32 width = slot->bitmap.width;
        AT::Unsigned32 pitch = slot->bitmap.pitch;

        if(pitch != width) {
          std::cout<<"font bitmap padding is not currently supported"<<std::endl;
        }

        AT::Field converted;
        switch(slot->bitmap.pixel_mode) {
          case(FT_PIXEL_MODE_MONO): {
            converted["format"] = "MONO";
            converted["bpp"] = 1;
            converted["pixel_count"] = rows * (pitch/8); //wrong, but safer
            converted["height"] = rows;
            converted["width"] = pitch;
            converted["frames"] = 1;
            break;
          }
          case(FT_PIXEL_MODE_GRAY): {
            converted["format"] = "GRAY";
            converted["bpp"] = 8;
            converted["pixel_count"] = rows * width;
            converted["height"] = rows;
            converted["width"] = width;
            converted["frames"] = 1;
            break;
          }
          default:
            std::cout<<"*** unknown pixel mode"<<std::endl;
            continue;
        };
        AT::Unsigned32 allocation = rows * pitch * sizeof(AT::Byte);
        AT::Byte *buffer = (AT::Byte *)malloc(allocation);

        memcpy(buffer, slot->bitmap.buffer, allocation);

        bitmap["header"] = converted;
        bitmap["buffer"][0] = (void *)buffer;

        if(options["persist"]) {
          results["glyph"][i] = bitmap;
          results["glyph"][i]["source"] = converted;
        }
        else {
          //
          // Find the next available power of two
          //
          AT::Unsigned32 target_width = 1;
          AT::Unsigned32 target_height = 1;

          while(width > target_width) { target_width = target_width << 1; }
          while(rows > target_height) { target_height = target_height << 1; }

          AT::Field padding;
          padding["pad"]["width"] = target_width;
          padding["pad"]["height"] = target_height;
          AT::Field rgb = AT::Raster::format(bitmap, "RGBA", padding);
          if(rgb.defined()) {
            AT::Raster::mirror(rgb);
            results["glyph"][i] = rgb;
            results["glyph"][i]["source"] = converted;
          }
          free(buffer);
        }
      }

      results["header"] = header;
      return(true);
    }
};

extern "C" __ASUKA_MODULE_EXPORT AT::Resource::Type *handle(void) {
  static ttf *reference = new ttf;
  return(reference);
}


// Local Variables:
// mode:C++
// End:
