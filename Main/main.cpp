#include "App.h"

#include "models.h"

int main() {
	renderer::App app;

	renderer::Renderer* rend = app.GetRenderer();

	std::vector< std::pair< std::vector<renderer::vertex>, std::vector<uint16_t> > > data{
		{ {}, {} },
		{ {}, {} }
	};

	assets::loadModel("assets/SuzanneSmooth.obj", data[0].first, data[0].second);
	assets::loadModel("assets/cube.obj", data[1].first, data[1].second);

	rend->UpdateSolidMeshes(data);

	auto mesh1 = rend->GetSolidMesh(0);
	mesh1->position.x = 1.5f;

	auto mesh2 = rend->GetSolidMesh(1);
	mesh2->position.x = -2.5f;

	auto mesh3 = rend->CopySolidMesh(0);
	mesh3->position.y = 1.5f;

	rend->LoadTexture("assets/water.jpg");

	app.StartApp();

	return 0;
}
