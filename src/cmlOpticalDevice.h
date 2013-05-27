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

#include "ofVectorMath.h"
#include "ofLog.h"

namespace cml
{
  class OpticalDevice
  {
    public:

      class Config
      {
        public:

          Config() 
          {
            near = 0.1;
            far = 20.;
          };

          void size( int w, int h )
          {
            width = w;
            height = h;
          };

          void intrinsics( 
              float _cx, float _cy, 
              float _fx, float _fy )
          {
            cx = _cx;
            cy = _cy;
            fx = _fx;
            fy = _fy;
          };

          void extrinsics( 
              ofVec3f _X,
              ofVec3f _Y,
              ofVec3f _Z, 
              ofVec3f _T )
          {
            X.set( _X );
            Y.set( _Y );
            Z.set( _Z );
            T.set( _T );
          };

          float near, far;
          int width, height;

          float cx, cy, fx, fy;
          ofVec3f X, Y, Z, T;
      };

      struct Frustum
      {
        float left, right;
        float bottom, top;
        float near, far;
      };

      OpticalDevice( 
          OpticalDevice::Config config ); 

      virtual ~OpticalDevice();

      void unproject( 
          int x2d, int y2d, 
          float z, float *x, float *y );

      ofVec2f project( 
          const ofVec3f& p3 );

      ofVec3f loc() { return _loc; };
      ofVec3f fwd() { return _fwd; };
      ofVec3f up() { return _up; };
      ofVec3f trg() { return _trg; };

      Frustum gl_frustum()
      {
        return _frustum;
      };

      float* gl_projection_matrix() 
      { 
        return _KK; 
      };

      float* gl_modelview_matrix() 
      { 
        return _RT; 
      };

      OpticalDevice::Config config() 
      { 
        return cfg; 
      };

      int width() { return cfg.width; };
      int height() { return cfg.height; };
      int near() { return cfg.near; };
      int far() { return cfg.far; };

      int to_idx( int x, int y );
      void to_xy( int idx, int& x, int& y );


    protected:

      OpticalDevice::Config cfg;


    private:

      struct cv2gl
      {
        /*
         * Intrinsics from opencv to opengl
         *
         * http://www.songho.ca/opengl/gl_projectionmatrix.html
         * http://www.songho.ca/opengl/gl_transform.html
         */

        /*
         * frustum solution from 
         * https://github.com/kylemcdonald/ofxCv/blob/master/libs/ofxCv/src/Calibration.cpp#l57 
         * https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
         */

          void frustum( 
              OpticalDevice::Config& _cfg, 
              OpticalDevice::Frustum& frs )
          {

            float w = _cfg.width;
            float h = _cfg.height;
            float cx = _cfg.cx;
            float cy = _cfg.cy;
            float fx = _cfg.fx;
            float fy = _cfg.fy;
            float near = _cfg.near;
            float far = _cfg.far;

            frs.left = near * (-cx) / fx;
            frs.right = near * (w - cx) / fx;

            frs.bottom = near * (cy - h) / fy; 
            frs.top = near * (cy) / fy;

            frs.near = near;
            frs.far = far;
          };  

          /*
           * projection matrix solution from
           * http://opencv.willowgarage.com/wiki/Posit
           */

          void KK( 
              OpticalDevice::Config& _cfg, 
              float* KK )
          {

            float w = _cfg.width;
            float h = _cfg.height;
            float cx = _cfg.cx;
            float cy = _cfg.cy;
            float fx = _cfg.fx;
            float fy = _cfg.fy;
            float near = _cfg.near;
            float far = _cfg.far;

            float A = 2. * fx / w;
            float B = 2. * fy / h;
            float C = 2. * (cx / w) - 1.;
            float D = 2. * (cy / h) - 1.;
            float E = - (far + near) / (far - near);
            float F = -2. * far * near / (far - near);

            // opengl: col-major
            KK[0]= A; KK[4]= 0.; KK[8]= C; KK[12]= 0.;
            KK[1]= 0.; KK[5]= B; KK[9]= D; KK[13]= 0.;
            KK[2]= 0.; KK[6]= 0.; KK[10]= E; KK[14]= F;
            KK[3]= 0.; KK[7]= 0.;	KK[11]= -1.; KK[15]= 0.;	 
          };

          void RT( 
              OpticalDevice::Config& _cfg, 
              float* RT )
          {
            ofVec3f x = _cfg.X; 
            ofVec3f y = _cfg.Y;
            ofVec3f z = _cfg.Z;
            ofVec3f t = _cfg.T; 

            // opengl: col-major	
            RT[0]= x.x; RT[4]= y.x; RT[8]= z.x;	RT[12]= t.x;
            RT[1]= x.y;	RT[5]= y.y;	RT[9]= z.y;	RT[13]= t.y;
            RT[2]= x.z;	RT[6]= y.z;	RT[10]=z.z; RT[14]= t.z;
            RT[3]= 0.;	RT[7]= 0.;	RT[11]= 0.;	RT[15]= 1.; 
          };

      };

      Frustum _frustum; //glFrustum( ... )
      float _KK[16]; //glMultMatrixf( KK )
      float _RT[16]; //glMultMatrixf( RT )

      ofVec3f _loc, _fwd, _up, _trg;

      void printM( 
          float* M, int rows, int cols, 
          bool colmajor = true ); 

  }; 
}; 

