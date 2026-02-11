#include "App.h"

#include "lm2.hpp"

int main() {
	lm2::vec3 a{ 3, 4, 5 };
	lm2::vec3 b{ 1, 1.5f, 0.5f };

	std::cout << a << "\n";

	std::cout << a + 5.0f << "\n";

	std::cout << a * b << "\n";

	renderer::App app;

	app.StartApp();

	return 0;
}
