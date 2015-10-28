#pragma once

#include "ofMain.h"

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
            //mm units: near far
            near = 200.0;
            far = 6000.0;
            far_clamp = 5000.0;

            //init at world origin
            X = ofVec3f( 1.,0.,0. );
            Y = ofVec3f( 0.,1.,0. );
            Z = ofVec3f( 0.,0.,1. );
            T = ofVec3f( 0.,0.,0. );
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

          float near, far, far_clamp;
          int width, height;

          float cx, cy, fx, fy;
          ofVec3f X, Y, Z, T;

          string _name;
          void name(string n) 
          { _name = n; };
          string name() 
          { return _name; };
      };

      struct Frustum
      {
        float left, right;
        float bottom, top;
        float near, far;
      };

      OpticalDevice( OpticalDevice::Config config ); 

      virtual ~OpticalDevice();

      virtual void unproject( 
          int x2d, int y2d, 
          float z, float *x, float *y );

      virtual void project(
          const ofVec3f& p3, ofVec2f& p2 );

      int to_idx(int x, int y);
      void to_xy(int idx, int& x, int& y); 

      Frustum& gl_frustum()
      {
        return _frustum;
      };

      Frustum& set_frustum( float left, float right, float bottom, float top )
      {
        _frustum.left = left;
        _frustum.right = right;
        _frustum.bottom = bottom;
        _frustum.top = top;
      };

      float* gl_projection_matrix() 
      { 
        return _KK; 
      };

      float* gl_modelview_matrix() 
      { 
        return _RT; 
      };

      //getters RT vecs
      ofVec3f& loc() { return _loc; };
      ofVec3f& fwd() { return _fwd; };
      ofVec3f& up() { return _up; };
      ofVec3f& trg() { return _trg; };
      ofVec3f& left() { return _left; };

      //setters RT vecs
      void loc( ofVec3f& loc_ ) 
      {
        _RT[12] = loc_.x; 
        _RT[13] = loc_.y; 
        _RT[14] = loc_.z;
        _loc.set( loc_ );
      };

      void fwd( ofVec3f& fwd_ ) 
      {
        _RT[8] = fwd_.x; 
        _RT[9] = fwd_.y; 
        _RT[10] = fwd_.z;
        _fwd.set( fwd_ );
      };

      void up( ofVec3f& up_ ) 
      { 
        _RT[4] = up_.x; 
        _RT[5] = up_.y; 
        _RT[6] = up_.z;
        _up.set( up_ );
      };

      void left( ofVec3f& left_ ) 
      { 
        _RT[0] = left_.x; 
        _RT[1] = left_.y; 
        _RT[2] = left_.z;
        _left.set( left_ );
      };

      void trg( ofVec3f& trg_ ) 
      {
        _trg.set( trg_ );
      };

      int width, height, near, far, far_clamp;
      int cx, cy, fx, fy;

      OpticalDevice::Config config; 

      void log( ofLogLevel level )
      {
        Frustum& F = _frustum;
        ofLog(level) 
          << "cml::OpticalDevice:" << "\n" 
          << "name: " << config.name() << "\n" 
          << "\n"
          << "frustum:" << "\n" 
          << "L: " << F.left << "\n"
          << "R: " << F.right << "\n" 
          << "T: " << F.top << "\n"
          << "B: " << F.bottom << "\n"
          << "N: " << F.near << "\n"
          << "F: " << F.far << "\n"
          << "\n"
          << "extrinsics:" << "\n" 
          << "T: " << config.T << "\n"
          << "R x axis: " << ofToString(config.X) 
          << "\n"
          << "R y axis: " << ofToString(config.Y) 
          << "\n"
          << "R z axis: " << ofToString(config.Z) 
          << "\n"
          << "\n";
      };

    private:

      Frustum _frustum; //glFrustum( ... )
      float _KK[16]; //glMultMatrixf( KK )
      float _RT[16]; //glMultMatrixf( RT )

      // RT vecs
      ofVec3f _loc,_fwd,_up,_trg,_left;

      void make_RT_vecs( float* RT )
      {
        // opengl: col-major	
        _loc = ofVec3f( RT[12], RT[13], RT[14] );
        _fwd = ofVec3f( RT[8], RT[9], RT[10] );
        _up = ofVec3f( RT[4], RT[5], RT[6] );
        _left = ofVec3f( RT[0], RT[1], RT[2] );
        _trg = _loc + _fwd;
      };

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

        void make_frustum( 
          OpticalDevice::Config& _cfg, 
          OpticalDevice::Frustum& F )
        {

          float w = _cfg.width;
          float h = _cfg.height;
          float cx = _cfg.cx;
          float cy = _cfg.cy;
          float fx = _cfg.fx;
          float fy = _cfg.fy;
          float far = _cfg.far;
          float near = _cfg.near;

          F.left = near * (-cx) / fx;
          F.right = near * (w - cx) / fx;

          F.bottom = near * (cy - h) / fy; 
          F.top = near * (cy) / fy;

          F.near = near;
          F.far = far; 
        };  

        /*
         * projection matrix solution from
         * http://opencv.willowgarage.com/wiki/Posit
         */

        void make_projection_matrix( 
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

        void make_modelview_matrix( 
          OpticalDevice::Config& _cfg, 
          float* RT )
        {
          ofVec3f& x = _cfg.X; 
          ofVec3f& y = _cfg.Y;
          ofVec3f& z = _cfg.Z;
          ofVec3f& t = _cfg.T; 

          // opengl: col-major	
          RT[0]= x.x; RT[4]= y.x; RT[8]= z.x;	RT[12]= t.x;
          RT[1]= x.y;	RT[5]= y.y;	RT[9]= z.y;	RT[13]= t.y;
          RT[2]= x.z;	RT[6]= y.z;	RT[10]=z.z; RT[14]= t.z;
          RT[3]= 0.;	RT[7]= 0.;	RT[11]= 0.;	RT[15]= 1.; 
        }; 
  }; 
}; 

