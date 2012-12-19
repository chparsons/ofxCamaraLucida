#pragma once

#include "cmlDepth2Mesh.h"

namespace cml 
{
    class Depth2Mesh_freenect : public Depth2Mesh
    {
        public:

            Depth2Mesh_freenect(){};
            ~Depth2Mesh_freenect(){};

            void update( uint16_t *raw_depth_pix ) 
            {
                int len = mesh->length();

                for (int i = 0; i < len; i++)
                {
                    int xdepth, ydepth, idepth;
                    mesh->to_depth( i, &xdepth, &ydepth, &idepth );

                    uint16_t raw_depth = raw_depth_pix[idepth];
                    float z = ((cml::Kinect*)depth)->z_mts(raw_depth);

                    // ofxKinect gives raw depth as distance in mm
                    //float z = raw_depth_pix[idepth] * 0.001;

                    float x, y;
                    depth->unproject(xdepth, ydepth, z, &x, &y);

                    mesh->set_vertex( i, x, y, z );
                }
                mesh->update();
            };
    };
};


