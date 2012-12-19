#pragma once

namespace cml
{
    class OpticalDevice
    {
        public:

            struct Config
            {
                int width, height;
                float near, far;
                float cx, cy, fx, fy;
                ofVec3f X, Y, Z, T;
            };

            OpticalDevice(){};

            OpticalDevice( const OpticalDevice::Config& config )
            {
                this->width = config.width;
                this->height = config.height;
                this->near = config.near;
                this->far = config.far;
                this->cx = config.cx;
                this->cy = config.cy;
                this->fx = config.fx;
                this->fy = config.fy;

                KK_cv2gl();
                RT_cv2gl( config.X, config.Y, config.Z, config.T );

                loc = ofVec3f( RT[12], RT[13], RT[14] );	
                fwd = ofVec3f( RT[8], RT[9], RT[10] );
                up = ofVec3f( RT[4], RT[5], RT[6] );
                trg = loc + fwd;

                //_T = ofVec3f( RT[12], rgb_RT[13], rgb_RT[14] );
                //_RT = ofMatrix4x4(
                //RT[0],  RT[1],  RT[2],  RT[12],
                //RT[4],  RT[5],  RT[6],  RT[13],
                //RT[8],  RT[9],  RT[10], RT[14],
                //0.,     0.,	    0.,	    1.
                //);                
                //_R.preMultTranslate(-_T);
                //_R = ofMatrix4x4::getTransposedOf(_R);
            }; 

            virtual ~OpticalDevice(){};

            virtual void unproject(int x2d, int y2d, 
                    float z, float *x, float *y)
            {
                *x = (x2d - cx) * z / fx;
                *y = (y2d - cy) * z / fy;
            };

            virtual ofVec2f project(const ofVec3f& p3)
            {
                ofVec2f p2;
                p2.x = (p3.x * fx / p3.z) + cx;
                p2.y = (p3.y * fy / p3.z) + cy;
                return p2;
            };

            int to_idx(int x, int y)
            {
                return y * width + x;
            };

            void to_xy(int idx, int& x, int& y)
            {
                x = idx % width;
                y = (idx - x) / width;
            }

            int width, height;
            float near, far;	
            float cx, cy, fx, fy;

            ofVec3f loc, fwd, up, trg;        

            float* gl_projection_matrix() { return KK; };
            float* gl_modelview_matrix() { return RT; };

        private:

            float KK[16]; //glMultMatrixf( KK )
            float RT[16]; //glMultMatrixf( RT )

            /*
             * Intrinsics from opencv to opengl
             *
             * http://opencv.willowgarage.com/wiki/Posit
             * http://www.songho.ca/opengl/gl_projectionmatrix.html
             * http://www.songho.ca/opengl/gl_transform.html
             */

            void KK_cv2gl()
            {
                float A = 2. * fx / width;
                float B = 2. * fy / height;
                float C = 2. * (cx / width) - 1.;
                float D = 2. * (cy / height) - 1.;
                float E = - (far + near) / (far - near);
                float F = -2. * far * near / (far - near);

                // opengl: col-major
                KK[0]= A;   KK[4]= 0.;  KK[8]= C;   KK[12]= 0.;
                KK[1]= 0.;  KK[5]= B;   KK[9]= D;   KK[13]= 0.;
                KK[2]= 0.;  KK[6]= 0.;  KK[10]= E;  KK[14]= F;
                KK[3]= 0.;	KK[7]= 0.;	KK[11]= -1.;KK[15]= 0.;	

                //another solution by Kyle McDonald...
                //https://github.com/kylemcdonald/ofxCv/blob/master/libs/ofxCv/src/Calibration.cpp
                //glFrustum(
                //near * (-cx) / fx, near * (w - cx) / fx,
                //near * (cy - h) / fy, near * (cy) / fy,
                //near, far );
            };

            /*
             * RT from opencv row-major to opengl col-major
             */

            void RT_cv2gl( 
                    ofVec3f x, ofVec3f y, ofVec3f z, ofVec3f t )
            {
                //	opengl: col-major	
                RT[0]= x.x; RT[4]= y.x; RT[8]= z.x;	RT[12]= t.x;
                RT[1]= x.y;	RT[5]= y.y;	RT[9]= z.y;	RT[13]= t.y;
                RT[2]= x.z;	RT[6]= y.z;	RT[10]=z.z; RT[14]= t.z;
                RT[3]= 0.;	RT[7]= 0.;	RT[11]= 0.;	RT[15]= 1.;
            };

            void printM( 
                    float* M, int rows, int cols, 
                    bool colmajor = true )
            {
                if ( ofGetLogLevel() != OF_LOG_VERBOSE )
                    return;

                if (colmajor)
                {
                    for (int j = 0; j < cols; j++)
                    {
                        printf("\n");
                        for (int i = 0; i < rows; i++)
                        {
                            printf("%9.3f ", M[i*cols+j]);
                        }	
                    }
                }
                else
                {
                    for (int i = 0; i < rows; i++)
                    {
                        printf("\n");
                        for (int j = 0; j < cols; j++)
                        {
                            printf("%9.3f ", M[i*cols+j]);
                        }	
                    }
                }
                printf("\n\n");
            };
    };
};

