/*
 * Camara Lucida
 * www.camara-lucida.com.ar
 *
 * Copyright (C) 2011  Christian Parsons
 * www.chparsons.com.ar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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


