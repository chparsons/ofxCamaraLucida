//	Copyright (C) 2011  Christian Parsons
//	www.chparsons.com.ar
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "ofMath.h"
#include "ofVectorMath.h"

#ifndef ARRAY_LEN
#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0]))
#endif

namespace cml 
{
	class CoordConverter
	{
	public:
		
		void set(int cv_width, int cv_height, int screen_width, int screen_height);
		
		void to_cv(const ofVec3f& screen, ofVec3f& cv);
		void to_cv(int index, ofVec3f& cv);
		int to_index(const ofVec3f& cv);
		void to_screen(const ofVec3f& cv, ofVec3f& screen);
		
		int cv_width();
		int cv_height();
		int screen_width();
		int screen_height();
		
	private:
		
		int _cv_width, _cv_height;
		int _screen_width, _screen_height;
	};

	inline void CoordConverter::set(int cv_width, int cv_height, int screen_width, int screen_height)
	{
		_cv_width = cv_width;
		_cv_height = cv_height;
		_screen_width = screen_width;
		_screen_height = screen_height;
	}

	inline void CoordConverter::to_cv(const ofVec3f& screen, ofVec3f& cv)
	{
		cv.x = (screen.x / _screen_width) * _cv_width;
		cv.y = (screen.y / _screen_height) * _cv_height;
		
		cv.x = CLAMP(cv.x, 0., _cv_width-1.);
		cv.y = CLAMP(cv.y, 0., _cv_height-1.);
	}

	inline void CoordConverter::to_cv(int index, ofVec3f& cv)
	{
		cv.x = index % _cv_width;
		cv.y = (float)(index - cv.x) / _cv_width;
		
		cv.x = CLAMP(cv.x, 0., _cv_width-1.);
		cv.y = CLAMP(cv.y, 0., _cv_height-1.);
	}

	inline int CoordConverter::to_index(const ofVec3f& cv)
	{
		return cv.y * _cv_width + cv.x;
	}

	inline void CoordConverter::to_screen(const ofVec3f& cv, ofVec3f& screen)
	{
		screen.x = (cv.x / _cv_width) * _screen_width;
		screen.y = (cv.y / _cv_height) * _screen_height;
		
		screen.x = CLAMP(screen.x, 0., _screen_width-1.);
		screen.y = CLAMP(screen.y, 0., _screen_height-1.);
	}

	inline int CoordConverter::cv_width()
	{
		return _cv_width;
	}

	inline int CoordConverter::cv_height()
	{
		return _cv_height;
	}

	inline int CoordConverter::screen_width()
	{
		return _screen_width;
	}

	inline int CoordConverter::screen_height()
	{
		return _screen_height;
	}
		
	//http://www.cs.rit.edu/~ncs/color/t_convert.html
	//
	// r,g,b values are from 0 to 1
	// h = [0,360], s = [0,1], v = [0,1]
	//		if s == 0, then h = -1 (undefined)
	//
	// s = saturation = level of white (0=white, 1=no white)
	// v = value or brightness = level of black (0=black, 1=no black)

	static void RGBtoHSV( float r, float g, float b, 
							float *h, float *s, float *v )
	{
		float min, max, delta;
		
		float rgb[3] = { r, g, b };
		max = *std::max_element(rgb, rgb+3);
		min = *std::min_element(rgb, rgb+3);
		
		*v = max;				// v
		delta = max - min;
		if( max != 0 )
			*s = delta / max;		// s
		else {
			// r = g = b = 0		// s = 0, v is undefined
			*s = 0;
			*h = -1;
			return;
		}
		if( r == max )
			*h = ( g - b ) / delta;		// between yellow & magenta
		else if( g == max )
			*h = 2 + ( b - r ) / delta;	// between cyan & yellow
		else
			*h = 4 + ( r - g ) / delta;	// between magenta & cyan
		*h *= 60;				// degrees
		if( *h < 0 )
			*h += 360;
	}

	static void HSVtoRGB( float h, float s, float v,
							float *r, float *g, float *b)
	{
		int i;
		float f, p, q, t;
		if( s == 0 ) {
			// achromatic (grey)
			*r = *g = *b = v;
			return;
		}
		h /= 60;			// sector 0 to 5
		i = floor( h );
		f = h - i;			// factorial part of h
		p = v * ( 1 - s );
		q = v * ( 1 - s * f );
		t = v * ( 1 - s * ( 1 - f ) );
		switch( i ) {
			case 0:
				*r = v;
				*g = t;
				*b = p;
				break;
			case 1:
				*r = q;
				*g = v;
				*b = p;
				break;
			case 2:
				*r = p;
				*g = v;
				*b = t;
				break;
			case 3:
				*r = p;
				*g = q;
				*b = v;
				break;
			case 4:
				*r = t;
				*g = p;
				*b = v;
				break;
			default:		// case 5:
				*r = v;
				*g = p;
				*b = q;
				break;
		}
	}		
};