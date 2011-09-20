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
		using_opencl = opencl != NULL;
	};
	
	Mesh_freenect_opencl::~Mesh_freenect_opencl()
	{
		dispose_pts();
	};
	
	ofVec3f Mesh_freenect_opencl::coord_sys()
	{
		return ofVec3f(-1., -1., 1.);	
	}
	
	void Mesh_freenect_opencl::init_pts()
	{
		_base_depth_raw = 1024.; //5mts~
		_base_depth_mts = k1 * tanf((_base_depth_raw / k2) + k3) - k4;
		
		depth_xoff = xml_config->getValue("depth_xoff", -7); 
		
		init_zlut();
		
		pts3d = new float4[vbo_length];
		for (int i = 0; i < vbo_length; i++) 
		{
			pts3d[i] = float4(0, 0, 0);
		}
		
		if (using_opencl)
		{
			init_cl();
		}
	}

	void Mesh_freenect_opencl::dispose_pts()
	{
		delete[] pts3d;
		pts3d = NULL;
		
		raw_depth_pix = NULL;
		opencl = NULL;
	}

	void Mesh_freenect_opencl::update_pts()
	{
		if (using_opencl)
		{
			update_cl();
		}
		else 
		{
			for (int i = 0; i < vbo_length; i++)
			{
				int mcol = i % mesh_w;
				int mrow = (i - mcol) / mesh_w;
				
				int col = mcol * mesh_step;
				int row = mrow * mesh_step;
				
				int depth_idx = row * calib->depth_width + col;
				
				ushort raw_depth = raw_depth_pix[depth_idx];
				
				float z = _zlut[raw_depth];
				float x = (col + depth_xoff - calib->cx_d) * z / calib->fx_d;
				float y = (row - calib->cy_d) * z / calib->fy_d;
				
				pts3d[i].x = x;
				pts3d[i].y = y;
				pts3d[i].z = z;
			}
		}
	}

	float* Mesh_freenect_opencl::pts0x()
	{
		return &pts3d[0].x;
	}

	int Mesh_freenect_opencl::sizeof_pts()
	{
		return sizeof(float4);
	}
	
	void Mesh_freenect_opencl::print()
	{
		cout << "### Mesh_freenect_opencl ConvertProjectiveToRealWorld" << endl;
		
		for (int i = 0; i < vbo_length; i++)
		{
			int mcol = i % mesh_w;
			int mrow = (i - mcol) / mesh_w;
			
			int col = mcol * mesh_step;
			int row = mrow * mesh_step;
			
			int depth_idx = row * calib->depth_width + col;
			
			ushort raw_depth = raw_depth_pix[depth_idx];
			
			float z = _zlut[raw_depth];
			float x = (col + depth_xoff - calib->cx_d) * z / calib->fx_d;
			float y = (row - calib->cy_d) * z / calib->fy_d;
			
			cout << "### \t 2d " << col << ", " << row << ", depth " << raw_depth << "\t 3d: " << x << ", " << y << ", " << z << endl;
		}
	}

	void Mesh_freenect_opencl::init_zlut()
	{
		for (int i = 0; i < raw_depth_size; i++) 
		{
			_zlut[i] = raw_depth_to_meters(i);
		}
	}

	//http://openkinect.org/wiki/Imaging_Information
	//http://nicolas.burrus.name/index.php/Research/KinectCalibration

	float Mesh_freenect_opencl::raw_depth_to_meters(uint16_t raw_depth)
	{
		if (raw_depth < _base_depth_raw)
		{
			return k1 * tanf((raw_depth / k2) + k3) - k4; // calculate in meters
		}
		return _base_depth_mts;
	}

	uint16_t Mesh_freenect_opencl::get_base_depth_raw()
	{
		return _base_depth_raw;
	}

	float Mesh_freenect_opencl::get_base_depth_mts()
	{
		return _base_depth_mts;
	}

	float Mesh_freenect_opencl::z_mts(uint16_t raw_depth)
	{
		return _zlut[raw_depth];
	}

	float Mesh_freenect_opencl::z_mts(int x, int y, bool tex_coords)
	{
		if (tex_coords)
		{
			x = (int) ( (float)x / tex_width * calib->depth_width );
			y = (int) ( (float)y / tex_height * calib->depth_height );
		}
		
		int depth_idx = y * calib->depth_width + x;
		uint16_t raw_depth = raw_depth_pix[depth_idx];
		
		return _zlut[raw_depth];
	}

	ofVec3f Mesh_freenect_opencl::raw_depth_to_p3d(int x_depth, int y_depth)
	{
		ofVec3f p3d;
		p3d.z = z_mts(x_depth, y_depth);
		p3d.x = (x_depth + depth_xoff - calib->cx_d) * p3d.z / calib->fx_d;
		p3d.y = (y_depth - calib->cy_d) * p3d.z / calib->fy_d;
		return p3d;
	}

	ofVec3f Mesh_freenect_opencl::raw_depth_to_p3d(uint16_t raw_depth, int x_depth, int y_depth)
	{
		ofVec3f p3d;
		p3d.z = _zlut[raw_depth];
		p3d.x = (x_depth + depth_xoff - calib->cx_d) * p3d.z / calib->fx_d;
		p3d.y = (y_depth - calib->cy_d) * p3d.z / calib->fy_d;
		return p3d;
	}

	ofVec2f Mesh_freenect_opencl::p3d_to_depth(const ofVec3f& p3d)
	{
		ofVec2f d2d;
		d2d.x = (p3d.x * calib->fx_d / p3d.z) + calib->cx_d - depth_xoff;
		d2d.y = (p3d.y * calib->fy_d / p3d.z) + calib->cy_d;
		return d2d;
	}

	ofVec2f Mesh_freenect_opencl::raw_depth_to_rgb(uint16_t raw_depth, int x_depth, int y_depth)
	{
		//	P3D' = R.P3D + T
		//	P2D_rgb.x = (P3D'.x * fx_rgb / P3D'.z) + cx_rgb
		//	P2D_rgb.y = (P3D'.y * fy_rgb / P3D'.z) + cy_rgb
		
		ofVec2f rgb2d;
		
		float xoff = -8;
		ofVec3f p3d = raw_depth_to_p3d(raw_depth, x_depth, y_depth);
		
		ofVec3f p3d_rgb = calib->RT_rgb * p3d;
		
		rgb2d.x = (p3d_rgb.x * calib->fx_rgb / p3d_rgb.z) + calib->cx_rgb;
		rgb2d.y = (p3d_rgb.y * calib->fy_rgb / p3d_rgb.z) + calib->cy_rgb;
		
		CLAMP(rgb2d.x, 0, calib->rgb_width-1);
		CLAMP(rgb2d.y, 0, calib->rgb_height-1);
		
		return rgb2d;
	}


	// opencl


	void Mesh_freenect_opencl::init_cl()
	{
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL init");
		
		// decouple opencl setup
		//opencl->setupFromOpenGL();
		
		opencl->loadProgramFromFile("camara_lucida/vertex.cl");
		kernel_vertex_update = opencl->loadKernel("update_vertex");
		
		ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL init buffers... raw_depth_pix "+ofToString(raw_depth_pix));
		
		//	TODO why this doesn't work?
		//	cl_buff_pts3d.initFromGLObject(vbo.getVertId());
		
		cl_buff_pts3d.initBuffer(sizeof(float4) * vbo_length, CL_MEM_READ_WRITE, pts3d);
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
		
		cl_buff_pts3d.read(pts3d, 0, sizeof(float4) * vbo_length);
	}

	
	// ui
	
	
	void Mesh_freenect_opencl::keyPressed(ofKeyEventArgs &args)
	{		
		super::keyPressed(args);
		
		if (pressed[key_change_depth_xoff])
		{
			if (args.key == OF_KEY_UP)
			{
				depth_xoff++;
				if (using_opencl)
				{
					kernel_vertex_update->setArg(7, depth_xoff);
				}
				ofLog(OF_LOG_VERBOSE, "cml::Mesh_freenect_opencl.depth_xoff = "+ofToString(depth_xoff));
			}
			else if (args.key == OF_KEY_DOWN)
			{
				depth_xoff--;
				if (using_opencl)
				{
					kernel_vertex_update->setArg(7, depth_xoff);
				}
				ofLog(OF_LOG_VERBOSE, "cml::Mesh_freenect_opencl.depth_xoff = "+ofToString(depth_xoff));
			}
		}
	}
};