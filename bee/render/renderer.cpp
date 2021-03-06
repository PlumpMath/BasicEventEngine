/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDERER
#define BEE_RENDERER 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "renderer.hpp" // Include the engine headers

#include "../util/files.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/resources.hpp"

#include "camera.hpp"
#include "drawing.hpp"
#include "render.hpp"
#include "rgba.hpp"
#include "shader.hpp"

#include "../resource/texture.hpp"

namespace bee {
	Renderer::Renderer() :
		window(nullptr),
		sdl_renderer(nullptr),
		context(nullptr),

		program(nullptr),

		render_is_3d(false),
		render_camera(nullptr),
		projection_cache(nullptr),

		triangle_vao(-1),
		triangle_vbo(-1),
		triangle_ibo(-1)
	{}
	Renderer::~Renderer() {
		if (render_camera != nullptr) {
			delete render_camera;
			render_camera = nullptr;
		}
		if (projection_cache != nullptr) {
			delete projection_cache;
			projection_cache = nullptr;
		}

		if (program != nullptr) {
			delete program;
			program = nullptr;
		}

		if (context != nullptr) {
			opengl_close();
		}
		if (sdl_renderer != nullptr) {
			sdl_renderer_close();
		}
		if (window != nullptr) {
			SDL_DestroyWindow(window);
			window = nullptr;
		}
	}

	int Renderer::opengl_init() {
		messenger::send({"engine", "renderer"}, E_MESSAGE::INFO, "Compiling OpenGL shaders...");

		context = SDL_GL_CreateContext(window);
		if (context == nullptr) {
			messenger::send({"engine", "renderer"}, E_MESSAGE::ERROR, "Couldn't create OpenGL context: " + get_sdl_error() + "\n");
			return 1; // Return 1 when the OpenGL context could not be created
		}

		// Initialize GLEW
		glewExperimental = GL_TRUE;
		GLenum glew_error = glewInit();
		if (glew_error != GLEW_OK) {
			messenger::send({"engine", "renderer"}, E_MESSAGE::ERROR, "Couldn't initialize GLEW: " + std::string(reinterpret_cast<const char*>(glewGetErrorString(glew_error))) + "\n");
			return 2; // Return 2 when GLEW could not be initialized
		}

		if (get_options().is_vsync_enabled) {
			SDL_GL_SetSwapInterval(1);
		} else {
			SDL_GL_SetSwapInterval(0);
		}

		// Get shader filenames
		const std::string vs_fn_default = "bee/render/shader/default.vertex.glsl";
		const std::string vs_fn_user = "resources/vertex.glsl";
		std::string vs_fn (vs_fn_default);
		if (file_exists(vs_fn_user)) {
			vs_fn = vs_fn_user;
		}
		const std::string gs_fn_default = "bee/render/shader/default.geometry.glsl";
		const std::string gs_fn_user = "resources/geometry.glsl";
		std::string gs_fn (gs_fn_default);
		if (file_exists(gs_fn_user)) {
			gs_fn = gs_fn_user;
		}
		const std::string fs_fn_default = "bee/render/shader/default.fragment.glsl";
		const std::string fs_fn_user = "resources/fragment.glsl";
		const std::string fs_fn_basic_default = "bee/render/shader/basic.fragment.glsl";
		const std::string fs_fn_basic_user = "resources/basic.fragment.glsl";
		std::string fs_fn (fs_fn_default);
		if (get_options().is_basic_shaders_enabled == true) {
			fs_fn = fs_fn_basic_default;
			if (file_exists(fs_fn_basic_user)) {
				fs_fn = fs_fn_basic_user;
			}
		} else {
			if (file_exists(fs_fn_user)) {
				fs_fn = fs_fn_user;
			}
		}

		program = new ShaderProgram();

		Shader vertex_shader (vs_fn, GL_VERTEX_SHADER);
		program->add_shader(vertex_shader);

		program->add_attrib("v_position", true);
		//program->add_attrib("v_normal", true);
		program->add_attrib("v_texcoord", true);

		program->add_uniform("projection", true);
		program->add_uniform("view", true);
		program->add_uniform("model", true);
		program->add_uniform("port", true);

		Shader geometry_shader (gs_fn, GL_GEOMETRY_SHADER);
		program->add_shader(geometry_shader);

		program->add_uniform("rotation", true);

		Shader fragment_shader (fs_fn, GL_FRAGMENT_SHADER);
		program->add_shader(fragment_shader);

		program->add_uniform("f_texture", true);
		program->add_uniform("colorize", true);
		program->add_uniform("is_primitive", true);
		program->add_uniform("flip", true);

		program->add_uniform("time", false);

		program->add_uniform("is_lightable", false);
		program->add_uniform("light_amount", false);
		for (size_t i=0; i<BEE_MAX_LIGHTS; i++) {
			program->add_uniform("lighting[" + bee_itos(i) + "].type", false);
			program->add_uniform("lighting[" + bee_itos(i) + "].position", false);
			program->add_uniform("lighting[" + bee_itos(i) + "].direction", false);
			program->add_uniform("lighting[" + bee_itos(i) + "].attenuation", false);
			program->add_uniform("lighting[" + bee_itos(i) + "].color", false);
		}

		program->add_uniform("lightable_amount", false);
		for (size_t i=0; i<BEE_MAX_LIGHTABLES; i++) {
			program->add_uniform("lightable[" + bee_itos(i) + "].position", false);
			program->add_uniform("lightable[" + bee_itos(i) + "].vertex_amount", false);
			for (size_t j=0; j<BEE_MAX_MASK_VERTICES; j++) {
				program->add_uniform("lightable[" + bee_itos(i) + "].mask[" + bee_itos(j) + "]", false);
			}
		}

		program->link();
		render::set_program(program);

		draw_set_color({255, 255, 255, 255});
		glEnable(GL_TEXTURE_2D);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		int va = 0, vi = 0;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &va);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &vi);

		if (
			(get_options().renderer_type == E_RENDERER::OPENGL4)
			&&(va == 4)&&(vi == 1)
		) {
			messenger::send({"engine", "renderer"}, E_MESSAGE::INFO, "Now rendering with OpenGL 4.1");
		} else if (
			(get_options().renderer_type == E_RENDERER::OPENGL3)
			&&(va == 3)&&(vi == 3)
		) {
			messenger::send({"engine", "renderer"}, E_MESSAGE::INFO, "Now rendering with OpenGL 3.3");
		} else {
			messenger::send({"engine", "renderer"}, E_MESSAGE::INFO, "Now rendering with OpenGL " + bee_itos(va) + "." + bee_itos(vi));
		}

		projection_cache = new glm::mat4(1.0f);

		// Generate the triangle buffers for primitive drawing
		glGenBuffers(1, &triangle_vao);
		glGenBuffers(1, &triangle_vbo);

		GLushort elements[] = {
			0, 1, 2,
		};
		glGenBuffers(1, &triangle_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		return 0; // Return 0 on success
	}
	int Renderer::opengl_close() {
		glDeleteBuffers(1, &triangle_vao);
		glDeleteBuffers(1, &triangle_vbo);
		glDeleteBuffers(1, &triangle_ibo);

		delete projection_cache;

		if (program != nullptr) {
			delete program;
			program = nullptr;
		}

		if (context != nullptr) {
			SDL_GL_DeleteContext(context);
			context = nullptr;
		}

		return 0;
	}
	int Renderer::sdl_renderer_init() {
		messenger::send({"engine", "renderer"}, E_MESSAGE::ERROR, "The SDL renderer is deprecated and will be removed soon");

		int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
		if (get_options().is_vsync_enabled) {
			renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
		}

		sdl_renderer = SDL_CreateRenderer(window, -1, renderer_flags);
		if (sdl_renderer == nullptr) {
			messenger::send({"engine", "renderer"}, E_MESSAGE::ERROR, "Couldn't create SDL renderer: " + get_sdl_error());
			return 1; // Return 1 when the SDL renderer could not be created
		}

		SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
		SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);

		messenger::send({"engine", "renderer"}, E_MESSAGE::INFO, "Now rendering with SDL2");

		return 0; // Return 0 on success
	}
	int Renderer::sdl_renderer_close() {
		messenger::send({"engine", "renderer"}, E_MESSAGE::ERROR, "The SDL renderer is deprecated and will be removed soon");

		if (sdl_renderer != nullptr) {
			SDL_DestroyRenderer(sdl_renderer);
			sdl_renderer = nullptr;
		}

		return 0;
	}

	int Renderer::reset() {
		opengl_close();
		opengl_init();

		// Reload textures
		for (size_t i=0; i<Texture::get_amount(); i++) {
			Texture* t = Texture::get(i);
			if (t != nullptr) {
				if (t->get_is_loaded()) {
					t->free();
					t->load();
				}
			}
		}

		return 0;
	}
}

#endif // BEE_RENDERER
