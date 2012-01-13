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

#include "cmlMesh.h"

namespace cml 
{
	Mesh::Mesh()
	{
		_render_enabled = true;
	}
	
	Mesh::~Mesh()
	{
		vbo.clear();
		
		delete[] ibo;
		ibo = NULL;
		
		delete[] vbo_texcoords;
		vbo_texcoords = NULL;
		
		delete[] vbo_color;
		vbo_color = NULL;
		
		xml_config = NULL;
		calib = NULL;
	}
	
	void Mesh::init(ofxXmlSettings *xml_config,
					cml::Calibration *calib,
					int tex_width, int tex_height)
	{
		this->xml_config = xml_config;
		this->calib = calib;
		
		this->tex_width = tex_width;
		this->tex_height = tex_height;
		
		init_keys();
		
		mesh_step = xml_config->getValue("mesh_step", 2);
		
		mesh_w = (float)calib->depth_width/mesh_step;
		mesh_h = (float)calib->depth_height/mesh_step;
		
		vbo_length = mesh_w * mesh_h; 
		ibo_length = vbo_length * 4;
		
		ibo = new uint[ibo_length];
		for (int i = 0; i < vbo_length; i++) 
		{
			int mcol = i % mesh_w;
			int mrow = (i - mcol) / mesh_w;
			
			if ( ( mcol < mesh_w - mesh_step ) && ( mrow < mesh_h - mesh_step ) ) 
			{
				int ibo_idx = i * 4;
				
				ibo[ibo_idx+0] = (uint)( mrow * mesh_w + mcol );
				ibo[ibo_idx+1] = (uint)( (mrow + mesh_step) * mesh_w + mcol );
				ibo[ibo_idx+2] = (uint)( (mrow + mesh_step) * mesh_w + (mcol + mesh_step) );
				ibo[ibo_idx+3] = (uint)( mrow * mesh_w + (mcol + mesh_step) );
			}
		}
		
		vbo_texcoords = new ofVec2f[vbo_length];
		for (int i = 0; i < vbo_length; i++) 
		{
			int mcol = i % mesh_w;
			int mrow = (i - mcol) / mesh_w;
			
			float t = ( (float)mcol / mesh_w ) * tex_width;
			float u = ( (float)mrow / mesh_h ) * tex_height;
			
			vbo_texcoords[i] = ofVec2f(t, u);
		}
		
		vbo_color = new ofFloatColor[vbo_length];
		for (int i = 0; i < vbo_length; i++) 
		{
			vbo_color[i] = ofFloatColor();
		}
		
		init_pts();
		
		if (is_render_enabled())
		{
			//replace by ofMesh.add %Vertices %Normals %Indices %Colors %TexCoords
			vbo.setVertexData( pts0x(), 3, vbo_length, GL_DYNAMIC_DRAW, sizeof_pts() );
			vbo.setNormalData( normals0x(), vbo_length, GL_DYNAMIC_DRAW, sizeof_normals() );
			vbo.setIndexData(ibo, ibo_length, GL_STATIC_DRAW);
			vbo.setColorData(vbo_color, vbo_length, GL_STATIC_DRAW);
			vbo.setTexCoordData(vbo_texcoords, vbo_length, GL_STATIC_DRAW);
		}
		
		init_end();
	}
	
	void Mesh::init_end()
	{
		/// override me
	}

	void Mesh::update()
	{
		update_pts();
		vbo.updateVertexData(pts0x(), vbo_length);
		vbo.updateNormalData(normals0x(), vbo_length);
	}

	void Mesh::render()
	{
		//@see ofVbo.draw()
		vbo.bind();
		//replace by ofMesh.draw()
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.getIndexId());
		glDrawElements(GL_QUADS, ibo_length, GL_UNSIGNED_INT, NULL);
		vbo.unbind();		
	}
	
	int Mesh::get_raw_depth_idx(int vbo_idx, int& x, int& y)
	{
		int col = vbo_idx % mesh_w;
		int row = (vbo_idx - col) / mesh_w;
		
		x = col * mesh_step;
		y = row * mesh_step;
		
		return y * calib->depth_width + x;
	}
	
	int Mesh::get_raw_depth_idx(int vbo_idx)
	{
		if (mesh_step == 1)
			return vbo_idx;
		int x, y;
		return get_raw_depth_idx(vbo_idx, x, y);
	}
	
	void Mesh::enable_render(bool val)
	{
		_render_enabled = val;
	}
	
	bool Mesh::is_render_enabled()
	{
		return _render_enabled;
	}
	
	// ui
	
	string Mesh::get_keyboard_help()
	{
		return "";
	}
	
	void Mesh::init_keys()
	{
		for (int i = 0; i < 512; i++) 
			pressed[i] = false;
	}
	
	void Mesh::keyPressed(ofKeyEventArgs &args)
	{
		pressed[args.key] = true;
	}
	
	void Mesh::keyReleased(ofKeyEventArgs &args)
	{
		pressed[args.key] = false;
	}
};