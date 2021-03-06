/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RESOURCE_H
#define BEE_RESOURCE_H 1

namespace bee {
	class Resource { // The abstract class for all resource types
		public:
			virtual ~Resource() {};
			virtual int reset() =0;
			virtual int print() const =0;
			virtual int get_id() const =0;
			virtual std::string get_name() const {return "";}
			virtual int load() {return 0;}
			virtual int free() {return 0;}
	};
}

#endif // BEE_RESOURCE_H
