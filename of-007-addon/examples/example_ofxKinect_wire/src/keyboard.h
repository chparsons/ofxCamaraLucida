#pragma once

namespace key
{
	static char keyboard_help = 'h';
	
	static char fullscreen = 'q';
	static char projector = 'w';
	
	static char debug_camaralucida = 'e';
	static char debug_depth_texture = 'r';
	static char debug_hue_texture = 't';
	static char print_mesh = 'p';
	
	static string get_help()
	{
		return "keyboard help: \n "+string(1, fullscreen)+" fullscreen \n "+string(1, projector)+" toggle projector \n "+string(1, debug_camaralucida)+" debug mode \n "+string(1, debug_depth_texture)+" debug_depth_texture \n "+string(1, debug_hue_texture)+" debug_hue_texture (overrides depth_texture) \n "+string(1, print_mesh)+" print_mesh";
	}
}