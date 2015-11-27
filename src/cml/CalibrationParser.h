#pragma once

#include "cml/Config.h"
#include "cml/OpticalDevice.h"
#include "ofxOpenCv.h"

namespace cml
{
  class CalibrationParser
  {
    public:

      CalibrationParser(){};
      ~CalibrationParser(){};

      void parse( 
        cml::Config config,
        OpticalDevice::Config& proj_cfg,
        OpticalDevice::Config& depth_cfg )
        //OpticalDevice::Config& rgb_cfg )
      {
        depth_cfg.name("depth camera");
        proj_cfg.name("projector");

        //use kinect rgb calibration with registration turned on 
        parse_intrinsics( config.kinect_intrinsics_file, true, depth_cfg );
        parse_intrinsics( config.projector_intrinsics_file, true, proj_cfg );

        parse_extrinsics( config.projector_kinect_extrinsics_file, proj_cfg );
      };

    private:

      void parse_intrinsics( string filename, bool undistorted, OpticalDevice::Config& device )
      {
        bool absolute = false;
        cv::FileStorage fs(ofToDataPath(filename, absolute), cv::FileStorage::READ);

        cv::Size imageSize;
        cv::Size sensorSize;
        cv::Mat cameraMatrix;
        cv::Mat distCoeffs;
        float reprojectionError;

        cv::Point2d fov;
        double focalLength, aspectRatio;
        cv::Point2d principalPoint;

        string prefix = undistorted ? "undistorted_" : "";

        fs[prefix+"cameraMatrix"] >> cameraMatrix;
        fs[prefix+"imageSize_width"] >> imageSize.width;
        fs[prefix+"imageSize_height"] >> imageSize.height;
        fs[prefix+"sensorSize_width"] >> sensorSize.width;
        fs[prefix+"sensorSize_height"] >> sensorSize.height;
        fs[prefix+"distCoeffs"] >> distCoeffs;
        fs[prefix+"reprojectionError"] >> reprojectionError; 

        cv::calibrationMatrixValues(
            cameraMatrix, imageSize, 
            sensorSize.width, 
            sensorSize.height,
            fov.x, fov.y, 
            focalLength, 
            principalPoint, 
            aspectRatio );

        float fx = cameraMatrix.at<double>(0, 0);
        float fy = cameraMatrix.at<double>(1, 1);
        float cx = cameraMatrix.at<double>(0, 2);
        float cy = cameraMatrix.at<double>(1, 2);
        //float cx = principalPoint.x;
        //float cy = principalPoint.y;

        device.size( imageSize.width, imageSize.height );
        device.intrinsics(cx,cy,fx,fy);
      };

      void parse_extrinsics( string filename, OpticalDevice::Config& device  )
      {
        bool absolute = false;
        cv::FileStorage fs(ofToDataPath(filename, absolute), cv::FileStorage::READ);

        cv::Mat R, T;

        fs["R"] >> R;
        fs["T"] >> T;

        if ( R.type() != CV_32FC1 ) 
          R.convertTo( R, CV_32FC1 );

        if ( T.type() != CV_32FC1 ) 
          T.convertTo( T, CV_32FC1 );

        T *= 1000.0; //mts->mm units
        //transpose to get proj->camera
        //T *= -1;
        //cv::transpose(R,R);

        // opencv: row-major	

        // R x axis
        ofVec3f X = ofVec3f(
            R.at<float>(0,0),  //xx
            R.at<float>(1,0),  //xy
            R.at<float>(2,0)); //xz

        // R y axis	
        ofVec3f Y = ofVec3f(
            R.at<float>(0,1),  //yx
            R.at<float>(1,1),  //yy
            R.at<float>(2,1)); //yz

        // R z axis	
        ofVec3f Z = ofVec3f(
            R.at<float>(0,2),  //yx
            R.at<float>(1,2),  //yy
            R.at<float>(2,2)); //yz

        ofVec3f Tvec = ofVec3f(
            T.at<float>(0,0),  //tx
            T.at<float>(1,0),  //ty
            T.at<float>(2,0)); //tz 

        device.extrinsics( X, Y, Z, Tvec ); 
      };

  };
};

