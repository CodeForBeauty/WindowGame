#include "lm2.hpp"
#include "testlib.hpp"
#include <iostream>

using namespace lm2;

TEST_CASE(VectorAddition) {
	vec2 v2{ 1, 1 };

	ASSERT_CONDITION(equal(v2 + 1.0f, 2.0f));
	ASSERT_CONDITION(equal(v2 + vec2{ 1, 1 }, 2.0f));

	vec3 v3{ 1, 1, 1 };

	ASSERT_CONDITION(equal(v3 + 1.0f, 2.0f));
	ASSERT_CONDITION(equal(v3 + vec3{ 1, 1, 1 }, 2.0f));

	vec4 v4{ 1, 1, 1, 1 };

	ASSERT_CONDITION(equal(v4 + 1.0f, 2.0f));
	ASSERT_CONDITION(equal(v4 + vec4{ 1, 1, 1, 1 }, 2.0f));
}

TEST_CASE(VectorSubtraction) {
	vec2 v2{ 1, 1 };

	ASSERT_CONDITION(equal(v2 - 1.0f, 0.0f));
	ASSERT_CONDITION(equal(v2 - vec2{ 1, 1 }, 0.0f));

	vec3 v3{ 1, 1, 1 };

	ASSERT_CONDITION(equal(v3 - 1.0f, 0.0f));
	ASSERT_CONDITION(equal(v3 - vec3{ 1, 1, 1 }, 0.0f));

	vec4 v4{ 1, 1, 1, 1 };

	ASSERT_CONDITION(equal(v4 - 1.0f, 0.0f));
	ASSERT_CONDITION(equal(v4 - vec4{ 1, 1, 1, 1 }, 0.0f));
}

TEST_CASE(VectorMuliplication) {
	vec2 v2{ 1, 1 };

	ASSERT_CONDITION(equal(v2 * 2.0f, 2.0f));
	ASSERT_CONDITION(equal(v2 * vec2{ 2, 2 }, 2.0f));

	vec3 v3{ 1, 1, 1 };

	ASSERT_CONDITION(equal(v3 * 2.0f, 2.0f));
	ASSERT_CONDITION(equal(v3 * vec3{ 2, 2, 2 }, 2.0f));

	vec4 v4{ 1, 1, 1, 1 };

	ASSERT_CONDITION(equal(v4 * 2.0f, 2.0f));
	ASSERT_CONDITION(equal(v4 * vec4{ 2, 2, 2, 2 }, 2.0f));
}

TEST_CASE(VectorDivision) {
	vec2 v2{ 1, 1 };

	ASSERT_CONDITION(equal(v2 / 2.0f, 0.5f));
	ASSERT_CONDITION(equal(v2 / 0.0f, 0.0f));
	ASSERT_CONDITION(equal(v2 / vec2{ 2, 2 }, 0.5f));
	ASSERT_CONDITION(equal(v2 / vec2{ 0, 0 }, 0.0f));

	vec3 v3{ 1, 1, 1 };

	ASSERT_CONDITION(equal(v3 / 2.0f, 0.5f));
	ASSERT_CONDITION(equal(v3 / 0.0f, 0.0f));
	ASSERT_CONDITION(equal(v3 / vec3{ 2, 2, 2 }, 0.5f));
	ASSERT_CONDITION(equal(v3 / vec3{ 0, 0, 0 }, 0.0f));

	vec4 v4{ 1, 1, 1, 1 };

	ASSERT_CONDITION(equal(v4 / 2.0f, 0.5f));
	ASSERT_CONDITION(equal(v4 / 0.0f, 0.0f));
	ASSERT_CONDITION(equal(v4 / vec4{ 2, 2, 2, 2 }, 0.5f));
	ASSERT_CONDITION(equal(v4 / vec4{ 0, 0, 0, 0 }, 0.0f));
}

TEST_CASE(VectorModulo) {
	vec2 v2{ 5, 5 };

	ASSERT_CONDITION(equal(v2 % 2.0f, 1.0f));
	ASSERT_CONDITION(equal(v2 % 0.0f, 0.0f));
	ASSERT_CONDITION(equal(v2 % vec2{ 2, 2 }, 1.0f));
	ASSERT_CONDITION(equal(v2 % vec2{ 0, 0 }, 0.0f));

	vec3 v3{ 5, 5, 5 };

	ASSERT_CONDITION(equal(v3 % 2.0f, 1.0f));
	ASSERT_CONDITION(equal(v3 % 0.0f, 0.0f));
	ASSERT_CONDITION(equal(v3 % vec3{ 2, 2, 2 }, 1.0f));
	ASSERT_CONDITION(equal(v3 % vec3{ 0, 0, 0 }, 0.0f));

	vec4 v4{ 5, 5, 5, 5 };

	ASSERT_CONDITION(equal(v4 % 2.0f, 1.0f));
	ASSERT_CONDITION(equal(v4 % 0.0f, 0.0f));
	ASSERT_CONDITION(equal(v4 % vec4{ 2, 2, 2, 2 }, 1.0f));
	ASSERT_CONDITION(equal(v4 % vec4{ 0, 0, 0, 0 }, 0.0f));
}

TEST_CASE(VectorUnaryOperations) {
	vec2 v2{ 5, 5 };

	++v2;
	ASSERT_CONDITION(equal(v2, 6.0f));
	--v2;
	ASSERT_CONDITION(equal(v2, 5.0f));
	ASSERT_CONDITION(equal(-v2, -5.0f));


	vec3 v3{ 5, 5, 5 };

	++v3;
	ASSERT_CONDITION(equal(v3, 6.0f));
	--v3;
	ASSERT_CONDITION(equal(v3, 5.0f));
	ASSERT_CONDITION(equal(-v3, -5.0f));

	vec4 v4{ 5, 5, 5, 5 };

	++v4;
	ASSERT_CONDITION(equal(v4, 6.0f));
	--v4;
	ASSERT_CONDITION(equal(v4, 5.0f));
	ASSERT_CONDITION(equal(-v4, -5.0f));
}

TEST_CASE(VectorDotCross) {
	ASSERT_CONDITION(dot(vec2{ 1, 0 }, vec2{ 0, 1 }) == 0);
	ASSERT_CONDITION(dot(vec2{ -1, 0 }, vec2{ 1, 0 }) == -1);
	ASSERT_CONDITION(dot(vec2{ -1, 1 }, vec2{ 1, 1 }) == 0);

	ASSERT_CONDITION(dot(vec2{ 0, 0 }, vec2{ 0, 0 }) == 0);

	ASSERT_CONDITION(dot(vec3{ 1, 0, 1 }, vec3{ 0, 1, 1 }) == 1);
	ASSERT_CONDITION(dot(vec3{ -1, 0, -1 }, vec3{ 1, 0, -1 }) == 0);
	ASSERT_CONDITION(dot(vec3{ -1, 1, 0 }, vec3{ 1, 1, 0 }) == 0);

	ASSERT_CONDITION(dot(vec3{ 0, 0, 0 }, vec3{ 0, 0, 0 }) == 0);

	ASSERT_CONDITION(dot(vec4{ 1, 0, 1, 0 }, vec4{ 0, 1, 1, 0 }) == 1);
	ASSERT_CONDITION(dot(vec4{ -1, 0, -1, 1 }, vec4{ 1, 0, -1, 1 }) == 1);
	ASSERT_CONDITION(dot(vec4{ -1, 1, 0, -1 }, vec4{ 1, 1, 0, 1 }) == -1);

	ASSERT_CONDITION(dot(vec4{ 0, 0, 0, 0 }, vec4{ 0, 0, 0, 0 }) == 0);

	ASSERT_CONDITION(cross(vec2{ 1, 0 }, vec2{ 0, 1 }) == 1);
	ASSERT_CONDITION(cross(vec2{ 0, 1 }, vec2{ 1, 0 }) == -1);

	ASSERT_CONDITION(equal(cross(vec3{ 0, 1, 0 }, vec3{ 1, 0, 0 }), vec3{ 0, 0, -1 }));
	ASSERT_CONDITION(equal(cross(vec3{ 1, 0, 0 }, vec3{ 0, 1, 0 }), vec3{ 0, 0, 1 }));
	ASSERT_CONDITION(equal(cross(vec3{ 0, 0, 1 }, vec3{ 0, 1, 0 }), vec3{ -1, 0, 0 }));
}

TEST_CASE(VectorMagnitudeNormalize) {
	ASSERT_CONDITION(magnitudeSquared(vec2{ 1, 0 }) == 1);
	ASSERT_CONDITION(magnitudeSquared(vec2{ 2, 1 }) == 5);
	ASSERT_CONDITION(magnitudeSquared(vec2{ -2, -1 }) == 5);

	ASSERT_CONDITION(magnitude(vec2{ 1, 0 }) == 1);
	ASSERT_CONDITION(std::abs(magnitude(vec2{ 1, 1 }) - std::sqrt(2)) < 0.0001f);

	ASSERT_CONDITION(magnitudeSquared(vec3{ 1, 0, 1 }) == 2);
	ASSERT_CONDITION(magnitudeSquared(vec3{ 2, 1, 0 }) == 5);
	ASSERT_CONDITION(magnitudeSquared(vec3{ -2, -1, -1 }) == 6);

	ASSERT_CONDITION(std::abs(magnitude(vec3{ 1, 0, 1 }) - std::sqrt(2)) < 0.0001f);
	ASSERT_CONDITION(std::abs(magnitude(vec3{ 0, 1, 1 }) - std::sqrt(2)) < 0.0001f);

	ASSERT_CONDITION(equal(normalize(vec2{ 0, 1 }), vec2{ 0, 1 }));
	ASSERT_CONDITION(equal(normalize(vec2{ 0, 5 }), vec2{ 0, 1 }));
	ASSERT_CONDITION(equal(normalize(vec2{ 5, 0 }), vec2{ 1, 0 }));
	ASSERT_CONDITION(equal(normalize(vec2{ 5, 5 }), vec2{ 0.707106f, 0.707106f }));

	ASSERT_CONDITION(equal(normalize(vec3{ 5, 0, 0 }), vec3{ 1, 0, 0 }));
	ASSERT_CONDITION(equal(normalize(vec3{ 5, 5, 0 }), vec3{ 0.707106f, 0.707106f, 0 }));
	ASSERT_CONDITION(equal(normalize(vec3{ 5, 0, 5 }), vec3{ 0.707106f, 0, 0.707106f }));
}

TEST_CASE(MatrixVectorMultiplication) {
	ASSERT_CONDITION(equal(position2d(vec2{ 0, 1 }) * vec3 { 1, 0, 1 }, vec3{ 1, 1, 1 }));
	ASSERT_CONDITION(equal(position3d(vec3{ 0, 1, 2 }) * vec4 { 1, 0, 1, 1 }, vec4{ 1, 1, 3, 1 }));
}

TEST_CASE(MatrixMatrixMultiplication) {
	mat3 posMat1 = position2d(vec2{ 0, 1 }) * position2d(vec2{ 1, 1 });
	ASSERT_CONDITION(equal(posMat1 * vec3 { 1, 0, 1 }, vec3{ 2, 2, 1 }));
	mat4 posMat2 = position3d(vec3{ 0, 1, 1 }) * position3d(vec3{ 1, 1, 1 });
	ASSERT_CONDITION(equal(posMat2 * vec4 { 1, 0, 1, 1 }, vec4{ 2, 2, 3, 1 }));
}

TEST_CASE(RotationMatrix) {
	mat2 rot2D = rotation2D(90.0f);
	ASSERT_CONDITION(equal(rot2D * vec2{ 1, 0 }, vec2{ 0, -1 }, 0.001f));
	mat3 rot3D = rotation3D(vec3{ 90.0f, 0, 0 });
	ASSERT_CONDITION(equal(rot3D * vec3{ 0, 1, 0 }, vec3{ 0, 0, -1 }, 0.001f));
}

int main() {
	RUN_TESTS();

	return 0;
}
