#pragma once

namespace key
{
	static char keyboard_help = 'h';
	
	static char fullscreen = 'q';
	static char projector = 'w';
	
	static char toggle_debug = 'd';
	static char debug_tex = 't';
	static char toggle_wireframe = 'r';
	static char print_mesh = 'p';
	
	static string get_help()
	{
		return "keyboard help: \n "+string(1, fullscreen)+" fullscreen \n "+string(1, projector)+" toggle projector \n "+string(1, toggle_debug)+" debug mode \n "+string(1, debug_tex)+" switch debug_tex \n "+string(1, toggle_wireframe)+" toggle_wireframe \n "+string(1, print_mesh)+" print_mesh";
	}
}