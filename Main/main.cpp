#include "App.h"

#include "scene.h"

int main() {
	renderer::App app;

	renderer::Renderer* rend = app.GetRenderer();

	{
		assets::SceneData scene = assets::loadSceneFromFile("assets/First.scene");

		rend->UpdateSolidMeshes(scene.solidMeshes);

		auto mesh1 = rend->GetSolidMesh(0);
		mesh1->position.x = 1.5f;

		auto mesh3 = rend->CopySolidMesh(0);
		mesh3->position.y = 1.5f;
		mesh3->position.z -= 1.0f;

		rend->UploadTextures(scene.texData);
	}

	app.StartApp();

	return 0;
}
