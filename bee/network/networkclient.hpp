/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_NETWORKCLIENT_H
#define BEE_CORE_NETWORKCLIENT_H 1

#include <SDL2/SDL_net.h>

namespace bee {
	struct NetworkClient {
		UDPsocket sock;
		int channel;
		Uint32 last_recv;

		NetworkClient();
	};
}

#endif // BEE_CORE_NETWORKCLIENT_H