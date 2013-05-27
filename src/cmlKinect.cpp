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

#include "cmlKinect.h"

namespace cml
{

  //TODO doesnt need to be an OpticalDevice

  Kinect::Kinect(
      const OpticalDevice::Config& config ) 
    : OpticalDevice( config )
  {
    uint16_t size = 2048; //11bits
    _zlut = new float[size];
    for (int i = 0; i < size; i++) 
    {
      _zlut[i] = z_raw_to_mts( i );
    }
  }; 

  Kinect::~Kinect()
  {
    delete _zlut; 
    _zlut = NULL;
  };

  float Kinect::z_mts( uint16_t raw_depth )
  {
    return _zlut[raw_depth];
  };

  float Kinect::z_mts( 
      uint16_t *raw_depth_pix, int _x, int _y )
  {
    int idx = to_idx(_x, _y);
    uint16_t raw_depth = raw_depth_pix[idx];
    return _zlut[raw_depth];
  };

  void Kinect::raw_depth_to_p3(
      uint16_t *raw_depth_pix, 
      int _x, int _y, ofVec3f *p3 )
  {
    float zmts, x, y;
    zmts = z_mts(raw_depth_pix, _x, _y);
    unproject( _x, _y, zmts, &x, &y );
    p3->x = x;
    p3->y = y;
    p3->z = zmts;
  };

  ofVec3f Kinect::raw_depth_to_p3(
      uint16_t *raw_depth_pix, 
      int _x, int _y)
  {
    ofVec3f p3;
    raw_depth_to_p3( 
        raw_depth_pix, _x, _y, &p3 );
    return p3;
  };

  void Kinect::raw_depth_to_p3(
      uint16_t raw_depth, 
      int _x, int _y, ofVec3f *p3 )
  {
    float zmts, x, y;
    zmts = z_mts( raw_depth );
    unproject( _x, _y, zmts, &x, &y );
    p3->x = x;
    p3->y = y;
    p3->z = zmts;
  }; 

  ofVec3f Kinect::raw_depth_to_p3(
      uint16_t raw_depth, 
      int _x, int _y)
  {
    ofVec3f p3;
    raw_depth_to_p3( raw_depth, _x, _y, &p3 );
    return p3;
  };

  //ofVec2f Kinect::raw_depth_to_rgb(
  //uint16_t raw_depth, 
  //int _x, int _y,
  //OpticalDevice* rgb)
  //{
  ////	TODO
  ////	P3D' = R.P3D + T
  ////	P2D_rgb.x = (P3D'.x * fx_rgb / P3D'.z) + cx_rgb
  ////	P2D_rgb.y = (P3D'.y * fy_rgb / P3D'.z) + cy_rgb

  //ofVec2f rgb2d;

  //float xoff = -8;
  //ofVec3f p3 = raw_depth_to_p3(raw_depth, _x, _y);

  //ofVec3f p3rgb = rgb->RT * p3d;

  //rgb2d.x = (p3rgb.x * rgb->fx / p3rgb.z) + rgb->cx;
  //rgb2d.y = (p3rgb.y * rgb->fy / p3rgb.z) + rgb->cy;

  //CLAMP(rgb2d.x, 0, rgb->width-1);
  //CLAMP(rgb2d.y, 0, rgb->height-1);

  //return rgb2d;
  //};

  //http://openkinect.org/wiki/Imaging_Information
  //http://nicolas.burrus.name/index.php/Research/KinectCalibration
  float Kinect::z_raw_to_mts( uint16_t raw_depth )
  {
    raw_depth = CLAMP(raw_depth, 0, 1024); //5mts~
    return 0.1236 * tanf( ( (float)raw_depth / 2842.5 ) + 1.1863 ) - 0.0370;
    //return 1.0 / ( (float)raw_depth * -0.0030711016 + 3.3309495161);
  }; 
};

