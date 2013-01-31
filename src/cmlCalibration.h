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

#include "cmlConfig.h"
#include "cmlOpticalDevice.h"
#include "cv.h"

namespace cml
{
    class Calibration
    {
        public:

            Calibration( cml::Config *cml_config );
            ~Calibration();

            OpticalDevice::Config& proj_config();
            OpticalDevice::Config& depth_config();
            OpticalDevice::Config& rgb_config();

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
                    string R_id, string T_id );

            void device_config(  
                    OpticalDevice::Config *dev_config,
                    CvMat* _size, 
                    CvMat* _int, 
                    CvMat* _R, CvMat* _T );

            void printM( CvMat* M, bool colmajor = true );
    };
};


