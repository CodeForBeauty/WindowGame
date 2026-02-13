/*
* A single header library for basic linear math
*/
#pragma once

#ifndef LM2_NO_OUTPUT_FUNCTIONS
#include <iostream>
#endif

namespace lm2 {

constexpr double PI = 3.14;
constexpr double E = 2.7;
constexpr double PIrad = PI / 180;

// Vector types
template<typename T>
struct vector2D {
	T x, y;
};

template<typename T>
struct vector3D {
	T x, y, z;
};

template<typename T>
struct vector4D {
	T x, y, z, w;
};

using vec2 = vector2D<float>;
using vec3 = vector3D<float>;
using vec4 = vector4D<float>;

// Matrix types
template<typename T>
struct matrix2x2 {
	vector2D<T> x, y;
};

template<typename T>
struct matrix3x3 {
	vector3D<T> x, y, z;
};

template<typename T>
struct matrix4x4 {
	vector4D<T> x, y, z, w;
};

using mat2 = matrix2x2<float>;
using mat3 = matrix3x3<float>;
using mat4 = matrix4x4<float>;

// Quaternion types
template<typename T>
struct quaternionT {
	T w, x, y, z;
};

using quaternion = quaternionT<float>;


// Functions
// Vector
// Dot
template<typename T>
T dot(vector2D<T> a, vector2D<T> b) {
	return a.x * b.x + a.y * b.y;
}
template<typename T>
T dot(vector3D<T> a, vector3D<T> b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
template<typename T>
T dot(vector4D<T> a, vector4D<T> b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
// Cross
template<typename T>
T cross(vector2D<T> a, vector2D<T> b) {
	return a.x * b.y - a.y * b.x;
}
template<typename T>
vector3D<T> cross(vector3D<T> a, vector3D<T> b) {
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}
// Magnitude
// Squared
template<typename T>
T magnitudeSquared(vector2D<T> vec) {
	return vec.x * vec.x + vec.y * vec.y;
}
template<typename T>
T magnitudeSquared(vector3D<T> vec) {
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}
template<typename T>
T magnitudeSquared(vector4D<T> vec) {
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
}
// Normal
template<typename T>
T magnitude(vector2D<T> vec) {
	return std::sqrt(magnitudeSquared(vec));
}
template<typename T>
T magnitude(vector3D<T> vec) {
	return std::sqrt(magnitudeSquared(vec));
}
template<typename T>
T magnitude(vector4D<T> vec) {
	return std::sqrt(magnitudeSquared(vec));
}
// Normalize
template<typename T>
vector2D<T> normalize(vector2D<T> vec) {
	return vec / magnitude(vec);
}
template<typename T>
vector3D<T> normalize(vector3D<T> vec) {
	return vec / magnitude(vec);
}template<typename T>
vector4D<T> normalize(vector4D<T> vec) {
	return vec / magnitude(vec);
}

// Matrix functions
// Get identity Matrices
template<typename T>
matrix2x2<T> identity2x2() {
	return {
		{ static_cast<T>(1.0), static_cast<T>(0.0) },
		{ static_cast<T>(0.0), static_cast<T>(1.0) },
	};
}
template<typename T>
matrix3x3<T> identity3x3() {
	return {
		{ static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0) },
		{ static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0) },
		{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0) },
	};
}
template<typename T>
matrix4x4<T> identity4x4() {
	return {
		{ static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0) },
		{ static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0) },
		{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0) },
		{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0) },
	};
}
// Position Matrices
template<typename T>
matrix3x3<T> position2d(vector2D<T> pos) {
	return {
		{ static_cast<T>(1.0), static_cast<T>(0.0), pos.x },
		{ static_cast<T>(0.0), static_cast<T>(1.0), pos.y },
		{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0) },
	};
}
template<typename T>
matrix4x4<T> position3d(vector3D<T> pos) {
	return {
		{ static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0), pos.x },
		{ static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0), pos.y },
		{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0), pos.z },
		{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0) },
	};
}

// Projection Matrices
// ratio = height / width
template<typename T>
matrix4x4<T> ortho(T left, T right, T bottom, T top, T near, T far, T ratio) {
	return {
		{ static_cast<T>(2.0) / (right - left) * ratio, static_cast<T>(0),                    static_cast<T>(0),                   -( (right + left) / (right - left) ) },
		{ static_cast<T>(0),                            static_cast<T>(2.0) / (top - bottom), static_cast<T>(0),                   -( (top + bottom) / (top - bottom) ) },
		{ static_cast<T>(0),                            static_cast<T>(0),                    static_cast<T>(-2.0) / (far - near), -( (far + near) / (far - near) ) },
		{ static_cast<T>(0),                            static_cast<T>(0),                    static_cast<T>(0),                   static_cast<T>(1.0) },
	};
}
template<typename T>
matrix4x4<T> ortho(T width, T height, T near, T far) {
	return ortho(-width, width, -height, height, near, far, height / width);
}
// ratio = height / width
template<typename T>
matrix4x4<T> perspective(T fov, T near, T far, T ratio) {
	T y = static_cast<T>(1) / std::tan(fov / static_cast<T>(2));
	return {
		{ y * ratio, 0,  0,                                0 },
		{ 0,         y,  0,                                0 },
		{ 0,         0, -( (far + near) / (far - near) ), -( (2 * near * far) / (far - near) ) },
		{ 0,         0, -1,                                0 },
	};
}

// Equal
template<typename T>
bool equal(vector2D<T> a, vector2D<T> b, T epsilon = 0.0001) {
	return (
		std::abs(a.x - b.x) < epsilon &&
		std::abs(a.y - b.y) < epsilon
		);
}
template<typename T>
bool equal(vector3D<T> a, vector3D<T> b, T epsilon = 0.0001) {
	return (
		std::abs(a.x - b.x) < epsilon &&
		std::abs(a.y - b.y) < epsilon &&
		std::abs(a.z - b.z) < epsilon
		);
}
template<typename T>
bool equal(vector4D<T> a, vector4D<T> b, T epsilon = 0.0001) {
	return (
		std::abs(a.x - b.x) < epsilon &&
		std::abs(a.y - b.y) < epsilon &&
		std::abs(a.z - b.z) < epsilon &&
		std::abs(a.w - b.w) < epsilon
		);
}
template<typename T>
bool equal(vector2D<T> a, T b, T epsilon = 0.0001) {
	return equal(a, { b, b }, epsilon);
}
template<typename T>
bool equal(vector3D<T> a, T b, T epsilon = 0.0001) {
	return equal(a, { b, b, b }, epsilon);
}
template<typename T>
bool equal(vector4D<T> a, T b, T epsilon = 0.0001) {
	return equal(a, { b, b, b, b }, epsilon);
}

// Operator overloads

// Component-vise operations
// vector2D
template<typename T>
vector2D<T> operator+(vector2D<T> a, vector2D<T> b) {
	return { a.x + b.x, a.y + b.y };
}
template<typename T>
vector2D<T> operator-(vector2D<T> a, vector2D<T> b) {
	return { a.x - b.x, a.y - b.y };
}
template<typename T>
vector2D<T> operator*(vector2D<T> a, vector2D<T> b) {
	return { a.x * b.x, a.y * b.y };
}
template<typename T>
vector2D<T> operator/(vector2D<T> a, vector2D<T> b) {
	return { 
		b.x != 0 ? a.x / b.x : 0,
		b.y != 0 ? a.y / b.y : 0
	};
}
// vector3D
template<typename T>
vector3D<T> operator+(vector3D<T> a, vector3D<T> b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}
template<typename T>
vector3D<T> operator-(vector3D<T> a, vector3D<T> b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}
template<typename T>
vector3D<T> operator*(vector3D<T> a, vector3D<T> b) {
	return { a.x * b.x, a.y * b.y, a.z * b.z };
}
template<typename T>
vector3D<T> operator/(vector3D<T> a, vector3D<T> b) {
	return {
		b.x != 0 ? a.x / b.x : 0,
		b.y != 0 ? a.y / b.y : 0,
		b.z != 0 ? a.z / b.z : 0
	};
}
// vector4D
template<typename T>
vector4D<T> operator+(vector4D<T> a, vector4D<T> b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}
template<typename T>
vector4D<T> operator-(vector4D<T> a, vector4D<T> b) {
	return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}
template<typename T>
vector4D<T> operator*(vector4D<T> a, vector4D<T> b) {
	return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}
template<typename T>
vector4D<T> operator/(vector4D<T> a, vector4D<T> b) {
	return {
		b.x != 0 ? a.x / b.x : 0,
		b.y != 0 ? a.y / b.y : 0,
		b.z != 0 ? a.z / b.z : 0,
		b.w != 0 ? a.w / b.w : 0,
	};
}
// Scalar operations
// vector2D
template<typename T>
vector2D<T> operator+(vector2D<T> a, T s) {
	return { a.x + s, a.y + s };
}
template<typename T>
vector2D<T> operator-(vector2D<T> a, T s) {
	return { a.x - s, a.y - s };
}
template<typename T>
vector2D<T> operator*(vector2D<T> a, T s) {
	return { a.x * s, a.y * s };
}
template<typename T>
vector2D<T> operator/(vector2D<T> a, T s) {
	if (s == 0) {
		return { 0, 0 };
	}
	return { a.x / s, a.y / s };
}
// vector3D
template<typename T>
vector3D<T> operator+(vector3D<T> a, T s) {
	return { a.x + s, a.y + s, a.z + s };
}
template<typename T>
vector3D<T> operator-(vector3D<T> a, T s) {
	return { a.x - s, a.y - s, a.z - s};
}
template<typename T>
vector3D<T> operator*(vector3D<T> a, T s) {
	return { a.x * s, a.y * s, a.z * s };
}
template<typename T>
vector3D<T> operator/(vector3D<T> a, T s) {
	if (s == 0) {
		return { 0, 0 };
	}
	return { a.x / s, a.y / s, a.z / s };
}
// vector4D
template<typename T>
vector4D<T> operator+(vector4D<T> a, T s) {
	return { a.x + s, a.y + s, a.z + s, a.w + s };
}
template<typename T>
vector4D<T> operator-(vector4D<T> a, T s) {
	return { a.x - s, a.y - s, a.z - s, a.w - s };
}
template<typename T>
vector4D<T> operator*(vector4D<T> a, T s) {
	return { a.x * s, a.y * s, a.z * s, a.w * s };
}
template<typename T>
vector4D<T> operator/(vector4D<T> a, T s) {
	if (s == 0) {
		return { 0, 0 };
	}
	return { a.x / s, a.y / s, a.z / s, a.w / s };
}
// Modulo
template<typename T>
vector2D<T> operator%(vector2D<T> a, T s) {
	if (s == 0) {
		return { 0,  0 };
	}
	return { std::fmod(a.x, s), std::fmod(a.y, s) };
}
template<typename T>
vector3D<T> operator%(vector3D<T> a, T s) {
	if (s == 0) {
		return { 0,  0 };
	}
	return { std::fmod(a.x, s), std::fmod(a.y, s), std::fmod(a.z, s) };
}
template<typename T>
vector4D<T> operator%(vector4D<T> a, T s) {
	if (s == 0) {
		return { 0,  0 };
	}
	return { std::fmod(a.x, s), std::fmod(a.y, s), std::fmod(a.z, s), std::fmod(a.w, s) };
}
// With vectors
template<typename T>
vector2D<T> operator%(vector2D<T> a, vector2D<T> b) {
	return {
		b.x != 0 ? std::fmod(a.x, b.x) : 0,
		b.y != 0 ? std::fmod(a.y, b.y) : 0
	};
}
template<typename T>
vector3D<T> operator%(vector3D<T> a, vector3D<T> b) {
	return {
		b.x != 0 ? std::fmod(a.x, b.x) : 0,
		b.y != 0 ? std::fmod(a.y, b.y) : 0,
		b.z != 0 ? std::fmod(a.z, b.z) : 0
	};
}
template<typename T>
vector4D<T> operator%(vector4D<T> a, vector4D<T> b) {
	return {
		b.x != 0 ? std::fmod(a.x, b.x) : 0,
		b.y != 0 ? std::fmod(a.y, b.y) : 0,
		b.z != 0 ? std::fmod(a.z, b.z) : 0,
		b.w != 0 ? std::fmod(a.w, b.w) : 0
	};
}
// Compound assign operations
// vector2D
template<typename T>
vector2D<T>& operator+=(vector2D<T>& a, vector2D<T> b) {
	return a = a + b;
}
template<typename T>
vector2D<T>& operator-=(vector2D<T>& a, vector2D<T> b) {
	return a = a - b;
}
template<typename T>
vector2D<T>& operator*=(vector2D<T>& a, vector2D<T> b) {
	return a = a * b;
}
template<typename T>
vector2D<T>& operator/=(vector2D<T>& a, vector2D<T> b) {
	return a = a / b;
}
// vector3D
template<typename T>
vector3D<T>& operator+=(vector3D<T>& a, vector3D<T> b) {
	return a = a + b;
}
template<typename T>
vector3D<T>& operator-=(vector3D<T>& a, vector3D<T> b) {
	return a = a - b;
}
template<typename T>
vector3D<T>& operator*=(vector3D<T>& a, vector3D<T> b) {
	return a = a * b;
}
template<typename T>
vector3D<T>& operator/=(vector3D<T>& a, vector3D<T> b) {
	return a = a / b;
}
// vector4D
template<typename T>
vector4D<T>& operator+=(vector4D<T>& a, vector4D<T> b) {
	return a = a + b;
}
template<typename T>
vector4D<T>& operator-=(vector4D<T>& a, vector4D<T> b) {
	return a = a - b;
}
template<typename T>
vector4D<T>& operator*=(vector4D<T>& a, vector4D<T> b) {
	return a = a * b;
}
template<typename T>
vector4D<T>& operator/=(vector4D<T>& a, vector4D<T> b) {
	return a = a / b;
}
// Compound assign operations with scalar
// vector2D
template<typename T>
vector2D<T>& operator+=(vector2D<T>& a, T s) {
	return a = a + s;
}
template<typename T>
vector2D<T>& operator-=(vector2D<T>& a, T s) {
	return a = a - s;
}
template<typename T>
vector2D<T>& operator*=(vector2D<T>& a, T s) {
	return a = a * s;
}
template<typename T>
vector2D<T>& operator/=(vector2D<T>& a, T s) {
	return a = a / s;
}
// vector3D
template<typename T>
vector3D<T>& operator+=(vector3D<T>& a, T s) {
	return a = a + s;
}
template<typename T>
vector3D<T>& operator-=(vector3D<T>& a, T s) {
	return a = a - s;
}
template<typename T>
vector3D<T>& operator*=(vector3D<T>& a, T s) {
	return a = a * s;
}
template<typename T>
vector3D<T>& operator/=(vector3D<T>& a, T s) {
	return a = a / s;
}
// vector4D
template<typename T>
vector4D<T>& operator+=(vector4D<T>& a, T s) {
	return a = a + s;
}
template<typename T>
vector4D<T>& operator-=(vector4D<T>& a, T s) {
	return a = a - s;
}
template<typename T>
vector4D<T>& operator*=(vector4D<T>& a, T s) {
	return a = a * s;
}
template<typename T>
vector4D<T>& operator/=(vector4D<T>& a, T s) {
	return a = a / s;
}
// Modulo
template<typename T>
vector2D<T>& operator%=(vector2D<T>& a, T s) {
	return a = a % s;
}
template<typename T>
vector3D<T>& operator%=(vector3D<T>& a, T s) {
	return a = a % s;
}
template<typename T>
vector4D<T>& operator%=(vector4D<T>& a, T s) {
	return a = a % s;
}

// Unary operations
// Negate
template<typename T>
vector2D<T> operator-(vector2D<T> a) {
	return { -a.x, -a.y };
}
template<typename T>
vector3D<T> operator-(vector3D<T> a) {
	return { -a.x, -a.y, -a.z };
}
template<typename T>
vector4D<T> operator-(vector4D<T> a) {
	return { -a.x, -a.y, -a.z, -a.w };
}
// Increment
template<typename T>
vector2D<T>& operator++(vector2D<T>& a) {
	++a.x;
	++a.y;
	return a;
}
template<typename T>
vector3D<T>& operator++(vector3D<T>& a) {
	++a.x;
	++a.y;
	++a.z;
	return a;
}
template<typename T>
vector4D<T>& operator++(vector4D<T>& a) {
	++a.x;
	++a.y;
	++a.z;
	++a.w;
	return a;
}
// Decrement
template<typename T>
vector2D<T>& operator--(vector2D<T>& a) {
	--a.x;
	--a.y;
	return a;
}
template<typename T>
vector3D<T>& operator--(vector3D<T>& a) {
	--a.x;
	--a.y;
	--a.z;
	return a;
}
template<typename T>
vector4D<T>& operator--(vector4D<T>& a) {
	--a.x;
	--a.y;
	--a.z;
	--a.w;
	return a;
}


#ifndef LM2_NO_OUTPUT_FUNCTIONS

template<typename T>
std::ostream& operator<<(std::ostream& os, vector2D<T> vec) {
	return os << "X: " << vec.x << " Y: " << vec.y;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, vector3D<T> vec) {
	return os << "X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, vector4D<T> vec) {
	return os << "X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << " W: " << vec.w;
}


template<typename T>
std::ostream& operator<<(std::ostream& os, matrix2x2<T> mat) {
	return os << "X: " << mat.x << "\nY: " << mat.y;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, matrix3x3<T> mat) {
	return os << "X: " << mat.x << "\nY: " << mat.y << "\nZ: " << mat.z;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, matrix4x4<T> mat) {
	return os << "X: " << mat.x << "\nY: " << mat.y << "\nZ: " << mat.z << "\nW: " << mat.w;
}

#endif // #ifndef LM2_NO_OUTPUT_FUNCTIONS
} // namespace lm2
