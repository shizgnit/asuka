/*
================================================================================
  $RCSfile: timer.cpp,v $ $Revision: 1.1.1.1 $ $State: Exp $
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


#include "raster.hpp"

__ASUKA_BEGIN_NAMESPACE

__ASUKA_EXPORT AT::Field Raster::copy(AT::Field &image) {
  AT::Field results;

  AT::Unsigned32 pixels = image["header"]["pixel_count"];
  AT::Unsigned32 bpp = image["header"]["bpp"];
  AT::Unsigned32 bytes = pixels *(bpp/8);

  std::cout<<"pixels: "<<pixels<<std::endl;
  std::cout<<"bits per pixel: "<<bpp<<std::endl;

  results = image;
  for(AT::Unsigned32 i=0; i < image["header"]["frames"]; i++) {
    AT::Byte *image_buffer = (AT::Byte *)(void *)image["buffer"][i];
    AT::Byte *target_buffer = (AT::Byte *)malloc(bytes * sizeof(AT::Byte));

    memcpy(image_buffer, target_buffer, bytes * sizeof(AT::Byte));

    results["buffer"][i] = (void *)target_buffer;
  }

  return(results);
}

__ASUKA_EXPORT AT::Field Raster::format(AT::Field &image, AT::Field format, AT::Field options) {
  AT::Field results;

  std::ofstream output("C:\\Logs\\format.txt", std::ios_base::app);

  output<<"============================================="<<std::endl;

  //
  // Perform some simple validations
  //
  if(image["header"]["format"] == format) {
    output<<"format didn't change"<<std::endl;
    return(results);
  }
  if(!image["header"]["format"].defined()) {
    output<<"image doesn't specify a format"<<std::endl;
    return(results);
  }
  if(!AT::rx("[RGBA][RGBA][RGBA]|GRAY|MONO", image["header"]["format"])) {
    output<<"supported formats: RGB, BGR, GRAY and MONO"<<std::endl;
    return(results);
  }

  //
  // This copy is important... specifically for the _internal key which
  // contains a pointer to the deallocation resource.
  //
  output<<"*** making a copy"<<std::endl;
  results = image;
  output<<"*** done"<<std::endl;

  //
  // Setup some useful variables...
  //
  AT::Character *relocation = format;
  AT::Character *channel = image["header"]["format"];
  AT::Unsigned32 channels = strlen(channel);

  AT::Unsigned32 source_width = image["header"]["width"];
  AT::Unsigned32 source_height = image["header"]["height"];

  AT::Unsigned32 target_width = source_width;
  AT::Unsigned32 target_height = source_height;
  if(options["pad"].defined()) {
    target_width = options["pad"]["width"];
    target_height = options["pad"]["height"];
  }

  AT::Unsigned32 pixels = target_width * target_height;
  AT::Unsigned32 bpp = image["header"]["bpp"];

  AT::Unsigned32 allocation = pixels * strlen(relocation) * sizeof(AT::Byte);

  AT::Boolean alpha = false;
  for(AT::Unsigned32 m=0; m < strlen(channel); m++) {
    if(channel[m] == 'A') {
      alpha = true;
    }
  }

  output<<"> "<<allocation<<" bytes for each target buffer"<<std::endl;
  output<<"> translating '"<<channel<<"' to '"<<format<<"'"<<std::endl;
  output<<"> pixels: "<<pixels<<std::endl;
  output<<"> bits per pixel: "<<bpp<<std::endl;
  output<<"> width: "<<source_width<<std::endl;
  output<<"> height: "<<source_height<<std::endl;

  //
  // Validate the alpha channel is valid
  //
  AT::Boolean use_alpha = true;
  if(options["alpha"].defined()) {
    if(options["alpha"]["header"]["bpp"] != 8) {
      output<<"8 bpp required for alpha channel"<<std::endl;
      use_alpha = false;
    }
    if(options["alpha"]["header"]["format"] != "GRAY") {
      output<<"grayscale required for alpha channel"<<std::endl;
      use_alpha = false;
    }
  }
  else {
    use_alpha = false;
  }

  //
  // Reformat all the frames
  //
  for(AT::Unsigned32 i=0; i < image["header"]["frames"]; i++) {
    output<<"working on frame '"<<i<<"'"<<std::endl;

    //
    // Pull the most relevant alpha channel
    //
    AT::Byte *alpha_channel = NULL;
    if(use_alpha) {
      output<<"*** using the alpha channel"<<std::endl;
      AT::Unsigned32 frames = options["alpha"]["header"]["frames"];
      AT::Unsigned32 n = i < frames ? i : frames - 1;
      output<<"alpha channel provided, using frame '"<<n<<"'"<<std::endl;
      alpha_channel = (AT::Byte *)(void *)options["alpha"]["buffer"][n];
    }

    AT::Unsigned32 source = 0;
    AT::Unsigned32 target = 0;

    AT::Byte *source_pixel = (AT::Byte *)malloc(256);
    memset(source_pixel, 0, 256);

    AT::Byte *source_buffer = (AT::Byte *)(void *)image["buffer"][i];
    AT::Byte *target_buffer = (AT::Byte *)malloc(allocation);

    for(AT::Unsigned32 y=0; y < target_height; y++) {
      for(AT::Unsigned32 x=0; x < target_width; x++) {
        //
        // Pad if we are outside the source data
        //
        if(x >= source_width || y >= source_height) {
          for(AT::Unsigned32 n=0; n < strlen(relocation); n++) {
            target_buffer[target++] = 0;
          }
          continue;
        }

        AT::Unsigned32 p = (y * source_width) + x;
        
        //
        // Extract the source data
        //
        switch(bpp) {
          case(1): {
            source = p/8;
            AT::Unsigned16 byte = source_buffer[source];
            AT::Unsigned16 offset = p % 8;
            AT::Unsigned16 pixel = byte & (1 << offset) ? 255 : 0;
            source_pixel['R'] = pixel;
            source_pixel['G'] = pixel;
            source_pixel['B'] = pixel;
            break;
          }
          case(8): {
            source = p;
            AT::Unsigned16 alpha = source_buffer[source];
            AT::Unsigned16 intensity = alpha > 0 ? 255 : 0;
            source_pixel['R'] = intensity;
            source_pixel['G'] = intensity;
            source_pixel['B'] = intensity;
            source_pixel['A'] = alpha;
            break;
          }
          case(16): {
            AT::Unsigned16 hi = source_buffer[source++];
            AT::Unsigned16 lo = source_buffer[source++];
            AT::Unsigned16 pixel = (hi << 8) | lo;
            source_pixel[channel[0]] = (pixel & 0xF800) >> 11;
            source_pixel[channel[1]] = (pixel & 0x07E0) >> 5;
            source_pixel[channel[2]] = (pixel & 0x001F);
            break;
          }
          case(24): {
            for(AT::Unsigned32 n=0; n < strlen(channel); n++) {
              source_pixel[channel[n]] = source_buffer[source++];
            }
            break;
          }
          case(32): {
            for(AT::Unsigned32 n=0; n < strlen(channel); n++) {
              source_pixel[channel[n]] = source_buffer[source++];
            }
            break;
          }
        };

        //
        // Pull the alpha channel data if not already provided
        //
        if(!alpha) {
          if(use_alpha && alpha_channel != NULL) {
            source_pixel['A'] = alpha_channel[p];
          }
          else {
            source_pixel['A'] = 255;
          }
        }

        //
        // Populate the target buffer
        //
        for(AT::Unsigned32 n=0; n < strlen(relocation); n++) {
          target_buffer[target++] = (AT::Byte)source_pixel[relocation[n]];
        }

        // Done pixel manipulation
      }
    }

    results["buffer"][i] = (void *)target_buffer;
  }
  
  results["header"] = image["header"];

  //
  // Set the modified values
  //
  results["header"]["width"] = target_width;
  results["header"]["height"] = target_height;
  results["header"]["bpp"] = strlen(relocation) * sizeof(AT::Byte) * 8;
  results["header"]["format"] = format;

  output.close();

  return(results);
}

__ASUKA_EXPORT AT::Boolean Raster::mirror(AT::Field &image) {
  AT::Unsigned32 bpp = image["header"]["bpp"];
  AT::Unsigned32 width = image["header"]["width"];
  AT::Unsigned32 height = image["header"]["height"];

  AT::Unsigned32 allocation = width * (bpp/8) * sizeof(AT::Byte);

  AT::Byte *temporary = (AT::Byte *)malloc(allocation);

  //
  // Reformat all the frames
  //
  for(AT::Unsigned32 i=0; i < image["header"]["frames"]; i++) {
    AT::Byte *buffer = (AT::Byte *)(void *)image["buffer"][i];
    for(AT::Unsigned32 y=0; y < height; y++) {
      AT::Byte *line = buffer + (allocation * y);
      memcpy(temporary, line, allocation);
      for(AT::Unsigned32 x=0; x < width; x++) {
        AT::Unsigned32 current = x  * (bpp/8) * sizeof(AT::Byte);
        AT::Unsigned32 offset = allocation - (current + (bpp/8));
        memcpy(temporary + current, line + offset, bpp/8);
      }
      memcpy(line, temporary, allocation);
    }
  }

  return(true);
}

__ASUKA_END_NAMESPACE

// Local Variables:
// mode:C++
// End:
