#pragma once
#define STRINGIFY(x) #x

#include "ofMain.h"
#include "cml/DepthCamera.h"

namespace cml
{

class RenderShader
{
  public: 

  string fragment()
  {
    return "#version 120\n #extension GL_ARB_texture_rectangle : enable\n " STRINGIFY(

    uniform sampler2DRect render_tex;

    void main()
    {
      vec2 p2 = gl_TexCoord[0].st;
      vec4 color = texture2DRect( render_tex, p2 );
      //gl_FragColor = color * gl_Color;
      gl_FragColor = color;
    }

    ); //shader code
  };

  string vertex()
  {
    return "#version 120\n #extension GL_EXT_gpu_shader4 : enable\n #extension GL_ARB_texture_rectangle : enable\n " STRINGIFY(

    uniform sampler2DRect depth_tex;
    uniform sampler2DRect render_tex;

    //depth calib in mts 
    uniform float xoff;
    uniform float near;
    uniform float far;
    uniform float width;
    uniform float height;
    uniform float fx;
    uniform float fy;
    uniform float cx;
    uniform float cy;
    uniform float k1;
    uniform float k2;
    uniform float k3;
    uniform float k4; 

    float lerp2d( float x, float x1, float x2, float y1, float y2 ) 
    {
      return (x-x1) / (x2-x1) * (y2-y1) + y1;
    }

    float round( float n )
    {
      return fract(n) < 0.5 ? floor(n) : ceil(n);
    }

    vec3 unproject( vec2 p2, float z ) 
    {
      return vec3( (p2.x + xoff - cx) * z / fx, (p2.y - cy) * z / fy, z );
    }

    //cml::DepthCamera::init_float_tex 
    //float z_norm_to_mts( float z_norm ) 
    //{
      //return lerp2d( z_norm, 0.0, 1.0, near, far );
    //}  

    //float z_raw_to_mts( float z_raw ) 
    //{
      //return k1 * tan( (z_raw / k2) + k3 ) - k4;
    //}


    void main()
    {	
      vec2 render_tex_size = vec2( textureSize2DRect( render_tex, 0 ) );

      gl_TexCoord[0] = gl_MultiTexCoord0;

      vec2 p2 = gl_TexCoord[0].st;

      //render tex coord to depth coord
      vec2 d2 = vec2( 
        p2.x / render_tex_size.x * width,
        p2.y / render_tex_size.y * height
      );

      float depth = texture2DRect( depth_tex, d2 ).r;

      //float zmts = z_norm_to_mts(depth);

      //tex depth in mm 
      float zmts = depth / 1000.0; 

      //see cml::Mesh::update
      zmts = clamp( ( zmts == 0.0 ? 5.0 : zmts ), 0.0, 5.0 );

      vec4 p3 = vec4( unproject( d2, zmts ), 1.);

      //vec4 p3 = vec4( gl_Vertex );
      //p3.z = zmts;

      gl_Position = gl_ModelViewProjectionMatrix * p3;

      //gl_Position = ftransform();

      //Values written to gl_FrontColor are clamped to the range [0,1]
      gl_FrontColor = gl_Color;
      //gl_FrontColor = vec4( lerp2d(zmts,0.5,5.0,0.,1.) );
    }

    ); //shader code
  };

  void init( ofShader& shader )
  {
    shader.setupShaderFromSource( GL_FRAGMENT_SHADER, fragment() );
    shader.setupShaderFromSource( GL_VERTEX_SHADER, vertex() );
    shader.linkProgram();
  };

  void update( ofShader& shader, DepthCamera* depth, ofTexture& render_tex, ofTexture& depth_ftex )
  {
    ofVec4f& k = depth->k;

    shader.setUniform1f("width", depth->width);
    shader.setUniform1f("height", depth->height);
    shader.setUniform1f("xoff", depth->xoff);
    shader.setUniform1f("near", depth->near);
    shader.setUniform1f("far", depth->far);
    shader.setUniform1f("cx", depth->cx);
    shader.setUniform1f("cy", depth->cy);
    shader.setUniform1f("fx", depth->fx);
    shader.setUniform1f("fy", depth->fy);
    shader.setUniform1f("k1", k[0]);
    shader.setUniform1f("k2", k[1]);
    shader.setUniform1f("k3", k[2]);
    shader.setUniform1f("k4", k[3]);

    shader.setUniformTexture( "render_tex", render_tex, 0 );
    shader.setUniformTexture( "depth_tex", depth_ftex, 1 );

  };

};

};

