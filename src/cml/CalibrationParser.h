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
        //kinect:
        //we can use kinect rgb calibration 
        //for depth camera bc we have registration
        //which gives the depth map 
        //as if it were seen by the rgb camera
        load_intrinsics( config.kinect_intrinsics_file, false, depth_cfg );
        //set depth extrinsics
        //to be the world origin
        depth_cfg.extrinsics( 
            ofVec3f( 1.,0.,0. ),
            ofVec3f( 0.,1.,0. ),
            ofVec3f( 0.,0.,1. ),
            ofVec3f( 0.,0.,0. ) );

        //projector:
        load_intrinsics( config.projector_intrinsics_file, false, proj_cfg );
        load_extrinsics( config.projector_kinect_extrinsics_file, proj_cfg );

      };

    private:

      void load_intrinsics( string filename, bool undistorted, OpticalDevice::Config& device )
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

        //TODO wtf ??? 
        //cy => principalPoint.y != cameraMatrix.at<double>(1, 2) 
        float cx = cameraMatrix.at<double>(0, 2);
        float cy = cameraMatrix.at<double>(1, 2);
        //float cx = principalPoint.x;
        //float cy = principalPoint.y;

        device.size( imageSize.width, imageSize.height );
        device.intrinsics(cx,cy,fx,fy);

        ofLogNotice("cml::CalibrationParser") 
          << "\n" << "intrinsics loaded"
          << "\n" << "filename: " << filename
          << "\n" << "image width: " << imageSize.width
          << "\n" << "image height: " << imageSize.height
          << "\n" << "principal point: " << principalPoint
          << "\n" << "cx: " << cx
          << "\n" << "cy: " << cy
          << "\n" << "fx: " << fx
          << "\n" << "fy: " << fy
          << "\n" << "camera matrix: \n" << cameraMatrix
          << "\n";
      };

      void load_extrinsics( string filename, OpticalDevice::Config& device  )
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

        ofLogNotice("cml::CalibrationParser") 
          << "\n" << "extrinsics loaded"
          << "\n" << "filename: " << filename
          << "\n" << "T: \n" << T
          << "\n" << "R: \n" << R
          << "\n" << "T vec: " << ofToString(Tvec)
          << "\n" << "R x axis: " << ofToString(X)
          << "\n" << "R y axis: " << ofToString(Y)
          << "\n" << "R z axis: " << ofToString(X)
          << "\n";
      };

  };
};

