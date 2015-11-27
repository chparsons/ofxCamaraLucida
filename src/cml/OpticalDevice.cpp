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
    far_clamp = config.far_clamp;
    cx = config.cx;
    cy = config.cy;
    fx = config.fx;
    fy = config.fy;

    update_frustum();
    update_projection_matrix();
    update_modelview_matrix();
    update_RT_vecs(); 
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

  void OpticalDevice::update_projection_matrix() 
  {
    float w = config.width;
    float h = config.height;
    float cx = config.cx;
    float cy = config.cy;
    float fx = config.fx;
    float fy = config.fy;
    float near = config.near;
    float far = config.far;

    float A = 2. * fx / w;
    float B = 2. * fy / h;
    float C = 2. * (cx / w) - 1.;
    float D = 2. * (cy / h) - 1.;
    float E = - (far + near) / (far - near);
    float F = -2. * far * near / (far - near);

    // opengl: col-major
    KK[0]= A;  KK[4]= 0.; KK[8]= C;    KK[12]= 0.;
    KK[1]= 0.; KK[5]= B;  KK[9]= D;    KK[13]= 0.;
    KK[2]= 0.; KK[6]= 0.; KK[10]= E;   KK[14]= F;
    KK[3]= 0.; KK[7]= 0.;	KK[11]= -1.; KK[15]= 0.;
  };

  void OpticalDevice::update_projection_matrix_from_frustum()
  {
    OpticalDevice::Frustum& f = _frustum; 

    //ofMatrix4x4 fmat;
    //fmat.makeFrustumMatrix(
        //f.left, f.right,
        //f.bottom, f.top,
        //f.near, f.far);

    float A = 2. * f.near / (f.right - f.left);
    float B = 2. * f.near / (f.top - f.bottom);
    float C = (f.right + f.left) / (f.right - f.left);
    float D = (f.top + f.bottom) / (f.top - f.bottom);
    float E = -(f.far + f.near) / (f.far - f.near);
    float F = -(2. * f.far * f.near) / (f.far - f.near);

    // opengl: col-major
    KK[0]= A;  KK[4]= 0.; KK[8]= C;    KK[12]= 0.;
    KK[1]= 0.; KK[5]= B;  KK[9]= D;    KK[13]= 0.;
    KK[2]= 0.; KK[6]= 0.; KK[10]= E;   KK[14]= F;
    KK[3]= 0.; KK[7]= 0.;	KK[11]= -1.; KK[15]= 0.;
  };

  void OpticalDevice::update_modelview_matrix()
  {
    ofVec3f& x = config.X; 
    ofVec3f& y = config.Y;
    ofVec3f& z = config.Z;
    ofVec3f& t = config.T; 

    ofLog() << "update MV: T = " << t;

    // opengl: col-major	
    RT[0]= x.x; RT[4]= y.x; RT[8]= z.x;	RT[12]= t.x;
    RT[1]= x.y;	RT[5]= y.y;	RT[9]= z.y;	RT[13]= t.y;
    RT[2]= x.z;	RT[6]= y.z;	RT[10]=z.z; RT[14]= t.z;
    RT[3]= 0.;	RT[7]= 0.;	RT[11]= 0.;	RT[15]= 1.; 
  };

  void OpticalDevice::update_RT_vecs()
  {
    // opengl: col-major	
    _loc = ofVec3f( RT[12], RT[13], RT[14] );
    _fwd = ofVec3f( RT[8], RT[9], RT[10] );
    _up = ofVec3f( RT[4], RT[5], RT[6] );
    _left = ofVec3f( RT[0], RT[1], RT[2] );
    _trg = _loc + _fwd;
  };

  void OpticalDevice::update_frustum()
  {
    float w = config.width;
    float h = config.height;
    float cx = config.cx;
    float cy = config.cy;
    float fx = config.fx;
    float fy = config.fy;
    float far = config.far;
    float near = config.near;

    OpticalDevice::Frustum& F = _frustum;

    float left = near * (-cx) / fx;
    float right = near * (w - cx) / fx;

    float bottom = near * (cy - h) / fy; 
    float top = near * (cy) / fy;

    F.left = left;
    F.right = right;

    //flip Y
    F.bottom = bottom; //-top;
    F.top = top; //-bottom;

    F.near = near;
    F.far = far; 
  };

};


