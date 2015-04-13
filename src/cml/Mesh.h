#pragma once

#include "cml/Calibration.h"
#include "cml/DepthCamera.h"
#include "ofVbo.h"
#include "ofVectorMath.h"
#include "ofColor.h"

namespace cml 
{
  class Mesh
  {
    public:

      Mesh( int res, 
          int depth_width, 
          int depth_height,
          int tex_width, 
          int tex_height );
      ~Mesh();

      void dispose();
      void update( uint16_t *depth_pix_mm, DepthCamera* depth );
      void render();

      void log()
      {
        for (int i = 0; i < mesh_len; i++)
          cout 
            << "cml mesh 3d vertex "
            << pts3d[i] 
            << endl;
      };

      void to_depth( 
          int mesh_idx, 
          int *depth_idx );

      void to_depth( 
          int mesh_idx, 
          int *x_depth, int *y_depth, 
          int *depth_idx );

    private:

      ofVbo vbo;

      int res;
      int depth_width;
      int width;
      int height;
      int mesh_len;
      int ibo_len;	

      ofVec3f* pts3d;
      uint* ibo;
      ofVec2f* texcoords;
      ofFloatColor* colors;

      void init_pts();
      void init_ibo();
      void init_texcoords( int tex_width, int tex_height );
      void init_colors();

      /// conversion utils

      // mesh <--> depth

      void to_depth_coord( 
          int x_mesh, int y_mesh,
          int *x_depth, int *y_depth);

      void to_mesh_coord( 
          int mesh_idx, 
          int *x_mesh, int *y_mesh);

      int to_depth_idx(int x_depth, int y_depth);

      //void to_mesh_coord(int x_depth, int y_depth, 
      //int *x_mesh, int *y_mesh);

      //int to_mesh_idx(int depth_idx);	
      //int to_mesh_idx(int x_mesh, int y_mesh);

  };
};

