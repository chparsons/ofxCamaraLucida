#pragma once

#include "cmlCalibration.h"
#include "ofVbo.h"
#include "ofVectorMath.h"
#include "ofColor.h"

namespace cml 
{
    class Mesh
    {
        public:

            Mesh( int step, 
                    int depth_width, int depth_height,
                    int tex_width, int tex_height );
            ~Mesh();

            void dispose();
            void update();
            void render();

            void set_vertex( int i, float x, float y, float z );

            int length() { return mesh_length; }; 

            void to_depth(int mesh_idx, 
                    int *x_depth, int *y_depth, int *depth_idx);

        private:

            ofVbo vbo;

            void *pts3d;
            void init_pts();
            void dispose_pts();

            float* pts0x();
            int sizeof_pts();

            int step;
            int depth_width;
            int width;
            int height;
            int mesh_length;
            int ibo_length;	

            uint* ibo;
            ofVec2f* vbo_texcoords;
            ofFloatColor* vbo_color;

            void init_ibo();
            void init_texcoords(int tex_width, int tex_height);
            void init_colors();

            /// conversion utils

            // mesh <--> depth

            void to_depth_coord( 
                    int x_mesh, int y_mesh,
                    int *x_depth, int *y_depth);

            void to_mesh_coord( int mesh_idx, 
                    int *x_mesh, int *y_mesh);

            int to_depth_idx( int x_depth, int y_depth );

            //void to_mesh_coord(int x_depth, int y_depth, 
            //int *x_mesh, int *y_mesh);

            //int to_mesh_idx(int depth_idx);	
            //int to_mesh_idx(int x_mesh, int y_mesh);

    };
};
