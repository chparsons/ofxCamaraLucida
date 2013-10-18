#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle: enable
//#extension GL_ARB_draw_buffers: enable

uniform sampler2DRect depth_tex;
uniform sampler2DRect render_tex;

/* depth calib in mts */
uniform float near, far;
uniform float width, height;
uniform float fx, fy;
uniform float cx, cy;
uniform float k1, k2, k3, k4;

vec3 unproject( vec2 p2, float z ) 
{
  return vec3(
    (p2.x-cx) * z / fx, (p2.y-cy) * z / fy, z );
}

float lerp2d( float x, 
    float x1, float x2, 
    float y1, float y2 ) 
{
  return (x-x1) / (x2-x1) * (y2-y1) + y1;
}

float round( float n )
{
  return fract(n) < 0.5 ? floor(n) : ceil(n);
}

/*
 * WARNING
 * this interpolation is related 
 * to init_float_tex in cml::DepthCamera
 */
float z_norm_to_mts( float z_norm ) 
{
  return lerp2d(z_norm, 1., -0.1, near, far);
}

/*float z_raw_to_mts( float z_raw ) */
/*{*/
  /*return k1 * tan( (z_raw / k2) + k3 ) - k4;*/
/*}*/


void main()
{	

  vec2 render_tex_size = vec2( textureSize2DRect( render_tex, 0 ) );

  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec2 p2 = gl_TexCoord[0].st;

  vec2 d2 = vec2( 
    round(p2.x/render_tex_size.x * width),
    round(p2.y/render_tex_size.y * height) 
  );

  float depth = texture2DRect(depth_tex, d2).r;

  float z_mts = z_norm_to_mts( depth );

  vec4 p3 = vec4( unproject( d2, z_mts ), 1.);

  /*vec4 p3 = vec4( gl_Vertex );*/
  /*p3.z = z_mts;*/

  gl_Position = gl_ModelViewProjectionMatrix*p3;

  /*gl_Position = ftransform();*/

  gl_FrontColor  = gl_Color;
	/*gl_FrontColor  = vec4( lerp2d(z_mts,0.5,5.0,0.,1.) );*/
}



