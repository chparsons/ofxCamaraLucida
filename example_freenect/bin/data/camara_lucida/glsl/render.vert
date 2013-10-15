#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle: enable
//#extension GL_ARB_draw_buffers: enable

vec2 render_tex_size = vec2( 1024., 768. );

uniform sampler2DRect depth_tex;
/*uniform sampler2DRect render_tex;*/

float width = 640.;
float height = 480.;
float near_mm = 500.;
float far_mm = 4000.;
float fx = 595.796;
float fy = 597.563;
float cx = 309.162;
float cy = 246.334;
float k1 = 0.1236;
float k2 = 2842.5;
float k3 = 1.1863;
float k4 = 0.0370;
float depth_xoff = 0.;

vec3 unproject( vec2 p2, float z ) 
{

  return vec3(
    (p2.x + depth_xoff - cx) * z / fx,
    (p2.y - cy) * z / fy,
    z
  );

}

float lerp( float n, float min, float max ) 
{
  return (1.0 - n) * (max - min) + min;
}

float lerp2d( float x, float x1, float x2, float y1, float y2 ) 
{
  return (x-x1) / (x2-x1) * (y2-y1) + y1;
}

float z_norm_to_mm( float z_norm ) 
{
  return lerp2d( z_norm,1.,-1.,near_mm,far_mm );
}

float z_raw_to_mm( float z_raw ) 
{
  return ( k1 * tan( ( z_raw / k2 ) + k3 ) - k4 ) * 1000.;
}


void main()
{	

  /*ivec2 depth_size = textureSize2DRect( depth_tex );*/
  /*ivec2 render_tex_size = textureSize2DRect( render_tex );*/

  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec2 p2 = gl_TexCoord[0].st;
  vec2 d2 = vec2( 
    p2.x / render_tex_size.x * width,
    p2.y / render_tex_size.y * height );
  float depth = texture2DRect(depth_tex, d2).r;
  float z_mm = z_norm_to_mm( depth );
  float z_mts = z_mm * 0.001;
  vec4 p3 = vec4( unproject( d2, z_mts ), 1.);
  
  /*vec4 p3 = vec4( gl_Vertex );*/
  /*p3.z = z_mts;*/
	
  gl_Position = gl_ModelViewProjectionMatrix*p3;

  /*gl_Position = ftransform();*/

	gl_FrontColor  = gl_Color;
}



