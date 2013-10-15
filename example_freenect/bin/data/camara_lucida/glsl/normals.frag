#version 120
#extension GL_ARB_texture_rectangle: enable

varying vec3 normal;

void main()
{
	vec3 n = normalize(normal);
	//vec3 n = normal;
	gl_FragColor = vec4(n.x, n.y, n.z, 1.0);
}
