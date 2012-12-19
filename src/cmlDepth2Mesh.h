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

