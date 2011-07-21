//	Cámara Lúcida
//	www.camara-lucida.com.ar
//
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


//	reserved user input on debug mode (toggle_debug)
//		'x' reset view
//		'v' switch view projector, depth cam, world
//		'c' + key up/down: change depht_xoffset
//		'z' + mouse_drag: zoom in/out
//		mouse_drag: rotate

#pragma once

namespace camaralucida 
{
	namespace key
	{
		static const char view_type = 'v';
		static const char reset_view = 'x';
		static const char change_depth_xoff = 'c'; // + key up/down
		static const char zoom = 'z';
	}
}