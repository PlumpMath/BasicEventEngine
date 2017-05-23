/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_NETWORKCLIENT
#define _BEE_CORE_NETWORKCLIENT 1

#include "networkclient.hpp"

BEE::NetworkClient::NetworkClient() :
	sock(nullptr),
	channel(-1),
	last_recv(0)
{}

#endif // _BEE_CORE_NETWORKCLIENT
