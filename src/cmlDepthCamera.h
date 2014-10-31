#pragma once

#include "cmlOpticalDevice.h"
#include "ofTexture.h"

namespace cml
{

  class DepthCamera : public OpticalDevice
  {
    public:

      DepthCamera(
        const OpticalDevice::Config& config ); 

      ~DepthCamera();

      /*
       * float texture in range [0,1]
       * mapped from [near_mm,far_mm]
       * to use in shaders
       */
      ofTexture& get_float_tex_ref( 
          uint16_t *mm_depth_pix = NULL );

      ofTexture& get_hue_tex_ref( 
          uint16_t *mm_depth_pix = NULL );

      const ofVec4f& k() { return _k; };

      float z_mts( uint16_t raw_depth );
      float z_mts( 
          uint16_t *raw_depth_pix, 
          int _x, int _y );

    private:

      ofVec4f _k; 
      float *_zlut;
      float z_raw_to_mts( uint16_t raw_depth ); 

      ofTexture ftex;
      ofFloatPixels fpix;
      float *flut;
      void init_float_tex( int w, int h );

      ofTexture htex;
      uint8_t *hpix;
      ofColor *hlut;
      void init_hue_tex( int w, int h );

  };
};


