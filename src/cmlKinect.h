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

namespace cml
{
    class Kinect : public OpticalDevice
    {
        public:

            Kinect(const OpticalDevice::Config& config, int xoff) 
                : OpticalDevice( config ),
                k1( 0.1236 ),
                k2( 2842.5 ),
                k3( 1.1863 ),
                k4( 0.0370 )
            {
		        this->xoff = xoff; 

                uint16_t size = 2048; //11bits
                _zlut = new float[size];
                for (int i = 0; i < size; i++) 
                {
                    _zlut[i] = raw_depth_to_mts(i);
                }
            }; 

            ~Kinect()
            {
                delete _zlut; _zlut = NULL;
            };

            void unproject(int x2d, int y2d, float z, float *x, float *y)
            {
                *x = (x2d + xoff - cx) * z / fx;
                *y = (y2d - cy) * z / fy;
            };

            void unproject(int x2d, int y2d, float z, ofVec2f *p2)
            {
                unproject( x2d, y2d, z, &(p2->x), &(p2->y) );
            };
            
            ofVec2f unproject(int x2d, int y2d, float z)
            {
                float x, y;
                unproject( x2d, y2d, x, &x, &y );
                return ofVec2f( x, y );
            };

            void project(const ofVec3f& p3, float *x, float *y)
            {
                *x = (p3.x * fx / p3.z) + cx - xoff;
                *y = (p3.y * fy / p3.z) + cy;
            };
            
            ofVec2f project(const ofVec3f& p3)
            {
                float x, y;
                project( p3, &x, &y );
                return ofVec2f( x, y );;
            };

            float z_mts(uint16_t raw_depth)
            {
                return _zlut[raw_depth];
            };

            float z_mts(uint16_t *raw_depth_pix, int _x, int _y)
            {
                int idx = to_idx(_x, _y);
                uint16_t raw_depth = raw_depth_pix[idx];
                return _zlut[raw_depth];
            };

            void raw_depth_to_p3(uint16_t *raw_depth_pix, 
                    int _x, int _y, ofVec3f *p3)
            {
                float z, x, y;
                z = z_mts(raw_depth_pix, _x, _y);
                unproject( _x, _y, z, &x, &y );
                p3->x = x;
                p3->y = y;
                p3->z = z;
            };
            
            ofVec3f raw_depth_to_p3(uint16_t *raw_depth_pix, 
                    int _x, int _y)
            {
                ofVec3f p3;
                raw_depth_to_p3(raw_depth_pix, _x, _y, &p3);
                return p3;
            };

            void raw_depth_to_p3(uint16_t raw_depth, 
                    int _x, int _y, ofVec3f *p3)
            {
                float z, x, y;
                z = z_mts(raw_depth);
                unproject( _x, _y, z, &x, &y );
                p3->x = x;
                p3->y = y;
                p3->z = z;
            }; 

            ofVec3f raw_depth_to_p3(uint16_t raw_depth, 
                    int _x, int _y)
            {
                ofVec3f p3;
                raw_depth_to_p3(raw_depth, _x, _y, &p3);
                return p3;
            };

            void change_xoff( int val )
            {
                xoff += val;
            };

            //ofVec2f raw_depth_to_rgb(
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

        private:

            float k1, k2, k3, k4;

            float *_zlut;
            int xoff;

            //http://openkinect.org/wiki/Imaging_Information
            //http://nicolas.burrus.name/index.php/Research/KinectCalibration
            float raw_depth_to_mts(uint16_t raw_depth)
            {
                raw_depth = CLAMP(raw_depth, 0, 1024); //5mts~
                return k1 * tanf(((float)raw_depth/k2) + k3) - k4;
            }; 
    };
};


