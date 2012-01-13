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

#include "cmlMesh_freenect_opencl.h"

namespace cml 
{
	Mesh_freenect_opencl::Mesh_freenect_opencl(uint16_t *raw_depth_pix,
											   MSA::OpenCL *opencl)
	: Mesh_freenect(raw_depth_pix) 
	{
		this->opencl = opencl;
	};
	
	Mesh_freenect_opencl::~Mesh_freenect_opencl()
	{
		opencl = NULL;
	};
	
	void Mesh_freenect_opencl::init_pts()
	{
		init_data();
		
		pts3d = new float4[vbo_length];
		pts3d_const = new float4[vbo_length];
		normals = new float4[vbo_length];

		memset( pts3d, 0, vbo_length*sizeof(float4) );	
		memcpy( pts3d_const, pts3d, vbo_length*sizeof(float4) );
		memset( normals, 0, vbo_length*sizeof(float4) );
	}
	
	void Mesh_freenect_opencl::init_end()
	{
		init_cl();
	}

	void Mesh_freenect_opencl::dispose_pts()
	{
		delete[] (float4*)pts3d;
		pts3d = NULL;
		
		delete[] pts3d_const;
		pts3d_const = NULL;
		
		delete[] (float4*)normals;
		normals = NULL;
	}

	void Mesh_freenect_opencl::update_pts()
	{
		update_cl();
	}
	
	//// dynamic buffers mesh impl

	float* Mesh_freenect_opencl::pts0x()
	{
		return &((float4*)pts3d)[0].x;
	}

	int Mesh_freenect_opencl::sizeof_pts()
	{
		return sizeof(float4);
	}
	
	float* Mesh_freenect_opencl::normals0x()
	{
		return &((float4*)normals)[0].x;
	}
	
	int Mesh_freenect_opencl::sizeof_normals()
	{
		return sizeof(float4);
	}
	
	
	// opencl


	void Mesh_freenect_opencl::init_cl()
	{
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL init");
		
		opencl->loadProgramFromFile( xml_config->getValue("opencl_kernel", "camara_lucida/vertex.cl") );
		kernel_vertex_update = opencl->loadKernel("update_vertex");
		
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL init buffers... raw_depth_pix "+ofToString(raw_depth_pix));
		
		cl_buff_pts3d.initFromGLObject( vbo.getVertId() );
		cl_buff_pts3d_const.initBuffer(sizeof(float4) * vbo_length, CL_MEM_READ_ONLY, pts3d_const);
		cl_buff_normals.initBuffer(sizeof(float4) * vbo_length, CL_MEM_READ_WRITE, normals);
		cl_buff_raw_depth.initBuffer(sizeof(uint16_t) * calib->depth_width * calib->depth_height, CL_MEM_READ_ONLY, raw_depth_pix);
		
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL set args");
		
		float4 depth_intrinsics(calib->cx_d, calib->cy_d,
								calib->fx_d, calib->fy_d);
		
		kernel_vertex_update->setArg(0, cl_buff_pts3d.getCLMem());
		kernel_vertex_update->setArg(1, cl_buff_normals.getCLMem());
		kernel_vertex_update->setArg(2, cl_buff_raw_depth.getCLMem());
		kernel_vertex_update->setArg(3, cl_buff_pts3d_const.getCLMem());
		kernel_vertex_update->setArg(4, mesh_step);
		kernel_vertex_update->setArg(5, depth_xoff);
		kernel_vertex_update->setArg(6, depth_intrinsics);
	}

	void Mesh_freenect_opencl::update_cl()
	{
		cl_buff_raw_depth.write(raw_depth_pix, 0, sizeof(uint16_t)*calib->depth_width*calib->depth_height);
		
		kernel_vertex_update->run1D(vbo_length);
		
		// TODO normals not working !!!!!
		//cl_buff_normals.read(normals, 0, sizeof(float4)*vbo_length);
		
		memcpy( pts3d_const, pts3d, vbo_length*sizeof(float4) );
	}
	
	
	//
	
	
	void Mesh_freenect_opencl::keyPressed(ofKeyEventArgs &args)
	{		
		Mesh_freenect::keyPressed(args);
		
		if (args.key == key_depth_xoff_inc || 
			args.key == key_depth_xoff_dec)
		{
			kernel_vertex_update->setArg(5, depth_xoff);
		}
	}
	
};