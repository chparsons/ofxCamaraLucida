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

#include "cmlDepthmap.h"

namespace cml 
{

  Depthmap::Depthmap() 
  { 
    flut = NULL;
    hlut = NULL;
    hpix = NULL;
  };

  Depthmap::~Depthmap(){};

  void Depthmap::init( 
      cml::OpticalDevice* depth, Mesh* mesh )
  {
    this->depth = depth;
    this->mesh = mesh;
  };

  void Depthmap::dispose()
  {
    ofLog(OF_LOG_VERBOSE,
        "cml::Depthmap::dispose");

    depth = NULL;
    mesh = NULL;

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
  };

  /*
   * float texture
   */

  ofTexture& Depthmap::get_float_tex_ref( 
      uint16_t *mm_depth_pix, 
      float mm_near, float mm_far )
  {
    int w = depth->width();
    int h = depth->height();

    init_float_tex( 
        w, h, mm_near, mm_far );

    int len = w * h;

    for (int i = 0; i < len; i++)
    {
      //uint16_t raw_depth = raw_depth_pix[i];
      //uint16_t mm = depth->z_mts(raw_depth)*1000;
      uint16_t mm = mm_depth_pix[ i ];
      fpix[ i ] = flut[ mm ]; 
    }

    ftex.loadData( fpix );
    return ftex; 
  };

  void Depthmap::init_float_tex( 
      int w, int h, 
      float mm_near, float mm_far )
  {
    if ( ftex.isAllocated() )
      return;

    ftex.allocate( w, h, GL_LUMINANCE32F_ARB );
    fpix.allocate( w, h, 1);
    fpix.set( 0 );

    flut = new float[ 10000 ];
    flut[0] = 0;
    for ( int i = 1; i < 10000; i++ )
    {
      flut[ i ] = ofMap( i, 
          mm_near, mm_far, 1., 0., true );
    }
  };

  /*
   * hue texture
   */ 

	ofTexture& Depthmap::get_hue_tex_ref(
      uint16_t *mm_depth_pix ) 
	{
    int w = depth->width();
    int h = depth->height();

	  init_hue_tex(	w, h );

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

  void Depthmap::init_hue_tex( int w, int h )
	{
    if ( htex.isAllocated() )
      return;

    htex.allocate( w, h, GL_RGB );
    //hpix.allocate( w, h, 1);
    //hpix.set( 0 );

    hpix = new uint8_t[ w * h * 3 ];

    // init hue lut

    float depth_near = 800; // 0.8; 
    float depth_far = 5000; // 5.0;
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

};


