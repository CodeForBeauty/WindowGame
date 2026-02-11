#include "App.h"

using namespace renderer;


App::App(int windowWidth, int windowHeight, const char* appName)
	: mWindow{ windowWidth, windowHeight, appName }, mRenderer{mWindow, appName}, mName{appName} {
}

void App::StartApp() {

	Start();

	while (true) {
		if (mWindow.PollEvents()) {
			break;
		}

		Update();

		mRenderer.Render(mWindow.GetWidth(), mWindow.GetHeight());

		PostUpdate();
	}

	End();
	mRenderer.Cleanup();
}
