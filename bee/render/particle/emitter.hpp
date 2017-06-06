/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_EMITTER_H
#define _BEE_RENDER_PARTICLE_EMITTER_H 1

#include "../../engine.hpp"

namespace bee {
	class ParticleEmitter {
		public:
			double x, y;
			unsigned int w, h;
			Instance* following;

			E_PS_SHAPE shape;
			E_PS_DISTR distribution;

			Particle* particle_type;

			int number;
			int number_count;

			ParticleEmitter();
	};
}

#endif // _BEE_RENDER_PARTICLE_EMITTER_H
