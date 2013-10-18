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

#include "cmlCamaraLucida.h"

namespace cml
{
  CamaraLucida::CamaraLucida(string cfg_path) : 
    render_texture( events.render_texture ),
    render_3d( events.render_3d ),
    render_2d( events.render_2d )
  {
    init( cfg_path );
  };

  CamaraLucida::~CamaraLucida(){}; 

  void CamaraLucida::dispose()
  {
    ofLog(OF_LOG_VERBOSE,
        "cml::CamaraLucida::dispose");

    dispose_events();

    //delete config; config = NULL;
    delete proj; proj = NULL;
    delete depth; depth = NULL;
    delete rgb; rgb = NULL;
    delete renderer; renderer = NULL;
    delete mesh; mesh = NULL;
  };

  void CamaraLucida::render()
  { 
    renderer->render( 
        &events, 
        mesh, 
        depth_ftex,
        gpu(),
        wireframe() );

    render_screenlog();
    render_help();
  };

  void CamaraLucida::update(
      uint16_t *mm_depth_pix )
  {
    if ( _gpu ) update_gpu( mm_depth_pix );
    else update_cpu( mm_depth_pix );
  };

  void CamaraLucida::update_gpu(
      uint16_t *mm_depth_pix )
  {
    depth_ftex = depth->get_float_tex_ref( 
        mm_depth_pix );
  };

  void CamaraLucida::update_cpu(
      uint16_t *mm_depth_pix )
  {

    int len = mesh->length();

    for ( int i = 0; i < len; i++ )
    {
      int xdepth, ydepth, idepth;

      mesh->to_depth( i, 
          &xdepth, &ydepth, &idepth );

      //uint16_t raw_depth=raw_depth_pix[idepth];
      //float z = depth->z_mts(raw_depth);

      // mm to mts
      float zmts = mm_depth_pix[idepth]*0.001;
      zmts = CLAMP((zmts==0.?5.:zmts),0.,5.);

      float x, y;

      depth->unproject(
          xdepth, ydepth, zmts, &x, &y );

      mesh->set_vertex( i, x, y, zmts );

    }

    mesh->update();
  };

  void CamaraLucida::toggle_debug()
  {
    debug( ! debug() );
  }; 

  void CamaraLucida::debug( bool val )
  {
    _debug = val;
    renderer->debug( _debug );
  };

  bool CamaraLucida::debug()
  {
    return _debug;
  };

  float CamaraLucida::tex_width() 
  { 
    return config->tex_width; 
  };

  float CamaraLucida::tex_height() 
  { 
    return config->tex_height; 
  };

  void CamaraLucida::init( string cfg_path )
  {
    ofLog(OF_LOG_VERBOSE,
        "cml::CamaraLucida::init");

    _gpu = true;

    this->cfg_path = cfg_path;

    xml.loadFile(cfg_path);
    xml.pushTag("camaralucida");

    init_keys(); 
    init_events();

    config = new Config( &xml );

    OpticalDevice::Config proj_cfg;
    OpticalDevice::Config depth_cfg;
    OpticalDevice::Config rgb_cfg;

    Calibration calib( 
        config, proj_cfg, depth_cfg, rgb_cfg ); 

    proj = new OpticalDevice( proj_cfg );
    depth = new cml::DepthCamera( depth_cfg );
    rgb = new OpticalDevice( rgb_cfg );

    mesh = new Mesh( 
      config->mesh_step, 
      depth->width(), depth->height(),
      config->tex_width, config->tex_height );

    renderer = new Renderer(
        config, proj, depth, rgb );

    depth_ftex = depth->get_float_tex_ref();

    _wire = false;
    _debug = false;
    _render_help = false;

  };

  void CamaraLucida::init_keys()
  {
    for (int i = 0; i < 512; i++) 
      pressed[i] = false;

    xml.pushTag( "ui" );

    key.debug = xml.getValue("debug","")[0];
    key.help = xml.getValue("help","")[0];

    key.view_next = xml.getValue(
        "viewpoint:next","")[0];
    key.view_prev = xml.getValue(
        "viewpoint:prev","")[0];

    key.scene_reset = xml.getValue(
        "scene_ctrl:reset","")[0];
    key.scene_zoom = xml.getValue(
        "scene_ctrl:zoom","")[0];

    xml.popTag();
  };

  void CamaraLucida::keyPressed(ofKeyEventArgs &args)
  {
    pressed[args.key] = true;

    if (args.key == key.debug)
    {
      debug( ! debug() );
    }
    else if (args.key == key.help)
    {
      _render_help = !_render_help;
    }

    if ( ! _debug ) 
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
  };

  void CamaraLucida::keyReleased(ofKeyEventArgs &args)
  {
    pressed[args.key] = false;
  };

  void CamaraLucida::mousePressed(ofMouseEventArgs &args)
  {
    renderer->mousePressed( args.x, args.y );
  };

  void CamaraLucida::mouseDragged(ofMouseEventArgs &args)
  {
    renderer->mouseDragged( args.x, args.y, 
        pressed[key.scene_zoom] );
  };

  void CamaraLucida::render_screenlog()
  {
    if ( ! _debug ) return;

    string view = renderer->get_viewpoint_info();
    float fps = ofGetFrameRate();

    ofEnableAlphaBlending();  
    glColor4f(0, 0, 0, 0.7);
    ofRect(0, 
        ofGetHeight()-25, 
        ofGetWidth(), 25);
    glColor3f(1, 1, 1);

    ofDrawBitmapString( view + " / fps: " + ofToString(fps) + " / gpu " + ofToString(gpu()), 10, ofGetHeight()-10);

    ofDisableAlphaBlending(); 
  };

  void CamaraLucida::render_help()
  {
    if ( ! _render_help ) return;

    string d = "Camara Lucida \n www.camara-lucida.com.ar \n www.chparsons.com.ar \n\n config file = "+cfg_path+" \n\n debug = "+string(1, key.debug)+" \n\n next viewpoint = "+string(1, key.view_next)+" \n prev viewpoint = "+string(1, key.view_prev)+" \n\n drag mouse to rotate \n\t zoom = "+string(1, key.scene_zoom)+" + drag \n\t reset = "+string(1, key.scene_reset);

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

  void CamaraLucida::init_events()
  {
    ofAddListener(ofEvents().keyPressed, 
        this, &CamaraLucida::keyPressed);

    ofAddListener(ofEvents().keyReleased, 
        this, &CamaraLucida::keyReleased);

    ofAddListener(ofEvents().mouseDragged, 
        this, &CamaraLucida::mouseDragged);

    ofAddListener(ofEvents().mousePressed, 
        this, &CamaraLucida::mousePressed);
  };

  void CamaraLucida::dispose_events()
  {
    ofRemoveListener(ofEvents().keyPressed, 
        this, &CamaraLucida::keyPressed);

    ofRemoveListener(ofEvents().keyReleased, 
        this, &CamaraLucida::keyReleased);

    ofRemoveListener(ofEvents().mouseDragged, 
        this, &CamaraLucida::mouseDragged);

    ofRemoveListener(ofEvents().mousePressed, 
        this, &CamaraLucida::mousePressed);
  };
};





