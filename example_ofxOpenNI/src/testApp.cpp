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
	
	init_kinect();
	
	init_keys();
	
	string config = ofToDataPath("camara_lucida/camara_lucida_config.xml");
	
	const XnDepthPixel* depth_map = depth_generator.GetDepthMap(); 
	
	mesh = new cml::Mesh_openni( (uint16_t*)depth_map, &depth_generator );
	
	camluc.init(config, mesh);
	
	ofAddListener(camluc.render_texture, this, &testApp::render_texture);
	ofAddListener(camluc.render_2d, this, &testApp::render_2d);
}

void testApp::update()
{
	update_kinect();
	camluc.update();
}

void testApp::draw()
{	
	depth.draw(0, 0, 400, 300);
	camluc.render();
}

void testApp::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
	
	ofRemoveListener(camluc.render_texture, this, &testApp::render_texture);
	ofRemoveListener(camluc.render_2d, this, &testApp::render_2d);
	
	camluc.dispose();
	
	delete mesh;
	mesh = NULL;
}

void testApp::render_2d(ofEventArgs &args)
{
	if (pressed[key::keyboard_help])
	{
		int roff = 200;
		int toff = roff+50;
		
		glColor3f(1, 1, 1);
		ofDrawBitmapString(key::get_help()+"\n\n"+camluc.get_keyboard_help(), 
						   toff, toff+12);	
		
		glColor4f(0, 0, 0, 0.1);
		ofRect(roff, roff, ofGetWidth()-roff*2, ofGetHeight()-roff*2);
		
		glColor3f(1, 1, 1);
	}
	
    //if (_debug_tex == DEBUG_DEPTH_TEX)
    //{
        //depth.draw(0, 0, 400, 300);
    //}
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
	
	switch(_debug_tex)
	{
		case DEBUG_DEPTH_TEX:
			glColor3f(1, 1, 1);
			depth.draw(0, 0, w, h);
			break;
	}
	
	glColor3f(1, 1, 0);
	ofCircle(400, 200, 100);
}

void testApp::init_kinect()
{
	context.setupUsingXMLFile("");
	
	depth.setup(&context);	
	//user.setup(&context, &depth);
	
	depth_generator = depth.getXnDepthGenerator();
	
//	rgb.setup(&context);
//	rgb_generator = rgb.getXnImageGenerator();
//	depth_generator.GetAlternativeViewPointCap().SetViewPoint(rgb_generator);
}

void testApp::update_kinect()
{
	context.update();
	depth.update();
	//user.update();
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
