//	Camara Lucida
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

#include "cmlMesh_freenect.h"
#include "MSAOpenCL.h"

namespace cml 
{
	class Mesh_freenect_opencl : public Mesh_freenect
	{
	public:
		
		Mesh_freenect_opencl(uint16_t *raw_depth_pix,
							 MSA::OpenCL *opencl = NULL);
		~Mesh_freenect_opencl();
		
		void keyPressed(ofKeyEventArgs &args);
		
	protected:
		
		//// Mesh impl
		
		void init_end();
		void init_pts();
		void update_pts();
		void dispose_pts();
		
		float* pts0x();
		int sizeof_pts();
		
		float* normals0x();
		int sizeof_normals();
		
		////
		
	private:
		
		float4* pts3d_const;
		
		void init_cl();
		void update_cl();
		
		MSA::OpenCL			*opencl;
		MSA::OpenCLKernel	*kernel_vertex_update;
		MSA::OpenCLBuffer	cl_buff_pts3d;
		MSA::OpenCLBuffer	cl_buff_pts3d_const;
		MSA::OpenCLBuffer	cl_buff_normals;
		MSA::OpenCLBuffer	cl_buff_raw_depth;
	};
};