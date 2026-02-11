/*
* A single header library for basic linear math
*/

#ifndef LM2_NO_OUTPUT_FUNCTIONS
#include <iostream>
#endif

namespace lm2 {

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
	return { a.x / b.x, a.y / b.y };
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
	return { a.x / b.x, a.y / b.y, a.z / b.z };
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
	return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
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
	return { a.x / s, a.y / s, a.z / s, a.w / s };
}
// Modulo
template<typename T>
vector2D<T> operator%=(vector2D<T> a, T s) {
	return {a.x % s, a.y % s };
}
template<typename T>
vector3D<T> operator%=(vector3D<T> a, T s) {
	return { a.x % s, a.y % s, a.z % s };
}
template<typename T>
vector4D<T> operator%=(vector4D<T> a, T s) {
	return { a.x % s, a.y % s, a.z % s, a.w % s };
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
vector2D<T>& operator-(vector2D<T>& a) {
	a = { -a.x, -a.y };
	return a;
}
template<typename T>
vector3D<T>& operator-(vector3D<T>& a) {
	a = { -a.x, -a.y, -a.z };
	return a;
}
template<typename T>
vector4D<T>& operator-(vector4D<T>& a) {
	a = { -a.x, -a.y, -a.z, -a.w };
	return a;
}
// Compound add
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
// Compound subtract
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