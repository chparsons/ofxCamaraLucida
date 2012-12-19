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

#include "cmlMesh_opencl.h"

namespace cml 
{
	Mesh_opencl::Mesh_opencl(uint16_t *raw_depth_pix,
							 MSA::OpenCL *opencl)
	: Mesh(raw_depth_pix) 
	{
		this->opencl = opencl;
	};
	
	Mesh_opencl::~Mesh_opencl()
	{
		opencl = NULL;
	};
	
	void Mesh_opencl::init(cml::MeshData *meshdata,
						   cml::Calibration *calib,
						   ofxXmlSettings *xml)
	{
		Mesh::init(meshdata, calib, xml);
		init_cl();
	}
	
	void Mesh_opencl::init_pts()
	{
		pts3d = new float4[vbo_length];
		normals = new float4[vbo_length];
		
		memset( pts3d, 0, vbo_length*sizeof(float4) );	
		memset( normals, 0, vbo_length*sizeof(float4) );
		
		//_pts3d_const = new float4[vbo_length];
		_normals_calc = new float4[vbo_length];

		//memcpy( _pts3d_const, pts3d, vbo_length*sizeof(float4) );
		memset( _normals_calc, 0, vbo_length*sizeof(float4) );
	}
		
	void Mesh_opencl::dispose_pts()
	{
		delete[] (float4*)pts3d;
		pts3d = NULL;
		
		delete[] (float4*)normals;
		normals = NULL;
		
		//delete[] _pts3d_const;
		//_pts3d_const = NULL;
		
		delete[] _normals_calc;
		_normals_calc = NULL;
	}

	void Mesh_opencl::update_pts()
	{
		update_cl();
	}
		
	
	/// dynamic buffers

	
	float* Mesh_opencl::pts0x()
	{
		return &((float4*)pts3d)[0].x;
	}

	int Mesh_opencl::sizeof_pts()
	{
		return sizeof(float4);
	}
	
	float* Mesh_opencl::normals0x()
	{
		return &((float4*)normals)[0].x;
	}
	
	int Mesh_opencl::sizeof_normals()
	{
		return sizeof(float4);
	}
	
	
	/// opencl


	void Mesh_opencl::init_cl()
	{
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL init");
		
		opencl->loadProgramFromFile( xml->getValue("files:cl_vertex_kernel", "") );
		kernel_vertex_update = opencl->loadKernel("update");
		
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL init buffers...");
		
		cl_buff_pts3d.initFromGLObject( vbo.getVertId() );
		//cl_buff_pts3d.initBuffer(sizeof(float4) * vbo_length, CL_MEM_READ_WRITE, pts3d);
		//cl_buff_normals.initFromGLObject( vbo.getNormalId() );
		cl_buff_normals.initBuffer(sizeof(float4) * vbo_length, CL_MEM_READ_WRITE, _normals_calc);

		//cl_buff_pts3d_const.initBuffer(sizeof(float4) * vbo_length, CL_MEM_READ_ONLY, _pts3d_const);
		cl_buff_raw_depth.initBuffer(sizeof(uint16_t) * calib->depth_width * calib->depth_height, CL_MEM_READ_ONLY, raw_depth_pix);
		cl_buff_zlut.initBuffer(sizeof(float) * get_raw_depth_size(), CL_MEM_READ_ONLY, _zlut);
		
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL set args");
		
		float4 depth_intrinsics(calib->cx_d, calib->cy_d,
								calib->fx_d, calib->fy_d);
		int mesh_step = get_step();
		
		kernel_vertex_update->setArg(0, cl_buff_pts3d.getCLMem() );
		kernel_vertex_update->setArg(1, cl_buff_normals.getCLMem() );
		kernel_vertex_update->setArg(2, cl_buff_raw_depth.getCLMem() );
		//kernel_vertex_update->setArg(3, cl_buff_pts3d_const.getCLMem() );
		kernel_vertex_update->setArg(3, cl_buff_zlut.getCLMem() );
		kernel_vertex_update->setArg(4, mesh_step );
		kernel_vertex_update->setArg(5, depth_xoff );
		kernel_vertex_update->setArg(6, depth_intrinsics );
		
		cl_buff_zlut.write(_zlut, 0, sizeof(float) * get_raw_depth_size());
	}

	void Mesh_opencl::update_cl()
	{
		cl_buff_raw_depth.write(raw_depth_pix, 0, sizeof(uint16_t)*calib->depth_width*calib->depth_height);
		//cl_buff_pts3d_const.write(_pts3d_const, 0, sizeof(float4) * vbo_length);
		
		kernel_vertex_update->run1D(vbo_length);
		
		//cl_buff_pts3d.read(pts3d, 0, sizeof(float4)*vbo_length);
		//cl_buff_normals.read(_normals_calc, 0, sizeof(float4)*vbo_length);
		
		//memcpy( _pts3d_const, pts3d, vbo_length*sizeof(float4) );
	}
	
	
	//
	
	
	void Mesh_opencl::keyPressed(ofKeyEventArgs &args)
	{		
		Mesh::keyPressed(args);
		
		if (args.key == key_depth_xoff_inc || 
			args.key == key_depth_xoff_dec)
		{
			//kernel_vertex_update->setArg(xxx, depth_xoff);
		}
	}
	
	void Mesh_opencl::print()
	{
		cout << "### cml::Mesh_opencl print" << endl;
		
		for (int i = 0; i < vbo_length; i++)
		{
			int x_depth, y_depth;
			int depth_idx = get_raw_depth_idx(i, x_depth, y_depth);
			uint16_t raw_depth = raw_depth_pix[depth_idx];
			
//			float z = _zlut[raw_depth];
//			float x, y;
//			project_depth( x_depth, y_depth, z, x, y );

			float4 *v = &((float4*)pts3d)[i];
			float x = v->x;
			float y = v->y;
			float z = v->z;			
			float4 n = _normals_calc[i];
			
			cout << "### \t 2d " << x_depth << ", " << y_depth << ", depth " << raw_depth << "\t 3d: " << x << ", " << y << ", " << z << ", normal: " << n.x << ", " << n.y << ", " << n.z << ", " << n.w << endl;
			//cout << "### \t 2d " << x_depth << ", " << y_depth << ", depth " << raw_depth << "\t 3d: " << x << ", " << y << ", " << z << endl;
		}
	}
	
};
