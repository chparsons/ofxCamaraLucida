#pragma once

#include "cmlDepth2Mesh.h"
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnLog.h>

namespace cml 
{
    class Depth2Mesh_openni : public Depth2Mesh
    {
        public:

            Depth2Mesh_openni();
            ~Depth2Mesh_openni();

            void update( 
                    uint16_t *raw_depth_pix, 
                    xn::DepthGenerator *depth_generator ) 
            {
                //const XnDepthPixel* depth_map = depth_generator->GetDepthMap(); 

                int len = mesh->length();

                XnPoint3D _pts2d[len];

                for (int i = 0; i < len; i++)
                {
                    int xdepth, ydepth, idepth;
                    mesh->to_depth( i, &xdepth, &ydepth, &idepth );
                    _pts2d[i].X = xdepth;
                    _pts2d[i].Y = ydepth;
                    _pts2d[i].Z = raw_depth_pix[ idepth ];
                }

                XnPoint3D _pts3d[len];
                depth_generator->ConvertProjectiveToRealWorld( len, _pts2d, _pts3d ); 

                for (int i = 0; i < len; i++)
                {
                    XnVector3D p3d = _pts3d[i];
                    XnVector3D p2d = _pts2d[i];

                    p3d.X *= 0.001;
                    p3d.Y *= -0.001;
                    p3d.Z *= 0.001;

                    if (p3d.Z == 0) p3d.Z = get_base_depth_mts();

                    float z = p3d.Z;
                    float x, y;
                    depth->unproject( p2d.X, p2d.Y, z, &x, &y );

                    mesh->set_vertex( i, x, y, z );
                }
            };
    };
};


