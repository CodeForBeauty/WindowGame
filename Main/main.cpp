#include "App.h"

#include "models.h"

int main() {
	renderer::App app;

	renderer::Renderer* rend = app.GetRenderer();

	std::vector<renderer::vertex> vertices;
	std::vector<unsigned int> indices;

	assets::loadModel("assets/suzanne.obj", vertices, indices);

	rend->UpdateData(vertices, indices);

	app.StartApp();

	return 0;
}
