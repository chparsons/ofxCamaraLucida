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

#include "cmlDepthCamera.h"

namespace cml
{

  DepthCamera::DepthCamera(
      const OpticalDevice::Config& config ) 
    : OpticalDevice( config )
  {
    _k = ofVec4f(0.1236, 2842.5, 1.1863, 0.0370);

    flut = NULL;
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
    if ( flut != NULL )
      delete[] flut;

    htex.clear();
    //hpix.clear();
    if ( hpix != NULL )
      delete[] hpix;
    if ( hlut != NULL )
      delete[] hlut;

    delete _zlut; 
    _zlut = NULL;
  };

  /*
   * float texture
   */

  ofTexture& DepthCamera::get_float_tex_ref( 
      uint16_t *mm_depth_pix )
  {

    int w = width();
    int h = height();

    init_float_tex( w, h );

    if ( mm_depth_pix == NULL )
      return ftex; 

    int len = w * h;

    for (int i = 0; i < len; i++)
    {
      //uint16_t raw_depth = raw_depth_pix[i];
      //uint16_t mm = z_mts(raw_depth)*1000;
      uint16_t mm = mm_depth_pix[ i ];
      fpix[ i ] = flut[ mm ]; 
    }

    ftex.loadData( fpix );
    return ftex; 
  };

  void DepthCamera::init_float_tex(int w, int h)
  {
    if ( ftex.isAllocated() )
      return;

    ftex.allocate( w, h, GL_LUMINANCE32F_ARB );
    fpix.allocate( w, h, 1 );
    fpix.set( 0 );

    int near_mm = (int)(near() * 1000);
    int far_mm = (int)(far() * 1000);

    flut = new float[ far_mm ];
    flut[0] = 0;
    for ( int i = 1; i < far_mm; i++ )
    {
      /*
       * WARNING
       * this interpolation is related 
       * to z_norm_to_mts in render.vert shader
       */
      flut[ i ] = ofMap( i, 
          near_mm, far_mm, 
          1., 0., true );
    }
  };

  /*
   * hue texture
   */ 

	ofTexture& DepthCamera::get_hue_tex_ref(
      uint16_t *mm_depth_pix ) 
	{
    int w = width();
    int h = height();

	  init_hue_tex(	w, h );

    if ( mm_depth_pix == NULL )
      return htex; 

    int len = w * h;

		for (int i = 0; i < len; i++)
		{
      uint16_t mm = mm_depth_pix[ i ];
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

  float DepthCamera::z_mts( uint16_t raw_depth )
  {
    return _zlut[ raw_depth ];
  };

  float DepthCamera::z_mts( 
      uint16_t *raw_depth_pix, 
      int _x, int _y )
  {
    int i = to_idx( _x, _y );
    uint16_t raw_depth = raw_depth_pix[ i ];
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

