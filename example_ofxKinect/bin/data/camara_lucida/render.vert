#version 120
#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_texture_rectangle: enable
//#extension GL_ARB_draw_buffers: enable

//#define KINECT_W		640.0
//#define KINECT_H		480.0
//#define BASE_RAW_DEPTH 	1024.0 //5mts~
//#define BASE_DEPTH_MTS	(k1 * tan( _raw_depth / k2 + k3 ) - k4)
//#define k1				0.1236
//#define k2				2842.5
//#define k3				1.1863
//#define k4				0.0370

//attribute float raw_depth;
//float vbo_idx;

//uniform float mesh_step;
//uniform float depth_xoff;
//uniform float fx_d;
//uniform float fy_d;
//uniform float cx_d;
//uniform float cy_d;
//
//float raw_depth_to_meters(float _raw_depth)
//{
//	if (_raw_depth < BASE_RAW_DEPTH)
//	{
//		return k1 * tan( _raw_depth / k2 + k3 ) - k4; // calculate in meters
//	}
//	return BASE_DEPTH_MTS;
//}

//varying vec3 normal;

void main()
{	
//	vbo_idx = float(gl_VertexID);
//
//	float mesh_w = KINECT_W/mesh_step;
//	float mesh_h = KINECT_H/mesh_step;
//	float vbo_length = mesh_w * mesh_h; 
//	
//	float mcol = mod(vbo_idx, mesh_w); //vbo_idx % mesh_w;
//	float mrow = (vbo_idx - mcol) / mesh_w;
//	
//	float col = mcol * mesh_step;
//	float row = mrow * mesh_step;
//	
//	float z = raw_depth_to_meters(raw_depth);
//	float x = (col + depth_xoff - cx_d) * z / fx_d;
//	float y = (row - cy_d) * z / fy_d;


	//normal = normalize(gl_NormalMatrix * gl_Normal);
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	//vec4 v = vec4(gl_Vertex);
	//v.z = raw_depth * 0.001;
	
//	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * v;
//	gl_Position = gl_ModelViewProjectionMatrix * v;
	gl_Position = ftransform();
	
	gl_FrontColor  = gl_Color;
}



