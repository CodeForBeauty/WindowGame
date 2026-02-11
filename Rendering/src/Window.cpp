#include "Window.h"

#include <SDL3/SDL.h>

#include <stdexcept>

using namespace renderer;


Window::Window(int width, int height, const char* name)
	: mWidth{ width }, mHeight{ height }, mName{ name }, mSDLWindow{ nullptr } {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
		throw std::runtime_error("SDL failed to initialize");
	}

	mSDLWindow = SDL_CreateWindow(name, width, height, SDL_WINDOW_RESIZABLE);
}

Window::~Window() {
	SDL_Quit();
}

SDL_Window* Window::GetSDLWindow() const {
	return mSDLWindow;
}

int Window::GetWidth() const {
	return mWidth;
}

int Window::GetHeight() const {
	return mHeight;
}

bool Window::PollEvents() {
	if (SDL_PollEvent(&mCurrentEvent)) {
		if (mCurrentEvent.type == SDL_EVENT_QUIT) {
			return true;
		}
	}
	return false;
}
