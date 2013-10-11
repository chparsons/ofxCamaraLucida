/*
 * Camara Lucida
 * www.camara-lucida.com.ar
 *
 * Copyright (C) 2011  Christian Parsons
 * www.chparsons.com.ar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ofxXmlSettings.h"

namespace cml
{
  class Config
  {
    public:

      Config( ofxXmlSettings *xml )
      {
        mesh_step = xml->getValue(
            "mesh_step", 2);

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

      int mesh_step;
      int tex_width, tex_height, tex_nsamples;
  };
};

