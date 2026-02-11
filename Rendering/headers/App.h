#pragma once

#include "Window.h"
#include "Renderer.h"

namespace renderer {

class App {
public:
	App(int windowWidth = 800, int windowHeight = 600, const char* appName = "App");

	void StartApp();

protected:
	virtual void Start() {};
	virtual void Update() {};
	virtual void PostUpdate() {};
	virtual void End() {};

	Window mWindow;
	Renderer mRenderer;
private:

	const char* mName;
};

} // namespace render
