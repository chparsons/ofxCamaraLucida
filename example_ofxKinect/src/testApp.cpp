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

#include "testApp.h"

void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetWindowPosition(0,0);
	
	if (!init_kinect())
		return;
	
	init_keys();
	
	string config = ofToDataPath("camara_lucida/camara_lucida_config.xml");
	
	mesh = new cml::Mesh(raw_depth_pix);
	
	camluc.init(config, mesh);
	
	ofAddListener(camluc.render_texture, this, &testApp::render_texture);
	ofAddListener(camluc.render_2d, this, &testApp::render_2d);
	ofAddListener(camluc.render_3d, this, &testApp::render_3d);
}

void testApp::update()
{
	if (!update_kinect())
		return;
	
	if (kinect.isFrameNew())
		camluc.update();
}

void testApp::draw()
{	
//	light.enable();
	camluc.render();
}

void testApp::render_texture(ofEventArgs &args)
{
	glClearColor(0.5, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	float w = mesh->get_tex_width();
	float h = mesh->get_tex_height();
	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glColor3f(1, 1, 1);
	
	//kinect.enableCalibrationUpdate( _debug_tex == DEBUG_DEPTH_TEX );
	switch(_debug_tex)
	{
		case DEBUG_DEPTH_TEX:
			kinect.getDepthTextureReference().draw(0, 0, w, h);
			break;
		case DEBUG_HUE_TEX:
			mesh->get_hue_tex_ref().draw(0, 0, w, h);
			break;
		case DEBUG_NORMALS_TEX:
			mesh->get_normals_tex_ref().draw(0, 0, w, h);
			break;
	}
	
	glColor3f(1, 1, 0);
	ofCircle(800, 200, 60);
}

void testApp::render_3d(ofEventArgs &args)
{
//	light.setPosition(0, 0, 0.5+sin(ofGetElapsedTimef()*0.8)*0.5 );
//	//light.draw();
//	light.transformGL();
//	ofPushMatrix();
//	ofTranslate( light.getPosition() );
//	ofBox(.01);
//	ofDrawAxis(.03);
//	ofPopMatrix();
//	light.restoreTransformGL();
//	
//	glTranslatef(-0.3, 0.3, 1);
//	glColor3f(1, 1, 1);
//	glutWireTeapot(0.1);
//	
//	ofDisableLighting();
}

void testApp::render_2d(ofEventArgs &args)
{
	if (pressed[key::keyboard_help])
	{
		int roff = 200;
		int toff = roff+50;
		
		glColor4f(0, 0, 0, 0.1);
		ofRect(roff, roff, ofGetWidth()-roff*2, ofGetHeight()-roff*2);
		
		glColor3f(1, 1, 1);
		ofDrawBitmapString(key::get_help()+"\n\n"+camluc.get_keyboard_help(), 
						   toff, toff+12);	
	}				
}

bool testApp::init_kinect()
{
	//kinect.enableCalibrationUpdate(false);
	kinect.enableDepthNearValueWhite(false);
	
	kinect.init(false, true, true);
	kinect.setVerbose(false);
	kinect.open();
	
	return update_kinect();
}

bool testApp::update_kinect()
{
	if (!kinect.isConnected())
		return false;
	
	kinect.update();
	raw_depth_pix = kinect.getRawDepthPixels();
	rgb_pix = kinect.getPixels();
	
	return true;
}

void testApp::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
	
	ofRemoveListener(camluc.render_texture, this, &testApp::render_texture);
	ofRemoveListener(camluc.render_2d, this, &testApp::render_2d);
	ofRemoveListener(camluc.render_3d, this, &testApp::render_3d);
	
	camluc.dispose();
	kinect.close();
	
	delete mesh;
	mesh = NULL;
}

void testApp::keyPressed(int key)
{
	pressed[key] = true;
	
	if (key == key::fullscreen)
	{
		ofToggleFullscreen();
	}
	else if (key == key::projector)
	{
		if (ofGetWindowPositionX() == 0)
		{
			ofSetWindowPosition(1440,0);
		}
		else
		{
			ofSetWindowPosition(0,0);
		}
	}
	else if (key == key::toggle_debug)
	{
		camluc.toggle_debug();
	}
	else if (key == key::toggle_wireframe)
	{
		camluc.toggle_wireframe();
	}
	else if (key == key::debug_tex)
	{
		++_debug_tex;
		_debug_tex = _debug_tex == DEBUG_TEX_LENGTH ? 0 : _debug_tex;
	}
	else if (key == key::print_mesh)
	{
		mesh->print();
	}
}

void testApp::keyReleased(int key)
{
	pressed[key] = false;
}

void testApp::init_keys()
{
	for (int i = 0; i < 512; i++) 
		pressed[i] = false;
}

void testApp::mouseMoved(int x, int y )
{
	
}

void testApp::mouseDragged(int x, int y, int button)
{
	
}

void testApp::mousePressed(int x, int y, int button)
{
	
}

void testApp::mouseReleased(int x, int y, int button)
{
	
}

void testApp::resized(int w, int h)
{
	
}

void testApp::debug()
{
	
}
