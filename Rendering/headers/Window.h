#pragma once

#include <SDL3/SDL.h>


namespace renderer {

class Window {
public:
	Window(int width, int height, const char* name);
	~Window();
	Window(Window&) = delete;
	Window(const Window&) = delete;

	SDL_Window* GetSDLWindow() const;

	int GetWidth() const;
	int GetHeight() const;

	// Returns true if close event has been detected
	bool PollEvents();

private:
	int mWidth;
	int mHeight;
	const char* mName;

	SDL_Window* mSDLWindow;

	SDL_Event mCurrentEvent;
};

} // namespace render
