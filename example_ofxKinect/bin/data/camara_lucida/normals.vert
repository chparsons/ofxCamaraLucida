#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle: enable

varying vec3 normal;

void main()
{	
	//normal = normalize(gl_NormalMatrix * gl_Normal);
	normal = normalize(gl_Normal);
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor  = gl_Color;
	
	gl_Position = ftransform();
}



