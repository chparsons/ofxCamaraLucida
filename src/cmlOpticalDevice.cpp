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

#include "cmlOpticalDevice.h"

namespace cml
{
  OpticalDevice::OpticalDevice( 
      OpticalDevice::Config config )
  {
    this->cfg = config;

    cv2gl c;
    c.frustum( cfg, _frustum );
    c.KK( cfg, _KK );
    c.RT( cfg, _RT );

    // opengl: col-major	
    _loc = ofVec3f( _RT[12], _RT[13], _RT[14] );
    _fwd = ofVec3f( _RT[8], _RT[9], _RT[10] );
    _up = ofVec3f( _RT[4], _RT[5], _RT[6] );

    _trg = _loc + _fwd;

    //_T = ofVec3f(RT[12],rgb_RT[13],rgb_RT[14]);
    //_RT = ofMatrix4x4(
    //RT[0],  RT[1],  RT[2],  RT[12],
    //RT[4],  RT[5],  RT[6],  RT[13],
    //RT[8],  RT[9],  RT[10], RT[14],
    //0.,     0.,	    0.,	    1.
    //);                
    //_R.preMultTranslate(-_T);
    //_R = ofMatrix4x4::getTransposedOf(_R);
  }; 

  OpticalDevice::~OpticalDevice(){};

  void OpticalDevice::unproject( 
      int x2d, int y2d, 
      float z, float *x, float *y)
  {
    *x = (x2d - cfg.cx) * z / cfg.fx;
    *y = (y2d - cfg.cy) * z / cfg.fy;
  };

  ofVec2f OpticalDevice::project( 
      const ofVec3f& p3 )
  {
    ofVec2f p2;
    p2.x = (p3.x * cfg.fx / p3.z) + cfg.cx;
    p2.y = (p3.y * cfg.fy / p3.z) + cfg.cy;
    return p2;
  };  

  int OpticalDevice::to_idx( int x, int y )
  {
    return y * cfg.width + x;
  };

  void OpticalDevice::to_xy( 
      int idx, int& x, int& y )
  {
    x = idx % cfg.width;
    y = (idx - x) / cfg.width;
  }; 

  void OpticalDevice::printM( float* M, 
      int rows, int cols, bool colmajor)
  {
    if ( ofGetLogLevel() != OF_LOG_VERBOSE )
      return;

    if (colmajor)
    {
      for (int j = 0; j < cols; j++)
      {
        printf("\n");
        for (int i = 0; i < rows; i++)
        {
          printf("%9.3f ", M[i*cols+j]);
        }	
      }
    }
    else
    {
      for (int i = 0; i < rows; i++)
      {
        printf("\n");
        for (int j = 0; j < cols; j++)
        {
          printf("%9.3f ", M[i*cols+j]);
        }	
      }
    }
    printf("\n\n");
  };

};


