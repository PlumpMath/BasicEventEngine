/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_ENUM_H
#define _BEE_ENUM_H 1

enum bee_rgba_t {
	c_cyan, c_aqua,
	c_black,
	c_blue,
	c_dkgray,
	c_magenta, c_fuchsia,
	c_gray,
	c_green,
	c_lime,
	c_silver, c_ltgray,
	c_maroon,
	c_navy,
	c_olive,
	c_orange,
	c_purple,
	c_red,
	c_teal,
	c_white,
	c_yellow
};

enum bee_renderer_t {
	BEE_RENDERER_SDL,
	BEE_RENDERER_OPENGL3,
	BEE_RENDERER_OPENGL4
};

enum bee_event_t {
	BEE_EVENT_UPDATE,
	BEE_EVENT_CREATE,
	BEE_EVENT_DESTROY,
	BEE_EVENT_ALARM,
	BEE_EVENT_STEP_BEGIN,
	BEE_EVENT_STEP_MID,
	BEE_EVENT_STEP_END,
	BEE_EVENT_KEYBOARD_PRESS,
	BEE_EVENT_MOUSE_PRESS,
	BEE_EVENT_KEYBOARD_INPUT,
	BEE_EVENT_MOUSE_INPUT,
	BEE_EVENT_KEYBOARD_RELEASE,
	BEE_EVENT_MOUSE_RELEASE,
	BEE_EVENT_CONTROLLER_AXIS,
	BEE_EVENT_CONTROLLER_PRESS,
	BEE_EVENT_CONTROLLER_RELEASE,
	BEE_EVENT_CONTROLLER_MODIFY,
	BEE_EVENT_PATH_END,
	BEE_EVENT_OUTSIDE_ROOM,
	BEE_EVENT_INTERSECT_BOUNDARY,
	BEE_EVENT_COLLISION,
	BEE_EVENT_CHECK_COLLISION_LIST,
	BEE_EVENT_DRAW,
	BEE_EVENT_ANIMATION_END,
	BEE_EVENT_ROOM_START,
	BEE_EVENT_ROOM_END,
	BEE_EVENT_GAME_START,
	BEE_EVENT_GAME_END,
	BEE_EVENT_WINDOW
};

#endif // _BEE_ENUM_H
