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

#include "cmlMesh.h"

namespace cml 
{
  Mesh::Mesh( int step, 
      int depth_width, int depth_height,
      int tex_width, int tex_height )
  {
    this->step = step;
    this->depth_width = depth_width;

    width = (float)depth_width / step;
    height = (float)depth_height / step;
    mesh_length = width * height;
    ibo_length = mesh_length * 4;

    init_ibo();
    init_texcoords( tex_width, tex_height );
    init_colors();        
    init_pts();

    // init vbo

    vbo.setVertexData( pts0x(), 3, mesh_length, 
        GL_DYNAMIC_DRAW, sizeof_pts() );

    //vbo.setNormalData( normals0x(), mesh_length, GL_DYNAMIC_DRAW, sizeof_normals() );

    vbo.setIndexData( ibo, 
        ibo_length, GL_STATIC_DRAW );

    vbo.setColorData( vbo_color, 
        mesh_length, GL_STATIC_DRAW );

    vbo.setTexCoordData( vbo_texcoords, 
        mesh_length, GL_STATIC_DRAW );
  }

  Mesh::~Mesh()
  {
    dispose(); 
  }

  void Mesh::dispose()
  {
    vbo.clear();
    dispose_pts();
    delete[] ibo; ibo = NULL;
    delete[] vbo_texcoords; vbo_texcoords = NULL;
    delete[] vbo_color; vbo_color = NULL;
  }

  void Mesh::init_ibo()
  {
    ibo = new uint[ibo_length];
    for (int i = 0; i < mesh_length; i++) 
    {
      int x_mesh, y_mesh;
      to_mesh_coord( i, &x_mesh, &y_mesh );

      if ( ( x_mesh < width - step ) && 
          ( y_mesh < height - step ) ) 
      {
        int ibo_idx = i * 4;

        ibo[ibo_idx+0] = (uint)( y_mesh * width + x_mesh );
        ibo[ibo_idx+1] = (uint)( (y_mesh + step) * width + x_mesh );
        ibo[ibo_idx+2] = (uint)( (y_mesh + step) * width + (x_mesh + step) );
        ibo[ibo_idx+3] = (uint)( y_mesh * width + (x_mesh + step) );
      }
    }
  }

  void Mesh::init_texcoords( 
      int tex_width, int tex_height )
  {
    vbo_texcoords = new ofVec2f[mesh_length];
    for (int i = 0; i < mesh_length; i++) 
    {
      int x_mesh, y_mesh;
      to_mesh_coord( i, &x_mesh, &y_mesh );

      float t = ((float)x_mesh /width) * tex_width;
      float u = ((float)y_mesh /height) * tex_height;

      vbo_texcoords[i] = ofVec2f(t, u);
    }
  }

  void Mesh::init_colors()
  {
    vbo_color = new ofFloatColor[mesh_length];
    for (int i = 0; i < mesh_length; i++) 
    {
      vbo_color[i] = ofFloatColor(1,1,1,1);
    }
  }

  void Mesh::update()
  {
    vbo.updateVertexData( pts0x(), mesh_length );
    //vbo.updateNormalData( normals0x(), mesh_length );
  }

  void Mesh::render()
  {
    //ofMesh/ofVboMesh don't support GL_QUADS
    vbo.drawElements( GL_QUADS, ibo_length );
  }

  void Mesh::set_vertex( int i, 
      float x, float y, float z )
  {
    ((ofVec3f*)pts3d)[i].x = x;
    ((ofVec3f*)pts3d)[i].y = y;
    ((ofVec3f*)pts3d)[i].z = z;
  }

  void Mesh::init_pts()
  {
    pts3d = new ofVec3f[mesh_length];
    memset( pts3d, 0, mesh_length*sizeof(ofVec3f) );
  }

  void Mesh::dispose_pts()
  {
    delete[] (ofVec3f*)pts3d; pts3d = NULL;
  }

  float* Mesh::pts0x()
  {
    return &((ofVec3f*)pts3d)[0].x;
  }

  int Mesh::sizeof_pts()
  {
    return sizeof(ofVec3f);
  }

  // conversion utils 

  // mesh <--> depth

  void Mesh::to_depth( int mesh_idx, 
      int *x_depth, int *y_depth, int *depth_idx )
  {
    int x_mesh, y_mesh;
    to_mesh_coord( mesh_idx, &x_mesh, &y_mesh );
    to_depth_coord(x_mesh, y_mesh, x_depth, y_depth);
    *depth_idx = to_depth_idx( *x_depth, *y_depth );
  }

  int Mesh::to_depth_idx( int x_depth, int y_depth )
  {
    return y_depth * depth_width + x_depth;
  }

  void Mesh::to_depth_coord(
      int x_mesh, int y_mesh, 
      int *x_depth, int *y_depth )
  {
    *x_depth = x_mesh * step;
    *y_depth = y_mesh * step;
  } 

  // mesh

  void Mesh::to_mesh_coord(
      int mesh_idx, 
      int *x_mesh, int *y_mesh )
  {
    *x_mesh = mesh_idx % width;
    *y_mesh = (mesh_idx - *x_mesh) / width;
  }	

  //void Mesh::to_mesh_coord(int x_depth, int y_depth,
  //int *x_mesh, int *y_mesh)
  //{
  //*x_mesh = (int)(x_depth / step);
  //*y_mesh = (int)(y_depth / step);
  //}

  //int Mesh::to_mesh_idx(int x_mesh, int y_mesh)
  //{
  //return y_mesh * width + x_mesh;
  //}

  //int Mesh::to_mesh_idx(int depth_idx)
  //{
  //if (step == 1)
  //return depth_idx;

  //int x_depth, y_depth;
  //get_depth_coord(depth_idx, x_depth, y_depth);

  //int x_mesh, y_mesh;
  //to_mesh_coord(x_depth, y_depth, &x_mesh, &y_mesh);

  //return to_mesh_idx(x_mesh, y_mesh);
  //}

};

