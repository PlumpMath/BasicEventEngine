/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_DESTROYER
#define BEE_RENDER_PARTICLE_DESTROYER 1

#include "destroyer.hpp"

namespace bee {
	ParticleDestroyer::ParticleDestroyer() :
		x(0.0),
		y(0.0),
		w(1),
		h(1),
		following(nullptr),

		shape(E_PS_SHAPE::RECTANGLE)
	{}
}

#endif // BEE_RENDER_PARTICLE_DESTROYER