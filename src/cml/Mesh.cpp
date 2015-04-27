#include "cml/Mesh.h"

namespace cml 
{
  Mesh::Mesh( 
      int res, 
      int depth_width, 
      int depth_height,
      int tex_width, 
      int tex_height )
  {
    this->res = res;
    this->depth_width = depth_width;

    width = (float)depth_width / res;
    height = (float)depth_height / res;
    mesh_len = width * height;
    ibo_len = mesh_len * 6;

    init_ibo();
    init_texcoords(tex_width,tex_height);
    init_colors();        
    init_pts();

    vbo.setVertexData( &(pts3d)[0].x, 3, mesh_len, /*GL_STATIC_DRAW*/ GL_DYNAMIC_DRAW, sizeof(ofVec3f) );
    //vbo.setNormalData( normals0x(), mesh_len, GL_DYNAMIC_DRAW, sizeof_normals() );
    vbo.setIndexData( ibo, ibo_len, GL_STATIC_DRAW );
    vbo.setColorData( colors, mesh_len, GL_STATIC_DRAW );
    vbo.setTexCoordData( texcoords, mesh_len, GL_STATIC_DRAW );
  }

  Mesh::~Mesh()
  {
    dispose(); 
  }

  void Mesh::update( uint16_t *depth_pix_mm, DepthCamera* depth )
  {

    float epsilon = std::numeric_limits<float>::epsilon();

    for ( int i = 0; i < mesh_len; i++ )
    {
      int xd, yd, idepth;

      to_depth( i, &xd, &yd, &idepth );

      float zmm = depth_pix_mm[idepth];
      zmm = CLAMP( ( zmm < epsilon ? depth->far_clamp : zmm ), 0.0, depth->far_clamp );

      float x, y;
      depth->unproject( xd, yd, zmm, &x, &y );

      pts3d[i].x = x;
      pts3d[i].y = y;
      pts3d[i].z = zmm;
    }

    vbo.updateVertexData( &(pts3d)[0].x, mesh_len );
    //vbo.updateNormalData( normals0x(), mesh_len );
  }

  void Mesh::render()
  {
    vbo.drawElements( GL_TRIANGLES, ibo_len );
  }

  void Mesh::dispose()
  {
    vbo.clear();
    delete[] pts3d; pts3d = NULL;
    delete[] ibo; ibo=NULL;
    delete[] texcoords; texcoords=NULL;
    delete[] colors; colors=NULL;
  }

  void Mesh::init_pts()
  {
    pts3d = new ofVec3f[ mesh_len ];
    memset(pts3d, 0, mesh_len*sizeof(ofVec3f));
  }

  void Mesh::init_ibo()
  {
    ibo = new uint[ ibo_len ];
    for ( int i = 0; i < mesh_len; i++ ) 
    {
      int x_mesh, y_mesh;
      to_mesh_coord(i, &x_mesh, &y_mesh);

      if ( ( x_mesh < width - res ) 
          && ( y_mesh < height - res ) ) 
      {
        int ibo_idx = i * 6;

        // triang 1
        ibo[ ibo_idx + 0 ] = (uint)( y_mesh * width + x_mesh );
        ibo[ ibo_idx + 1 ] = (uint)( y_mesh * width + (x_mesh + res) );
        ibo[ ibo_idx + 2 ] = (uint)( (y_mesh + res) * width + x_mesh );

        // triang 2
        ibo[ ibo_idx + 3 ] = (uint)( (y_mesh) * width + (x_mesh + res) );
        ibo[ ibo_idx + 4 ] = (uint)( (y_mesh + res) * width + (x_mesh + res) );
        ibo[ ibo_idx + 5 ] = (uint)( (y_mesh + res) * width + (x_mesh) );

      }
    }
  }

  void Mesh::init_texcoords( 
      int tex_width, int tex_height )
  {
    texcoords = new ofVec2f[mesh_len];
    for (int i = 0; i < mesh_len; i++) 
    {
      int x_mesh, y_mesh;
      to_mesh_coord(i, &x_mesh, &y_mesh);

      float t = ((float)x_mesh /width) * tex_width;
      float u = ((float)y_mesh /height) * tex_height;

      texcoords[i] = ofVec2f( t, u );
    }
  }

  void Mesh::init_colors()
  {
    colors = new ofFloatColor[ mesh_len ];
    for ( int i = 0; i < mesh_len; i++ ) 
      colors[i] = ofFloatColor(1,1,1,1);
  }
 

  // conversion utils 

  // mesh <--> depth

  void Mesh::to_depth( 
      int mesh_idx, 
      int *depth_idx )
  {
    int x_depth, y_depth;
    to_depth( 
        mesh_idx, 
        &x_depth, &y_depth,
        depth_idx );
  }

  void Mesh::to_depth( 
      int mesh_idx, 
      int *x_depth, int *y_depth, 
      int *depth_idx )
  {
    int x_mesh, y_mesh;

    to_mesh_coord( 
      mesh_idx, &x_mesh, &y_mesh );

    to_depth_coord(
      x_mesh, y_mesh, x_depth, y_depth);

    *depth_idx = to_depth_idx( 
      *x_depth, *y_depth );
  }

  int Mesh::to_depth_idx( 
      int x_depth, int y_depth )
  {
    return y_depth*depth_width+x_depth;
  }

  void Mesh::to_depth_coord(
      int x_mesh, int y_mesh, 
      int *x_depth, int *y_depth )
  {
    *x_depth = x_mesh * res;
    *y_depth = y_mesh * res;
  } 

  // mesh

  void Mesh::to_mesh_coord(
      int mesh_idx, 
      int *x_mesh, int *y_mesh )
  {
    *x_mesh = mesh_idx % width;
    *y_mesh = (mesh_idx - *x_mesh)/width;
  }	

};

