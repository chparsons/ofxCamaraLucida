//	Cámara Lúcida
//	www.camara-lucida.com.ar
//
//	Copyright (C) 2011  Christian Parsons
//	www.chparsons.com.ar
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "cmlMesh_openni.h"

namespace cml 
{
	Mesh_openni::Mesh_openni(xn::DepthGenerator *depth_generator)
	: Mesh()
	{
		this->depth_generator = depth_generator;
	}
	
	Mesh_openni::~Mesh_openni()
	{
		depth_generator = NULL;
	}
	 
	ofVec3f Mesh_openni::coord_sys()
	{
		return ofVec3f(-1., -1., 1.);
	}

	void Mesh_openni::init_pts()
	{
		pts3d = new ofVec3f[vbo_length];
		normals = new ofVec3f[vbo_length];
		
		memset( pts3d, 0, vbo_length*sizeof(ofVec3f) );	
		memset( normals, 0, vbo_length*sizeof(ofVec3f) );
	}

	void Mesh_openni::dispose_pts()
	{
		delete[] pts3d;
		pts3d = NULL;
		
		delete[] normals;
		normals = NULL;
	}

	void Mesh_openni::update_pts()
	{
		const XnDepthPixel* depth_map = depth_generator->GetDepthMap(); 
		
		XnPoint3D pts2d[vbo_length];
		
		for (int i = 0; i < vbo_length; i++)
		{
			int x2d, y2d;
			int depth_idx = get_raw_depth_idx(i, x2d, y2d);
			
			pts2d[i].X = x2d;
			pts2d[i].Y = y2d;
			pts2d[i].Z = (short)depth_map[ depth_idx ];
		}
		
		XnPoint3D _pts3d[vbo_length];
		depth_generator->ConvertProjectiveToRealWorld( vbo_length, pts2d, _pts3d ); 
		
		for (int i = 0; i < vbo_length; i++)
		{
			XnVector3D p3d = _pts3d[i];
			p3d.X *= 0.001;
			p3d.Y *= 0.001;
			p3d.Z *= 0.001;
			
			if (p3d.Z == 0) p3d.Z = 5.;
			
			pts3d[i].x = p3d.X;
			pts3d[i].y = -p3d.Y;
			pts3d[i].z = p3d.Z;
		}
	}

	//// dynamic buffers mesh impl
	
	float* Mesh_openni::pts0x()
	{
		return &pts3d[0].x;
	}
	
	int Mesh_openni::sizeof_pts()
	{
		return sizeof(ofVec3f);
	}
	
	float* Mesh_openni::normals0x()
	{
		return &((ofVec3f*)normals)[0].x;
	}
	
	int Mesh_openni::sizeof_normals()
	{
		return sizeof(ofVec3f);
	}
	
	////
	
	void Mesh_openni::debug_hue_texture(int x, int y, int width, int height)
	{
		//TODO
	}
	
	void Mesh_openni::print()
	{
		const XnDepthPixel* depth_map = depth_generator->GetDepthMap(); 
		
		XnPoint3D _pts2d[vbo_length];
		
		for (int i = 0; i < vbo_length; i++)
		{
			int mcol = i % mesh_w;
			int mrow = (i - mcol) / mesh_w;
			
			int col = mcol * mesh_step;
			int row = mrow * mesh_step;
			
			int depth_idx = row * calib->depth_width + col;
			
			_pts2d[i].X = col;
			_pts2d[i].Y = row;
			_pts2d[i].Z = (short)depth_map[ depth_idx ];
		}
		
		XnPoint3D _pts3d[vbo_length];
		depth_generator->ConvertProjectiveToRealWorld( vbo_length, _pts2d, _pts3d ); 
		
		cout << "### Mesh_openni ConvertProjectiveToRealWorld" << endl;
		
		for (int i = 0; i < vbo_length; i++)
		{
			XnVector3D p3d = _pts3d[i];
			XnVector3D p2d = _pts2d[i];
			
			p3d.X *= 0.001;
			p3d.Y *= 0.001;
			p3d.Z *= 0.001;
			
			cout << "### \t 2d " << p2d.X << ", " << p2d.Y << ", depth " << p2d.Z << "\t 3d: " << p3d.X << ", " << p3d.Y << ", " << p3d.Z << endl;
		}
	}
};