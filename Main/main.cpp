#include "App.h"

#include "models.h"

int main() {
	renderer::App app;

	renderer::Renderer* rend = app.GetRenderer();

	std::vector<renderer::vertex> vertices;
	std::vector<uint16_t> indices;

	assets::loadModel("assets/SuzanneSmooth.obj", vertices, indices);

	rend->UpdateData(vertices, indices);

	app.StartApp();

	return 0;
}
