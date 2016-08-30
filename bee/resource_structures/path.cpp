/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_PATH
#define _BEE_PATH 1

#include "path.hpp"

BEE::Path::Path () {
	if (BEE::resource_list->paths.game != nullptr) {
		game = BEE::resource_list->paths.game;
	}

	reset();
}
BEE::Path::Path (std::string new_name, std::string path) {
	reset();

	add_to_resources("resources/paths/"+path);
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add path resource: " + path);
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
}
BEE::Path::~Path() {
	BEE::resource_list->paths.remove_resource(id);
}
int BEE::Path::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == path_path) {
			return 1;
		}
		BEE::resource_list->paths.remove_resource(id);
		id = -1;
	}

	id = BEE::resource_list->paths.add_resource(this);
	BEE::resource_list->paths.set_resource(id, this);

	if (BEE::resource_list->paths.game != nullptr) {
		game = BEE::resource_list->paths.game;
	}

	return 0;
}
int BEE::Path::reset() {
	name = "";
	path_path = "";
	coordinate_list.clear();
	connection_type = 0;
	is_closed = true;

	return 0;
}
int BEE::Path::print() {
	std::string coordinate_string = get_coordinate_string();

	std::stringstream s;
	s <<
	"Path { "
	"\n	id              " << id <<
	"\n	name            " << name <<
	"\n	path_path       " << path_path <<
	"\n	coordinate_list\n" << debug_indent(coordinate_string, 2) <<
	"	connection_type	" << connection_type <<
	"\n	is_closed       " << is_closed <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}
int BEE::Path::get_id() {
	return id;
}
std::string BEE::Path::get_name() {
	return name;
}
std::string BEE::Path::get_path() {
	return path_path;
}
std::vector<bee_path_coord> BEE::Path::get_coordinate_list() {
	return coordinate_list;
}
std::string BEE::Path::get_coordinate_string() {
	if (coordinate_list.size() > 0) {
		std::vector<std::vector<std::string>> table;
		table.push_back({"(x", "y", "speed)"});

		for (auto it = coordinate_list.begin(); it != coordinate_list.end(); ++it) {
			table.push_back({bee_itos(std::get<0>(*it)), bee_itos(std::get<1>(*it)), bee_itos(std::get<2>(*it))});
		}

		return string_tabulate(table);
	}
	return "none\n";
}
int BEE::Path::get_connection_type() {
	return connection_type;
}
bool BEE::Path::get_is_closed() {
	return is_closed;
}
int BEE::Path::set_name(std::string new_name) {
	// Deny name change if game is currently running (?)
	// if (!game.is_running()) {
	name = new_name;
	return 0;
}
int BEE::Path::set_path(std::string path) {
	add_to_resources("resources/paths/"+path);
	path_path = "resources/paths/"+path;
	// Load XML Path data
	/*
	 * coordinate_list.push_back(x, y, speed);
	 */
	return 0;
}
int BEE::Path::add_coordinate(bee_path_coord new_coordinate) {
	coordinate_list.push_back(new_coordinate);
	return 0;
}
int BEE::Path::add_coordinate(int x, int y, double speed) {
	bee_path_coord new_coordinate (x, y, speed);
	coordinate_list.push_back(new_coordinate);
	return 0;
}
int BEE::Path::remove_last_coordinate() {
	if (!coordinate_list.empty()) {
		coordinate_list.pop_back();
		return 0;
	}
	return 1;
}
int BEE::Path::remove_coordinate(unsigned int index) {
	if (index < coordinate_list.size()) {
		coordinate_list.erase(coordinate_list.begin()+index);
		return 0;
	}
	return 1;
}
int BEE::Path::set_connection_type(int new_conneciton_type) {
	connection_type = new_conneciton_type;
	return 0;
}
int BEE::Path::set_is_closed(bool new_is_closed) {
	is_closed = new_is_closed;
	return 0;
}

int BEE::Path::draw(int xstart, int ystart) {
	for (auto it = coordinate_list.begin(); it != coordinate_list.end(); ++it) {
		if (it != --coordinate_list.end()) {
			int xs = xstart;
			int ys = ystart;

			int x1 = std::get<0>(*it);
			int y1 = std::get<1>(*it);
			++it;
			int x2 = std::get<0>(*it);
			int y2 = std::get<1>(*it);
			--it;

			game->draw_line(x1+xs, y1+ys, x2+xs, y2+ys, c_aqua, false);
		}
	}
	return 0;
}

#endif // _BEE_PATH
