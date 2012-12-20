#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "cmlConfig.h"
#include "cmlEvents.h"
#include "cmlOpticalDevice.h"
#include "cmlKinect.h"
#include "cmlCalibration.h"
#include "cmlMesh.h"
#include "cmlDepth2Mesh.h"
#include "cmlRenderer.h"

namespace cml
{
    class CamaraLucida
    {
        public:

            CamaraLucida( string config_path, Depth2Mesh* d2m ) : 
                render_texture( events.render_texture ),
                render_3d( events.render_3d ),
                render_2d( events.render_2d )
        {
            init( config_path, d2m );
        };

            ~CamaraLucida()
            {
                dispose(); 
            };

            void init( string config_path, Depth2Mesh* d2m )
            {
                this->d2m = d2m; 
                this->config_path = config_path;

                xml.loadFile(config_path);
                xml.pushTag("camaralucida");

                int k_xoff = xml.getValue("depth_xoff", -8); 

                init_keys(); 
                init_events();

                config = new Config( &xml );

                Calibration calib( config ); 

                proj = new OpticalDevice( calib.proj_config() );
                depth = new cml::Kinect( 
                        calib.depth_config(), k_xoff );
                rgb = new OpticalDevice( calib.rgb_config() );

                mesh = new Mesh( 
                        config->mesh_step, 
                        depth->width, depth->height,
                        config->tex_width, config->tex_height );

                renderer = new Renderer( config, 
                        proj, depth, rgb, &_debug );

                d2m->init( depth, mesh );

                _debug = false;
                _render_help = false;
            }; 

            void dispose()
            {
                dispose_events();

                delete config; config = NULL;
                delete proj; proj = NULL;
                delete depth; depth = NULL;
                delete rgb; rgb = NULL;
                delete renderer; renderer = NULL;
                delete mesh; mesh = NULL;

                //d2m->dispose();
                d2m = NULL;
            };

            void render()
            {
                renderer->render( &events, mesh );
                render_help();
            };

            void toggle_debug()
            {
                _debug = !_debug;
            };

            float tex_width() { return config->tex_width; };
            float tex_height() { return config->tex_height; };

            // events 

            ofEvent<ofEventArgs>& render_texture;
            ofEvent<ofEventArgs>& render_3d;
            ofEvent<ofEventArgs>& render_2d;

        private:

            Depth2Mesh* d2m;

            cml::Events events;
            OpticalDevice* proj;
            OpticalDevice* depth;
            OpticalDevice* rgb;
            Renderer* renderer;
            Mesh* mesh;

            Config *config;
            ofxXmlSettings xml;
            string config_path;

            bool _debug;
            bool _render_help;
            bool pressed[512];

            struct Key
            {
                char debug;
                char help;
                char view_next;
                char view_prev;
                char scene_reset;
                char scene_zoom;
                char xoff_inc;
                char xoff_dec; 
            };
            Key key;

            void init_keys()
            {
                for (int i = 0; i < 512; i++) 
                    pressed[i] = false;

                xml.pushTag( "ui" );

                key.debug = xml.getValue("debug","")[0];
                key.help = xml.getValue("help","")[0];

                key.xoff_inc = xml.getValue("depth_xoff:inc","")[0]; 
                key.xoff_dec = xml.getValue("depth_xoff:dec","")[0];

                key.view_next = xml.getValue("viewpoint:next","")[0];
                key.view_prev = xml.getValue("viewpoint:prev","")[0];

                key.scene_reset = xml.getValue("scene_ctrl:reset","")[0];
                key.scene_zoom = xml.getValue("scene_ctrl:zoom","")[0];
                xml.popTag();
            };

            void keyPressed(ofKeyEventArgs &args)
            {
                pressed[args.key] = true;

                if (args.key == key.debug)
                {
                    toggle_debug();
                }
                else if (args.key == key.help)
                {
                    _render_help = !_render_help;
                }

                if (!_debug) 
                    return;

                if (args.key == key.view_next)
                {
                    renderer->next_view();        
                }
                else if (args.key == key.view_prev)
                {
                    renderer->prev_view(); 
                }
                else if (args.key == key.scene_reset)
                {
                    renderer->reset_scene();
                }
                else if (args.key == key.xoff_inc)
                {
                    ((cml::Kinect*)depth)->change_xoff( 1 );
                }
                else if (args.key == key.xoff_dec)
                {
                    ((cml::Kinect*)depth)->change_xoff( -1 );
                }
            };

            void keyReleased(ofKeyEventArgs &args)
            {
                pressed[args.key] = false;
            };

            void mousePressed(ofMouseEventArgs &args)
            {
                renderer->mousePressed( args.x, args.y );
            };

            void mouseDragged(ofMouseEventArgs &args)
            {
                renderer->mouseDragged( args.x, args.y, 
                        pressed[key.scene_zoom] );
            };

            void render_help()
            {
                if ( ! _render_help ) return;

                string d = "Camara Lucida \n www.camara-lucida.com.ar \n www.chparsons.com.ar \n\n config file = "+config_path+" \n\n debug = "+string(1, key.debug)+" \n\n next viewpoint = "+string(1, key.view_next)+" \n prev viewpoint = "+string(1, key.view_prev)+" \n\n drag mouse to rotate \n\t zoom = "+string(1, key.scene_zoom)+" + drag \n\t reset = "+string(1, key.scene_reset)+" \n\n depth_xoff inc = "+string(1, key.xoff_inc)+" \n depth_xoff dec = "+string(1, key.xoff_dec);

                int roff = 200;
                int toff = roff+50;
                int rw = ofGetWidth()-roff*2;
                int rh = ofGetHeight()-roff*2;

                ofEnableAlphaBlending();  
                glColor4f(0, 0, 0, 0.7);
                ofRect(roff, roff, rw, rh);
                glColor3f(1, 1, 1);
                ofDrawBitmapString( d, toff, toff+12 );
                ofDisableAlphaBlending(); 
            };

            void init_events()
            {
                ofAddListener(ofEvents().keyPressed, this, 
                        &CamaraLucida::keyPressed);

                ofAddListener(ofEvents().keyReleased, this, 
                        &CamaraLucida::keyReleased);

                ofAddListener(ofEvents().mouseDragged, this, 
                        &CamaraLucida::mouseDragged);

                ofAddListener(ofEvents().mousePressed, this, 
                        &CamaraLucida::mousePressed);
            };

            void dispose_events()
            {
                ofRemoveListener(ofEvents().keyPressed, this, 
                        &CamaraLucida::keyPressed);

                ofRemoveListener(ofEvents().keyReleased, this, 
                        &CamaraLucida::keyReleased);

                ofRemoveListener(ofEvents().mouseDragged, this, 
                        &CamaraLucida::mouseDragged);

                ofRemoveListener(ofEvents().mousePressed, this, 
                        &CamaraLucida::mousePressed);
            };
    };
};



