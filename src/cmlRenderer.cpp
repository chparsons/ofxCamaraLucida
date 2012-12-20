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

#include "cmlRenderer.h"

namespace cml 
{
    Renderer::Renderer( 
            cml::Config* config,
            OpticalDevice* proj, 
            OpticalDevice* depth, 
            OpticalDevice* rgb,
            bool *debug )
    {
        this->proj = proj;
        this->depth = depth;
        this->rgb = rgb;
        this->debug = debug;

        ofFbo::Settings s;
        s.width			    = config->tex_width;
        s.height		    = config->tex_height;
        s.numSamples		= config->tex_nsamples;
        s.numColorbuffers	= 1;
        s.internalformat	= GL_RGBA;

        fbo.allocate(s);
        //shader.load(config->render_shader_path);

		init_gl_scene_control();
    };

    Renderer::~Renderer()
    {
        dispose(); 
    }

    void Renderer::dispose()
	{
        proj = NULL;
        depth = NULL;
        rgb = NULL;
        debug = NULL;
    }

    void Renderer::render( cml::Events *ev, Mesh *mesh )
    {
        // texture

        fbo.bind();
        //ofEnableAlphaBlending();  
        //glEnable(GL_BLEND);  
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA); 

        ofNotifyEvent( ev->render_texture, ev->void_args );

        fbo.unbind();
        //ofDisableAlphaBlending(); 
        //glDisable(GL_BLEND);  			

        // gl init

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3d

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, ofGetWidth(), ofGetHeight());

        //if (!_wireframe)
            glPolygonMode(GL_FRONT, GL_FILL);
        //else
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

        glColor3f(1, 1, 1);

        gl_projection();	
        gl_viewpoint();
        gl_scene_control();

        if ( *debug )
        {
            render_world_CS();
            render_proj_CS();
            render_rgb_CS();
            render_proj_ppal_point();
        }

        //glEnable(GL_BLEND);  
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
        //glBlendFuncSeparate(GL_ONE, GL_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
        //ofEnableAlphaBlending();

        //shader.begin();
        fbo.getTextureReference(0).bind();

        //shader.setUniform1i("render_tex", 0);
        //shader.setUniformTexture("normals_tex", mesh->get_normals_tex_ref(), 1);


        mesh->render();
        //ofNotifyEvent( ev->render_mesh, ev->void_args );


        fbo.getTextureReference(0).unbind();
        //shader.end();

        //glDisable(GL_BLEND);
        //ofDisableAlphaBlending(); 

        glScalef( 1, -1, 1 );

        ofNotifyEvent( ev->render_3d, ev->void_args );

        // 2d hud

        //glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT, GL_FILL);
        glColor3f(1, 1, 1);

        gl_ortho();

        ofNotifyEvent( ev->render_2d, ev->void_args );

        render_screenlog();
    }

    // gl

    void Renderer::gl_ortho()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, ofGetWidth(), ofGetHeight(), 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void Renderer::gl_projection()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        switch(_viewpoint)
        {
            case V_PROJ:
                glMultMatrixf( proj->gl_projection_matrix() );
                break;
            case V_DEPTH:
                glMultMatrixf( depth->gl_projection_matrix() );
                break;
            case V_RGB:
                glMultMatrixf( rgb->gl_projection_matrix() );
                break;
        }
    }

    void Renderer::gl_viewpoint()
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glScalef( -1., -1., 1. );	

        switch(_viewpoint)
        {
            case V_DEPTH:			
                gluLookAt(
                        0., 0., 0,		//loc
                        0., 0., 1.,		//target
                        0., 1., 0.		//up			
                        );
                break;

            case V_PROJ:
                gluLookAt(
                        proj->loc.x, proj->loc.y, proj->loc.z,
                        proj->trg.x, proj->trg.y, proj->trg.z,
                        proj->up.x, proj->up.y, proj->up.z 
                        );
                break;

            case V_RGB:
                gluLookAt(
                        rgb->loc.x, rgb->loc.y, rgb->loc.z,
                        rgb->trg.x, rgb->trg.y, rgb->trg.z,
                        rgb->up.x, rgb->up.y, rgb->up.z
                        );
                break;
        }
    }

    // scene control

    void Renderer::gl_scene_control()
    {
        glTranslatef( 0, 0, tZ );
        glTranslatef( rot_pivot.x, rot_pivot.y, rot_pivot.z );
        glRotatef( rotX, 1, 0, 0);
        glRotatef( rotY, 0, 1, 0);
        glRotatef( rotZ, 0, 0, 1);
        glTranslatef( -rot_pivot.x, -rot_pivot.y, -rot_pivot.z );
    }

    void Renderer::init_gl_scene_control()
    {
        float *RT = proj->gl_modelview_matrix();
        rot_pivot = ofVec3f( RT[12], RT[13], RT[14] );

        pmouse = ofVec2f();

        tZ_delta = -0.05;
        rot_delta = -0.2;

        tZini = 0;
        rotXini = 0;
        rotYini = 0;
        rotZini = 0;

        reset_scene();
    }

    void Renderer::reset_scene()
    {
        tZ = tZini;
        rotX = rotXini;
        rotY = rotYini;
        rotZ = rotZini;
    }

    void Renderer::next_view()
    {
        ++_viewpoint;
        _viewpoint = _viewpoint == V_LENGTH ? 0 : _viewpoint;
    }

    void Renderer::prev_view()
    {
        --_viewpoint;
        _viewpoint = _viewpoint == -1 ? V_LENGTH-1 : _viewpoint;
    }

    void Renderer::render_proj_ppal_point()
	{
		glPointSize(3);
		glColor3f(1, 1, 0); //yellow
		
		float ts = 0.5;
		
		glPushMatrix();
			glMultMatrixf( proj->gl_modelview_matrix() );
			
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

    void Renderer::render_world_CS()
	{
		render_axis(0.1);
	}

    void Renderer::render_proj_CS()
	{
		glPushMatrix();
		glMultMatrixf( proj->gl_modelview_matrix() );
		render_axis(0.1);
		glPopMatrix();
	}

	void Renderer::render_rgb_CS()
	{
		glPushMatrix();
		glMultMatrixf( rgb->gl_modelview_matrix() );
		render_axis(0.05);
		glPopMatrix();
	}

	void Renderer::render_axis(float s)
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

	void Renderer::render_screenlog()
	{
		if ( ! *debug) return;
        ofEnableAlphaBlending();  
        glColor4f(0, 0, 0, 0.7);
        ofRect(0, ofGetHeight()-25, ofGetWidth(), 25);
        glColor3f(1, 1, 1);
		ofDrawBitmapString(_viewpoint_str()+" / fps: "+ofToString(ofGetFrameRate()), 10, ofGetHeight()-10);
        ofDisableAlphaBlending(); 
	}

	string Renderer::_viewpoint_str()
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

	void Renderer::mouseDragged(int x, int y, bool zoom)
	{
		if ( ! *debug) return;
		
		ofVec2f m = ofVec2f( x, y );
		ofVec2f dist = m - pmouse;
		
		if ( zoom )
		{
			tZ += -dist.y * tZ_delta;	
		}
		else
		{
			rotX -= dist.y * rot_delta;
			rotY += dist.x * rot_delta;
		}

		pmouse.set( x, y );
	}

	void Renderer::mousePressed(int x, int y)
	{
		pmouse.set( x, y );	
	}
};
