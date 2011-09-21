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


#include "cmlMesh_freenect_opencl.h"

namespace cml 
{
	Mesh_freenect_opencl::Mesh_freenect_opencl(uint16_t *raw_depth_pix,
											   MSA::OpenCL *opencl)
	{
		this->raw_depth_pix = raw_depth_pix;
		this->opencl = opencl;
	};
	
	Mesh_freenect_opencl::~Mesh_freenect_opencl()
	{
		dispose_pts();
	};
	
	void Mesh_freenect_opencl::init_pts()
	{
		super::init_pts();
		
		pts3d_cl = new float4[vbo_length];
		for (int i = 0; i < vbo_length; i++) 
		{
			pts3d_cl[i] = float4(0, 0, 0);
		}
		
		init_cl();
	}

	void Mesh_freenect_opencl::dispose_pts()
	{
		delete[] pts3d_cl;
		pts3d_cl = NULL;
		
		opencl = NULL;
	}

	void Mesh_freenect_opencl::update_pts()
	{
		update_cl();
	}

	float* Mesh_freenect_opencl::pts0x()
	{
		return &pts3d_cl[0].x;
	}

	int Mesh_freenect_opencl::sizeof_pts()
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
		
		//	TODO why this doesn't work?
		//	cl_buff_pts3d.initFromGLObject(vbo.getVertId());
		
		cl_buff_pts3d.initBuffer(sizeof(float4) * vbo_length, CL_MEM_READ_WRITE, pts3d_cl);
		cl_buff_raw_depth.initBuffer(sizeof(uint16_t) * calib->depth_width * calib->depth_height, CL_MEM_READ_ONLY, raw_depth_pix);
		
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL set args");
		
		kernel_vertex_update->setArg(0, cl_buff_pts3d.getCLMem());
		kernel_vertex_update->setArg(1, cl_buff_raw_depth.getCLMem());
		kernel_vertex_update->setArg(2, mesh_step);
		kernel_vertex_update->setArg(3, calib->cx_d);
		kernel_vertex_update->setArg(4, calib->cy_d);
		kernel_vertex_update->setArg(5, calib->fx_d);
		kernel_vertex_update->setArg(6, calib->fy_d);
		kernel_vertex_update->setArg(7, depth_xoff);
	}

	void Mesh_freenect_opencl::update_cl()
	{
		cl_buff_raw_depth.write(raw_depth_pix, 0, sizeof(uint16_t) * calib->depth_width * calib->depth_height);
		
		kernel_vertex_update->run1D(vbo_length);
		
		cl_buff_pts3d.read(pts3d_cl, 0, sizeof(float4) * vbo_length);
	}
	
	
	//
	
	
	void Mesh_freenect_opencl::keyPressed(ofKeyEventArgs &args)
	{		
		super::keyPressed(args);
		
		if (pressed[key_change_depth_xoff])
		{
			kernel_vertex_update->setArg(7, depth_xoff);
		}
	}
	
};