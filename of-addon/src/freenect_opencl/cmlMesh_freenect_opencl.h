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


//	reserved user input on debug mode (toggle_debug)
//		'c' + key up/down: change depht_xoffset


#pragma once

#include "cmlMesh.h"
#include "MSAOpenCL.h"

namespace cml 
{
	class Mesh_freenect_opencl : public Mesh
	{
	public:
		
		Mesh_freenect_opencl(uint16_t *raw_depth_pix,
							  MSA::OpenCL *opencl = NULL);
		~Mesh_freenect_opencl();
		
		ofVec3f coord_sys();
		
		void print();
		
	protected:
		
		// implementation stuff...
		
		typedef Mesh super;
		
		void init_pts();
		void dispose_pts();
		void update_pts();
		
		float* pts0x();
		int sizeof_pts();
		
		float4* pts3d;
		
		//
		
	private:
		
		uint16_t *raw_depth_pix;
		
		static const char key_change_depth_xoff = 'c'; // + key up/down
		
		static const float k1 = 0.1236;
		static const float k2 = 2842.5;
		static const float k3 = 1.1863;
		static const float k4 = 0.0370;
		static const uint16_t raw_depth_size = 2048; //11bits
		
		int depth_xoff;
		
		uint16_t _base_depth_raw;
		float _base_depth_mts;
		float _zlut[raw_depth_size];
		
		//
		
		void init_zlut();
		
		float z_mts(uint16_t raw_depth);
		float z_mts(int x, int y, bool tex_coords = false);
		
		float raw_depth_to_meters(uint16_t raw_depth);
		
		ofVec3f raw_depth_to_p3d(uint16_t raw_depth, int x_depth, int y_depth);
		ofVec3f raw_depth_to_p3d(int x_depth, int y_depth);
		ofVec2f p3d_to_depth(const ofVec3f& p3d);
		ofVec2f raw_depth_to_rgb(uint16_t raw_depth, int x_depth, int y_depth);
		
		uint16_t get_base_depth_raw();
		float get_base_depth_mts();
		
		
		// open cl
		
		void init_cl();
		void update_cl();
		
		bool using_opencl;
		
		MSA::OpenCL			*opencl;
		MSA::OpenCLKernel	*kernel_vertex_update;
		MSA::OpenCLBuffer	cl_buff_pts3d;
		MSA::OpenCLBuffer	cl_buff_raw_depth;
		
		
		// ui
		
		void keyPressed(ofKeyEventArgs &args);
	};
};