#version 120
#extension GL_ARB_texture_rectangle: enable

uniform sampler2DRect tex0;

void main()
{
	vec4 color = texture2DRect(tex0, gl_TexCoord[0].st);
	gl_FragColor = color * gl_Color;
}
