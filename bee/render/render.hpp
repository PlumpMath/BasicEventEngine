/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_H
#define BEE_RENDER_H 1

#include <string>

#include "camera.hpp"

namespace bee { namespace render {
	int set_is_lightable(bool);

	std::string opengl_prepend_version(const std::string&);

	int set_3d(bool);
	int set_camera(Camera*);
	bool get_3d();
	glm::mat4 get_projection();
	glm::mat4 calc_projection();
	Camera get_camera();
}}

#endif // BEE_RENDER_H
