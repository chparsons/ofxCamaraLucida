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

#include "cmlCalibration.h"

namespace cml
{
  Calibration::Calibration( 
      cml::Config *cml_config,
      OpticalDevice::Config& proj_cfg,
      OpticalDevice::Config& depth_cfg,
      OpticalDevice::Config& rgb_cfg
      )
  {
    string kcalib = 
      cml_config->kinect_calib_path;
    string pcalib = 
      cml_config->projector_calib_path;

    device_config( 
        pcalib, 
        "proj_intrinsics", 
        "proj_size", 
        "R", "T",
        proj_cfg );

    device_config( 
        kcalib, 
        "depth_intrinsics", 
        "depth_size", 
        "R", "T",
        depth_cfg );

    //override depth extrinsics
    //to be the world origin
    //RT in depth config is RT depth->rgb
    depth_cfg.extrinsics( 
        ofVec3f( 1.,0.,0. ),
        ofVec3f( 0.,1.,0. ),
        ofVec3f( 0.,0.,1. ),
        ofVec3f( 0.,0.,0. ) );

    device_config( 
        kcalib, 
        "rgb_intrinsics", 
        "rgb_size", 
        "R", "T",
        rgb_cfg );
  };

  Calibration::~Calibration(){};

  void Calibration::device_config( 
      string calib_path, 
      string int_id,
      string size_id,
      string R_id, string T_id, 
      OpticalDevice::Config& dev_config )
  {

    CvMat* _int = (CvMat*)cvLoad( 
        calib_path.c_str(), 
        NULL, int_id.c_str() );

    CvMat* _size = (CvMat*)cvLoad( 
        calib_path.c_str(), 
        NULL, size_id.c_str() );

    CvMat* _R = (CvMat*)cvLoad( 
        calib_path.c_str(), 
        NULL, R_id.c_str() );

    CvMat* _T = (CvMat*)cvLoad( 
        calib_path.c_str(), 
        NULL, T_id.c_str() );

    int w = (int)cvGetReal2D( _size, 0, 0 );
    int h = (int)cvGetReal2D( _size, 0, 1 );

    float fx = (float)cvGetReal2D( _int, 0, 0 );
    float fy = (float)cvGetReal2D( _int, 1, 1 );
    float cx = (float)cvGetReal2D( _int, 0, 2 );
    float cy = (float)cvGetReal2D( _int, 1, 2 );

    // opencv: row-major	

    // R x axis
    ofVec3f X = ofVec3f(
        (float)cvGetReal2D( _R, 0, 0 ), //xx
        (float)cvGetReal2D( _R, 1, 0 ), //xy
        (float)cvGetReal2D( _R, 2, 0 )  //xz
        );

    // R y axis	
    ofVec3f Y = ofVec3f(
        (float)cvGetReal2D( _R, 0, 1 ), //yx
        (float)cvGetReal2D( _R, 1, 1 ), //yy
        (float)cvGetReal2D( _R, 2, 1 )  //yz
        );

    // R z axis	
    ofVec3f Z = ofVec3f(
        (float)cvGetReal2D( _R, 0, 2 ), //zx
        (float)cvGetReal2D( _R, 1, 2 ), //zy
        (float)cvGetReal2D( _R, 2, 2 )  //zz
        );

    // T
    ofVec3f T = ofVec3f(
        (float)cvGetReal2D( _T, 0, 0 ), //tx
        (float)cvGetReal2D( _T, 1, 0 ), //ty
        (float)cvGetReal2D( _T, 2, 0 )  //tz
        ); 


    dev_config.size( w, h );
    dev_config.intrinsics( cx, cy, fx, fy );
    dev_config.extrinsics( X, Y, Z, T ); 


    ofLog(OF_LOG_VERBOSE, "cml::Calibration");
    ofLog(OF_LOG_VERBOSE, calib_path);
    ofLog(OF_LOG_VERBOSE, int_id);
    printM( _int );
    //ofLog( OF_LOG_VERBOSE, "R" );
    //printM( _R );
    //ofLog( OF_LOG_VERBOSE, "T" );
    //printM( _T );
    ofLog(OF_LOG_VERBOSE,"RX:"+ofToString(X));
    ofLog(OF_LOG_VERBOSE,"RY:"+ofToString(Y));
    ofLog(OF_LOG_VERBOSE,"RZ:"+ofToString(Z));
    ofLog(OF_LOG_VERBOSE,"T:"+ofToString(T));


    cvReleaseMat( &_int );
    cvReleaseMat( &_size );
    cvReleaseMat( &_R );
    cvReleaseMat( &_T );
  };

  void Calibration::printM( 
      CvMat* M, bool colmajor )
  {
    if ( ofGetLogLevel() != OF_LOG_VERBOSE )
      return;

    int i,j;
    if (colmajor)
    {
      for (i = 0; i < M->rows; i++)
      {
        printf("\n");				
        switch( CV_MAT_DEPTH(M->type) )
        {
          case CV_32F:
          case CV_64F:
            for (j = 0; j < M->cols; j++)
              printf("%9.3f ", (float)cvGetReal2D( M, i, j ));
            break;
          case CV_8U:
          case CV_16U:
            for (j = 0; j < M->cols; j++)
              printf("%6d",(int)cvGetReal2D( M, i, j ));
            break;
          default:
            break;
        }
      }
    }
    else
    {
      for (j = 0; j < M->cols; j++)
      {
        printf("\n");				
        switch( CV_MAT_DEPTH(M->type) )
        {
          case CV_32F:
          case CV_64F:
            for (i = 0; i < M->rows; i++)
              printf("%9.3f ", (float)cvGetReal2D( M, i, j ));
            break;
          case CV_8U:
          case CV_16U:
            for (i = 0; i < M->rows; i++)
              printf("%6d",(int)cvGetReal2D( M, i, j ));
            break;
          default:
            break;
        }
      }
    }
    printf("\n\n");
  };

};

