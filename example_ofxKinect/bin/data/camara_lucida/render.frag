#version 120
#extension GL_ARB_texture_rectangle: enable

uniform sampler2DRect render_tex;
//varying vec3 normal;

void main()
{
	//vec3 n = normalize(normal);
	
	vec4 color = texture2DRect(render_tex, gl_TexCoord[0].st);
	gl_FragColor = color * gl_Color;
}
