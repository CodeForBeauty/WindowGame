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

int main() {
	RUN_TESTS();

	return 0;
}
