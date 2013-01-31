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

#include "cmlDepthmap.h"

namespace cml 
{
    Depthmap::Depthmap() 
    { 
        zlut_mm2f = NULL;
    };

    Depthmap::~Depthmap(){};

    void Depthmap::init( 
            cml::OpticalDevice* depth, Mesh* mesh )
    {
        this->depth = depth;
        this->mesh = mesh;
    };

    void Depthmap::dispose()
    {
        ofLog(OF_LOG_VERBOSE,"cml::Depthmap::dispose");

        depth = NULL;
        mesh = NULL;

        float_tex.clear();
        float_pix.clear();
        if ( zlut_mm2f != NULL )
            delete[] zlut_mm2f;
    };

    ofTexture& Depthmap::get_float_tex_ref( 
            uint16_t *mm_depth_pix, 
            float mm_near, float mm_far )
    {
        init_float_tex( 
                depth->width(), depth->height(), 
                mm_near, mm_far );

        int len = depth->width() * depth->height();
        for (int i = 0; i < len; i++)
        {
            //uint16_t raw_depth = raw_depth_pix[i];
            //uint16_t mm = depth->z_mts(raw_depth)*1000;
            uint16_t mm = mm_depth_pix[ i ];
            float_pix[i] = zlut_mm2f[ mm ]; 
        }
        float_tex.loadData( float_pix );
        return float_tex; 
    };

    void Depthmap::init_float_tex( 
            int w, int h, 
            float mm_near, float mm_far )
    {
        if ( float_tex.isAllocated() )
            return;

        float_tex.allocate( w, h, GL_LUMINANCE32F_ARB );
        float_pix.allocate( w, h, 1);
        float_pix.set( 0 );

        zlut_mm2f = new float[ 10000 ];
        zlut_mm2f[0] = 0;
        for (int i = 1; i < 10000; i++) 
            zlut_mm2f[i] = ofMap(i, mm_near, mm_far, 1., 0., true);
    };
};


