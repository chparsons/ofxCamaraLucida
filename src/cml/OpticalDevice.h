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
        update_projection_matrix_from_frustum();
      };

      float* gl_projection_matrix() 
      { 
        return KK; 
      };

      float* gl_modelview_matrix() 
      { 
        return RT; 
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
        RT[12] = loc_.x; 
        RT[13] = loc_.y; 
        RT[14] = loc_.z;
        _loc.set( loc_ );
      };

      void fwd( ofVec3f& fwd_ ) 
      {
        RT[8] = fwd_.x; 
        RT[9] = fwd_.y; 
        RT[10] = fwd_.z;
        _fwd.set( fwd_ );
      };

      void up( ofVec3f& up_ ) 
      { 
        RT[4] = up_.x; 
        RT[5] = up_.y; 
        RT[6] = up_.z;
        _up.set( up_ );
      };

      void left( ofVec3f& left_ ) 
      { 
        RT[0] = left_.x; 
        RT[1] = left_.y; 
        RT[2] = left_.z;
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
      float KK[16]; //glMultMatrixf( KK )
      float RT[16]; //glMultMatrixf( RT )

      // RT vecs
      ofVec3f _loc,_fwd,_up,_trg,_left;

      void update_RT_vecs();
      void update_modelview_matrix(); 

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
        void update_frustum();  

        /*
         * http://opencv.willowgarage.com/wiki/Posit
         */
        void update_projection_matrix();

        /*
         * http://docs.unity3d.com/ScriptReference/Camera-projectionMatrix.html
         */
        void update_projection_matrix_from_frustum();
  }; 
}; 

