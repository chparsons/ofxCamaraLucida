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

#pragma once

#include "ofVectorMath.h"
#include "ofLog.h"

namespace cml
{
    class OpticalDevice
    {
        public:

            struct Config
            {
                int width, height;
                float near, far;
                float cx, cy, fx, fy;
                ofVec3f X, Y, Z, T;
            };

            OpticalDevice( const OpticalDevice::Config& config ); 

            virtual ~OpticalDevice();

            virtual void unproject( 
                    int x2d, int y2d, 
                    float z, float *x, float *y );

            virtual ofVec2f project( const ofVec3f& p3 );

            ofVec3f loc, fwd, up, trg;

            float* gl_projection_matrix();
            float* gl_modelview_matrix();

            int width();
            int height();

            int near();
            int far();

            int to_idx(int x, int y);
            void to_xy(int idx, int& x, int& y);

        protected:

            int _width, _height;
            float _near, _far;	
            float _cx, _cy, _fx, _fy;
        
        private:

            float KK[16]; //glMultMatrixf( KK )
            float RT[16]; //glMultMatrixf( RT )

            /*
             * Intrinsics from opencv to opengl
             */
            void KK_cv2gl();

            /*
             * RT from opencv row-major to opengl col-major
             */
            void RT_cv2gl( 
                    ofVec3f x, ofVec3f y, ofVec3f z, ofVec3f t );

            void printM( 
                    float* M, int rows, int cols, 
                    bool colmajor = true ); 

    };
};

