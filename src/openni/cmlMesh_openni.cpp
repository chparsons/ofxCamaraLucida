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

#include "cmlMesh_openni.h"

namespace cml 
{
	Mesh_openni::Mesh_openni(uint16_t *raw_depth_pix,
							 xn::DepthGenerator *depth_generator)
	: Mesh(raw_depth_pix)
	{
		this->depth_generator = depth_generator;
	}
	
	Mesh_openni::~Mesh_openni()
	{
		depth_generator = NULL;
	}
	 
	void Mesh_openni::update_pts()
	{
		//const XnDepthPixel* depth_map = depth_generator->GetDepthMap(); 
		
		XnPoint3D _pts2d[mesh_length];
		
		for (int i = 0; i < mesh_length; i++)
		{
			int x_depth, y_depth;
			int depth_idx = to_depth_idx(i, x_depth, y_depth);
			
			_pts2d[i].X = x_depth;
			_pts2d[i].Y = y_depth;
			_pts2d[i].Z = raw_depth_pix[ depth_idx ];
		}
		
		XnPoint3D _pts3d[mesh_length];
		depth_generator->ConvertProjectiveToRealWorld( mesh_length, _pts2d, _pts3d ); 
		
		for (int i = 0; i < mesh_length; i++)
		{
			XnVector3D p3d = _pts3d[i];
			XnVector3D p2d = _pts2d[i];
			
			p3d.X *= 0.001;
			p3d.Y *= -0.001;
			p3d.Z *= 0.001;
			
			if (p3d.Z == 0) p3d.Z = get_base_depth_mts();
			
			float z = p3d.Z;
			float x, y;
			unproject_depth( p2d.X, p2d.Y, z, x, y );
						
			((ofVec3f*)pts3d)[i].x = x;
			((ofVec3f*)pts3d)[i].y = y;
			((ofVec3f*)pts3d)[i].z = z;
		}
	}

	////
	
	void Mesh_openni::print()
	{
		//const XnDepthPixel* depth_map = depth_generator->GetDepthMap(); 
		
		XnPoint3D _pts2d[mesh_length];
		
		for (int i = 0; i < mesh_length; i++)
		{
			int x_depth, y_depth;
			int depth_idx = to_depth_idx(i, x_depth, y_depth);
			
			_pts2d[i].X = x_depth;
			_pts2d[i].Y = y_depth;
			_pts2d[i].Z = raw_depth_pix[ depth_idx ];
		}
		
		XnPoint3D _pts3d[mesh_length];
		depth_generator->ConvertProjectiveToRealWorld( mesh_length, _pts2d, _pts3d ); 
		
		cout << "### Mesh_openni ConvertProjectiveToRealWorld" << endl;
		
		for (int i = 0; i < mesh_length; i++)
		{
			XnVector3D p3d = _pts3d[i];
			XnVector3D p2d = _pts2d[i];
			
			p3d.X *= 0.001;
			p3d.Y *= -0.001;
			p3d.Z *= 0.001;
			
			float z = p3d.Z;
			float x, y;
			unproject_depth( p2d.X, p2d.Y, z, x, y );
			
			cout << "### \t 2d " << p2d.X << ", " << p2d.Y << ", depth " << p2d.Z << "\t 3d: " << x << ", " << y << ", " << z << endl;
		}
	}
};
