#version 120
#extension GL_ARB_texture_rectangle: enable

uniform sampler2DRect render_tex;

void main()
{

  vec2 p2 = gl_TexCoord[0].st;
  vec4 color = texture2DRect( render_tex, p2 );

  /*gl_FragColor = color * gl_Color;*/
  gl_FragColor = color;

}

