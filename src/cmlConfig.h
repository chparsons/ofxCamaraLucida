#pragma once

#include "ofxXmlSettings.h"

namespace cml
{
    class Config
    {
        public:

            Config( ofxXmlSettings *xml )
            {
                near = xml->getValue("near", 0.1);
                far = xml->getValue("far", 20.0);
                mesh_step = xml->getValue("mesh_step", 2);

                tex_width = xml->getValue(
                        "texture:width", 1024);
                tex_height = xml->getValue(
                        "texture:height", 768);
                tex_nsamples = xml->getValue(
                        "texture:num_samples", 1);

                kinect_calib_path = ofToDataPath(
                        xml->getValue( 
                            "calibration:kinect", "") );

                projector_calib_path = ofToDataPath(
                        xml->getValue( 
                            "calibration:projector", "") );
                
                //render_shader_path = ofToDataPath(
                        //xml.getValue(
                            //"files:render_shader","") );
            };

            ~Config(){};

            string kinect_calib_path;
            string projector_calib_path;
            float near, far;
            int mesh_step;
            int tex_width, tex_height, tex_nsamples;
    };
};
