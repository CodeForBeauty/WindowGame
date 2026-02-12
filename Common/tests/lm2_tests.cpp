#include "lm2.hpp"
#include "testlib.hpp"
#include <iostream>

using namespace lm2;

TEST_CASE(VectorAddition) {
	vec2 v2{ 1, 1 };

	ASSERT_CONDITION(equal(v2 + 1.0f, vec2(2, 2)));
	ASSERT_CONDITION(equal(v2 + vec2{ 1, 0 }, vec2(2, 1)));
	ASSERT_CONDITION(equal(v2 + vec2{ 0, 1 }, vec2(1, 2)));

	vec3 v3{ 1, 1, 1 };

	ASSERT_CONDITION(equal(v3 + 1.0f, vec3{ 2, 2, 2 }));
	ASSERT_CONDITION(equal(v3 + vec3{ 1, 0, 0 }, vec3{ 2, 1, 1 }));
	ASSERT_CONDITION(equal(v3 + vec3{ 0, 1, 0 }, vec3{ 1, 2, 1 }));
	ASSERT_CONDITION(equal(v3 + vec3{ 0, 0, 1 }, vec3{ 1, 1, 2 }));

	vec4 v4{ 1, 1, 1, 1 };

	ASSERT_CONDITION(equal(v4 + 1.0f, vec4{ 2, 2, 2, 2 }));
	ASSERT_CONDITION(equal(v4 + vec4{ 1, 0, 0, 0 }, vec4{ 2, 1, 1, 1 }));
	ASSERT_CONDITION(equal(v4 + vec4{ 0, 1, 0, 0 }, vec4{ 1, 2, 1, 1 }));
	ASSERT_CONDITION(equal(v4 + vec4{ 0, 0, 1, 0 }, vec4{ 1, 1, 2, 1 }));
	ASSERT_CONDITION(equal(v4 + vec4{ 0, 0, 0, 1 }, vec4{ 1, 1, 1, 2 }));
}

int main() {
	RUN_TESTS();

	return 0;
}
