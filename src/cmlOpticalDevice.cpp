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

#include "cmlOpticalDevice.h"

namespace cml
{
    OpticalDevice::OpticalDevice( 
            const OpticalDevice::Config& config )
    {
        this->_width = config.width;
        this->_height = config.height;
        this->_near = config.near;
        this->_far = config.far;
        this->_cx = config.cx;
        this->_cy = config.cy;
        this->_fx = config.fx;
        this->_fy = config.fy;

        KK_cv2gl();
        RT_cv2gl( config.X, config.Y, config.Z, config.T );

        loc = ofVec3f( RT[12], RT[13], RT[14] );	
        fwd = ofVec3f( RT[8], RT[9], RT[10] );
        up = ofVec3f( RT[4], RT[5], RT[6] );
        trg = loc + fwd;

        //_T = ofVec3f( RT[12], rgb_RT[13], rgb_RT[14] );
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
            int x2d, int y2d, 
            float z, float *x, float *y)
    {
        *x = (x2d - _cx) * z / _fx;
        *y = (y2d - _cy) * z / _fy;
    };

    ofVec2f OpticalDevice::project(const ofVec3f& p3)
    {
        ofVec2f p2;
        p2.x = (p3.x * _fx / p3.z) + _cx;
        p2.y = (p3.y * _fy / p3.z) + _cy;
        return p2;
    }; 

    float* OpticalDevice::gl_projection_matrix() 
    { 
        return KK; 
    };

    float* OpticalDevice::gl_modelview_matrix() 
    { 
        return RT; 
    };

    int OpticalDevice::width() 
    { 
        return _width; 
    };

    int OpticalDevice::height() 
    { 
        return _height; 
    };

    int OpticalDevice::near() 
    { 
        return _near; 
    };

    int OpticalDevice::far() 
    { 
        return _far; 
    };

    int OpticalDevice::to_idx(int x, int y)
    {
        return y * _width + x;
    };

    void OpticalDevice::to_xy(int idx, int& x, int& y)
    {
        x = idx % _width;
        y = (idx - x) / _width;
    };

    /*
     * Intrinsics from opencv to opengl
     *
     * http://opencv.willowgarage.com/wiki/Posit
     * http://www.songho.ca/opengl/gl_projectionmatrix.html
     * http://www.songho.ca/opengl/gl_transform.html
     */
    void OpticalDevice::KK_cv2gl()
    {
        float A = 2. * _fx / _width;
        float B = 2. * _fy / _height;
        float C = 2. * (_cx / _width) - 1.;
        float D = 2. * (_cy / _height) - 1.;
        float E = - (_far + _near) / (_far - _near);
        float F = -2. * _far * _near / (_far - _near);

        // opengl: col-major
        KK[0]= A;   KK[4]= 0.;  KK[8]= C;   KK[12]= 0.;
        KK[1]= 0.;  KK[5]= B;   KK[9]= D;   KK[13]= 0.;
        KK[2]= 0.;  KK[6]= 0.;  KK[10]= E;  KK[14]= F;
        KK[3]= 0.;	KK[7]= 0.;	KK[11]= -1.;KK[15]= 0.;	

        //another solution by Kyle McDonald...
        //https://github.com/kylemcdonald/ofxCv/blob/master/libs/ofxCv/src/Calibration.cpp
        //glFrustum(
        //_near * (-_cx) / _fx, _near * (w - _cx) / _fx,
        //_near * (_cy - h) / _fy, _near * (_cy) / _fy,
        //_near, _far );
    };

    void OpticalDevice::RT_cv2gl( 
            ofVec3f x, ofVec3f y, ofVec3f z, ofVec3f t )
    {
        //	opengl: col-major	
        RT[0]= x.x; RT[4]= y.x; RT[8]= z.x;	RT[12]= t.x;
        RT[1]= x.y;	RT[5]= y.y;	RT[9]= z.y;	RT[13]= t.y;
        RT[2]= x.z;	RT[6]= y.z;	RT[10]=z.z; RT[14]= t.z;
        RT[3]= 0.;	RT[7]= 0.;	RT[11]= 0.;	RT[15]= 1.;
    };

    void OpticalDevice::printM( 
            float* M, int rows, int cols, bool colmajor)
    {
        if ( ofGetLogLevel() != OF_LOG_VERBOSE )
            return;

        if (colmajor)
        {
            for (int j = 0; j < cols; j++)
            {
                printf("\n");
                for (int i = 0; i < rows; i++)
                {
                    printf("%9.3f ", M[i*cols+j]);
                }	
            }
        }
        else
        {
            for (int i = 0; i < rows; i++)
            {
                printf("\n");
                for (int j = 0; j < cols; j++)
                {
                    printf("%9.3f ", M[i*cols+j]);
                }	
            }
        }
        printf("\n\n");
    };

};


