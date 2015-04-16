#include "cml/OpticalDevice.h"

namespace cml
{
  OpticalDevice::OpticalDevice( OpticalDevice::Config config )
  {
    this->config = config;

    width = config.width;
    height = config.height;
    near = config.near;
    far = config.far;
    cx = config.cx;
    cy = config.cy;
    fx = config.fx;
    fy = config.fy;

    make_frustum( config, _frustum );
    make_projection_matrix( config, _KK );
    make_modelview_matrix( config, _RT );

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
      int x2d, int y2d, float z, 
      float *x, float *y )
  {
    *x = (float)(x2d - cx) * z / fx;
    *y = (float)(y2d - cy) * z / fy;
  };

  void OpticalDevice::project( 
      const ofVec3f& p3, ofVec2f& p2 )
  {
    p2.x = (p3.x * fx / p3.z) + cx;
    p2.y = (p3.y * fy / p3.z) + cy;
  };  

  int OpticalDevice::to_idx(int x, int y)
  {
    return y * width + x;
  };

  void OpticalDevice::to_xy( 
      int idx, int& x, int& y )
  {
    x = idx % width;
    y = (idx - x) / width;
  }; 
};


