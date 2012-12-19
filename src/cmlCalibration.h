#pragma once

#include "cmlConfig.h"
#include "cmlOpticalDevice.h"
#include "cv.h"

namespace cml
{
    class Calibration
    {
        public:

            Calibration( cml::Config *cml_config )
            {
                near = cml_config->near;
                far = cml_config->far;

                string kcalib = cml_config->kinect_calib_path;
                string pcalib = cml_config->projector_calib_path;
                
                device_config( &proj_cfg, pcalib, 
                        "proj_intrinsics", 
                        "proj_size", 
                        "R", "T" );

                device_config( &depth_cfg, kcalib, 
                        "depth_intrinsics", 
                        "depth_size", 
                        "R", "T" );
                
                device_config( &rgb_cfg, kcalib, 
                        "rgb_intrinsics", 
                        "rgb_size", 
                        "R", "T" );
            };

            ~Calibration(){};

            OpticalDevice::Config& proj_config()
            {
                return proj_cfg; 
            };

            OpticalDevice::Config& depth_config()
            {
                return depth_cfg; 
            };

            OpticalDevice::Config& rgb_config()
            {
                return rgb_cfg; 
            };

        private:

            OpticalDevice::Config proj_cfg;
            OpticalDevice::Config depth_cfg;
            OpticalDevice::Config rgb_cfg;
            
            float near, far;

            void device_config( 
                    OpticalDevice::Config *dev_config,
                    string calib_path, 
                    string int_id,
                    string size_id,
                    string R_id, string T_id )
            {
                CvMat* _int = (CvMat*)cvLoad( calib_path.c_str(), 
                        NULL, int_id.c_str() );

                CvMat* _size = (CvMat*)cvLoad( calib_path.c_str(), 
                        NULL, size_id.c_str() );

                CvMat* _R = (CvMat*)cvLoad( calib_path.c_str(), 
                        NULL, R_id.c_str() );

                CvMat* _T = (CvMat*)cvLoad( calib_path.c_str(), 
                        NULL, T_id.c_str() );

                if (ofGetLogLevel() == OF_LOG_VERBOSE)
                {
                    ofLog(OF_LOG_VERBOSE,
                            "\n cml::Calibration "+calib_path);
                    ofLog( OF_LOG_VERBOSE, "intrinsics" );
                    printM( _int );
                    //ofLog( OF_LOG_VERBOSE, "R" );
                    //printM( _R );
                    //ofLog( OF_LOG_VERBOSE, "T" );
                    //printM( _T );
                }
                
                device_config( dev_config, _size, _int, _R, _T );
                
                cvReleaseMat( &_int );
                cvReleaseMat( &_size );
                cvReleaseMat( &_R );
                cvReleaseMat( &_T );
            };

            void device_config(  
                    OpticalDevice::Config *dev_config,
                    CvMat* _size, 
                    CvMat* _int, 
                    CvMat* _R, CvMat* _T )
            {
                int w = (int)cvGetReal2D( _size, 0, 0 );
                int h = (int)cvGetReal2D( _size, 0, 1 );

                float fx = (float)cvGetReal2D( _int, 0, 0 );
                float fy = (float)cvGetReal2D( _int, 1, 1 );
                float cx = (float)cvGetReal2D( _int, 0, 2 );
                float cy = (float)cvGetReal2D( _int, 1, 2 );

                //	opencv: row-major	
                //	R x axis
                ofVec3f X = ofVec3f(
                        (float)cvGetReal2D( _R, 0, 0 ), //xx
                        (float)cvGetReal2D( _R, 1, 0 ), //xy
                        (float)cvGetReal2D( _R, 2, 0 )  //xz
                        );
                //	R y axis	
                ofVec3f Y = ofVec3f(
                        (float)cvGetReal2D( _R, 0, 1 ), //yx
                        (float)cvGetReal2D( _R, 1, 1 ), //yy
                        (float)cvGetReal2D( _R, 2, 1 )  //yz
                        );
                //	R z axis	
                ofVec3f Z = ofVec3f(
                        (float)cvGetReal2D( _R, 0, 2 ), //zx
                        (float)cvGetReal2D( _R, 1, 2 ), //zy
                        (float)cvGetReal2D( _R, 2, 2 )  //zz
                        );
                //	T
                ofVec3f T = ofVec3f(
                        (float)cvGetReal2D( _T, 0, 0 ), //tx
                        (float)cvGetReal2D( _T, 1, 0 ), //ty
                        (float)cvGetReal2D( _T, 2, 0 )  //tz
                        );

                if (ofGetLogLevel() == OF_LOG_VERBOSE)
                { 
                    ofLog(OF_LOG_VERBOSE, "RX: "+ofToString(X) );
                    ofLog(OF_LOG_VERBOSE, "RY: "+ofToString(Y) );
                    ofLog(OF_LOG_VERBOSE, "RZ: "+ofToString(Z) );
                    ofLog(OF_LOG_VERBOSE, "T: "+ofToString(T) );
                }

                dev_config->near = this->near;
                dev_config->far = this->far;

                dev_config->width = w;
                dev_config->height = h;

                dev_config->cx = cx;
                dev_config->cy = cy;
                dev_config->fx = fx;
                dev_config->fy = fy;

                dev_config->X.set( X );
                dev_config->Y.set( Y );
                dev_config->Z.set( Z );
                dev_config->T.set( T );
            };

            void printM( CvMat* M, bool colmajor = true )
            {
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
};


