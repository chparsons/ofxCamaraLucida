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

#include "CamaraLucida.h"

void CamaraLucida::init(string kinect_calibration_filename,
						string proj_calibration_filename,
						string config_filename,
						uint16_t *raw_depth_pix, uint8_t *rgb_pix, 
						int tex_width, int tex_height, int tex_num_samples, 
						MSA::OpenCL* opencl)
{
	_inited = true;
	
	config.loadFile(config_filename);
	config.pushTag("camaralucida");
	depth_xoff = config.getValue("depth_xoff", -7); 
	mesh_step = config.getValue("mesh_step", 2);
	near = config.getValue("near", 0.1);
	far = config.getValue("far", 20.0);
	
	load_data(kinect_calibration_filename, proj_calibration_filename);
	
	_base_depth_raw = 1024.; //5mts~
	_base_depth_mts = k1 * tanf((_base_depth_raw / k2) + k3) - k4;
	
	init_zlut();
	
	proj_loc = ofVec3f(	proj_RT[12], proj_RT[13], proj_RT[14] );	
	proj_fwd = ofVec3f(	proj_RT[8], proj_RT[9], proj_RT[10] );
	proj_up = ofVec3f( proj_RT[4], proj_RT[5], proj_RT[6] );
	proj_trg = proj_loc + proj_fwd;
	
	init_keys();
	init_events();
	
	init_gl_scene_control();
	
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	
	init_fbo(tex_width, tex_height, tex_num_samples);
	init_vbo();
	
	using_opencl = opencl != NULL;
	
	if (using_opencl)
	{
		init_cl(raw_depth_pix, opencl);
	}
}

bool CamaraLucida::inited()
{
	if (!_inited)
	{
		if (!_not_init_alert)
		{
			ofLog(OF_LOG_ERROR, "Camara Lucida not inited");
			_not_init_alert = true;
		}
		return false;
	}
	return true;
}

CamaraLucida::~CamaraLucida()
{	
	ofLog(OF_LOG_VERBOSE, "~CamaraLucida");
	
	if (!inited())
		return;
	
	dispose_events();
	dispose_vbo();
	
	cvReleaseMat(&rgb_int);	
	cvReleaseMat(&depth_int);
	
	cvReleaseMat(&drgb_R);
	cvReleaseMat(&drgb_T);
	
	cvReleaseMat(&proj_int);	
	cvReleaseMat(&proj_R);
	cvReleaseMat(&proj_T);
}

void CamaraLucida::update(uint16_t *raw_depth_pix, uint8_t *rgb_pix)
{
	if (!inited())
		return;
	
	update_keys();
	
	if (using_opencl)
	{
		update_cl(raw_depth_pix);
	}
	else 
	{
		update_mesh(raw_depth_pix);	
	}
	
	vbo.updateVertexData(&vbo_3d[0].x, vbo_length);
}

void CamaraLucida::render()
{
	if (!inited())
		return;
	
	fbo.bind();
	//ofEnableAlphaBlending();  
	//glEnable(GL_BLEND);  
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA); 
	
	ofNotifyEvent( render_texture, void_event_args );
	
	fbo.unbind();
	//ofDisableAlphaBlending(); 
	//glDisable(GL_BLEND);  
	
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	
	glColor3f(1, 1, 1);
	glViewport(0, 0, ofGetWidth(), ofGetHeight());
	
	gl_ortho();
	
	ofNotifyEvent( render_hud, void_event_args );
	
	render_screenlog();
	
	gl_projection();	
	gl_viewpoint();
	
	if (_debug)
	{
		gl_scene_control();
		render_world_CS();
		render_proj_CS();
		render_rgb_CS();
	}
	
	render_mesh();
}

void CamaraLucida::toggle_debug()
{
	_debug = !_debug;
	if (!_debug)
	{
		reset_gl_scene_control();
	}
}


// utils


float CamaraLucida::z_mts(uint16_t raw_depth)
{
	return _zlut[raw_depth];
}

float CamaraLucida::z_mts(uint16_t *raw_depth_pix, int x, int y, bool tex_coords)
{
	if (tex_coords)
	{
		x = (int) ( (float)x / fbo.getWidth() * d_width );
		y = (int) ( (float)y / fbo.getHeight() * d_height );
	}
	
	int depth_idx = y * d_width + x;
	uint16_t raw_depth = raw_depth_pix[depth_idx];
	
	return _zlut[raw_depth];
}

ofVec3f CamaraLucida::raw_depth_to_p3d(uint16_t *raw_depth_pix, int x_depth, int y_depth)
{
	ofVec3f p3d;
	p3d.z = z_mts(raw_depth_pix, x_depth, y_depth);
	p3d.x = (x_depth + depth_xoff - cx_d) * p3d.z / fx_d;
	p3d.y = (y_depth - cy_d) * p3d.z / fy_d;
	return p3d;
}

ofVec3f CamaraLucida::raw_depth_to_p3d(uint16_t raw_depth, int x_depth, int y_depth)
{
	ofVec3f p3d;
	p3d.z = _zlut[raw_depth];
	p3d.x = (x_depth + depth_xoff - cx_d) * p3d.z / fx_d;
	p3d.y = (y_depth - cy_d) * p3d.z / fy_d;
	return p3d;
}

ofVec2f CamaraLucida::p3d_to_depth(const ofVec3f& p3d)
{
	ofVec2f d2d;
	d2d.x = (p3d.x * fx_d / p3d.z) + cx_d - depth_xoff;
	d2d.y = (p3d.y * fy_d / p3d.z) + cy_d;
	return d2d;
}
	
ofVec2f CamaraLucida::raw_depth_to_rgb(uint16_t raw_depth, int x_depth, int y_depth)
{
	//	P3D' = R.P3D + T
	//	P2D_rgb.x = (P3D'.x * fx_rgb / P3D'.z) + cx_rgb
	//	P2D_rgb.y = (P3D'.y * fy_rgb / P3D'.z) + cy_rgb

	ofVec2f rgb2d;
	
	float xoff = -8;
	ofVec3f p3d = raw_depth_to_p3d(raw_depth, x_depth, y_depth);
	
	ofVec3f p3d_rgb = RT_rgb * p3d;
	
	rgb2d.x = (p3d_rgb.x * fx_rgb / p3d_rgb.z) + cx_rgb;
	rgb2d.y = (p3d_rgb.y * fy_rgb / p3d_rgb.z) + cy_rgb;
	
	CLAMP(rgb2d.x, 0, rgb_width-1);
	CLAMP(rgb2d.y, 0, rgb_height-1);
	
	return rgb2d;
}


// fbo


void CamaraLucida::init_fbo(int tex_width, int tex_height, int tex_num_samples)
{
	ofFbo::Settings s;
	s.width				= tex_width;
	s.height			= tex_height;
	s.numSamples		= tex_num_samples;
	s.numColorbuffers	= 1;
	s.internalformat	= GL_RGBA;
	
	fbo.setup(s);
	//fbo.setup(s.width, s.height, s.internalformat, s.numSamples);
}


// open cl


void CamaraLucida::init_cl(uint16_t *raw_depth_pix, MSA::OpenCL* opencl)
{
	ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL init");
	
	// delegate opencl setup
	//opencl->setupFromOpenGL();
	
	opencl->loadProgramFromFile("vertex.cl");
	kernel_vertex_update = opencl->loadKernel("update_vertex");
	
	ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL init buffers... raw_depth_pix "+ofToString(raw_depth_pix));
	
	//	cl_buff_vbo_3d.initFromGLObject(vbo.getVertId());
	//	cl_buff_ibo.initFromGLObject(vbo.getIndexId());
	//	cl_buff_ibo.initBuffer(sizeof(uint) * ibo_length, CL_MEM_READ_WRITE, ibo);
	
	cl_buff_vbo_3d.initBuffer(sizeof(float4) * vbo_length, CL_MEM_READ_WRITE, vbo_3d);
	cl_buff_raw_depth.initBuffer(sizeof(uint16_t) * d_width * d_height, CL_MEM_READ_ONLY, raw_depth_pix);
	
	ofLog(OF_LOG_VERBOSE, "Camara Lucida Open CL set args");
	
	kernel_vertex_update->setArg(0, cl_buff_vbo_3d.getCLMem());
	//kernel_vertex_update->setArg(1, cl_buff_ibo.getCLMem());
	kernel_vertex_update->setArg(1, cl_buff_raw_depth.getCLMem());
	kernel_vertex_update->setArg(2, mesh_step);
	kernel_vertex_update->setArg(3, cx_d);
	kernel_vertex_update->setArg(4, cy_d);
	kernel_vertex_update->setArg(5, fx_d);
	kernel_vertex_update->setArg(6, fy_d);
	kernel_vertex_update->setArg(7, depth_xoff);
}

void CamaraLucida::update_cl(uint16_t *raw_depth_pix)
{
	cl_buff_raw_depth.write(raw_depth_pix, 0, sizeof(uint16_t) * d_width * d_height);
	
	kernel_vertex_update->run1D(vbo_length);
	
	cl_buff_vbo_3d.read(vbo_3d, 0, sizeof(float4) * vbo_length);
	//cl_buff_ibo.read(ibo, 0, sizeof(uint) * ibo_length);
}


// vbo


void CamaraLucida::init_vbo()
{
	mesh_w = (float)d_width/mesh_step;
	mesh_h = (float)d_height/mesh_step;
	
	vbo_length = mesh_w * mesh_h; 
	ibo_length = vbo_length * 4;
	
	vbo_3d = new float4[vbo_length];
	for (int i = 0; i < vbo_length; i++) 
	{
		vbo_3d[i] = float4(0, 0, 0);
	}
	
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
		
		float t = ((float)mcol/mesh_w) * fbo.getWidth();
		float u = ((float)mrow/mesh_h) * fbo.getHeight();
		
		vbo_texcoords[i] = ofVec2f(t, u);
	}
	
	vbo_color = new ofColor[vbo_length];
	for (int i = 0; i < vbo_length; i++) 
	{
		vbo_color[i] = ofColor();
	}
	
	vbo.setVertexData(&vbo_3d[0].x, 3, vbo_length, GL_DYNAMIC_DRAW, sizeof(float4));
	vbo.setIndexData(ibo, ibo_length, GL_STATIC_DRAW);
	vbo.setColorData(vbo_color, vbo_length, GL_STATIC_DRAW);
	vbo.setTexCoordData(vbo_texcoords, vbo_length, GL_STATIC_DRAW);
}

void CamaraLucida::dispose_vbo()
{
	vbo.clear();
	
	delete[] ibo;
	ibo = NULL;
	
	delete[] vbo_3d;
	vbo_3d = NULL;
	
	delete[] vbo_texcoords;
	vbo_texcoords = NULL;
	
	delete[] vbo_color;
	vbo_color = NULL;
}

void CamaraLucida::render_mesh()
{	
//	if (using_opencl)
//		opencl.finish();
	
	if (!vbo.getIsAllocated())
		return;
	
	//glEnable(GL_BLEND);  
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
	//glBlendFuncSeparate(GL_ONE, GL_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
	//ofEnableAlphaBlending();
	
	fbo.getTexture(0).bind();
	vbo.bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.getIndexId());
	glDrawElements(GL_QUADS, ibo_length, GL_UNSIGNED_INT, NULL);
	vbo.unbind();	
	fbo.getTexture(0).unbind();
	
	//glDisable(GL_BLEND);
	//ofDisableAlphaBlending();  
}


void CamaraLucida::update_mesh(const uint16_t *raw_depth_pix)
{
	for (int i = 0; i < vbo_length; i++)
	{
		update_vertex(i, vbo_3d, raw_depth_pix, 
					  vbo_length, mesh_step, mesh_w, mesh_h,
					  d_width, d_height,
					  cx_d, cy_d, fx_d, fy_d, depth_xoff);
	}
}

void CamaraLucida::update_vertex(int vbo_idx, float4* vbo_buff, 
						const ushort* raw_depth_buff, 
						const int vbo_length, const int mesh_step, 
						const int mesh_w, const int mesh_h,
						const int d_width, const int d_height,
						const float cx_d, const float cy_d, 
						const float fx_d, const float fy_d, 
						const int depth_xoff)
{
	//	int vbo_idx = get_global_id(0);
	//	
	//	int mesh_w = d_width/mesh_step;
	//	int mesh_h = d_height/mesh_step;
	//	int vbo_length = mesh_w * mesh_h; 
	
	int mcol = vbo_idx % mesh_w;
	int mrow = (vbo_idx - mcol) / mesh_w;
	
	int col = mcol * mesh_step;
	int row = mrow * mesh_step;
	
	int depth_idx = row * d_width + col;
	
	
	// set VBO pts
	
	ushort raw_depth = raw_depth_buff[depth_idx];
	
	float z = _zlut[raw_depth];
	float x = (col + depth_xoff - cx_d) * z / fx_d;
	float y = (row - cy_d) * z / fy_d;
	
	vbo_buff[vbo_idx].x = x;
	vbo_buff[vbo_idx].y = y;
	vbo_buff[vbo_idx].z = z;
	//vbo_buff[vbo_idx].w = 0;
	
	// set color
	//vbo_color[vbo_idx].set(r,g,b);
}


// gl


void CamaraLucida::gl_ortho()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ofGetWidth(), ofGetHeight(), 0, -1, 1);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CamaraLucida::gl_projection()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	switch(view_type)
	{
		case V_PROJ:
			glMultMatrixf(proj_KK);
			break;
		case V_DEPTH:
			glMultMatrixf(depth_KK);
			break;
	}
}

void CamaraLucida::gl_viewpoint()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glScalef(1., -1., 1.);
	
	switch(view_type)
	{
		case V_DEPTH:			
			gluLookAt(0., 0., 0,		//loc
					  0., 0., 1.,		//target
					  0., 1., 0.);		//up			
			break;
			
		case V_PROJ:
			gluLookAt(proj_loc.x, proj_loc.y, proj_loc.z,	//loc
					  proj_trg.x, proj_trg.y, proj_trg.z,	//target
					  proj_up.x, proj_up.y, proj_up.z);		//up
			
			break;
	}
}


// gl debug


void CamaraLucida::init_gl_scene_control()
{
	rot_pivot = ofVec3f( proj_RT[12], proj_RT[13], proj_RT[14] );
	
	pmouse = ofVec2f();
	
	tZ_delta = -0.05;
	rot_delta = -0.2;
	
	tZini = 0;
	rotXini = 0;
	rotYini = 0;
	rotZini = 0;
	
	reset_gl_scene_control();
}

void CamaraLucida::reset_gl_scene_control()
{
	tZ = tZini;
	rotX = rotXini;
	rotY = rotYini;
	rotZ = rotZini;
}

void CamaraLucida::gl_scene_control()
{
	glTranslatef(0, 0, tZ);
	glTranslatef(rot_pivot.x, rot_pivot.y, rot_pivot.z);
	glRotatef(rotX, 1, 0, 0);
	glRotatef(rotY, 0, 1, 0);
	glRotatef(rotZ, 0, 0, 1);
	glTranslatef(-rot_pivot.x, -rot_pivot.y, -rot_pivot.z);
}

void CamaraLucida::render_ppal_point()
{
	glPointSize(5);
	glBegin(GL_POINTS);
	glColor3f(1,1,0);
	glVertex3f(0, 0, 0);
	glEnd();
}	

void CamaraLucida::render_world_CS()
{
	render_axis(0.2);
}

void CamaraLucida::render_proj_CS()
{
	glPushMatrix();
	glMultMatrixf(proj_RT);
	render_axis();
	glPopMatrix();
}

void CamaraLucida::render_rgb_CS()
{
	glPushMatrix();
	glMultMatrixf(drgb_RT);
	render_axis();
	glPopMatrix();
}

void CamaraLucida::render_axis(float s)
{
	glBegin(GL_LINES);
	
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(s, 0, 0);
	
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, s, 0);
	
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, s);
	
	glEnd();
}

void CamaraLucida::render_screenlog()
{
	if (!_debug) return;
	ofDrawBitmapString(view_type_str()+" /depth_xoff: "+ofToString(depth_xoff)+" /fps: "+ofToString(ofGetFrameRate()), 10, ofGetHeight()-10);
}

string CamaraLucida::view_type_str()
{
	switch(view_type)
	{
		case V_PROJ:
			return "projector viewpoint";
			break;
		case V_DEPTH:
			return "depth camera viewpoint";
			break;
	}
}


// ui


void CamaraLucida::init_events()
{
	ofAddListener(ofEvents.keyPressed, this, &CamaraLucida::keyPressed);
	ofAddListener(ofEvents.keyReleased, this, &CamaraLucida::keyReleased);
	ofAddListener(ofEvents.mouseDragged, this, &CamaraLucida::mouseDragged);
	ofAddListener(ofEvents.mousePressed, this, &CamaraLucida::mousePressed);
}

void CamaraLucida::dispose_events()
{
	ofRemoveListener(ofEvents.keyPressed, this, &CamaraLucida::keyPressed);
	ofRemoveListener(ofEvents.keyReleased, this, &CamaraLucida::keyReleased);
	ofRemoveListener(ofEvents.mouseDragged, this, &CamaraLucida::mouseDragged);
	ofRemoveListener(ofEvents.mousePressed, this, &CamaraLucida::mousePressed);
}

void CamaraLucida::init_keys()
{
	for (int i = 0; i < 512; i++) 
		pressed[i] = false;
}

void CamaraLucida::update_keys()
{
	
}

void CamaraLucida::keyPressed(ofKeyEventArgs &args)
{
	pressed[args.key] = true;
	
	if (!_debug) return;
	
	switch(args.key)
	{		
		case key_view_type:
			++view_type;
			view_type = view_type == V_TYPE_LENGTH ? 0 : view_type;
			break;
			
		case key_reset_view:
			reset_gl_scene_control();
			break;
	}
	
	if (pressed[key_change_depth_xoff])
	{
		if (args.key == OF_KEY_UP)
		{
			depth_xoff++;
			if (using_opencl)
			{
				kernel_vertex_update->setArg(7, depth_xoff);
			}
		}
		else if (args.key == OF_KEY_DOWN)
		{
			depth_xoff--;
			if (using_opencl)
			{
				kernel_vertex_update->setArg(7, depth_xoff);
			}
		}
	}
}

void CamaraLucida::keyReleased(ofKeyEventArgs &args)
{
	pressed[args.key] = false;
}

void CamaraLucida::mouseDragged(ofMouseEventArgs &args)
{
	if (!_debug) return;
	
	ofVec2f m = ofVec2f(args.x, args.y);
	ofVec2f dist = m - pmouse;
	
	if (pressed[key_zoom])
	{
		tZ += -dist.y * tZ_delta;	
	}
	else
	{
		rotX += -dist.y * rot_delta;
		rotY += -dist.x * rot_delta;
	}
	pmouse.set(args.x, args.y);
}

void CamaraLucida::mousePressed(ofMouseEventArgs &args)
{
	pmouse.set(args.x, args.y);	
}



// data / conversion


	
void CamaraLucida::load_data(string kinect_calibration_filename, string proj_calibration_filename)
{
	const char* kinect_calibration_filename_str = kinect_calibration_filename.c_str();
	const char* proj_calibration_filename_str = proj_calibration_filename.c_str();
	
	//	rgb
	
	rgb_int = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "rgb_intrinsics");
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n rgb_intrinsics opencv (kinect_calibration.yml)");
	printM(rgb_int);
	
	fx_rgb = (float)cvGetReal2D( rgb_int, 0, 0 );
	fy_rgb = (float)cvGetReal2D( rgb_int, 1, 1 );
	cx_rgb = (float)cvGetReal2D( rgb_int, 0, 2 );
	cy_rgb = (float)cvGetReal2D( rgb_int, 1, 2 );
	
	CvMat* rgb_size = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "rgb_size");
	rgb_width = (int)cvGetReal2D( rgb_size, 0, 0 );
	rgb_height = (int)cvGetReal2D( rgb_size, 0, 1 );
	cvReleaseMat(&rgb_size);
	
	convertKKopencv2opengl(rgb_int, rgb_width, rgb_height, near, far, rgb_KK);
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n rgb_intrinsics converted to opengl");
	printM(rgb_KK, 4, 4);
	
	
	//	depth
	
	depth_int = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "depth_intrinsics");
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n depth_intrinsics opencv (kinect_calibration.yml)");
	printM(depth_int);
	
	fx_d = (float)cvGetReal2D( depth_int, 0, 0 );
	fy_d = (float)cvGetReal2D( depth_int, 1, 1 );
	cx_d = (float)cvGetReal2D( depth_int, 0, 2 );
	cy_d = (float)cvGetReal2D( depth_int, 1, 2 );
	
	CvMat* d_size = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "depth_size");
	d_width = (int)cvGetReal2D( d_size, 0, 0 );
	d_height = (int)cvGetReal2D( d_size, 0, 1 );
	cvReleaseMat(&d_size);
	
	convertKKopencv2opengl(depth_int, d_width, d_height, near, far, depth_KK);
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n depth_intrinsics converted to opengl");
	printM(depth_KK, 4, 4);
	
	
	//	depth/rgb RT
	
	drgb_R = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "R");
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n drgb_R opencv (kinect_calibration.yml)");
	printM(drgb_R);
	
	drgb_T = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "T");
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n drgb_T opencv (kinect_calibration.yml)");
	printM(drgb_T);
	
	convertRTopencv2opengl(drgb_R, drgb_T, drgb_RT);
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n drgb_RT converted to opengl");
	printM(drgb_RT, 4, 4);
	
	//	T_rgb = ofVec3f(
	//		drgb_RT[12],	drgb_RT[13], drgb_RT[14] );
	RT_rgb = ofMatrix4x4(
						 drgb_RT[0],	drgb_RT[1], drgb_RT[2],		drgb_RT[12],
						 drgb_RT[4],	drgb_RT[5], drgb_RT[6],		drgb_RT[13],
						 drgb_RT[8],	drgb_RT[9], drgb_RT[10],	drgb_RT[14],
						 0.,			0.,			0.,				1);
	//	R_rgb.preMultTranslate(-T_rgb);
	//	R_rgb = ofMatrix4x4::getTransposedOf(R_rgb);
	
	
	//	proyector
	
	proj_int = (CvMat*)cvLoad(proj_calibration_filename_str, NULL, "proj_intrinsics");
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n proj_intrinsics opencv (projector_calibration.yml)");
	printM(proj_int);
	
	fx_p = (float)cvGetReal2D( proj_int, 0, 0 );
	fy_p = (float)cvGetReal2D( proj_int, 1, 1 );
	cx_p = (float)cvGetReal2D( proj_int, 0, 2 );
	cy_p = (float)cvGetReal2D( proj_int, 1, 2 );
	
	CvMat* p_size = (CvMat*)cvLoad(proj_calibration_filename_str, NULL, "proj_size");
	p_width = (int)cvGetReal2D( p_size, 0, 0 );
	p_height = (int)cvGetReal2D( p_size, 0, 1 );
	cvReleaseMat(&d_size);
	
	convertKKopencv2opengl(proj_int, p_width, p_height, near, far, proj_KK);
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n proj_intrinsics converted to opengl");
	printM(proj_KK, 4, 4);
	
	proj_R = (CvMat*)cvLoad(proj_calibration_filename_str, NULL, "R");
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n proj_R opencv (projector_calibration.yml)");
	printM(proj_R);
	
	proj_T = (CvMat*)cvLoad(proj_calibration_filename_str, NULL, "T");
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n proj_T opencv (projector_calibration.yml)");
	printM(proj_T);
	
	convertRTopencv2opengl(proj_R, proj_T, proj_RT);
	ofLog(OF_LOG_VERBOSE, "Camara Lucida \n proj_RT converted to opengl");
	printM(proj_RT, 4, 4);
}


//http://opencv.willowgarage.com/wiki/Posit
//http://www.songho.ca/opengl/gl_projectionmatrix.html
//http://www.songho.ca/opengl/gl_transform.html

void CamaraLucida::convertRTopencv2opengl(CvMat* opencvR, CvMat* opencvT, float *openglRT)
{
//	cvGetReal2D( M, row, col );
	
	//	opencv: row-major	
	//	R x axis
	float xx = (float)cvGetReal2D( opencvR, 0, 0 );
	float xy = (float)cvGetReal2D( opencvR, 1, 0 );
	float xz = (float)cvGetReal2D( opencvR, 2, 0 );
	//	R y axis	
	float yx = (float)cvGetReal2D( opencvR, 0, 1 );
	float yy = (float)cvGetReal2D( opencvR, 1, 1 );
	float yz = (float)cvGetReal2D( opencvR, 2, 1 );
	//	R z axis	
	float zx = (float)cvGetReal2D( opencvR, 0, 2 );
	float zy = (float)cvGetReal2D( opencvR, 1, 2 );
	float zz = (float)cvGetReal2D( opencvR, 2, 2 );
	//	T
	float tx = (float)cvGetReal2D( opencvT, 0, 0 );
	float ty = (float)cvGetReal2D( opencvT, 1, 0 );
	float tz = (float)cvGetReal2D( opencvT, 2, 0 );	
	
//	opengl: col-major	
	openglRT[0] = xx;	openglRT[4] = yx;	openglRT[8] = zx;	openglRT[12] = tx;
	openglRT[1] = xy;	openglRT[5] = yy;	openglRT[9] = zy;	openglRT[13] = ty;
	openglRT[2] = xz;	openglRT[6] = yz;	openglRT[10] = zz;	openglRT[14] = tz;
	openglRT[3] = 0.;	openglRT[7] = 0.;	openglRT[11] = 0.;	openglRT[15] = 1.;
}

void CamaraLucida::convertKKopencv2opengl(CvMat* opencvKK, float width, float height, float near, float far, float* openglKK)
{
	float fx = (float)cvGetReal2D( opencvKK, 0, 0 );
	float fy = (float)cvGetReal2D( opencvKK, 1, 1 );
	float cx = (float)cvGetReal2D( opencvKK, 0, 2 );
	float cy = (float)cvGetReal2D( opencvKK, 1, 2 );
	
	float A = 2.*fx/width;
	float B = 2.*fy/height;
	float C = 2.*(cx/width)-1.;
	float D = 2.*(cy/height)-1.;
	float E = -(far+near)/(far-near);
	float F = -2.*far*near/(far-near);
	
//	col-major
	openglKK[0]= A;		openglKK[4]= 0.;	openglKK[8]= C;		openglKK[12]= 0.;
	openglKK[1]= 0.;	openglKK[5]= B;		openglKK[9]= D;		openglKK[13]= 0.;
	openglKK[2]= 0.;	openglKK[6]= 0.;	openglKK[10]= E;	openglKK[14]= F;
	openglKK[3]= 0.;	openglKK[7]= 0.;	openglKK[11]= -1.;	openglKK[15]= 0.;	
}


void CamaraLucida::init_zlut()
{
	for (int i = 0; i < raw_depth_size; i++) 
	{
		_zlut[i] = raw_depth_to_meters(i);
	}
}


//http://openkinect.org/wiki/Imaging_Information
//http://nicolas.burrus.name/index.php/Research/KinectCalibration

float CamaraLucida::raw_depth_to_meters(uint16_t raw_depth)
{
	if (raw_depth < _base_depth_raw)
	{
		return k1 * tanf((raw_depth / k2) + k3) - k4; // calculate in meters
	}
	return _base_depth_mts;
}

uint16_t CamaraLucida::get_base_depth_raw()
{
	return _base_depth_raw;
}

float CamaraLucida::get_base_depth_mts()
{
	return _base_depth_mts;
}

// debugging


void CamaraLucida::printM(float* M, int rows, int cols, bool colmajor)
{
	if (ofGetLogLevel() != OF_LOG_VERBOSE)
	{
		return;
	}
		
	if (colmajor)
	{
		for (int j = 0; j < cols; j++)
		{
			printf("\n");
			for (int i = 0; i < rows; i++)
			{
				printf("%9.3f ", M[i*cols+j]);
			}	
		}
	}
	else
	{
		for (int i = 0; i < rows; i++)
		{
			printf("\n");
			for (int j = 0; j < cols; j++)
			{
				printf("%9.3f ", M[i*cols+j]);
			}	
		}
	}
	printf("\n");
}

void CamaraLucida::printM(CvMat* M, bool colmajor)
{
	if (ofGetLogLevel() != OF_LOG_VERBOSE)
	{
		return;
	}
	
	int i,j;
	if (colmajor)
	{
		for (i = 0; i < M->rows; i++)
		{
			printf("\n");				
			switch( CV_MAT_DEPTH(M->type) )
			{
				case CV_32F:
				case CV_64F:
					for (j = 0; j < M->cols; j++)
						printf("%9.3f ", (float)cvGetReal2D( M, i, j ));
					break;
				case CV_8U:
				case CV_16U:
					for (j = 0; j < M->cols; j++)
						printf("%6d",(int)cvGetReal2D( M, i, j ));
					break;
				default:
					break;
			}
		}
	}
	else
	{
		for (j = 0; j < M->cols; j++)
		{
			printf("\n");				
			switch( CV_MAT_DEPTH(M->type) )
			{
				case CV_32F:
				case CV_64F:
					for (i = 0; i < M->rows; i++)
						printf("%9.3f ", (float)cvGetReal2D( M, i, j ));
					break;
				case CV_8U:
				case CV_16U:
					for (i = 0; i < M->rows; i++)
						printf("%6d",(int)cvGetReal2D( M, i, j ));
					break;
				default:
					break;
			}
		}
	}
	printf("\n");
}
