/*
* A simple single header Unit testing library
*/

#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>

enum class LogColor {
	Normal,
	White,
	Red,
	Green,
	Yellow,
};

struct TestFail_Type {
	std::string_view what;
};

static class TestRunner {
public:
	void RegisterTest(const char* name, std::function<void()> func) {
		mTests.emplace_back(name, func);
	}

	void RunTests(std::string file) {
		int failed = 0;
		int passed = 0;
		Log("Running tests from: " + file, LogColor::White);
		for (auto& test : mTests) {
			std::cout << "  ";
			Log(test.first, LogColor::White);
			try {
				test.second();
				Log("    passed", LogColor::Green);
				passed++;
			}
			catch (TestFail_Type fail) {
				Log("    failed", LogColor::Red);
				std::cout << "    ";
				Log(fail.what, LogColor::Red);
				failed++;
			}
			catch (std::exception e) {
				std::cout << "    ";
				Log(e.what(), LogColor::Red);
				failed++;
			}
		}

		std::cout << "Passed: " << passed << ", Failed: " << failed << "\n";
	}

	void Log(std::string_view log, LogColor color) {
		switch (color) {
		case LogColor::Normal:
			break;
		case LogColor::White:
			std::cout << "\033[0;37m";
			break;
		case LogColor::Red:
			std::cout << "\033[0;31m";
			break;
		case LogColor::Green:
			std::cout << "\033[0;32m";
			break;
		case LogColor::Yellow:
			std::cout << "\033[0;33m";
			break;
		default:
			break;
		}
		std::cout << log << "\033[0m\n";
	}

private:
	std::vector< std::pair< std::string_view, std::function<void()> > >  mTests;
} testRunner_Test;

#define TEST_CASE(testName) \
	void testName##_Test(); \
	struct testName##_Registrar { \
		testName##_Registrar() { \
			testRunner_Test.RegisterTest(#testName, testName##_Test); \
		}\
	} testName##_RegInstance;\
	void testName##_Test()

#define RUN_TESTS() testRunner_Test.RunTests((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))

#define ASSERT_CONDITION(cond) \
	if (!(cond)) { \
		throw TestFail_Type{.what = #cond}; \
	} \
