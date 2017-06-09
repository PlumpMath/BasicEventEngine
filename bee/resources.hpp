/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RESOURCES_H
#define BEE_RESOURCES_H 1

#include <string> // Include the required library headers
#include <map>

namespace bee {
	class Resource { // The abstract class for all resource types
		public:
			virtual ~Resource() {};
			virtual int reset() =0;
			virtual int print() const =0;
			virtual int get_id() const =0;
			virtual std::string get_name() const =0;
			virtual std::string get_path() const =0;
			virtual int set_name(const std::string&) =0;
			virtual int set_path(const std::string&) =0;
	};
	class ResourceList { // The class which holds a list of each individual resource type
		public:
			std::map<int,Resource*> resources;
			int next_index;

			ResourceList();
			int reset();
			int add_resource(Resource*);
			int set_resource(int, Resource*);
			Resource* get_resource(int);
			unsigned int get_amount();
			int remove_resource(int);
	};
	class MetaResourceList { // The class which holds a list of all of the lists of resource types
		public:
			ResourceList sprites, sounds, backgrounds, fonts, paths, timelines, meshes, lights, objects, rooms;

			MetaResourceList();
			int reset();
	};
}

#endif // BEE_RESOURCES_H
