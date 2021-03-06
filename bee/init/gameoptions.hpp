/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_GAMEOPTIONS_H
#define BEE_INIT_GAMEOPTIONS_H 1

#include "../enum.hpp"

namespace bee {
	struct GameOptions {
		// Window options
		bool is_fullscreen, is_borderless;
		bool is_resizable, is_maximized;
		bool is_highdpi, is_visible;
		bool is_minimized;

		// Renderer options
		bee::E_RENDERER renderer_type;
		bool is_vsync_enabled;
		bool is_basic_shaders_enabled;

		// Miscellaneous options
		bool is_network_enabled;
		bool is_debug_enabled;

		// Commandline flags
		bool should_assert;
		bool single_run;
		bool is_headless;

		GameOptions();
		GameOptions(bool, bool, bool, bool, bool, bool, bee::E_RENDERER, bool, bool, bool, bool);
	};

	const GameOptions& get_options();
	int set_options(const GameOptions&);
}

#endif // BEE_INIT_GAMEOPTIONS_H
