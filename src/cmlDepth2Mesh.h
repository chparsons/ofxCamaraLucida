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

#include "cmlOpticalDevice.h"
#include "cmlMesh.h"

namespace cml 
{
    class Depth2Mesh
    {
        public:

            Depth2Mesh(){};

            virtual ~Depth2Mesh()
            {
                dispose();
            };

            virtual void init( OpticalDevice* depth, Mesh* mesh )
            {
                this->depth = depth;
                this->mesh = mesh;
            };

            virtual void dispose()
            {
                depth = NULL;
                mesh = NULL;
            };

        protected:

            OpticalDevice* depth;
            Mesh* mesh;
    };
};

