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
#include "cmlMesh.h"

namespace cml 
{
  class Depthmap
  {
    public:

      Depthmap();
      virtual ~Depthmap();

      virtual void init( 
          cml::OpticalDevice* depth, 
          Mesh* mesh );

      virtual void dispose();

      /*
       * float texture in range [0,1]
       * mapped from [mm_near,mm_far]
       * to pass to glsl
       */
      ofTexture& get_float_tex_ref( 
          uint16_t *mm_depth_pix, 
          float mm_near, float mm_far );

      /*
       * hue texture
       */
      ofTexture& get_hue_tex_ref(
          uint16_t *mm_depth_pix );

    protected:

      cml::OpticalDevice* depth;
      Mesh* mesh;

    private:

      ofTexture ftex;
      ofFloatPixels fpix;
      float *flut;
      void init_float_tex( 
          int w, int h, 
          float mm_near, float mm_far );

      ofTexture htex;
      uint8_t *hpix;
      ofColor *hlut;
      void init_hue_tex( int w, int h );

  };
};

