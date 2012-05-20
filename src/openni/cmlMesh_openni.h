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

#pragma once

#include "cmlMesh.h"
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnLog.h>

namespace cml
{
	class Mesh_openni : public Mesh
	{
	public:
		
		Mesh_openni(xn::DepthGenerator *depth_generator);
		~Mesh_openni();
		
		ofVec3f coord_sys();
		void debug_hue_texture(int x, int y, int width, int height);
		
		void print();
		
	protected:
		
		// Mesh implementation...
		
		void init_pts();
		void dispose_pts();
		void update_pts();
		
		float* pts0x();
		int sizeof_pts();
		
		float* normals0x();
		int sizeof_normals();
		
		//
		
	private:
		
		// TODO and what of we want the normals 
		// to be calculated with opencl here ?????
		
		ofVec3f* pts3d;
		ofVec3f* normals;
		
		xn::DepthGenerator *depth_generator;
		
	};
};