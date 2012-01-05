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

#include "CamaraLucida.h"

namespace cml 
{
	void CamaraLucida::init(string kinect_calibration_filename,
							string proj_calibration_filename,
							string xml_config_filename,
							cml::Mesh *mesh,
							int tex_width, int tex_height, 
							int tex_num_samples)
	{
		this->mesh = mesh;
		
		coord_sys = mesh->coord_sys();
		
		init_cml(kinect_calibration_filename, 
				 proj_calibration_filename,
				 xml_config_filename);
				
		if (mesh->is_render_enabled())
		{
			init_fbo(tex_width, tex_height, tex_num_samples);
		}
		
		mesh->init(&xml_config, &calib, tex_width, tex_height);
	}
	
	void CamaraLucida::init_cml(string kinect_calibration_filename,
								string proj_calibration_filename,
								string xml_config_filename)
	{
		_inited = true;
		
		xml_config.loadFile(xml_config_filename);
		xml_config.pushTag("camaralucida");
		
		calib.near = xml_config.getValue("near", 0.1);
		calib.far = xml_config.getValue("far", 20.0);
		
		load_data(kinect_calibration_filename, proj_calibration_filename);
		
		proj_loc = ofVec3f(	proj_RT[12], proj_RT[13], proj_RT[14] );	
		proj_fwd = ofVec3f(	proj_RT[8], proj_RT[9], proj_RT[10] );
		proj_up = ofVec3f( proj_RT[4], proj_RT[5], proj_RT[6] );
		proj_trg = proj_loc + proj_fwd;
		
		rgb_loc = ofVec3f( rgb_RT[12], rgb_RT[13], rgb_RT[14] );	
		rgb_fwd = ofVec3f( rgb_RT[8], rgb_RT[9], rgb_RT[10] );
		rgb_up = ofVec3f( rgb_RT[4], rgb_RT[5], rgb_RT[6] );
		rgb_trg = rgb_loc + rgb_fwd;
		
		init_keys();
		init_events();
		init_gl_scene_control();
	}
	
	void CamaraLucida::dispose()
	{
		ofLog(OF_LOG_VERBOSE, "CamaraLucida::dispose");
		
		if (!inited())
			return;
		
		dispose_events();
		
		mesh = NULL;
		
		cvReleaseMat(&rgb_int);	
		cvReleaseMat(&depth_int);
		
		cvReleaseMat(&rgb_R);
		cvReleaseMat(&rgb_T);
		
		cvReleaseMat(&proj_int);	
		cvReleaseMat(&proj_R);
		cvReleaseMat(&proj_T);
	}

	void CamaraLucida::update()
	{
		if (!inited())
			return;
				
		if (mesh->is_render_enabled())
			mesh->update();
	}

	void CamaraLucida::render()
	{
		if (!inited())
			return;
		
		if (mesh->is_render_enabled())
		{
			fbo.bind();
			//ofEnableAlphaBlending();  
			//glEnable(GL_BLEND);  
			//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA); 
		
			ofNotifyEvent( render_texture, void_event_args );
		
			fbo.unbind();
			//ofDisableAlphaBlending(); 
			//glDisable(GL_BLEND);  
		}
		
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	
		glPolygonMode(GL_FRONT, GL_FILL);
		// TODO wireframe it's not working with fbo textures.. why?
		// @see cmlMesh.enable_render();
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
		
		glColor3f(1, 1, 1);
		glViewport(0, 0, ofGetWidth(), ofGetHeight());
		
		gl_ortho();
		
		ofNotifyEvent( render_hud, void_event_args );
		
		render_screenlog();
		
		gl_projection();	
		gl_viewpoint();
		
		gl_scene_control();
		
		if (_debug)
		{
			render_world_CS();
			render_proj_CS();
			render_rgb_CS();
			render_proj_ppal_point();
		}
		
		//	if (using_opencl)
		//		opencl.finish();
		
		// TODO alpha blending!
		
		//glEnable(GL_BLEND);  
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
		//glBlendFuncSeparate(GL_ONE, GL_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
		//ofEnableAlphaBlending();
		
		if (mesh->is_render_enabled())
		{
			fbo.getTextureReference(0).bind();
		
			mesh->render();
		
			fbo.getTextureReference(0).unbind();
		}
		
		//glDisable(GL_BLEND);
		//ofDisableAlphaBlending(); 
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
		
		fbo.allocate(s);
		//fbo.allocate(s.width, s.height, s.internalformat, s.numSamples);
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
		
		switch(_viewpoint)
		{
			case V_PROJ:
				glMultMatrixf(proj_KK);
				break;
			case V_DEPTH:
				glMultMatrixf(depth_KK);
				break;
			case V_RGB:
				glMultMatrixf(rgb_KK);
				break;
		}
	}

	void CamaraLucida::gl_viewpoint()
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glScalef(coord_sys.x, coord_sys.y, coord_sys.z);	
		
		switch(_viewpoint)
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
				
			case V_RGB:
				gluLookAt(rgb_loc.x, rgb_loc.y, rgb_loc.z,	//loc
						  rgb_trg.x, rgb_trg.y, rgb_trg.z,		//target
						  rgb_up.x, rgb_up.y, rgb_up.z);		//up
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

	void CamaraLucida::render_proj_ppal_point()
	{
		glPointSize(5);
		glColor3f(1, 1, 0); //yellow
		
		float ts = 0.5;
		
		glPushMatrix();
			glMultMatrixf(proj_RT);
			
			glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, ts);
			glEnd();
		
			//float amp = 0.5;
			//float ts = sin( ofGetElapsedTimef() * 0.7 ) * amp + amp;
			glBegin(GL_POINTS);
			glVertex3f(0, 0, ts);
			glEnd();
		
		glPopMatrix();
	}	

	void CamaraLucida::render_world_CS()
	{
		render_axis(0.1);
	}

	void CamaraLucida::render_proj_CS()
	{
		glPushMatrix();
		glMultMatrixf(proj_RT);
		render_axis(0.1);
		glPopMatrix();
	}

	void CamaraLucida::render_rgb_CS()
	{
		glPushMatrix();
		glMultMatrixf(rgb_RT);
		render_axis(0.05);
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
		ofDrawBitmapString(_viewpoint_str()+" /fps: "+ofToString(ofGetFrameRate()), 10, ofGetHeight()-10);
	}

	string CamaraLucida::_viewpoint_str()
	{
		switch(_viewpoint)
		{
			case V_PROJ:
				return "projector viewpoint";
				break;
			case V_DEPTH:
				return "depth camera viewpoint";
				break;
			case V_RGB:
				return "rgb camera viewpoint";
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
		
		xml_config.pushTag("debug_keys");
			xml_config.pushTag("viewpoint");
				key_viewpoint_next = xml_config.getValue("next", "t")[0];
				key_viewpoint_prev = xml_config.getValue("prev", "v")[0];
				xml_config.popTag();
			xml_config.pushTag("scene_ctrl");
				key_scene_ctrl_reset = xml_config.getValue("reset", "x")[0];
				key_scene_ctrl_zoom = xml_config.getValue("zoom", "z")[0];
				xml_config.popTag();
		xml_config.popTag();
	}

	void CamaraLucida::keyPressed(ofKeyEventArgs &args)
	{
		pressed[args.key] = true;
		
		if (!_debug) 
			return;
		
		mesh->keyPressed(args);
		
		if (args.key == key_viewpoint_next)
		{		
			++_viewpoint;
			_viewpoint = _viewpoint == V_LENGTH ? 0 : _viewpoint;
		}
		else if (args.key == key_viewpoint_prev)
		{
			--_viewpoint;
			_viewpoint = _viewpoint == -1 ? V_LENGTH-1 : _viewpoint;
		}
		else if (args.key == key_scene_ctrl_reset)
		{
			reset_gl_scene_control();
		}
	}

	void CamaraLucida::keyReleased(ofKeyEventArgs &args)
	{
		pressed[args.key] = false;
		mesh->keyReleased(args);
	}

	void CamaraLucida::mouseDragged(ofMouseEventArgs &args)
	{
		if (!_debug) return;
		
		ofVec2f m = ofVec2f(args.x, args.y);
		ofVec2f dist = m - pmouse;
		
		if (pressed[key_scene_ctrl_zoom])
		{
			tZ += -dist.y * tZ_delta;	
		}
		else
		{
			rotX += coord_sys.x * dist.y * rot_delta;
			rotY += -coord_sys.y * dist.x * rot_delta;
		}
		pmouse.set(args.x, args.y);
	}

	void CamaraLucida::mousePressed(ofMouseEventArgs &args)
	{
		pmouse.set(args.x, args.y);	
	}



	// data conversion

		
	void CamaraLucida::load_data(string kinect_calibration_filename, string proj_calibration_filename)
	{
		const char* kinect_calibration_filename_str = kinect_calibration_filename.c_str();
		const char* proj_calibration_filename_str = proj_calibration_filename.c_str();
		
		//	rgb
		
		rgb_int = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "rgb_intrinsics");
		ofLog(OF_LOG_VERBOSE, "Camara Lucida \n rgb_intrinsics opencv (kinect_calibration.yml)");
		printM(rgb_int);
		
		calib.fx_rgb = (float)cvGetReal2D( rgb_int, 0, 0 );
		calib.fy_rgb = (float)cvGetReal2D( rgb_int, 1, 1 );
		calib.cx_rgb = (float)cvGetReal2D( rgb_int, 0, 2 );
		calib.cy_rgb = (float)cvGetReal2D( rgb_int, 1, 2 );
		
		CvMat* rgb_size = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "rgb_size");
		calib.rgb_width = (int)cvGetReal2D( rgb_size, 0, 0 );
		calib.rgb_height = (int)cvGetReal2D( rgb_size, 0, 1 );
		cvReleaseMat(&rgb_size);
		
		convertKKopencv2opengl(rgb_int, calib.rgb_width, calib.rgb_height, calib.near, calib.far, rgb_KK);
		ofLog(OF_LOG_VERBOSE, "Camara Lucida \n rgb_intrinsics converted to opengl");
		printM(rgb_KK, 4, 4);
		
		
		//	depth
		
		depth_int = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "depth_intrinsics");
		ofLog(OF_LOG_VERBOSE, "Camara Lucida \n depth_intrinsics opencv (kinect_calibration.yml)");
		printM(depth_int);
		
		calib.fx_d = (float)cvGetReal2D( depth_int, 0, 0 );
		calib.fy_d = (float)cvGetReal2D( depth_int, 1, 1 );
		calib.cx_d = (float)cvGetReal2D( depth_int, 0, 2 );
		calib.cy_d = (float)cvGetReal2D( depth_int, 1, 2 );
		
		CvMat* d_size = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "depth_size");
		calib.depth_width = (int)cvGetReal2D( d_size, 0, 0 );
		calib.depth_height = (int)cvGetReal2D( d_size, 0, 1 );
		cvReleaseMat(&d_size);
		
		convertKKopencv2opengl(depth_int, calib.depth_width, calib.depth_height, calib.near, calib.far, depth_KK);
		ofLog(OF_LOG_VERBOSE, "Camara Lucida \n depth_intrinsics converted to opengl");
		printM(depth_KK, 4, 4);
		
		
		//	depth/rgb RT
		
		rgb_R = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "R");
		ofLog(OF_LOG_VERBOSE, "Camara Lucida \n rgb_R opencv (kinect_calibration.yml)");
		printM(rgb_R);
		
		rgb_T = (CvMat*)cvLoad(kinect_calibration_filename_str, NULL, "T");
		ofLog(OF_LOG_VERBOSE, "Camara Lucida \n rgb_T opencv (kinect_calibration.yml)");
		printM(rgb_T);
		
		convertRTopencv2opengl(rgb_R, rgb_T, rgb_RT);
		ofLog(OF_LOG_VERBOSE, "Camara Lucida \n rgb_RT converted to opengl");
		printM(rgb_RT, 4, 4);
		
		//	T_rgb = ofVec3f(
		//		rgb_RT[12],	rgb_RT[13], rgb_RT[14] );
		calib.RT_rgb = ofMatrix4x4(
			 rgb_RT[0],	rgb_RT[1], rgb_RT[2],		rgb_RT[12],
			 rgb_RT[4],	rgb_RT[5], rgb_RT[6],		rgb_RT[13],
			 rgb_RT[8],	rgb_RT[9], rgb_RT[10],		rgb_RT[14],
			 0.,			0.,			0.,				1);
		//	R_rgb.preMultTranslate(-T_rgb);
		//	R_rgb = ofMatrix4x4::getTransposedOf(R_rgb);
		
		
		//	proyector
		
		proj_int = (CvMat*)cvLoad(proj_calibration_filename_str, NULL, "proj_intrinsics");
		ofLog(OF_LOG_VERBOSE, "Camara Lucida \n proj_intrinsics opencv (projector_calibration.yml)");
		printM(proj_int);
		
		calib.fx_p = (float)cvGetReal2D( proj_int, 0, 0 );
		calib.fy_p = (float)cvGetReal2D( proj_int, 1, 1 );
		calib.cx_p = (float)cvGetReal2D( proj_int, 0, 2 );
		calib.cy_p = (float)cvGetReal2D( proj_int, 1, 2 );
		
		CvMat* p_size = (CvMat*)cvLoad(proj_calibration_filename_str, NULL, "proj_size");
		calib.proj_width = (int)cvGetReal2D( p_size, 0, 0 );
		calib.proj_height = (int)cvGetReal2D( p_size, 0, 1 );
		cvReleaseMat(&p_size);
		
		convertKKopencv2opengl(proj_int, calib.proj_width, calib.proj_height, calib.near, calib.far, proj_KK);
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
		
		float A = 2. * fx / width;
		float B = 2. * fy / height;
		float C = 2. * (cx / width) - 1.;
		float D = 2. * (cy / height) - 1.;
		float E = - (calib.far + calib.near) / (calib.far - calib.near);
		float F = -2. * calib.far * calib.near / (calib.far - calib.near);
		
	//	col-major
		openglKK[0]= A;		openglKK[4]= 0.;	openglKK[8]= C;		openglKK[12]= 0.;
		openglKK[1]= 0.;	openglKK[5]= B;		openglKK[9]= D;		openglKK[13]= 0.;
		openglKK[2]= 0.;	openglKK[6]= 0.;	openglKK[10]= E;	openglKK[14]= F;
		openglKK[3]= 0.;	openglKK[7]= 0.;	openglKK[11]= -1.;	openglKK[15]= 0.;	
		
//		another solution by Kyle McDonald...
//		https://github.com/kylemcdonald/ofxCv/blob/master/libs/ofxCv/src/Calibration.cpp
//		glFrustum(
//				  nearDist * (-cx) / fx, nearDist * (w - cx) / fx,
//				  nearDist * (cy - h) / fy, nearDist * (cy) / fy,
//				  nearDist, farDist);
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
	
	void CamaraLucida::toggle_debug()
	{
		_debug = !_debug;
	}
	
	string CamaraLucida::get_keyboard_help()
	{
		return "camara lucida keys for debug mode: \n switch viewpoint "+string(1, key_viewpoint_next)+" and "+string(1, key_viewpoint_prev)+" \n scene control: drag mouse to rotate, "+string(1, key_scene_ctrl_zoom)+"+drag to zoom, "+string(1, key_scene_ctrl_reset)+" to reset";
	}
};