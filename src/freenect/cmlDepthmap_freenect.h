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

#include "cmlDepthmap.h"

namespace cml 
{
  class Depthmap_freenect : public Depthmap
  {
    public:

      Depthmap_freenect(){};
      ~Depthmap_freenect(){};

      void update( uint16_t *mm_depth_pix ) 
      {
        int len = mesh->length();

        for ( int i = 0; i < len; i++ )
        {
          int xdepth, ydepth, idepth;

          mesh->to_depth( i, 
              &xdepth, &ydepth, &idepth );

          //uint16_t raw_depth = raw_depth_pix[idepth];
          //float z = depth->z_mts(raw_depth);

          // ofxKinect gives raw depth as dist in mm
          // mm to mts
          float zmts = mm_depth_pix[idepth] * 0.001;
          zmts = CLAMP(
              (zmts == 0. ? 5. : zmts), 0., 5.);

          float x, y;

          depth->unproject(
              xdepth, ydepth, zmts, &x, &y );

          mesh->set_vertex( i, x, y, zmts );
        }

        mesh->update();
      };
  };
};


