#include "App.h"



using namespace renderer;


App::App(int windowWidth, int windowHeight, const char* appName)
	: mWindow{ windowWidth, windowHeight, appName }, mRenderer{mWindow, appName}, mName{appName} {
}

void App::StartApp() {

	Start();
	auto mesh =	mRenderer.GetSolidMesh(0);

	while (true) {
		if (mWindow.PollEvents()) {
			break;
		}

		mesh->rotation.y += 0.5f;

		Update();

		mRenderer.Render(mWindow.GetWidth(), mWindow.GetHeight());

		PostUpdate();
	}

	End();
	mRenderer.Cleanup();
}

Renderer* App::GetRenderer() {
	return &mRenderer;
}
