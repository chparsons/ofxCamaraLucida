#define KINECT_W	640
#define KINECT_H	480
#define K1		0.1236
#define K2		2842.5
#define K3		1.1863
#define K4		0.0370

float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, int clamp) 
{
	if (fabs(inputMin - inputMax) < FLT_EPSILON){
		return outputMin;
	} else {
		float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
	
		if( clamp == 1 ){
			if(outputMax < outputMin){
				if( outVal < outputMax )outVal = outputMax;
				else if( outVal > outputMin )outVal = outputMin;
			}else{
				if( outVal > outputMax )outVal = outputMax;
				else if( outVal < outputMin )outVal = outputMin;
			}
		}
		return outVal;
	}
}

__kernel void update_vertex(__global float4* vbo_buff, __global uint* ibo_buff, __global const ushort* raw_depth_buff, const int mesh_step, const float cx_d, const float cy_d, const float fx_d, const float fy_d, const int xoffset)
{
	int vbo_idx = get_global_id(0);


	int mesh_w = KINECT_W/mesh_step;
	int mesh_h = KINECT_H/mesh_step;
	int vbo_length = mesh_w * mesh_h; 

	
	int mcol = vbo_idx % mesh_w;
	int mrow = (vbo_idx - mcol) / mesh_w;
	
	int col = mcol * mesh_step;
	int row = mrow * mesh_step;
	
	int depth_idx = row * KINECT_W + col;
	
	
	// set VBO pts
	
	ushort raw_depth = raw_depth_buff[depth_idx];
	
	float z = 5.0f; //base depth mts
	float r = 1.0f; 
	float g = 1.0f; 
	float b = 1.0f; 
	
	if (raw_depth < 1024)
	{
		float a = (float)raw_depth / K2;
		z = K1 * tan( (float)(a + K3) ) - K4; // calculate in meters
		
		//float hue = ofMap(z, 1.0f, 2.5f, 0.0f, 1.0f, 1);
		//HSVtoRGB(hue*360, 1, 1, &r, &g, &b);
	}
	
	float x = (col + xoffset - cx_d) * z / fx_d;
	float y = (row - cy_d) * z / fy_d;
	
	
	vbo_buff[vbo_idx].x = x;
	vbo_buff[vbo_idx].y = y;
	vbo_buff[vbo_idx].z = z;
	vbo_buff[vbo_idx].w = 0;
	
	
	//vbo_color[vbo_idx].set(r,g,b);
	
	
	// set IBO
	
	if ( ( mcol < mesh_w - 2 ) && ( mrow < mesh_h - 2 ) ) 
	{
		int ibo_idx = vbo_idx * 4;
		
		ibo_buff[ibo_idx+0] = (uint)( mrow * mesh_w + mcol );
		ibo_buff[ibo_idx+1] = (uint)( (mrow + mesh_step) * mesh_w + mcol );
		ibo_buff[ibo_idx+2] = (uint)( (mrow + mesh_step) * mesh_w + (mcol + mesh_step) );
		ibo_buff[ibo_idx+3] = (uint)( mrow * mesh_w + (mcol + mesh_step) );
	}
}


