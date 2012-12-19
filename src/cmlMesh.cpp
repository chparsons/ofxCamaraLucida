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

#include "cmlMesh.h"

namespace cml 
{
	Mesh::Mesh(uint16_t *raw_depth_pix)
	{
		this->raw_depth_pix = raw_depth_pix;
		
		_coord_sys = ofVec3f(-1., -1., 1.);
	}
	
	Mesh::~Mesh()
	{
		vbo.clear();
		
		delete[] _zlut;
		_zlut = NULL;
		
		delete[] ibo;
		ibo = NULL;
		
		delete[] vbo_texcoords;
		vbo_texcoords = NULL;
		
		delete[] vbo_color;
		vbo_color = NULL;
		
		if (!_normals_shader_enabled)
		{
			delete[] normals_cpu_pix;
			normals_cpu_tex.clear();
		}
		
		delete[] hue_pix;
		delete[] hue_lut;
		hue_tex.clear();
		
		meshdata = NULL;
		calib = NULL;
		xml = NULL;
		raw_depth_pix = NULL;
	}
	
	void Mesh::init(cml::MeshData *meshdata, cml::Calibration *calib, ofxXmlSettings *xml)
	{
		this->meshdata = meshdata;
		this->calib = calib;
		this->xml = xml;
		
		mesh_length = get_width() * get_height(); 
		ibo_length = mesh_length * 4;
		
		init_keys();
		init_data();
		init_zlut();
		init_hue_tex();
		
		ibo = new uint[ibo_length];
		for (int i = 0; i < mesh_length; i++) 
		{
			int x_mesh, y_mesh;
			get_mesh_coord(i, x_mesh, y_mesh);
			
			if ( ( x_mesh < get_width() - get_step() ) && 
                    ( y_mesh < get_height() - get_step() ) ) 
			{
				int ibo_idx = i * 4;
				
				ibo[ibo_idx+0] = (uint)( y_mesh * get_width() + x_mesh );
				ibo[ibo_idx+1] = (uint)( (y_mesh + get_step()) * get_width() + x_mesh );
				ibo[ibo_idx+2] = (uint)( (y_mesh + get_step()) * get_width() + (x_mesh + get_step()) );
				ibo[ibo_idx+3] = (uint)( y_mesh * get_width() + (x_mesh + get_step()) );
			}
		}
		
		vbo_texcoords = new ofVec2f[mesh_length];
		for (int i = 0; i < mesh_length; i++) 
		{
			int x_mesh, y_mesh;
			get_mesh_coord(i, x_mesh, y_mesh);
			
			float t = ( (float)x_mesh / get_width() ) * get_tex_width();
			float u = ( (float)y_mesh / get_height() ) * get_tex_height();
			
			vbo_texcoords[i] = ofVec2f(t, u);
		}
		
		vbo_color = new ofFloatColor[mesh_length];
		for (int i = 0; i < mesh_length; i++) 
		{
			vbo_color[i] = ofFloatColor(1,1,1,1);
		}
		
		init_pts();
		
		vbo.setVertexData( pts0x(), 3, mesh_length, 
                GL_DYNAMIC_DRAW, sizeof_pts() );
		vbo.setNormalData( normals0x(), mesh_length, 
                GL_DYNAMIC_DRAW, sizeof_normals() );
		vbo.setIndexData( ibo, ibo_length, GL_STATIC_DRAW );
		vbo.setColorData( vbo_color, mesh_length, GL_STATIC_DRAW );
		vbo.setTexCoordData( vbo_texcoords, mesh_length, GL_STATIC_DRAW );
			
		init_normals();
	}
	
	void Mesh::update()
	{
		update_pts();
		
		vbo.updateVertexData( pts0x(), mesh_length );
		
		if (_normals_shader_enabled)
			update_normals_shader();
		else 
			update_normals_cpu();
		
		vbo.updateNormalData( normals0x(), mesh_length );
	}

	void Mesh::render()
	{
		//ofMesh and ofVboMesh don't support GL_QUADS...
		vbo.drawElements(GL_QUADS, ibo_length);
	}
	
	
	/// mesh process
	
	
	ofVec3f Mesh::coord_sys()
	{
		return _coord_sys;
	}
	
	void Mesh::init_pts()
	{
		pts3d = new ofVec3f[mesh_length];
		normals = new ofVec3f[mesh_length];
		
		memset( pts3d, 0, mesh_length*sizeof(ofVec3f) );
		memset( normals, 0, mesh_length*sizeof(ofVec3f) );
	}
	
	void Mesh::dispose_pts()
	{
		delete[] (ofVec3f*)pts3d;
		pts3d = NULL;
		
		delete[] (ofVec3f*)normals;
		normals = NULL;
	}
	
	void Mesh::update_pts()
	{
		for (int i = 0; i < mesh_length; i++)
		{
			int x_depth, y_depth;
			int depth_idx = to_depth_idx(i, x_depth, y_depth);
			uint16_t raw_depth = raw_depth_pix[depth_idx];
			
			float z = _zlut[raw_depth];
			float x, y;
			unproject_depth(x_depth, y_depth, z, x, y);
			
			((ofVec3f*)pts3d)[i].x = x;
			((ofVec3f*)pts3d)[i].y = y;
			((ofVec3f*)pts3d)[i].z = z;
		}
	}
	
	
	/// dynamic buffers
	
	
	float* Mesh::pts0x()
	{
		return &((ofVec3f*)pts3d)[0].x;
	}
	
	int Mesh::sizeof_pts()
	{
		return sizeof(ofVec3f);
	}
	
	float* Mesh::normals0x()
	{
		return &((ofVec3f*)normals)[0].x;
	}
	
	int Mesh::sizeof_normals()
	{
		return sizeof(ofVec3f);
	}
	
	
	/// normals
	
	
	void Mesh::init_normals()
	{
		_normals_shader_enabled = normals_shader.load( xml->getValue("files:normals_shader", "") );
		
		int w, h;
		
		if (!_normals_shader_enabled)
		{
			w = get_depth_width();
			h = get_depth_height();
			normals_cpu_pix = new uint8_t[ w * h * 3 ];
			return;
		}
		
		w = get_width();
		h = get_height();
		
		ofFbo::Settings s;
		s.width			    = w;
		s.height	    	= h;
		s.numSamples		= 1;
		s.numColorbuffers	= 1;
		s.internalformat	= GL_RGBA32F;
		
		normals_fbo.allocate(s);
		
		//int ntexlen = w * h * 3;
		//float *ntex = new float[ntexlen];
		//memset( ntex, 0, ntexlen * sizeof(float) );
		//get_normals_tex_ref().loadData(ntex, w, h, GL_RGB);
		//delete[] ntex;
		
		normals_fpix.allocate(w, h, 4);
	}
	
	void Mesh::update_normals_shader()
	{
		normals_fbo.bind();
		normals_shader.begin();
		
		render();
		
		normals_shader.end();
		normals_fbo.unbind();

		/// update normals buffer from gpu tex
		//normals_fbo.readToPixels(normals_fpix, 0);
		//for (int i = 0; i < mesh_length; i++)
		//{
			//int x_mesh, y_mesh;
			//get_mesh_coord(i, x_mesh, y_mesh);
			//ofFloatColor n = normals_fpix.getColor(
					//x_mesh, y_mesh);
			//((ofVec3f*)normals)[i] = ofVec3f(n.r, n.g, n.b);
		//}
	}
	
	void Mesh::update_normals_cpu()
	{
		for (int i = 0; i < mesh_length; i++)
		{
			// a
			ofVec3f a = ((ofVec3f*)pts3d)[i];
			
			int x_mesh, y_mesh;
			get_mesh_coord(i, x_mesh, y_mesh);
			
			// b
			int bx_mesh = CLAMP(x_mesh - 1, 0, get_width()-1);
			int by_mesh = y_mesh;
			int bi = get_mesh_idx( bx_mesh, by_mesh );
			ofVec3f b = ((ofVec3f*)pts3d)[ bi ];
			
			// c
			int cx_mesh = x_mesh;
			int cy_mesh = CLAMP(y_mesh - 1, 0, get_height()-1);
			int ci = get_mesh_idx( cx_mesh, cy_mesh );
			ofVec3f c = ((ofVec3f*)pts3d)[ ci ];
			
			// n calc:
			
			((ofVec3f*)normals)[i] = ((c - a).cross(b - a)).normalize();
		}
	}
	
	ofTexture& Mesh::get_normals_tex_ref()
	{
		if (_normals_shader_enabled)
		{
			return normals_fbo.getTextureReference(0);
		}
		
		int w = get_depth_width();
		int h = get_depth_height();
		
		if ( ! normals_cpu_tex.isAllocated() )
		{
			normals_cpu_tex.allocate(w, h, GL_RGB);
		}
		int dlen = w * h;
		for (int i = 0; i < dlen; i++)
		{
			int mesh_idx = to_mesh_idx(i);
			
			ofVec3f n = ((ofVec3f*)normals)[ mesh_idx ];
				
			normals_cpu_pix[i * 3 + 0] = (n.x+1)*.5 * 255.;
			normals_cpu_pix[i * 3 + 1] = (n.y+1)*.5 * 255.;
			normals_cpu_pix[i * 3 + 2] = (n.z+1)*.5 * 255.;
		}
		normals_cpu_tex.loadData(normals_cpu_pix, w, h, GL_RGB);
			
		return normals_cpu_tex;
	}
	
	void Mesh::render_normals()
	{
		if (_normals_shader_enabled)
			return;
		
		float ns = _normals_shader_enabled ? 0.001 : 0.01;
		// performance leak, just for debugging..
		glColor3f(1, 1, 0);
		for (int i = 0; i < mesh_length; i++)
		{
			ofVec3f p3d = ((ofVec3f*)pts3d)[i];
			ofVec3f n = ((ofVec3f*)normals)[i] * ns;
			glBegin(GL_LINES);
			glVertex3f(p3d.x, p3d.y, p3d.z);
			glVertex3f(p3d.x+n.x, p3d.y+n.y, p3d.z+n.z);
			glEnd();
		}
		glColor3f(1, 1, 1);
	}
	
	void Mesh::print_normals()
	{
		cout << "cmlMesh normals" << endl;
		
		if (!_normals_shader_enabled)
		{
			for (int i = 0; i < mesh_length; i++)
			{
				ofVec3f n = ((ofVec3f*)normals)[i];
				
				int x_mesh, y_mesh;
				get_mesh_coord(i, x_mesh, y_mesh);
				
				int x_depth, y_depth;
				to_depth_coord(x_mesh, y_mesh, x_depth, y_depth);
				
				cout << "\t @ idx[" << i << "] pix[" << x_depth << ", " << y_depth << "] = " << n.x << ", " << n.y << ", " << n.z << endl;
			}
			return;
		}
		
		normals_fbo.readToPixels(normals_fpix, 0);
		for (int i = 0; i < mesh_length; i++)
		{
			int x_mesh, y_mesh;
			get_mesh_coord(i, x_mesh, y_mesh);
			
			int x_depth, y_depth;
			to_depth_coord(x_mesh, y_mesh, x_depth, y_depth);
			
			//ofFloatColor n = normals_fpix.getColor(x_depth, y_depth);
			ofFloatColor n = normals_fpix.getColor(x_mesh, y_mesh);
			int pix_idx = normals_fpix.getPixelIndex(x_mesh, y_mesh);
			
			//cout << "\t @ idx[" << i << "]";
			//cout << " pix_idx[" << pix_idx << "]";
			//cout << endl;
			cout << "\t mesh[" << x_mesh << ", " << y_mesh << "]";
			//cout << " depth[" << x_depth << ", " << y_depth << "]"; 
			//cout << endl;
			cout << " normal[" << n.r << ", " << n.g << ", " << n.b << "]";
			cout << endl;
		}
	}	
	
	
	/// utils
	
	
	void Mesh::init_data()
	{
		_raw_depth_size = 2048; //11bits
		_base_depth_raw = 1024.; //5mts~
		_base_depth_mts = k1 * tanf(( (float)_base_depth_raw / k2) + k3) - k4;
		
		depth_xoff = xml->getValue("depth_xoff", -8); 
		
		key_depth_xoff_inc = 
            xml->getValue("debug_keys:depth_xoff:inc", "b")[0]; 
		key_depth_xoff_dec = 
            xml->getValue("debug_keys:depth_xoff:dec", "n")[0]; 
	}
	
	void Mesh::init_zlut()
	{		
		_zlut = new float[_raw_depth_size];
		for (int i = 0; i < _raw_depth_size; i++) 
		{
			_zlut[i] = raw_depth_to_meters(i);
		}
	}
	
	//http://openkinect.org/wiki/Imaging_Information
	//http://nicolas.burrus.name/index.php/Research/KinectCalibration
	
	float Mesh::raw_depth_to_meters(uint16_t raw_depth)
	{
        //TODO clamp raw_depth and remove _base_depth_mts
		if (raw_depth < _base_depth_raw)
		{
			return k1 * tanf(( (float)raw_depth / k2) + k3) - k4; // calculate in meters
		}
		return _base_depth_mts;
	}
	
	void Mesh::unproject_depth(int x_depth, int y_depth, 
							 float z, float& x, float& y)
	{
		x = (x_depth + depth_xoff - calib->cx_d) * z / calib->fx_d;
		y = (y_depth - calib->cy_d) * z / calib->fy_d;
	}
	
	///
	
	float Mesh::z_mts(uint16_t raw_depth)
	{
		return _zlut[raw_depth];
	}
	
	float Mesh::z_mts(int x_depth, int y_depth)
	{
		int depth_idx = get_depth_idx(x_depth, y_depth);
		uint16_t raw_depth = raw_depth_pix[depth_idx];
		return _zlut[raw_depth];
	}
	
	ofVec3f Mesh::raw_depth_to_p3d(int x_depth, int y_depth)
	{
		float x,y,z;
		z = z_mts(x_depth, y_depth);
		unproject_depth( x_depth, y_depth, z, x, y );
		return ofVec3f(x,y,z);
	}
	
	ofVec3f Mesh::raw_depth_to_p3d(uint16_t raw_depth, int x_depth, int y_depth)
	{
		float x,y,z;
		z = _zlut[raw_depth];
		unproject_depth( x_depth, y_depth, z, x, y );
		return ofVec3f(x,y,z);
	}
	
	ofVec2f Mesh::p3d_to_depth(const ofVec3f& p3d)
	{
		ofVec2f d2d;
		d2d.x = (p3d.x * calib->fx_d / p3d.z) + calib->cx_d - depth_xoff;
		d2d.y = (p3d.y * calib->fy_d / p3d.z) + calib->cy_d;
		return d2d;
	}
	
	ofVec2f Mesh::raw_depth_to_rgb(uint16_t raw_depth, int x_depth, int y_depth)
	{
		//	TODO
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
	
	
	// conversion utils 
	
	// depth
	
	void Mesh::get_depth_coord(int depth_idx, int& x_depth, int& y_depth)
	{
		x_depth = depth_idx % get_depth_width();
		y_depth = (depth_idx - x_depth) / get_depth_width();
	}
	
	int Mesh::get_depth_idx(int x_depth, int y_depth)
	{
		return y_depth * get_depth_width() + x_depth;
	}
	
	void Mesh::to_depth_coord(int x_mesh, int y_mesh, 
							  int& x_depth, int& y_depth)
	{
		x_depth = x_mesh * get_step();
		y_depth = y_mesh * get_step();
	}
	
	int Mesh::to_depth_idx(int mesh_idx, int& x_depth, int& y_depth)
	{
		int x_mesh, y_mesh;
		get_mesh_coord(mesh_idx, x_mesh, y_mesh);

		to_depth_coord(x_mesh, y_mesh, x_depth, y_depth);
		
		return get_depth_idx(x_depth, y_depth);
	}
	
	// mesh
	
	void Mesh::get_mesh_coord(int mesh_idx, int& x_mesh, int& y_mesh)
	{
		x_mesh = mesh_idx % get_width();
		y_mesh = (mesh_idx - x_mesh) / get_width();
	}
	
	int Mesh::get_mesh_idx(int x_mesh, int y_mesh)
	{
		return y_mesh * get_width() + x_mesh;
	}
	
	void Mesh::to_mesh_coord(int x_depth, int y_depth, 
							 int& x_mesh, int& y_mesh)
	{
		x_mesh = (int)(x_depth / get_step());
		y_mesh = (int)(y_depth / get_step());
	}
	
	int Mesh::to_mesh_idx(int depth_idx)
	{
		if (get_step() == 1)
			return depth_idx;
		
		int x_depth, y_depth;
		get_depth_coord(depth_idx, x_depth, y_depth);
		
		int x_mesh, y_mesh;
		to_mesh_coord(x_depth, y_depth, x_mesh, y_mesh);
		
		return get_mesh_idx(x_mesh, y_mesh);
	}
		
	
	/// getters
	
	
	uint16_t Mesh::get_raw_depth_size()
	{
		return _raw_depth_size;
	}
	
	uint16_t Mesh::get_base_depth_raw()
	{
		return _base_depth_raw;
	}
	
	float Mesh::get_base_depth_mts()
	{
		return _base_depth_mts;
	}
	
	int Mesh::get_step()
	{
		return meshdata->step;
	}
	
	int Mesh::get_width()
	{
		return meshdata->width;
	}
	
	int Mesh::get_height()
	{
		return meshdata->height;
	}
	
	int Mesh::get_tex_width()
	{
		return meshdata->tex_width;
	}
	
	int Mesh::get_tex_height()
	{
		return meshdata->tex_height;
	}
	
	int Mesh::get_depth_width()
	{
		return calib->depth_width;
	}
	
	int Mesh::get_depth_height()
	{
		return calib->depth_height;
	}
		
	
	/// hue tex
	
	
	void Mesh::init_hue_tex()
	{
		hue_pix = new uint8_t[ get_depth_width() * get_depth_height() * 3 ];
		hue_lut = new ofFloatColor[_raw_depth_size];
		set_hue_lut();
	}
	
	ofTexture& Mesh::get_hue_tex_ref()
	{
		int w = get_depth_width();
		int h = get_depth_height();
		
		if ( ! hue_tex.isAllocated() )
		{
			hue_tex.allocate(w, h, GL_RGB);
		}
		int dlen = w * h;
		for (int i = 0; i < dlen; i++)
		{
			ushort raw_depth = raw_depth_pix[i];
			ofFloatColor hue = hue_lut[raw_depth];
			hue_pix[i * 3 + 0] = hue.r;
			hue_pix[i * 3 + 1] = hue.g;
			hue_pix[i * 3 + 2] = hue.b;
		}
		hue_tex.loadData(hue_pix, w, h, GL_RGB);
		return hue_tex;
	}
	
	void Mesh::set_hue_lut(float depth_near, float depth_far,
						   float hue_near, float hue_far, bool clamp)
	{
		for (int i = 0; i < _raw_depth_size; i++) 
		{
			if (_zlut[i] < _base_depth_mts)
			{
				float hue = ofMap(_zlut[i], depth_near, depth_far, 
								  hue_near, hue_far, clamp);
				
				hue_lut[i] = ofFloatColor::fromHsb(hue * 255., 
												   255., 255., 255.);
			}
			else
			{
				hue_lut[i] = ofFloatColor(0, 0, 0);
			}
		}
	}
	
	
	/// ui
	
	
	void Mesh::print()
	{
		cout << "### cml::Mesh print /// mesh_length " << mesh_length << endl;
		
		//void *normptr = new ofVec3f[mesh_length];
		//&((ofVec3f*)normals)[0].x;
		//cout << "GL_NORMAL_ARRAY: " << (int)glIsEnabled(GL_NORMAL_ARRAY) << ", " << (int)normptr << endl;

		print_normals();
		
//		for (int i = 0; i < mesh_length; i++)
//		{
//			int x_depth, y_depth;
//			int depth_idx = to_depth_idx(i, x_depth, y_depth);
//			uint16_t raw_depth = raw_depth_pix[depth_idx];
//			
//			float z = _zlut[raw_depth];
//			float x, y;
//			unproject_depth( x_depth, y_depth, z, x, y );
//			
//			//ofVec3f n = ((ofVec3f*)normptr)[i];
//			//cout << "### \t 2d " << x_depth << ", " << y_depth << ", depth " << raw_depth << "\t normal " << n.x << ", " << n.y << ", " << n.z << "\t 3d: " << x << ", " << y << ", " << z << endl;
//			cout << "### \t 2d " << x_depth << ", " << y_depth << ", depth " << raw_depth << "\t 3d: " << x << ", " << y << ", " << z << endl;
//		}
	}
	
	string Mesh::get_keyboard_help()
	{
		return "\ndebug mode @ mesh: \n "+string(1, key_depth_xoff_inc)+" increment depth_xoff \n "+string(1, key_depth_xoff_dec)+" decrement depth_xoff";
	}
	
	void Mesh::init_keys()
	{
		for (int i = 0; i < 512; i++) 
			pressed[i] = false;
	}
	
	void Mesh::keyReleased(ofKeyEventArgs &args)
	{
		pressed[args.key] = false;
	}
	
	void Mesh::keyPressed(ofKeyEventArgs &args)
	{		
		pressed[args.key] = true;
		
		if (args.key == key_depth_xoff_inc)
		{
			depth_xoff++;
			ofLog(OF_LOG_VERBOSE, "cml::MeshWorker.depth_xoff = "+ofToString(depth_xoff));
		}
		else if (args.key == key_depth_xoff_dec)
		{
			depth_xoff--;
			ofLog(OF_LOG_VERBOSE, "cml::MeshWorker.depth_xoff = "+ofToString(depth_xoff));
		}
	}	
	
};
