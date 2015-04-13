#include "cml/DepthCamera.h"

namespace cml
{
  
  DepthCamera::DepthCamera(
    const OpticalDevice::Config& config ) 
    : OpticalDevice( config )
  {
    k = ofVec4f( 0.1236, 2842.5, 1.1863, 0.0370 );
    xoff = 0.0f; //-8.0f;

    flut_mm = NULL;
    hlut = NULL;
    hpix = NULL;

    uint16_t size = 2048; //11bits
    _zlut = new float[ size ];
    for ( int i = 0; i < size; i++ ) 
      _zlut[i] = z_raw_to_mts( i );

  }; 

  DepthCamera::~DepthCamera()
  {
    ofLog(OF_LOG_VERBOSE,"~cml::DepthCamera");

    ftex.clear();
    fpix.clear();
    if ( flut_mm != NULL )
      delete[] flut_mm;

    htex.clear();
    //hpix.clear();
    if ( hpix != NULL )
      delete[] hpix;
    if ( hlut != NULL )
      delete[] hlut;

    delete _zlut; 
    _zlut = NULL;
  };

  void DepthCamera::unproject( 
      int x2d, int y2d, float z, 
      float *x, float *y)
  {
    *x = (float)(x2d + xoff - cx)*z/fx;
    *y = (float)(y2d - cy)*z/fy;
  };

  ofVec2f DepthCamera::project( 
      const ofVec3f& p3 )
  {
    ofVec2f p2;
    p2.x = (p3.x * fx / p3.z) + cx - xoff;
    p2.y = (p3.y * fy / p3.z) + cy;
    return p2;
  };

  /*
   * float texture
   */

  ofTexture& DepthCamera::get_float_tex_ref( ofFloatPixels& depth_float_pix_mm )
  {
    ftex.loadData( depth_float_pix_mm );
    return ftex; 
  };

  ofTexture& DepthCamera::get_float_tex_ref( uint16_t *depth_pix_mm )
  {

    int w = width;
    int h = height;

    int len = w * h;

    for (int i = 0; i < len; i++)
    {
      //uint16_t raw_depth = raw_depth_pix[i];
      //uint16_t mm = raw_depth_to_mts(raw_depth) * 1000.0f;
      uint16_t mm = depth_pix_mm[ i ];
      fpix[ i ] = flut_mm[ mm ]; 
    }

    ftex.loadData( fpix );
    return ftex; 
  };

  ofTexture& DepthCamera::init_float_tex()
  {
    if ( ftex.isAllocated() )
      return ftex; 

    int w = width;
    int h = height;

    ftex.allocate( w, h, GL_LUMINANCE32F_ARB );
    fpix.allocate( w, h, 1 );
    fpix.set( 0 );

    float mts_to_mm = 1000.0f;
    float near_mm = near * mts_to_mm;
    float far_mm = far * mts_to_mm;

    flut_mm = new float[ (int)far_mm ];
    flut_mm[0] = 1.0f;
    for ( int i = 1; i < far_mm; i++ )
    {
      //cml::RenderShader->z_norm_to_mts 
      flut_mm[ i ] = ofMap( i, 
          near_mm, far_mm, 
          0.0, 1.0, true );
    }

    return ftex; 
  };

  /*
   * hue texture
   */ 

  ofTexture& DepthCamera::get_hue_tex_ref(
      uint16_t *depth_pix_mm ) 
  {
    int w = width;
    int h = height;

    init_hue_tex(	w, h );

    if ( depth_pix_mm == NULL )
      return htex; 

    int len = w * h;

    for (int i = 0; i < len; i++)
    {
      uint16_t mm = depth_pix_mm[ i ];
      ofColor hue = hlut[ mm ];
      hpix[ i * 3 + 0 ] = hue.r;
      hpix[ i * 3 + 1 ] = hue.g;
      hpix[ i * 3 + 2 ] = hue.b;
    }

    htex.loadData( hpix, w, h, GL_RGB );
    return htex;
  }

  void DepthCamera::init_hue_tex( int w, int h )
  {
    if ( htex.isAllocated() )
      return;

    htex.allocate( w, h, GL_RGB );
    //hpix.allocate( w, h, 1);
    //hpix.set( 0 );

    hpix = new uint8_t[ w * h * 3 ];

    // init hue lut

    float depth_near = 800.; // 0.8; 
    float depth_far = 5000.; // 5.0;
    float hue_near = 0.95; 
    float hue_far = 0.15;
    bool clamp = false;

    hlut = new ofColor[ 10000 ];
    hlut[0] = ofColor(0, 0, 0);

    for ( int i = 1; i < 10000; i++ ) 
    {
      float hue = ofMap( i, 
          depth_near, depth_far, 
          hue_near, hue_far, 
          clamp);

      hlut[ i ] = ofColor::fromHsb(
          hue * 255., 255., 255., 255. );
    }
  }

  float DepthCamera::raw_depth_to_mts( uint16_t raw_depth )
  {
    return _zlut[ raw_depth ];
  };

  float DepthCamera::raw_depth_to_mts( 
      uint16_t *raw_depth_pix, 
      int _x, int _y )
  {
    int i = to_idx( _x, _y );
    uint16_t raw_depth = raw_depth_pix[i];
    return _zlut[ raw_depth ];
  };


  //http://openkinect.org/wiki/Imaging_Information
  //http://nicolas.burrus.name/index.php/Research/KinectCalibration

  float DepthCamera::z_raw_to_mts(uint16_t raw_depth)
  {
    raw_depth = CLAMP(raw_depth, 0, 1024);//5mts~

    return 0.1236 * tanf( ( (float)raw_depth / 2842.5 ) + 1.1863 ) - 0.0370;

    //return 1.0 / ( (float)raw_depth * -0.0030711016 + 3.3309495161);
  }; 

};

