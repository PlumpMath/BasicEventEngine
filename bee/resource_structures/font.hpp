/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_FONT_H
#define _BEE_FONT_H 1

#include <iostream>
#include <map>

class TextData {
	public:
		std::map<int,SDL_Texture*> texture;
		std::string text;
		TextData() {texture.clear();text="";};
		TextData(SDL_Texture*, std::string);
		~TextData();
};
TextData::TextData(SDL_Texture* new_texture, std::string new_text) {
	texture.insert(std::make_pair(0, new_texture));
	text = new_text;
}
TextData::~TextData() {
	for (auto& t : texture) {
		SDL_DestroyTexture(t.second);
	}
	texture.clear();
	text = "";
}

class Font: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string font_path;
		int font_size;
		int style;
		int lineskip;

		TTF_Font* font;
		bool is_loaded = false;
	public:
		Font();
		Font(std::string, std::string, int);
		~Font();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		int get_font_size();
		int get_style();
		int get_lineskip();
		int get_lineskip_default();

		int set_name(std::string);
		int set_path(std::string);
		int set_font_size(int);
		int set_style(int);
		int set_lineskip(int);

		int load();
		int free();

		TextData* draw_internal(int, int, std::string, SDL_Color);
		TextData* draw(int, int, std::string, SDL_Color);
		TextData* draw(int, int, std::string);
		TextData* draw(TextData*, int, int, std::string, SDL_Color);
		TextData* draw(TextData*, int, int, std::string);
		int draw_fast(int, int, std::string, SDL_Color);
		int draw_fast(int, int, std::string);

		int get_string_width(std::string, int);
		int get_string_width(std::string);
		int get_string_height(std::string, int);
		int get_string_height(std::string);
};
Font::Font () {
	reset();
}
Font::Font (std::string new_name, std::string path, int new_font_size) {
	reset();

	add_to_resources("resources/fonts/"+path);
	if (id < 0) {
		std::cerr << "Failed to add font resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
	set_font_size(new_font_size);
}
Font::~Font() {
	resource_list.fonts.remove_resource(id);
}
int Font::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == font_path) {
			return 1;
		}
		resource_list.fonts.remove_resource(id);
		id = -1;
	} else {
		for (auto i : resource_list.fonts.resources) {
			if ((i.second != NULL)&&(i.second->get_path() == path)) {
				list_id = i.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = resource_list.fonts.add_resource(this);
	}
	resource_list.fonts.set_resource(id, this);

	return 0;
}
int Font::reset() {
	if (is_loaded) {
		free();
	}

	name = "";
	font_path = "";
	font_size = 16;
	style = TTF_STYLE_NORMAL;
	lineskip = 0;

	font = NULL;
	is_loaded = false;

	return 0;
}
int Font::print() {
	std::cout <<
	"Font { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	font_path	" << font_path <<
	"\n	font_size	" << font_size <<
	"\n	style		" << style <<
	"\n	lineskip	" << lineskip <<
	"\n	font		" << font <<
	"\n}\n";

	return 0;
}
int Font::get_id() {
	return id;
}
std::string Font::get_name() {
	return name;
}
std::string Font::get_path() {
	return font_path;
}
int Font::get_font_size() {
	return font_size;
}
int Font::get_style() {
	return style;
}
int Font::get_lineskip() {
	return lineskip;
}
int Font::get_lineskip_default() {
	if (is_loaded) {
		return TTF_FontLineSkip(font);
	}
	std::cerr << "Failed to get default lineskip, font not loaded: " << name << "\n";
	return -1;
}

int Font::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int Font::set_path(std::string path) {
	add_to_resources("resources/fonts/"+path);
	font_path = "resources/fonts/"+path;
	return 0;
}
int Font::set_font_size(int new_font_size) {
	font_size = new_font_size;
	return 0;
}
int Font::set_style(int new_style) {
	if (is_loaded)	{
		style = new_style; // bitmask of TTF_STYLE_BOLD, ITALIC, UNDERLINE, STRIKETHROUGH, and NORMAL
		TTF_SetFontStyle(font, style);
		return 0;
	}

	std::cerr << "Failed to set font style, font not loaded: " << name << "\n";
	return 1;
}
int Font::set_lineskip(int new_lineskip) {
	lineskip = new_lineskip;
	return 0;
}

int Font::load() {
	if (!is_loaded) {
		font = TTF_OpenFont(font_path.c_str(), font_size);
		if (font == NULL) {
			std::cerr << "Failed to load font " << font_path << ": " << TTF_GetError() << "\n";
			return 1;
		}

		is_loaded = true;
	}

	return 0;
}
int Font::free() {
	if (is_loaded) {
		TTF_CloseFont(font);
		font = NULL;

		is_loaded = false;
	}

	return 0;
}

TextData* Font::draw_internal(int x, int y, std::string text, SDL_Color color) {
	if (is_loaded) {
		if (text.size() > 0) {
			SDL_Surface* tmp_surface;
			tmp_surface = TTF_RenderUTF8_Blended(font, text.c_str(), color); // Slow but pretty
			if (tmp_surface == NULL) {
				std::cerr << "Failed to draw with font " << name << ": " << TTF_GetError() << "\n";
				return NULL;
			}

			SDL_Texture* texture;
			texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
			if (texture == NULL) {
				std::cerr << "Failed to create texture from surface: " << SDL_GetError() << "\n";
				return NULL;
			}

			SDL_FreeSurface(tmp_surface);

			SDL_Rect rect;
			rect.x = x;
			rect.y = y;
			SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

			SDL_RenderCopy(game->renderer, texture, NULL, &rect);

			TextData* textdata = new TextData(texture, text);

			return textdata;
		}

		return NULL;
	}

	std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
	return NULL;
}
TextData* Font::draw(int x, int y, std::string text, SDL_Color color) {
	if (is_loaded) {
		TextData *r = NULL, *textdata = NULL;
		std::map<int,std::string> lines = handle_newlines(text);

		if (lineskip == 0) {
			lineskip = TTF_FontLineSkip(font);
		}

		for (auto& l : lines) {
			r = draw_internal(x, y+lineskip*l.first, l.second, color);
			if (r != NULL) {
				if (textdata == NULL) {
					textdata = r;
				} else {
					textdata->texture.insert(std::make_pair(textdata->texture.size(), r->texture[0]));
					delete r;
				}
			}
		}
		return textdata;
	}

	std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
	return NULL;
}
TextData* Font::draw(int x, int y, std::string text) {
	SDL_Color color = {0, 0, 0, 255};
	return draw(x, y, text, color);
}
TextData* Font::draw(TextData* textdata, int x, int y, std::string text, SDL_Color color) {
	if (is_loaded) {
		if ((textdata != NULL)&&(textdata->text == text)) {
			std::map<int,std::string> lines = handle_newlines(text);
			for (auto& l : lines) {
				SDL_Rect rect;
				rect.x = x;
				rect.y = y;
				SDL_QueryTexture(textdata->texture[l.first], NULL, NULL, &rect.w, &rect.h);

				SDL_RenderCopy(game->renderer, textdata->texture[l.first], NULL, &rect);
			}
			return textdata;
		} else {
			if (textdata != NULL) {
				delete textdata;
			}
			return draw(x, y, text, color);
		}
	}

	std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
	return NULL;
}
TextData* Font::draw(TextData* textdata, int x, int y, std::string text) {
	SDL_Color color = {0, 0, 0, 255};
	return draw(textdata, x, y, text, color);
}
int Font::draw_fast(int x, int y, std::string text, SDL_Color color) {
	if (is_loaded) {
		if (text.size() > 0) {
			SDL_Surface* tmp_surface;
			tmp_surface = TTF_RenderUTF8_Solid(font, text.c_str(), color); // Fast but ugly
			if (tmp_surface == NULL) {
				std::cerr << "Failed to draw with font " << name << ": " << TTF_GetError() << "\n";
				return 1;
			}

			SDL_Texture* texture;
			texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
			if (texture == NULL) {
				std::cerr << "Failed to create texture from surface: " << SDL_GetError() << "\n";
				return 1;
			}

			SDL_FreeSurface(tmp_surface);

			SDL_Rect rect;
			rect.x = x;
			rect.y = y;
			SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

			SDL_RenderCopy(game->renderer, texture, NULL, &rect);
			SDL_DestroyTexture(texture);

			return 0;
		}

		return 1;
	}

	std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
	return 1;
}
int Font::draw_fast(int x, int y, std::string text) {
	SDL_Color color = {0, 0, 0, 255};
	return draw_fast(x, y, text, color);
}

int Font::get_string_width(std::string text, int size) {
	if (is_loaded) {
		int w = 0;
		TTF_SizeUTF8(font, text.c_str(), &w, NULL);
		return w;
	}

	std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
	return -1;
}
int Font::get_string_width(std::string text) {
	return get_string_width(text, font_size);
}
int Font::get_string_height(std::string text, int size) {
	if (is_loaded) {
		int h = 0;
		TTF_SizeUTF8(font, text.c_str(), NULL, &h);
		return h;
	}

	std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
	return -1;
}
int Font::get_string_height(std::string text) {
	return get_string_height(text, font_size);
}

#endif // _BEE_FONT_H
