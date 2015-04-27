#include "cml/CamaraLucida.h"

namespace cml
{

  CamaraLucida::CamaraLucida() : 
    render_texture(events.render_texture),
    render_3d( events.render_3d ),
    render_2d( events.render_2d )
  {
    cml::Config config;
    init( config );
  };

  CamaraLucida::CamaraLucida( cml::Config config ) : 
    render_texture(events.render_texture),
    render_3d( events.render_3d ),
    render_2d( events.render_2d )
  {
    init( config );
  };

  CamaraLucida::~CamaraLucida(){}; 

  void CamaraLucida::dispose()
  {
    ofLog(OF_LOG_VERBOSE, "cml::CamaraLucida::dispose");

    dispose_events();

    delete proj; proj = NULL;
    delete depth; depth = NULL;
    //delete rgb; rgb = NULL;
    delete renderer; renderer = NULL;
    delete mesh; mesh = NULL;
  };

  void CamaraLucida::render()
  { 
    renderer->render( &events, mesh, depth_ftex, gpu(), wireframe() );
    render_screenlog();
    render_help();
  };

  void CamaraLucida::update( uint16_t *depth_pix_mm )
  {
    if ( _gpu ) 
      update_gpu(depth_pix_mm);
    else 
      update_cpu(depth_pix_mm);
  };

  void CamaraLucida::update_gpu( uint16_t *depth_pix_mm )
  {
    depth_ftex = depth->get_float_tex_ref( depth_pix_mm );
  }; 

  void CamaraLucida::update_cpu( uint16_t *depth_pix_mm )
  {
    mesh->update( depth_pix_mm, depth );
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

  void CamaraLucida::init( cml::Config config )
  {
    ofLogVerbose("CamaraLucida")<<"init";

    this->config = config; 

    init_keys(); 
    init_events();

    OpticalDevice::Config proj_cfg;
    OpticalDevice::Config depth_cfg;
    //OpticalDevice::Config rgb_cfg;

    CalibrationParser calib;
    calib.parse(config, proj_cfg, depth_cfg); //rgb_cfg ); 

    depth = new DepthCamera( depth_cfg );
    proj = new OpticalDevice( proj_cfg );
    //rgb = new OpticalDevice( rgb_cfg );

    depth->log(OF_LOG_NOTICE);
    proj->log(OF_LOG_NOTICE);

    _tex_width = config.tex_width;
    _tex_height = config.tex_height;
    _depth_width = depth->width;
    _depth_height = depth->height;

    mesh = new Mesh( 
      config.mesh_res, 
      _depth_width, 
      _depth_height,
      _tex_width, 
      _tex_height );

    renderer = new Renderer( config, proj, depth );//rgb);

    depth_ftex = depth->init_float_tex();

    _gpu = true;
    _wire = false;
    _debug = false;
    _render_help = false;

  };

  void CamaraLucida::init_keys()
  {
    for (int i = 0; i < 512; i++) 
      pressed[i] = false;
  };

  void CamaraLucida::keyPressed(ofKeyEventArgs &args)
  {
    pressed[args.key] = true;

    if (args.key == config.key_debug)
    {
      debug( ! debug() );
    }
    else if (args.key == config.key_help)
    {
      _render_help = !_render_help;
    }

    if ( ! _debug ) 
      return;

    if (args.key == config.key_view_next)
    {
      renderer->next_view();        
    }
    //else if (args.key == config.key_view_prev)
    //{
      //renderer->prev_view(); 
    //}
    else if (args.key == config.key_scene_reset)
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
    renderer->mouseDragged( args.x, args.y, pressed[config.key_scene_zoom] );
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

    ofDrawBitmapString( view + " / fps: " + ofToString(fps) + " / gpu: " + (gpu()?"on":"off"), 10, ofGetHeight()-10);

    ofDisableAlphaBlending(); 
  };

  void CamaraLucida::render_help()
  {
    if ( ! _render_help ) return;

    string d = "Camara Lucida \n http://chparsons.com.ar/ \n\n keys: \n\n debug = "+string(1, config.key_debug)+" \n\n change viewpoint = "+string(1, config.key_view_next)+" \n\n drag mouse to rotate \n\t zoom = "+string(1, config.key_scene_zoom)+" + drag \n\t reset = "+string(1, config.key_scene_reset);

    int roff = 200;
    int toff = roff+50;
    int rw = ofGetWidth()-roff*2;
    int rh = ofGetHeight()-roff*2;

    ofEnableAlphaBlending();  
    glColor4f(0, 0, 0, 0.7);
    ofRect(roff, roff, rw, rh);
    glColor3f(1, 1, 1);
    ofDrawBitmapString(d, toff, toff+12);
    ofDisableAlphaBlending(); 
  };

  void CamaraLucida::init_events()
  {
    ofAddListener(ofEvents().keyPressed, this, &CamaraLucida::keyPressed);

    ofAddListener(ofEvents().keyReleased, this, &CamaraLucida::keyReleased);

    ofAddListener(ofEvents().mouseDragged, this, &CamaraLucida::mouseDragged);

    ofAddListener(ofEvents().mousePressed, this, &CamaraLucida::mousePressed);
  };

  void CamaraLucida::dispose_events()
  {
    ofRemoveListener(ofEvents().keyPressed, this, &CamaraLucida::keyPressed);

    ofRemoveListener(ofEvents().keyReleased, this, &CamaraLucida::keyReleased);

    ofRemoveListener(ofEvents().mouseDragged, this, &CamaraLucida::mouseDragged);

    ofRemoveListener(ofEvents().mousePressed, this, &CamaraLucida::mousePressed);
  };
};





