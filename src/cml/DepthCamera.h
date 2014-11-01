#pragma once

#include "cml/OpticalDevice.h"
#include "ofTexture.h"

namespace cml
{

  class DepthCamera : public OpticalDevice
  {
    public:

      DepthCamera( const OpticalDevice::Config& config ); 

      ~DepthCamera();

      virtual void unproject( 
          int x2d, int y2d, 
          float z, float *x, float *y );

      virtual ofVec2f project(
          const ofVec3f& p3 );

      /*
       * float texture in range [0,1]
       * mapped from [near_mm,far_mm]
       * to use in shaders
       */
      ofTexture& get_float_tex_ref( uint16_t *depth_pix_mm = NULL );

      ofTexture& get_hue_tex_ref( uint16_t *depth_pix_mm = NULL );

      float raw_depth_to_mts( uint16_t raw_depth );
      float raw_depth_to_mts( uint16_t *raw_depth_pix, int _x, int _y );

      float xoff;
      ofVec4f k; 

    private:

      float *_zlut;
      float z_raw_to_mts( uint16_t raw_depth ); 

      ofTexture ftex;
      ofFloatPixels fpix;
      float *flut_mm;
      void init_float_tex(int w, int h);

      ofTexture htex;
      uint8_t *hpix;
      ofColor *hlut;
      void init_hue_tex(int w, int h);

  };
};


