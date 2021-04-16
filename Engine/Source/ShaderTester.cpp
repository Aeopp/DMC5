//#include "ShaderTester.h"
//#include <d3d9.h>
//#include <d3dx9.h>
//#include <d3dx9mesh.h>
//#include <fstream>
//#include <ostream>
//#include "RenderTarget.h"
//#include "Shader.h"
//#include <array>
//#include "FMath.hpp"
//#include "RenderTarget.h"
//#include "Shader.h"
//#include "Resources.h"
//
//#include "StaticMesh.h"
//#include "TimeSystem.h"
//
//
//USING(ENGINE)
//
//#include <cstdint>
//#include <cfloat>
//#include <cmath>
//#include <cstdarg>
//#include <string>
//
//float DXScreenQuadVertices[24] = {
//	-1, -1, 0, 1,	0, 1,
//	-1, 1, 0, 1,	0, 0,
//	1, -1, 0, 1,	1, 1,
//	1, 1, 0, 1,		1, 0
//};
//HRESULT DXCreateEffect(LPDIRECT3DDEVICE9 device, LPCTSTR file, LPD3DXEFFECT* out)
//{
//	HRESULT hr;
//	LPD3DXBUFFER errors = NULL;
//
//	if (FAILED(hr = D3DXCreateEffectFromFile(device, file, NULL, NULL, D3DXSHADER_DEBUG, NULL, out, &errors))) {
//		if (errors) {
//			char* str = (char*)errors->GetBufferPointer();
//			std::cout << str << "\n\n";
//		}
//	}
//
//	if (errors)
//		errors->Release();
//
//	return hr;
//}
//// what: FUNC_PROTO\(([^\)]+)\)
//// with: $1
//
//#define ARRAY_SIZE(x)			(sizeof(x) / sizeof(x[0]))
//#define MAKE_DWORD_PTR(a, b)	reinterpret_cast<void*>(((a << 16) & 0xffff0000ULL) | b)
//
//namespace Math {
//
//	static const float PI = 3.141592f;
//	static const float TWO_PI = 6.283185f;
//	static const float HALF_PI = 1.570796f;
//	static const float ONE_OVER_SQRT_2 = 0.7071067f;
//	static const float ONE_OVER_SQRT_TWO_PI = 0.39894228f;
//
//	// --- Structures -------------------------------------------------------------
//
//	struct Complex
//	{
//		float a;
//		float b;
//
//		Complex();
//		Complex(float re, float im);
//
//		Complex& operator +=(const Complex& other);
//
//		inline Complex operator +(const Complex& other) { return Complex(a + other.a, b + other.b); }
//		inline Complex operator -(const Complex& other) { return Complex(a - other.a, b - other.b); }
//		inline Complex operator *(const Complex& other) { return Complex(a * other.a - b * other.b, b * other.a + a * other.b); }
//	};
//
//	struct Vector2
//	{
//		float x, y;
//
//		Vector2();
//		Vector2(const Vector2& other);
//		Vector2(float _x, float _y);
//		Vector2(const float* values);
//
//		Vector2& operator =(const Vector2& other);
//
//		inline operator float* () { return &x; }
//		inline operator const float* () const { return &x; }
//
//		inline float& operator [](int index) { return *(&x + index); }
//		inline float operator [](int index) const { return *(&x + index); }
//	};
//
//	struct Vector3
//	{
//		float x, y, z;
//
//		Vector3();
//		Vector3(const Vector3& other);
//		Vector3(float _x, float _y, float _z);
//		Vector3(const float* values);
//
//		Vector3 operator +(const Vector3& v) const;
//		Vector3 operator -(const Vector3& v) const;
//		Vector3 operator *(float s) const;
//
//		Vector3 operator -() const;
//
//		Vector3& operator =(const Vector3& other);
//		Vector3& operator +=(const Vector3& other);
//		Vector3& operator *=(float s);
//
//		inline operator float* () { return &x; }
//		inline operator const float* () const { return &x; }
//
//		inline float& operator [](int index) { return *(&x + index); }
//		inline float operator [](int index) const { return *(&x + index); }
//	};
//
//	struct Vector4
//	{
//		float x, y, z, w;
//
//		Vector4();
//		Vector4(const Vector4& other);
//		Vector4(const Vector3& v, float w);
//		Vector4(const Vector2& v, float z, float w);
//		Vector4(float _x, float _y, float _z, float _w);
//		Vector4(const float* values);
//
//		Vector4 operator +(const Vector4& v) const;
//		Vector4 operator -(const Vector4& v) const;
//		Vector4 operator *(float s) const;
//		Vector4 operator /(float s) const;
//
//		Vector4& operator =(const Vector4& other);
//		Vector4& operator /=(float s);
//
//		inline operator float* () { return &x; }
//		inline operator const float* () const { return &x; }
//
//		inline float& operator [](int index) { return *(&x + index); }
//		inline float operator [](int index) const { return *(&x + index); }
//	};
//
//	struct Quaternion
//	{
//		float x, y, z, w;
//
//		Quaternion();
//		Quaternion(const Quaternion& other);
//		Quaternion(float _x, float _y, float _z, float _w);
//
//		Quaternion& operator =(const Quaternion& other);
//
//		inline operator float* () { return &x; }
//		inline operator const float* () const { return &x; }
//
//		inline float& operator [](int index) { return *(&x + index); }
//		inline float operator [](int index) const { return *(&x + index); }
//	};
//
//	struct Matrix
//	{
//		// NOTE: row-major (multiply with it from the right)
//
//		float _11, _12, _13, _14;
//		float _21, _22, _23, _24;
//		float _31, _32, _33, _34;
//		float _41, _42, _43, _44;	// translation goes here
//
//		Matrix();
//		Matrix(const Matrix& other);
//		Matrix(float v11, float v22, float v33, float v44);
//		Matrix(
//			float v11, float v12, float v13, float v14,
//			float v21, float v22, float v23, float v24,
//			float v31, float v32, float v33, float v34,
//			float v41, float v42, float v43, float v44);
//		Matrix(const float* values);
//
//		Matrix& operator =(const Matrix& other);
//
//		inline operator float* () { return &_11; }
//		inline operator const float* () const { return &_11; }
//
//		inline float* operator [](int row) { return (&_11 + 4 * row); }
//		inline const float* operator [](int row) const { return (&_11 + 4 * row); }
//	};
//
//	// --- Template classes -------------------------------------------------------
//
//	template <typename T, int n>
//	class InterpolationArray
//	{
//		static_assert(n > 0, "InterpolationArray: template parameter must be greater than zero");
//
//	private:
//		T prev[n];
//		T curr[n];
//
//	public:
//		InterpolationArray& operator =(T t[n]);
//
//		void Advance(T t[n]);
//		void Set(T v1, ...);
//		void Smooth(T out[n], T alpha);
//
//		inline T operator [](int index) const { return curr[index]; }
//	};
//
//	template <typename T, int n>
//	InterpolationArray<T, n>& InterpolationArray<T, n>::operator =(T t[n])
//	{
//		for (int i = 0; i < n; ++i)
//			prev[i] = curr[i] = t[i];
//
//		return *this;
//	}
//
//	template <typename T, int n>
//	void InterpolationArray<T, n>::Advance(T t[n])
//	{
//		for (int i = 0; i < n; ++i) {
//			prev[i] = curr[i];
//			curr[i] += t[i];
//		}
//	}
//
//	template <typename T, int n>
//	void InterpolationArray<T, n>::Set(T v1, ...)
//	{
//		va_list args;
//		va_start(args, v1);
//
//		prev[0] = curr[0] = v1;
//
//		if (std::is_same<T, float>::value) {
//			// promoted to double
//			for (int i = 1; i < n; ++i) {
//				prev[i] = curr[i] = (float)va_arg(args, double);
//			}
//		}
//		else {
//			// ignore
//			for (int i = 1; i < n; ++i) {
//				prev[i] = curr[i] = va_arg(args, T);
//			}
//		}
//
//		va_end(args);
//	}
//
//	template <typename T, int n>
//	void InterpolationArray<T, n>::Smooth(T out[n], T alpha)
//	{
//		for (int i = 0; i < n; ++i)
//			out[i] = prev[i] + alpha * (curr[i] - prev[i]);
//	}
//
//	// --- Classes ----------------------------------------------------------------
//
//	class Float16
//	{
//	private:
//		uint16_t bits;
//
//	public:
//		Float16();
//		explicit Float16(float f);
//		explicit Float16(uint16_t s);
//
//		Float16& operator =(const Float16& other);
//		Float16& operator =(float f);
//		Float16& operator =(uint16_t s);
//		operator float() const;
//
//		inline operator uint16_t() const { return bits; }
//	};
//
//	static_assert(sizeof(Float16) == sizeof(uint16_t), "Math::Float16 must be 2 bytes in size");
//
//	class Color
//	{
//	public:
//		float r, g, b, a;
//
//		Color();
//		Color(float _r, float _g, float _b, float _a);
//		Color(uint32_t argb32);
//
//		Color& operator =(const Color& other);
//		Color& operator *=(const Color& other);
//
//		static Color Lerp(const Color& from, const Color& to, float frac);
//		static Color sRGBToLinear(uint32_t argb32);
//		static Color sRGBToLinear(uint8_t red, uint8_t green, uint8_t blue);
//		static Color sRGBToLinear(float red, float green, float blue, float alpha);
//
//		static inline uint8_t ArgbA32(uint32_t c) { return ((uint8_t)((c >> 24) & 0xff)); }
//		static inline uint8_t ArgbR32(uint32_t c) { return ((uint8_t)((c >> 16) & 0xff)); }
//		static inline uint8_t ArgbG32(uint32_t c) { return ((uint8_t)((c >> 8) & 0xff)); }
//		static inline uint8_t ArgbB32(uint32_t c) { return ((uint8_t)(c & 0xff)); }
//
//		operator uint32_t() const;
//
//		inline operator float* () { return &r; }
//		inline operator const float* () const { return &r; }
//	};
//
//	class AABox
//	{
//	public:
//		Vector3 Min;
//		Vector3 Max;
//
//		AABox();
//		AABox(const AABox& other);
//		AABox(const Vector3& size);
//		AABox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax);
//
//		AABox& operator =(const AABox& other);
//
//		bool Intersects(const AABox& other) const;
//
//		void Add(float x, float y, float z);
//		void Add(const Vector3& p);
//		void GetCenter(Vector3& out) const;
//		void GetSize(Vector3& out) const;
//		void GetHalfSize(Vector3& out) const;
//		void GetPlanes(Vector4 out[6]) const;
//		void Inset(float dx, float dy, float dz);
//		void TransformAxisAligned(const Matrix& m);
//
//		float Radius() const;
//		float RayIntersect(const Vector3& start, const Vector3& dir) const;
//		float Nearest(const Vector4& from) const;
//		float Farthest(const Vector4& from) const;
//	};
//
//	// --- Math functions ---------------------------------------------------------
//
//	uint8_t FloatToByte(float f);
//	int32_t ISqrt(int32_t n);
//	uint32_t NextPow2(uint32_t x);
//	uint32_t Log2OfPow2(uint32_t x);
//	uint32_t ReverseBits32(uint32_t bits);
//	uint32_t Vec3ToUbyte4(const Math::Vector3& v);
//
//	float Vec2Dot(const Vector2& a, const Vector2& b);
//	float Vec2Length(const Vector2& v);
//
//	float Vec3Dot(const Vector3& a, const Vector3& b);
//	float Vec3Length(const Vector3& v);
//	float Vec3Distance(const Vector3& a, const Vector3& b);
//
//	float PlaneDotCoord(const Vector4& plane, const Vector3& p);
//
//	void Vec2Normalize(Vector2& out, const Vector2& v);
//	void Vec2Subtract(Vector2& out, const Vector2& a, const Vector2& b);
//
//	void Vec3Lerp(Vector3& out, const Vector3& a, const Vector3& b, float s);
//	void Vec3Add(Vector3& out, const Vector3& a, const Vector3& b);
//	void Vec3Mad(Vector3& out, const Vector3& a, const Vector3& b, float s);
//	void Vec3Normalize(Vector3& out, const Vector3& v);
//	void Vec3Scale(Vector3& out, const Vector3& v, float scale);
//	void Vec3Subtract(Vector3& out, const Vector3& a, const Vector3& b);
//	void Vec3Cross(Vector3& out, const Vector3& a, const Vector3& b);
//	void Vec3Rotate(Vector3& out, const Vector3& v, const Quaternion& q);
//	void Vec3Transform(Vector4& out, const Vector3& v, const Matrix& m);
//	void Vec3TransformTranspose(Vector4& out, const Matrix& m, const Vector3& v);
//	void Vec3TransformCoord(Vector3& out, const Vector3& v, const Matrix& m);
//	void Vec3TransformCoordTranspose(Vector3& out, const Matrix& m, const Vector3& v);
//	void Vec3TransformNormal(Vector3& out, const Vector3& v, const Matrix& m);
//	void Vec3TransformNormalTranspose(Vector3& out, const Matrix& m, const Vector3& v);
//
//	void Vec4Lerp(Vector4& out, const Vector4& a, const Vector4& b, float s);
//	void Vec4Add(Vector4& out, const Vector4& a, const Vector4& b);
//	void Vec4Subtract(Vector4& out, const Vector4& a, const Vector4& b);
//	void Vec4Scale(Vector4& out, const Vector4& v, float scale);
//	void Vec4Transform(Vector4& out, const Vector4& v, const Matrix& m);
//	void Vec4TransformTranspose(Vector4& out, const Matrix& m, const Vector4& v);
//
//	void PlaneFromNormalAndPoint(Vector4& out, const Vector3& n, const Vector3& p);
//	void PlaneNormalize(Vector4& out, const Vector4& p);
//	void PlaneTransform(Vector4& out, const Vector4& p, const Matrix& m);
//	void PlaneTransformTranspose(Vector4& out, const Matrix& m, const Vector4& p);
//
//	void QuaternionConjugate(Quaternion& out, const Quaternion& q);
//	void QuaternionIdentity(Quaternion& out);
//	void QuaternionMultiply(Quaternion& out, const Quaternion& a, const Quaternion& b);
//	void QuaternionNormalize(Quaternion& out, const Quaternion& q);
//	void QuaternionRotationAxis(Quaternion& out, float angle, float x, float y, float z);
//
//	void MatrixIdentity(Matrix& out);
//	void MatrixInverse(Matrix& out, const Matrix& m);
//	void MatrixLookAtLH(Matrix& out, const Vector3& eye, const Vector3& look, const Vector3& up);
//	void MatrixLookAtRH(Matrix& out, const Vector3& eye, const Vector3& look, const Vector3& up);
//	void MatrixMultiply(Matrix& out, const Matrix& a, const Matrix& b);
//	void MatrixPerspectiveFovLH(Matrix& out, float fovy, float aspect, float nearplane, float farplane);
//	void MatrixPerspectiveFovRH(Matrix& out, float fovy, float aspect, float nearplane, float farplane);
//	void MatrixOrthoOffCenterRH(Matrix& out, float left, float right, float bottom, float top, float nearplane, float farplane);
//	void MatrixReflect(Math::Matrix& out, const Math::Vector4& plane);
//	void MatrixRotationAxis(Matrix& out, float angle, float x, float y, float z);
//	void MatrixRotationQuaternion(Matrix& out, const Quaternion& q);
//	void MatrixScaling(Matrix& out, float x, float y, float z);
//	void MatrixTranslation(Matrix& out, float x, float y, float z);
//	void MatrixTranspose(Matrix& out, Matrix m);
//	void MatrixViewVector(Matrix& out, const Vector3& viewdir);
//
//	void FrustumPlanes(Math::Vector4 out[6], const Math::Matrix& viewproj);
//	void FitToBoxPerspective(Vector2& out, const Vector3& refpoint, const Vector3& forward, const AABox& box);
//	void FitToBoxOrtho(Matrix& outproj, Vector2& outclip, const Matrix& view, const AABox& box);
//	void GetOrthogonalVectors(Vector3& out1, Vector3& out2, const Vector3& v);
//
//	int FrustumIntersect(const Math::Vector4 frustum[6], const AABox& box);
//
//	// --- Utility functions ------------------------------------------------------
//
//	float Gaussian(float x, float stddev);
//
//	std::string& GetPath(std::string& out, const std::string& str);
//	std::string& GetFile(std::string& out, const std::string& str);
//	std::string& GetExtension(std::string& out, const std::string& str);
//	std::string& ToLower(std::string& out, const std::string& str);
//
//	// --- Inline functions -------------------------------------------------------
//
//	template <typename T>
//	inline const T& Min(const T& a, const T& b) {
//		return ((a < b) ? a : b);
//	}
//
//	template <typename T>
//	inline const T& Max(const T& a, const T& b) {
//		return ((a > b) ? a : b);
//	}
//
//	template <typename T>
//	void Swap(T& a, T& b)
//	{
//		T tmp = a;
//		a = b;
//		b = tmp;
//	}
//
//	inline float DegreesToRadians(float value) {
//		return value * (PI / 180.0f);
//	}
//
//	inline float RadiansToDegrees(float value) {
//		return value * (180.0f / PI);
//	}
//
//	inline float Clamp(float value, float min, float max) {
//		return Min(Max(min, value), max);
//	}
//
//	inline float RandomFloat() {
//		return (rand() & 32767) / 32767.0f;
//	}
//
//	inline float Lerp(float a, float b, float s) {
//		return (1.0f - s) * a + s * b;
//	}
//
//	inline float VerticalFov(float hfov, float w, float h) {
//		return atan2f(h * tanf(hfov * 0.5f), w) * 2;
//	}
//
//	inline float HorizontalFov(float vfov, float w, float h) {
//		return atan2f(w * tanf(vfov * 0.5f), h) * 2;
//	}
//
//}
//
//// --- Operator overloads -----------------------------------------------------
//
//Math::Vector2 operator *(float f, const Math::Vector2& v);
//Math::Vector3 operator *(float f, const Math::Vector3& v);
//Math::Vector4 operator *(float f, const Math::Vector4& v);
//
//Math::Vector4 operator /(float f, const Math::Vector4& v);
//
//namespace Math {
//
//	// --- Structures impl --------------------------------------------------------
//
//	Complex::Complex()
//	{
//		a = b = 0;
//	}
//
//	Complex::Complex(float re, float im)
//	{
//		a = re;
//		b = im;
//	}
//
//	Complex& Complex::operator +=(const Complex& other)
//	{
//		a += other.a;
//		b += other.b;
//
//		return *this;
//	}
//
//	Vector2::Vector2()
//	{
//		x = y = 0;
//	}
//
//	Vector2::Vector2(const Vector2& other)
//	{
//		x = other.x;
//		y = other.y;
//	}
//
//	Vector2::Vector2(float _x, float _y)
//	{
//		x = _x;
//		y = _y;
//	}
//
//	Vector2::Vector2(const float* values)
//	{
//		x = values[0];
//		y = values[1];
//	}
//
//	Vector2& Vector2::operator =(const Vector2& other)
//	{
//		x = other.x;
//		y = other.y;
//
//		return *this;
//	}
//
//	Vector3::Vector3()
//	{
//		x = y = z = 0;
//	}
//
//	Vector3::Vector3(const Vector3& other)
//	{
//		x = other.x;
//		y = other.y;
//		z = other.z;
//	}
//
//	Vector3::Vector3(float _x, float _y, float _z)
//	{
//		x = _x;
//		y = _y;
//		z = _z;
//	}
//
//	Vector3::Vector3(const float* values)
//	{
//		x = values[0];
//		y = values[1];
//		z = values[2];
//	}
//
//	Vector3 Vector3::operator +(const Vector3& v) const
//	{
//		return Vector3(x + v.x, y + v.y, z + v.z);
//	}
//
//	Vector3 Vector3::operator -(const Vector3& v) const
//	{
//		return Vector3(x - v.x, y - v.y, z - v.z);
//	}
//
//	Vector3 Vector3::operator *(float s) const
//	{
//		return Vector3(x * s, y * s, z * s);
//	}
//
//	Vector3 Vector3::operator -() const
//	{
//		return Vector3(-x, -y, -z);
//	}
//
//	Vector3& Vector3::operator =(const Vector3& other)
//	{
//		x = other.x;
//		y = other.y;
//		z = other.z;
//
//		return *this;
//	}
//
//	Vector3& Vector3::operator +=(const Vector3& other)
//	{
//		x += other.x;
//		y += other.y;
//		z += other.z;
//
//		return *this;
//	}
//
//	Vector3& Vector3::operator *=(float s)
//	{
//		x *= s;
//		y *= s;
//		z *= s;
//
//		return *this;
//	}
//
//	Vector4::Vector4()
//	{
//		x = y = z = w = 0;
//	}
//
//	Vector4::Vector4(const Vector4& other)
//	{
//		x = other.x;
//		y = other.y;
//		z = other.z;
//		w = other.w;
//	}
//
//	Vector4::Vector4(const Vector3& v, float w)
//	{
//		x = v.x;
//		y = v.y;
//		z = v.z;
//
//		this->w = w;
//	}
//
//	Vector4::Vector4(const Vector2& v, float z, float w)
//	{
//		x = v.x;
//		y = v.y;
//
//		this->z = z;
//		this->w = w;
//	}
//
//	Vector4::Vector4(float _x, float _y, float _z, float _w)
//	{
//		x = _x;
//		y = _y;
//		z = _z;
//		w = _w;
//	}
//
//	Vector4::Vector4(const float* values)
//	{
//		x = values[0];
//		y = values[1];
//		z = values[2];
//		w = values[3];
//	}
//
//	Vector4 Vector4::operator +(const Vector4& v) const
//	{
//		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
//	}
//
//	Vector4 Vector4::operator -(const Vector4& v) const
//	{
//		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
//	}
//
//	Vector4 Vector4::operator *(float s) const
//	{
//		return Vector4(x * s, y * s, z * s, w * s);
//	}
//
//	Vector4 Vector4::operator /(float s) const
//	{
//		return Vector4(x / s, y / s, z / s, w / s);
//	}
//
//	Vector4& Vector4::operator =(const Vector4& other)
//	{
//		x = other.x;
//		y = other.y;
//		z = other.z;
//		w = other.w;
//
//		return *this;
//	}
//
//	Vector4& Vector4::operator /=(float s)
//	{
//		x /= s;
//		y /= s;
//		z /= s;
//		w /= s;
//
//		return *this;
//	}
//
//	Quaternion::Quaternion()
//	{
//		x = y = z = 0;
//		w = 1;
//	}
//
//	Quaternion::Quaternion(const Quaternion& other)
//	{
//		x = other.x;
//		y = other.y;
//		z = other.z;
//		w = other.w;
//	}
//
//	Quaternion::Quaternion(float _x, float _y, float _z, float _w)
//	{
//		x = _x;
//		y = _y;
//		z = _z;
//		w = _w;
//	}
//
//	Quaternion& Quaternion::operator =(const Quaternion& other)
//	{
//		x = other.x;
//		y = other.y;
//		z = other.z;
//		w = other.w;
//
//		return *this;
//	}
//
//	Matrix::Matrix()
//	{
//		_11 = _12 = _13 = _14 = 0;
//		_21 = _22 = _23 = _24 = 0;
//		_31 = _32 = _33 = _34 = 0;
//		_41 = _42 = _43 = _44 = 0;
//	}
//
//	Matrix::Matrix(const Matrix& other)
//	{
//		operator =(other);
//	}
//
//	Matrix::Matrix(float v11, float v22, float v33, float v44)
//	{
//		_12 = _13 = _14 = 0;
//		_21 = _23 = _24 = 0;
//		_31 = _32 = _34 = 0;
//		_41 = _42 = _43 = 0;
//
//		_11 = v11;
//		_22 = v22;
//		_33 = v33;
//		_44 = v44;
//	}
//
//	Matrix::Matrix(
//		float v11, float v12, float v13, float v14,
//		float v21, float v22, float v23, float v24,
//		float v31, float v32, float v33, float v34,
//		float v41, float v42, float v43, float v44)
//	{
//		_11 = v11;	_21 = v21;	_31 = v31;	_41 = v41;
//		_12 = v12;	_22 = v22;	_32 = v32;	_42 = v42;
//		_13 = v13;	_23 = v23;	_33 = v33;	_43 = v43;
//		_14 = v14;	_24 = v24;	_34 = v34;	_44 = v44;
//	}
//
//	Matrix::Matrix(const float* values)
//	{
//		_11 = values[0];	_21 = values[4];
//		_12 = values[1];	_22 = values[5];
//		_13 = values[2];	_23 = values[6];
//		_14 = values[3];	_24 = values[7];
//
//		_31 = values[8];	_41 = values[12];
//		_32 = values[9];	_42 = values[13];
//		_33 = values[10];	_43 = values[14];
//		_34 = values[11];	_44 = values[15];
//	}
//
//	Matrix& Matrix::operator =(const Matrix& other)
//	{
//		_11 = other._11;	_21 = other._21;
//		_12 = other._12;	_22 = other._22;
//		_13 = other._13;	_23 = other._23;
//		_14 = other._14;	_24 = other._24;
//
//		_31 = other._31;	_41 = other._41;
//		_32 = other._32;	_42 = other._42;
//		_33 = other._33;	_43 = other._43;
//		_34 = other._34;	_44 = other._44;
//
//		return *this;
//	}
//
//	// --- Float16 impl -----------------------------------------------------------
//
//	Float16::Float16()
//	{
//		bits = 0;
//	}
//
//	Float16::Float16(float f)
//	{
//		operator =(f);
//	}
//
//	Float16::Float16(uint16_t s)
//	{
//		bits = s;
//	}
//
//	Float16& Float16::operator =(float f)
//	{
//		uint32_t fp32 = *((uint32_t*)&f);
//		uint32_t signbit = (fp32 & 0x80000000) >> 16;
//		uint32_t mant = (fp32 & 0x007fffff);
//		int exp = ((fp32 & 0x7f800000) >> 23) - 112;
//
//		if (exp <= 0)
//			bits = 0;
//		else if (exp > 30)
//			bits = (uint16_t)(signbit | 0x7bff);
//		else
//			bits = (uint16_t)(signbit | (exp << 10) | (mant >> 13));
//
//		return *this;
//	}
//
//	Float16& Float16::operator =(uint16_t s)
//	{
//		bits = s;
//		return *this;
//	}
//
//	Float16& Float16::operator =(const Float16& other)
//	{
//		bits = other.bits;
//		return *this;
//	}
//
//	Float16::operator float() const
//	{
//		uint32_t magic = 126 << 23;
//		uint32_t fp32 = (bits & 0x8000) << 16;
//		uint32_t mant = (bits & 0x000003ff);
//		int exp = (bits >> 10) & 0x0000001f;
//
//		if (exp == 0) {
//			fp32 = magic + mant;
//			(*(float*)&fp32) -= (*(float*)&magic);
//		}
//		else {
//			mant <<= 13;
//
//			if (exp == 31)
//				exp = 255;
//			else
//				exp += 127 - 15;
//
//			fp32 |= (exp << 23);
//			fp32 |= mant;
//		}
//
//		return *((float*)&fp32);
//	}
//
//	// --- Color impl -------------------------------------------------------------
//
//	Color::Color()
//		: r(0), g(0), b(0), a(1)
//	{
//	}
//
//	Color::Color(float _r, float _g, float _b, float _a)
//		: r(_r), g(_g), b(_b), a(_a)
//	{
//	}
//
//	Color::Color(uint32_t argb32)
//	{
//		a = ArgbA32(argb32) / 255.0f;
//		r = ArgbR32(argb32) / 255.0f;
//		g = ArgbG32(argb32) / 255.0f;
//		b = ArgbB32(argb32) / 255.0f;
//	}
//
//	Color& Color::operator =(const Color& other)
//	{
//		r = other.r;
//		g = other.g;
//		b = other.b;
//		a = other.a;
//
//		return *this;
//	}
//
//	Color& Color::operator *=(const Color& other)
//	{
//		r *= other.r;
//		g *= other.g;
//		b *= other.b;
//		a *= other.a;
//
//		return *this;
//	}
//
//	Color Color::Lerp(const Color& from, const Color& to, float frac)
//	{
//		Color ret;
//
//		ret.r = from.r * (1 - frac) + to.r * frac;
//		ret.g = from.g * (1 - frac) + to.g * frac;
//		ret.b = from.b * (1 - frac) + to.b * frac;
//		ret.a = from.a * (1 - frac) + to.a * frac;
//
//		return ret;
//	}
//
//	Color Color::sRGBToLinear(uint32_t argb32)
//	{
//		Color ret = sRGBToLinear(ArgbR32(argb32), ArgbG32(argb32), ArgbB32(argb32));
//		ret.a = ArgbA32(argb32) / 255.0f;
//
//		return ret;
//	}
//
//	Color Color::sRGBToLinear(uint8_t red, uint8_t green, uint8_t blue)
//	{
//		Color ret;
//
//		float lo_r = (float)red / 3294.6f;
//		float lo_g = (float)green / 3294.6f;
//		float lo_b = (float)blue / 3294.6f;
//
//		float hi_r = powf((red / 255.0f + 0.055f) / 1.055f, 2.4f);
//		float hi_g = powf((green / 255.0f + 0.055f) / 1.055f, 2.4f);
//		float hi_b = powf((blue / 255.0f + 0.055f) / 1.055f, 2.4f);
//
//		ret.r = (red < 10 ? lo_r : hi_r);
//		ret.g = (green < 10 ? lo_g : hi_g);
//		ret.b = (blue < 10 ? lo_b : hi_b);
//		ret.a = 1;
//
//		return ret;
//	}
//
//	Color Color::sRGBToLinear(float red, float green, float blue, float alpha)
//	{
//		Color ret;
//
//		float lo_r = (float)red / 12.92f;
//		float lo_g = (float)green / 12.92f;
//		float lo_b = (float)blue / 12.92f;
//
//		float hi_r = powf((red + 0.055f) / 1.055f, 2.4f);
//		float hi_g = powf((green + 0.055f) / 1.055f, 2.4f);
//		float hi_b = powf((blue + 0.055f) / 1.055f, 2.4f);
//
//		ret.r = (red < 0.04f ? lo_r : hi_r);
//		ret.g = (green < 0.04f ? lo_g : hi_g);
//		ret.b = (blue < 0.04f ? lo_b : hi_b);
//		ret.a = alpha;
//
//		return ret;
//	}
//
//	Color::operator uint32_t() const
//	{
//		uint8_t _a = (uint8_t)(a * 255);
//		uint8_t _r = (uint8_t)(r * 255);
//		uint8_t _g = (uint8_t)(g * 255);
//		uint8_t _b = (uint8_t)(b * 255);
//
//		return (_a << 24) | (_r << 16) | (_g << 8) | _b;
//	}
//
//	// --- AABox impl -------------------------------------------------------------
//
//	static void Assign_If_Less(float& a, float b)
//	{
//		if (a < b)
//			a = b;
//	};
//
//	static void Assign_If_Greater(float& a, float b)
//	{
//		if (a > b)
//			a = b;
//	};
//
//	static void CalculateDistanceIncr(float& value, const Vector3& p, const Vector4& from)
//	{
//		float d = from.x * p.x + from.y * p.y + from.z * p.z + from.w;
//		Assign_If_Greater(value, d);
//	}
//
//	static void CalculateDistanceDecr(float& value, const Vector3& p, const Vector4& from)
//	{
//		float d = from.x * p.x + from.y * p.y + from.z * p.z + from.w;
//		Assign_If_Less(value, d);
//	}
//
//	AABox::AABox()
//	{
//		Min[0] = Min[1] = Min[2] = FLT_MAX;
//		Max[0] = Max[1] = Max[2] = -FLT_MAX;
//	}
//
//	AABox::AABox(const AABox& other)
//	{
//		operator =(other);
//	}
//
//	AABox::AABox(const Vector3& size)
//	{
//		Min = size * -0.5f;
//		Max = size * 0.5f;
//	}
//
//	AABox::AABox(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax)
//	{
//		Min = Vector3(xmin, ymin, zmin);
//		Max = Vector3(xmax, ymax, zmax);
//	}
//
//	AABox& AABox::operator =(const AABox& other)
//	{
//		if (&other != this) {
//			Min[0] = other.Min[0];
//			Min[1] = other.Min[1];
//			Min[2] = other.Min[2];
//
//			Max[0] = other.Max[0];
//			Max[1] = other.Max[1];
//			Max[2] = other.Max[2];
//		}
//
//		return *this;
//	}
//
//	bool AABox::Intersects(const AABox& other) const
//	{
//		if (Max[0] < other.Min[0] || Min[0] > other.Max[0])
//			return false;
//
//		if (Max[1] < other.Min[1] || Min[1] > other.Max[1])
//			return false;
//
//		if (Max[2] < other.Min[2] || Min[2] > other.Max[2])
//			return false;
//
//		return true;
//	}
//
//	void AABox::Add(float x, float y, float z)
//	{
//		Assign_If_Less(Max[0], x);
//		Assign_If_Less(Max[1], y);
//		Assign_If_Less(Max[2], z);
//
//		Assign_If_Greater(Min[0], x);
//		Assign_If_Greater(Min[1], y);
//		Assign_If_Greater(Min[2], z);
//	}
//
//	void AABox::Add(const Vector3& p)
//	{
//		Assign_If_Less(Max[0], p[0]);
//		Assign_If_Less(Max[1], p[1]);
//		Assign_If_Less(Max[2], p[2]);
//
//		Assign_If_Greater(Min[0], p[0]);
//		Assign_If_Greater(Min[1], p[1]);
//		Assign_If_Greater(Min[2], p[2]);
//	}
//
//	void AABox::GetCenter(Vector3& out) const
//	{
//		out[0] = (Min[0] + Max[0]) * 0.5f;
//		out[1] = (Min[1] + Max[1]) * 0.5f;
//		out[2] = (Min[2] + Max[2]) * 0.5f;
//	}
//
//	void AABox::GetSize(Vector3& out) const
//	{
//		out[0] = Max[0] - Min[0];
//		out[1] = Max[1] - Min[1];
//		out[2] = Max[2] - Min[2];
//	}
//
//	void AABox::GetHalfSize(Vector3& out) const
//	{
//		out[0] = (Max[0] - Min[0]) * 0.5f;
//		out[1] = (Max[1] - Min[1]) * 0.5f;
//		out[2] = (Max[2] - Min[2]) * 0.5f;
//	}
//
//	void AABox::Inset(float dx, float dy, float dz)
//	{
//		Min[0] += dx;
//		Min[1] += dy;
//		Min[2] += dz;
//
//		Max[0] -= dx;
//		Max[1] -= dy;
//		Max[2] -= dz;
//	}
//
//	void AABox::TransformAxisAligned(const Matrix& m)
//	{
//		Vector3 vertices[8] = {
//			{ Min[0], Min[1], Min[2] },
//			{ Min[0], Min[1], Max[2] },
//			{ Min[0], Max[1], Min[2] },
//			{ Min[0], Max[1], Max[2] },
//			{ Max[0], Min[1], Min[2] },
//			{ Max[0], Min[1], Max[2] },
//			{ Max[0], Max[1], Min[2] },
//			{ Max[0], Max[1], Max[2] }
//		};
//
//		for (int i = 0; i < 8; ++i)
//			Vec3TransformCoord(vertices[i], vertices[i], m);
//
//		Min[0] = Min[1] = Min[2] = FLT_MAX;
//		Max[0] = Max[1] = Max[2] = -FLT_MAX;
//
//		for (int i = 0; i < 8; ++i)
//			Add(vertices[i]);
//	}
//
//	void AABox::GetPlanes(Vector4 out[6]) const
//	{
//		PlaneFromNormalAndPoint(out[0], { 1, 0, 0 }, { Min[0], Min[1], Min[2] });	// left
//		PlaneFromNormalAndPoint(out[1], { -1, 0, 0 }, { Max[0], Min[1], Min[2] });	// right
//		PlaneFromNormalAndPoint(out[2], { 0, 1, 0 }, { Min[0], Min[1], Min[2] });	// bottom
//		PlaneFromNormalAndPoint(out[3], { 0, -1, 0 }, { Min[0], Max[1], Min[2] });	// top
//		PlaneFromNormalAndPoint(out[4], { 0, 0, -1 }, { Min[0], Min[1], Max[2] });	// front
//		PlaneFromNormalAndPoint(out[5], { 0, 0, 1 }, { Min[0], Min[1], Min[2] });	// back
//	}
//
//	float AABox::Radius() const
//	{
//		return Vec3Distance(Min, Max) * 0.5f;
//	}
//
//	float AABox::RayIntersect(const Vector3& start, const Vector3& dir) const
//	{
//		Vector3 m1, m2;
//		float t1, t2, t3, t4, t5, t6;
//
//		Vec3Subtract(m1, Min, start);
//		Vec3Subtract(m2, Max, start);
//
//		if (dir[0] == 0) {
//			t1 = (m1[0] >= 0 ? FLT_MAX : -FLT_MAX);
//			t2 = (m2[0] >= 0 ? FLT_MAX : -FLT_MAX);
//		}
//		else {
//			t1 = m1[0] / dir[0];
//			t2 = m2[0] / dir[0];
//		}
//
//		if (dir[1] == 0) {
//			t3 = (m1[1] >= 0 ? FLT_MAX : -FLT_MAX);
//			t4 = (m2[1] >= 0 ? FLT_MAX : -FLT_MAX);
//		}
//		else {
//			t3 = m1[1] / dir[1];
//			t4 = m2[1] / dir[1];
//		}
//
//		if (dir[2] == 0) {
//			t5 = (m1[2] >= 0 ? FLT_MAX : -FLT_MAX);
//			t6 = (m2[2] >= 0 ? FLT_MAX : -FLT_MAX);
//		}
//		else {
//			t5 = m1[2] / dir[2];
//			t6 = m2[2] / dir[2];
//		}
//
//		float tmin = Math::Max(Math::Max(Math::Min(t1, t2), Math::Min(t3, t4)), Math::Min(t5, t6));
//		float tmax = Math::Min(Math::Min(Math::Max(t1, t2), Math::Max(t3, t4)), Math::Max(t5, t6));
//
//		if (tmax < 0 || tmin > tmax)
//			return FLT_MAX;
//
//		return tmin;
//	}
//
//	float AABox::Nearest(const Vector4& from) const
//	{
//		float dist = FLT_MAX;
//
//		CalculateDistanceIncr(dist, { Min[0], Min[1], Min[2] }, from);
//		CalculateDistanceIncr(dist, { Min[0], Min[1], Max[2] }, from);
//		CalculateDistanceIncr(dist, { Min[0], Max[1], Min[2] }, from);
//		CalculateDistanceIncr(dist, { Min[0], Max[1], Max[2] }, from);
//		CalculateDistanceIncr(dist, { Max[0], Min[1], Min[2] }, from);
//		CalculateDistanceIncr(dist, { Max[0], Min[1], Max[2] }, from);
//		CalculateDistanceIncr(dist, { Max[0], Max[1], Min[2] }, from);
//		CalculateDistanceIncr(dist, { Max[0], Max[1], Max[2] }, from);
//
//		return dist;
//	}
//
//	float AABox::Farthest(const Vector4& from) const
//	{
//		float dist = -FLT_MAX;
//
//		CalculateDistanceDecr(dist, { Min[0], Min[1], Min[2] }, from);
//		CalculateDistanceDecr(dist, { Min[0], Min[1], Max[2] }, from);
//		CalculateDistanceDecr(dist, { Min[0], Max[1], Min[2] }, from);
//		CalculateDistanceDecr(dist, { Min[0], Max[1], Max[2] }, from);
//		CalculateDistanceDecr(dist, { Max[0], Min[1], Min[2] }, from);
//		CalculateDistanceDecr(dist, { Max[0], Min[1], Max[2] }, from);
//		CalculateDistanceDecr(dist, { Max[0], Max[1], Min[2] }, from);
//		CalculateDistanceDecr(dist, { Max[0], Max[1], Max[2] }, from);
//
//		return dist;
//	}
//
//	// --- Math functions impl ----------------------------------------------------
//
//	uint8_t FloatToByte(float f)
//	{
//		int32_t i = (int32_t)f;
//
//		if (i < 0)
//			return 0;
//		else if (i > 255)
//			return 255;
//
//		return (uint8_t)i;
//	}
//
//	int32_t ISqrt(int32_t n)
//	{
//		int32_t b = 0;
//
//		while (n >= 0) {
//			n = n - b;
//			b = b + 1;
//			n = n - b;
//		}
//
//		return b - 1;
//	}
//
//	uint32_t NextPow2(uint32_t x)
//	{
//		--x;
//
//		x |= x >> 1;
//		x |= x >> 2;
//		x |= x >> 4;
//		x |= x >> 8;
//		x |= x >> 16;
//
//		return ++x;
//	}
//
//	uint32_t Log2OfPow2(uint32_t x)
//	{
//		uint32_t ret = 0;
//
//		while (x >>= 1)
//			++ret;
//
//		return ret;
//	}
//
//	uint32_t ReverseBits32(uint32_t bits)
//	{
//		bits = (bits << 16) | (bits >> 16);
//		bits = ((bits & 0x00ff00ff) << 8) | ((bits & 0xff00ff00) >> 8);
//		bits = ((bits & 0x0f0f0f0f) << 4) | ((bits & 0xf0f0f0f0) >> 4);
//		bits = ((bits & 0x33333333) << 2) | ((bits & 0xcccccccc) >> 2);
//		bits = ((bits & 0x55555555) << 1) | ((bits & 0xaaaaaaaa) >> 1);
//
//		return bits;
//	}
//
//	uint32_t Vec3ToUbyte4(const Math::Vector3& v)
//	{
//		uint32_t ret = 0;
//		uint8_t* bytes = (uint8_t*)(&ret);
//
//		bytes[0] = FloatToByte((v[0] + 1.0f) * (255.0f / 2.0f) + 0.5f);
//		bytes[1] = FloatToByte((v[1] + 1.0f) * (255.0f / 2.0f) + 0.5f);
//		bytes[2] = FloatToByte((v[2] + 1.0f) * (255.0f / 2.0f) + 0.5f);
//
//		return ret;
//	}
//
//	float Vec2Dot(const Vector2& a, const Vector2& b)
//	{
//		return (a.x * b.x + a.y * b.y);
//	}
//
//	float Vec2Length(const Vector2& v)
//	{
//		return sqrtf(v.x * v.x + v.y * v.y);
//	}
//
//	void Vec2Normalize(Vector2& out, const Vector2& v)
//	{
//		float il = 1.0f / sqrtf(v.x * v.x + v.y * v.y);
//
//		out[0] = v[0] * il;
//		out[1] = v[1] * il;
//	}
//
//	void Vec2Subtract(Vector2& out, const Vector2& a, const Vector2& b)
//	{
//		out.x = a.x - b.x;
//		out.y = a.y - b.y;
//	}
//
//	float Vec3Dot(const Vector3& a, const Vector3& b)
//	{
//		return (a.x * b.x + a.y * b.y + a.z * b.z);
//	}
//
//	float Vec3Length(const Vector3& v)
//	{
//		return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
//	}
//
//	float Vec3Distance(const Vector3& a, const Vector3& b)
//	{
//		return Vec3Length(a - b);
//	}
//
//	float PlaneDotCoord(const Vector4& plane, const Vector3& p)
//	{
//		return (plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w);
//	}
//
//	void Vec3Lerp(Vector3& out, const Vector3& a, const Vector3& b, float s)
//	{
//		float invs = 1.0f - s;
//
//		out[0] = a[0] * invs + b[0] * s;
//		out[1] = a[1] * invs + b[1] * s;
//		out[2] = a[2] * invs + b[2] * s;
//	}
//
//	void Vec3Add(Vector3& out, const Vector3& a, const Vector3& b)
//	{
//		out[0] = a[0] + b[0];
//		out[1] = a[1] + b[1];
//		out[2] = a[2] + b[2];
//	}
//
//	void Vec3Mad(Vector3& out, const Vector3& a, const Vector3& b, float s)
//	{
//		out[0] = a[0] + b[0] * s;
//		out[1] = a[1] + b[1] * s;
//		out[2] = a[2] + b[2] * s;
//	}
//
//	void Vec3Normalize(Vector3& out, const Vector3& v)
//	{
//		float il = 1.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
//
//		out[0] = v[0] * il;
//		out[1] = v[1] * il;
//		out[2] = v[2] * il;
//	}
//
//	void Vec3Scale(Vector3& out, const Vector3& v, float scale)
//	{
//		out[0] = v[0] * scale;
//		out[1] = v[1] * scale;
//		out[2] = v[2] * scale;
//	}
//
//	void Vec3Subtract(Vector3& out, const Vector3& a, const Vector3& b)
//	{
//		out[0] = a[0] - b[0];
//		out[1] = a[1] - b[1];
//		out[2] = a[2] - b[2];
//	}
//
//	void Vec3Cross(Vector3& out, const Vector3& a, const Vector3& b)
//	{
//		out[0] = a[1] * b[2] - a[2] * b[1];
//		out[1] = a[2] * b[0] - a[0] * b[2];
//		out[2] = a[0] * b[1] - a[1] * b[0];
//	}
//
//	void Vec3Rotate(Vector3& out, const Vector3& v, const Quaternion& q)
//	{
//		Quaternion cq;
//		Quaternion p = { v[0], v[1], v[2], 0 };
//
//		QuaternionConjugate(cq, q);
//
//		QuaternionMultiply(p, p, cq);
//		QuaternionMultiply(p, q, p);
//
//		out[0] = p[0];
//		out[1] = p[1];
//		out[2] = p[2];
//	}
//
//	void Vec3Transform(Vector4& out, const Vector3& v, const Matrix& m)
//	{
//		Vector4 tmp;
//
//		tmp[0] = v[0] * m._11 + v[1] * m._21 + v[2] * m._31 + m._41;
//		tmp[1] = v[0] * m._12 + v[1] * m._22 + v[2] * m._32 + m._42;
//		tmp[2] = v[0] * m._13 + v[1] * m._23 + v[2] * m._33 + m._43;
//		tmp[3] = v[0] * m._14 + v[1] * m._24 + v[2] * m._34 + m._44;
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//		out[3] = tmp[3];
//	}
//
//	void Vec3TransformTranspose(Vector4& out, const Matrix& m, const Vector3& v)
//	{
//		Vector4 tmp;
//
//		tmp[0] = v[0] * m._11 + v[1] * m._12 + v[2] * m._13 + m._14;
//		tmp[1] = v[0] * m._21 + v[1] * m._22 + v[2] * m._23 + m._24;
//		tmp[2] = v[0] * m._31 + v[1] * m._32 + v[2] * m._33 + m._34;
//		tmp[3] = v[0] * m._41 + v[1] * m._42 + v[2] * m._43 + m._44;
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//		out[3] = tmp[3];
//	}
//
//	void Vec3TransformCoord(Vector3& out, const Vector3& v, const Matrix& m)
//	{
//		Vector4 tmp;
//
//		tmp[0] = v[0] * m._11 + v[1] * m._21 + v[2] * m._31 + m._41;
//		tmp[1] = v[0] * m._12 + v[1] * m._22 + v[2] * m._32 + m._42;
//		tmp[2] = v[0] * m._13 + v[1] * m._23 + v[2] * m._33 + m._43;
//		tmp[3] = v[0] * m._14 + v[1] * m._24 + v[2] * m._34 + m._44;
//
//		out[0] = tmp[0] / tmp[3];
//		out[1] = tmp[1] / tmp[3];
//		out[2] = tmp[2] / tmp[3];
//	}
//
//	void Vec3TransformCoordTranspose(Vector3& out, const Matrix& m, const Vector3& v)
//	{
//		Vector4 tmp;
//
//		tmp[0] = v[0] * m._11 + v[1] * m._12 + v[2] * m._13 + m._14;
//		tmp[1] = v[0] * m._21 + v[1] * m._22 + v[2] * m._23 + m._24;
//		tmp[2] = v[0] * m._31 + v[1] * m._32 + v[2] * m._33 + m._34;
//		tmp[3] = v[0] * m._41 + v[1] * m._42 + v[2] * m._43 + m._44;
//
//		out[0] = tmp[0] / tmp[3];
//		out[1] = tmp[1] / tmp[3];
//		out[2] = tmp[2] / tmp[3];
//	}
//
//	void Vec3TransformNormal(Vector3& out, const Vector3& v, const Matrix& m)
//	{
//		// NOTE: expects inverse transpose
//		Vector3 tmp;
//
//		tmp[0] = v[0] * m._11 + v[1] * m._21 + v[2] * m._31;
//		tmp[1] = v[0] * m._12 + v[1] * m._22 + v[2] * m._32;
//		tmp[2] = v[0] * m._13 + v[1] * m._23 + v[2] * m._33;
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//	}
//
//	void Vec3TransformNormalTranspose(Vector3& out, const Matrix& m, const Vector3& v)
//	{
//		// NOTE: expects inverse
//		Vector3 tmp;
//
//		tmp[0] = v[0] * m._11 + v[1] * m._12 + v[2] * m._13;
//		tmp[1] = v[0] * m._21 + v[1] * m._22 + v[2] * m._23;
//		tmp[2] = v[0] * m._31 + v[1] * m._32 + v[2] * m._33;
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//	}
//
//	void Vec4Lerp(Vector4& out, const Vector4& a, const Vector4& b, float s)
//	{
//		float invs = 1.0f - s;
//
//		out[0] = a[0] * invs + b[0] * s;
//		out[1] = a[1] * invs + b[1] * s;
//		out[2] = a[2] * invs + b[2] * s;
//		out[3] = a[3] * invs + b[3] * s;
//	}
//
//	void Vec4Add(Vector4& out, const Vector4& a, const Vector4& b)
//	{
//		out[0] = a[0] + b[0];
//		out[1] = a[1] + b[1];
//		out[2] = a[2] + b[2];
//		out[3] = a[3] + b[3];
//	}
//
//	void Vec4Subtract(Vector4& out, const Vector4& a, const Vector4& b)
//	{
//		out[0] = a[0] - b[0];
//		out[1] = a[1] - b[1];
//		out[2] = a[2] - b[2];
//		out[3] = a[3] - b[3];
//	}
//
//	void Vec4Scale(Vector4& out, const Vector4& v, float scale)
//	{
//		out[0] = v[0] * scale;
//		out[1] = v[1] * scale;
//		out[2] = v[2] * scale;
//		out[3] = v[3] * scale;
//	}
//
//	void Vec4Transform(Vector4& out, const Vector4& v, const Matrix& m)
//	{
//		float tmp[4];
//
//		tmp[0] = v[0] * m._11 + v[1] * m._21 + v[2] * m._31 + v[3] * m._41;
//		tmp[1] = v[0] * m._12 + v[1] * m._22 + v[2] * m._32 + v[3] * m._42;
//		tmp[2] = v[0] * m._13 + v[1] * m._23 + v[2] * m._33 + v[3] * m._43;
//		tmp[3] = v[0] * m._14 + v[1] * m._24 + v[2] * m._34 + v[3] * m._44;
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//		out[3] = tmp[3];
//	}
//
//	void Vec4TransformTranspose(Vector4& out, const Matrix& m, const Vector4& v)
//	{
//		float tmp[4];
//
//		tmp[0] = v[0] * m._11 + v[1] * m._12 + v[2] * m._13 + v[3] * m._14;
//		tmp[1] = v[0] * m._21 + v[1] * m._22 + v[2] * m._23 + v[3] * m._24;
//		tmp[2] = v[0] * m._31 + v[1] * m._32 + v[2] * m._33 + v[3] * m._34;
//		tmp[3] = v[0] * m._41 + v[1] * m._42 + v[2] * m._43 + v[3] * m._44;
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//		out[3] = tmp[3];
//	}
//
//	void PlaneFromNormalAndPoint(Vector4& out, const Vector3& n, const Vector3& p)
//	{
//		out.x = n.x;
//		out.y = n.y;
//		out.z = n.z;
//		out.w = -Vec3Dot(p, n);
//	}
//
//	void PlaneNormalize(Vector4& out, const Vector4& p)
//	{
//		float il = 1.0f / Vec3Length((const Vector3&)p);
//
//		out[0] = p[0] * il;
//		out[1] = p[1] * il;
//		out[2] = p[2] * il;
//		out[3] = p[3] * il;
//	}
//
//	void PlaneTransform(Vector4& out, const Vector4& p, const Matrix& m)
//	{
//		// NOTE: m is the inverse transpose of the original matrix
//		float tmp[4];
//
//		tmp[0] = p[0] * m._11 + p[1] * m._21 + p[2] * m._31 + p[3] * m._41;
//		tmp[1] = p[0] * m._12 + p[1] * m._22 + p[2] * m._32 + p[3] * m._42;
//		tmp[2] = p[0] * m._13 + p[1] * m._23 + p[2] * m._33 + p[3] * m._43;
//		tmp[3] = p[0] * m._14 + p[1] * m._24 + p[2] * m._34 + p[3] * m._44;
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//		out[3] = tmp[3];
//	}
//
//	void PlaneTransformTranspose(Vector4& out, const Matrix& m, const Vector4& p)
//	{
//		// NOTE: m is the inverse of the original matrix
//		float tmp[4];
//
//		tmp[0] = p[0] * m._11 + p[1] * m._12 + p[2] * m._13 + p[3] * m._14;
//		tmp[1] = p[0] * m._21 + p[1] * m._22 + p[2] * m._23 + p[3] * m._24;
//		tmp[2] = p[0] * m._31 + p[1] * m._32 + p[2] * m._33 + p[3] * m._34;
//		tmp[3] = p[0] * m._41 + p[1] * m._42 + p[2] * m._43 + p[3] * m._44;
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//		out[3] = tmp[3];
//	}
//
//	void QuaternionConjugate(Quaternion& out, const Quaternion& q)
//	{
//		out[0] = -q[0];
//		out[1] = -q[1];
//		out[2] = -q[2];
//		out[3] = q[3];
//	}
//
//	void QuaternionIdentity(Quaternion& out)
//	{
//		out[0] = out[1] = out[2] = 0;
//		out[3] = 1;
//	}
//
//	void QuaternionMultiply(Quaternion& out, const Quaternion& a, const Quaternion& b)
//	{
//		Quaternion tmp;
//
//		tmp[0] = a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1];
//		tmp[1] = a[3] * b[1] + a[1] * b[3] + a[2] * b[0] - a[0] * b[2];
//		tmp[2] = a[3] * b[2] + a[2] * b[3] + a[0] * b[1] - a[1] * b[0];
//		tmp[3] = a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2];
//
//		out[0] = tmp[0];
//		out[1] = tmp[1];
//		out[2] = tmp[2];
//		out[3] = tmp[3];
//	}
//
//	void QuaternionNormalize(Quaternion& out, const Quaternion& q)
//	{
//		float il = 1.0f / sqrtf(q[3] * q[3] + q[0] * q[0] + q[1] * q[1] + q[2] * q[2]);
//
//		out[3] = q[3] * il;
//		out[0] = q[0] * il;
//		out[1] = q[1] * il;
//		out[2] = q[2] * il;
//	}
//
//	void QuaternionRotationAxis(Quaternion& out, float angle, float x, float y, float z)
//	{
//		float l = sqrtf(x * x + y * y + z * z);
//		float ha = angle * 0.5f;
//		float sa = sinf(ha);
//
//		out[0] = (x / l) * sa;
//		out[1] = (y / l) * sa;
//		out[2] = (z / l) * sa;
//		out[3] = cosf(ha);
//	}
//
//	void MatrixIdentity(Matrix& out)
//	{
//		out._12 = out._13 = out._14 = 0;
//		out._21 = out._23 = out._24 = 0;
//		out._31 = out._32 = out._34 = 0;
//		out._41 = out._42 = out._43 = 0;
//
//		out._11 = out._22 = out._33 = out._44 = 1;
//	}
//
//	void MatrixInverse(Matrix& out, const Matrix& m)
//	{
//		float s[6] = {
//			m._11 * m._22 - m._12 * m._21,
//			m._11 * m._23 - m._13 * m._21,
//			m._11 * m._24 - m._14 * m._21,
//			m._12 * m._23 - m._13 * m._22,
//			m._12 * m._24 - m._14 * m._22,
//			m._13 * m._24 - m._14 * m._23
//		};
//
//		float c[6] = {
//			m._31 * m._42 - m._32 * m._41,
//			m._31 * m._43 - m._33 * m._41,
//			m._31 * m._44 - m._34 * m._41,
//			m._32 * m._43 - m._33 * m._42,
//			m._32 * m._44 - m._34 * m._42,
//			m._33 * m._44 - m._34 * m._43
//		};
//
//		float det = (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]);
//
//#ifdef _DEBUG
//		if (fabs(det) < 1e-9f)
//			throw 1;
//#endif
//
//		float r = 1.0f / det;
//
//		out._11 = r * (m._22 * c[5] - m._23 * c[4] + m._24 * c[3]);
//		out._12 = r * (m._13 * c[4] - m._12 * c[5] - m._14 * c[3]);
//		out._13 = r * (m._42 * s[5] - m._43 * s[4] + m._44 * s[3]);
//		out._14 = r * (m._33 * s[4] - m._32 * s[5] - m._34 * s[3]);
//
//		out._21 = r * (m._23 * c[2] - m._21 * c[5] - m._24 * c[1]);
//		out._22 = r * (m._11 * c[5] - m._13 * c[2] + m._14 * c[1]);
//		out._23 = r * (m._43 * s[2] - m._41 * s[5] - m._44 * s[1]);
//		out._24 = r * (m._31 * s[5] - m._33 * s[2] + m._34 * s[1]);
//
//		out._31 = r * (m._21 * c[4] - m._22 * c[2] + m._24 * c[0]);
//		out._32 = r * (m._12 * c[2] - m._11 * c[4] - m._14 * c[0]);
//		out._33 = r * (m._41 * s[4] - m._42 * s[2] + m._44 * s[0]);
//		out._34 = r * (m._32 * s[2] - m._31 * s[4] - m._34 * s[0]);
//
//		out._41 = r * (m._22 * c[1] - m._21 * c[3] - m._23 * c[0]);
//		out._42 = r * (m._11 * c[3] - m._12 * c[1] + m._13 * c[0]);
//		out._43 = r * (m._42 * s[1] - m._41 * s[3] - m._43 * s[0]);
//		out._44 = r * (m._31 * s[3] - m._32 * s[1] + m._33 * s[0]);
//	}
//
//	void MatrixLookAtLH(Matrix& out, const Vector3& eye, const Vector3& look, const Vector3& up)
//	{
//		Vector3 x, y, z;
//
//		Vec3Subtract(z, look, eye);
//		Vec3Normalize(z, z);
//
//		Vec3Cross(x, up, z);
//		Vec3Normalize(x, x);
//
//		Vec3Cross(y, z, x);
//		Vec3Normalize(y, y);
//
//		out._11 = x[0];		out._12 = y[0];		out._13 = z[0];		out._14 = 0.0f;
//		out._21 = x[1];		out._22 = y[1];		out._23 = z[1];		out._24 = 0.0f;
//		out._31 = x[2];		out._32 = y[2];		out._33 = z[2];		out._34 = 0.0f;
//
//		// sign depends on input data
//		out._41 = -Vec3Dot(x, eye);
//		out._42 = -Vec3Dot(y, eye);
//		out._43 = -Vec3Dot(z, eye);
//		out._44 = 1.0f;
//	}
//
//	void MatrixLookAtRH(Matrix& out, const Vector3& eye, const Vector3& look, const Vector3& up)
//	{
//		Vector3 x, y, z;
//
//		Vec3Subtract(z, look, eye);
//		Vec3Normalize(z, z);
//
//		Vec3Cross(x, z, up);
//		Vec3Normalize(x, x);
//
//		Vec3Cross(y, x, z);
//		Vec3Normalize(y, y);
//
//		out._11 = x[0];		out._12 = y[0];		out._13 = -z[0];		out._14 = 0.0f;
//		out._21 = x[1];		out._22 = y[1];		out._23 = -z[1];		out._24 = 0.0f;
//		out._31 = x[2];		out._32 = y[2];		out._33 = -z[2];		out._34 = 0.0f;
//
//		// sign depends on input data
//		out._41 = -Vec3Dot(x, eye);
//		out._42 = -Vec3Dot(y, eye);
//		out._43 = Vec3Dot(z, eye);
//		out._44 = 1.0f;
//	}
//
//	void MatrixMultiply(Matrix& out, const Matrix& a, const Matrix& b)
//	{
//		Matrix tmp;
//
//		tmp._11 = a._11 * b._11 + a._12 * b._21 + a._13 * b._31 + a._14 * b._41;
//		tmp._12 = a._11 * b._12 + a._12 * b._22 + a._13 * b._32 + a._14 * b._42;
//		tmp._13 = a._11 * b._13 + a._12 * b._23 + a._13 * b._33 + a._14 * b._43;
//		tmp._14 = a._11 * b._14 + a._12 * b._24 + a._13 * b._34 + a._14 * b._44;
//
//		tmp._21 = a._21 * b._11 + a._22 * b._21 + a._23 * b._31 + a._24 * b._41;
//		tmp._22 = a._21 * b._12 + a._22 * b._22 + a._23 * b._32 + a._24 * b._42;
//		tmp._23 = a._21 * b._13 + a._22 * b._23 + a._23 * b._33 + a._24 * b._43;
//		tmp._24 = a._21 * b._14 + a._22 * b._24 + a._23 * b._34 + a._24 * b._44;
//
//		tmp._31 = a._31 * b._11 + a._32 * b._21 + a._33 * b._31 + a._34 * b._41;
//		tmp._32 = a._31 * b._12 + a._32 * b._22 + a._33 * b._32 + a._34 * b._42;
//		tmp._33 = a._31 * b._13 + a._32 * b._23 + a._33 * b._33 + a._34 * b._43;
//		tmp._34 = a._31 * b._14 + a._32 * b._24 + a._33 * b._34 + a._34 * b._44;
//
//		tmp._41 = a._41 * b._11 + a._42 * b._21 + a._43 * b._31 + a._44 * b._41;
//		tmp._42 = a._41 * b._12 + a._42 * b._22 + a._43 * b._32 + a._44 * b._42;
//		tmp._43 = a._41 * b._13 + a._42 * b._23 + a._43 * b._33 + a._44 * b._43;
//		tmp._44 = a._41 * b._14 + a._42 * b._24 + a._43 * b._34 + a._44 * b._44;
//
//		out = tmp;
//	}
//
//	void MatrixPerspectiveFovLH(Matrix& out, float fovy, float aspect, float nearplane, float farplane)
//	{
//#ifdef VULKAN
//		out._22 = -1.0f / tanf(fovy / 2);
//		out._11 = out._22 / -aspect;
//#else
//		out._22 = 1.0f / tanf(fovy / 2);
//		out._11 = out._22 / aspect;
//#endif
//
//		out._12 = out._13 = out._14 = 0;
//		out._21 = out._23 = out._24 = 0;
//		out._31 = out._32 = 0;
//		out._41 = out._42 = out._43 = 0;
//
//		out._34 = 1.0f;
//
//#if defined(VULKAN) || defined(METAL) || defined(DIRECT3D9) || defined(DIRECT3D10)
//		// [0, 1]
//		out._33 = farplane / (nearplane - farplane);
//		out._43 = -nearplane * out._33;
//#else
//		// [-1, 1]
//		out._33 = (farplane + nearplane) / (nearplane - farplane);
//		out._43 = -2 * farplane * nearplane / (nearplane - farplane);
//#endif
//	}
//
//	void MatrixPerspectiveFovRH(Matrix& out, float fovy, float aspect, float nearplane, float farplane)
//	{
//#ifdef VULKAN
//		out._22 = -1.0f / tanf(fovy / 2);
//		out._11 = out._22 / -aspect;
//#else
//		out._22 = 1.0f / tanf(fovy / 2);
//		out._11 = out._22 / aspect;
//#endif
//
//		out._12 = out._13 = out._14 = 0;
//		out._21 = out._23 = out._24 = 0;
//		out._31 = out._32 = 0;
//		out._41 = out._42 = out._43 = 0;
//
//		out._34 = -1.0f;
//
//#if defined(VULKAN) || defined(METAL) || defined(DIRECT3D9) || defined(DIRECT3D10)
//		// [0, 1]
//		out._33 = farplane / (nearplane - farplane);
//		out._43 = nearplane * out._33;
//#else
//		// [-1, 1]
//		out._33 = (farplane + nearplane) / (nearplane - farplane);
//		out._43 = 2 * farplane * nearplane / (nearplane - farplane);
//#endif
//	}
//
//	void MatrixOrthoOffCenterRH(Matrix& out, float left, float right, float bottom, float top, float nearplane, float farplane)
//	{
//		out._11 = 2.0f / (right - left);
//		out._12 = out._13 = out._14 = 0;
//
//		out._22 = 2.0f / (top - bottom);
//		out._21 = out._23 = out._24 = 0;
//
//#if defined(VULKAN) || defined(METAL) || defined(DIRECT3D9) || defined(DIRECT3D10)
//		// [0, 1]
//		out._33 = 1.0f / (nearplane - farplane);
//		out._43 = nearplane * out._33;
//
//		out._41 = (right + left) / (right - left);
//		out._42 = (top + bottom) / (bottom - top);
//#else
//		// [-1, 1]
//		out._33 = -2.0f / (farplane - nearplane);
//		out._43 = -(farplane + nearplane) / (farplane - nearplane);
//
//		out._41 = -(right + left) / (right - left);
//		out._42 = -(top + bottom) / (top - bottom);
//#endif
//
//		out._31 = out._32 = out._34 = 0;
//		out._44 = 1;
//	}
//
//	void MatrixReflect(Math::Matrix& out, const Math::Vector4& plane)
//	{
//		Math::Vector4 p;
//		PlaneNormalize(p, plane);
//
//		out._11 = -2 * p[0] * p[0] + 1;
//		out._12 = -2 * p[1] * p[0];
//		out._13 = -2 * p[2] * p[0];
//
//		out._21 = -2 * p[0] * p[1];
//		out._22 = -2 * p[1] * p[1] + 1;
//		out._23 = -2 * p[2] * p[1];
//
//		out._31 = -2 * p[0] * p[2];
//		out._32 = -2 * p[1] * p[2];
//		out._33 = -2 * p[2] * p[2] + 1;
//
//		out._41 = -2 * p[0] * p[3];
//		out._42 = -2 * p[1] * p[3];
//		out._43 = -2 * p[2] * p[3];
//
//		out._14 = out._24 = out._34 = 0;
//		out._44 = 1;
//	}
//
//	void MatrixRotationAxis(Matrix& out, float angle, float x, float y, float z)
//	{
//		Vector3 u = { x, y, z };
//
//		float cosa = cosf(angle);
//		float sina = sinf(angle);
//
//		Vec3Normalize(u, u);
//
//		out._11 = cosa + u[0] * u[0] * (1.0f - cosa);
//		out._21 = u[0] * u[1] * (1.0f - cosa) - u[2] * sina;
//		out._31 = u[0] * u[2] * (1.0f - cosa) + u[1] * sina;
//		out._41 = 0;
//
//		out._12 = u[1] * u[0] * (1.0f - cosa) + u[2] * sina;
//		out._22 = cosa + u[1] * u[1] * (1.0f - cosa);
//		out._32 = u[1] * u[2] * (1.0f - cosa) - u[0] * sina;
//		out._42 = 0;
//
//		out._13 = u[2] * u[0] * (1.0f - cosa) - u[1] * sina;
//		out._23 = u[2] * u[1] * (1.0f - cosa) + u[0] * sina;
//		out._33 = cosa + u[2] * u[2] * (1.0f - cosa);
//		out._43 = 0;
//
//		out._14 = out._24 = out._34 = 0;
//		out._44 = 1;
//	}
//
//	void MatrixRotationQuaternion(Matrix& out, const Quaternion& q)
//	{
//		out._11 = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
//		out._12 = 2.0f * (q[0] * q[1] + q[2] * q[3]);
//		out._13 = 2.0f * (q[0] * q[2] - q[1] * q[3]);
//
//		out._21 = 2.0f * (q[0] * q[1] - q[2] * q[3]);
//		out._22 = 1.0f - 2.0f * (q[0] * q[0] + q[2] * q[2]);
//		out._23 = 2.0f * (q[1] * q[2] + q[0] * q[3]);
//
//		out._31 = 2.0f * (q[0] * q[2] + q[1] * q[3]);
//		out._32 = 2.0f * (q[1] * q[2] - q[0] * q[3]);
//		out._33 = 1.0f - 2.0f * (q[0] * q[0] + q[1] * q[1]);
//
//		out._14 = out._24 = out._34 = 0;
//		out._41 = out._42 = out._43 = 0;
//		out._44 = 1;
//	}
//
//	void MatrixScaling(Matrix& out, float x, float y, float z)
//	{
//		MatrixIdentity(out);
//
//		out._11 = x;
//		out._22 = y;
//		out._33 = z;
//	}
//
//	void MatrixTranslation(Matrix& out, float x, float y, float z)
//	{
//		MatrixIdentity(out);
//
//		out._41 = x;
//		out._42 = y;
//		out._43 = z;
//	}
//
//	void MatrixTranspose(Matrix& out, Matrix m)
//	{
//		out._11 = m._11;
//		out._12 = m._21;
//		out._13 = m._31;
//		out._14 = m._41;
//
//		out._21 = m._12;
//		out._22 = m._22;
//		out._23 = m._32;
//		out._24 = m._42;
//
//		out._31 = m._13;
//		out._32 = m._23;
//		out._33 = m._33;
//		out._34 = m._43;
//
//		out._41 = m._14;
//		out._42 = m._24;
//		out._43 = m._34;
//		out._44 = m._44;
//	}
//
//	void MatrixViewVector(Matrix& out, const Vector3& viewdir)
//	{
//		Vector3 x, z;
//		Vector3 y = { 0, 1, 0 };
//
//		Vec3Normalize(z, viewdir);
//
//		if (fabs(z.y) > 0.999f) {
//			y = { 1, 0, 0 };
//		}
//
//		Vec3Cross(x, y, z);
//		Vec3Cross(y, z, x);
//
//		out._11 = x[0];	out._12 = y[0];	out._13 = z[0];
//		out._21 = x[1];	out._22 = y[1];	out._23 = z[1];
//		out._31 = x[2];	out._32 = y[2];	out._33 = z[2];
//
//		out._14 = out._24 = out._34 = out._41 = out._42 = out._43 = 0;
//		out._44 = 1;
//	}
//
//	void FrustumPlanes(Math::Vector4 out[6], const Math::Matrix& viewproj)
//	{
//		out[0] = { viewproj._11 + viewproj._14, viewproj._21 + viewproj._24, viewproj._31 + viewproj._34, viewproj._41 + viewproj._44 };	// left
//		out[1] = { viewproj._14 - viewproj._11, viewproj._24 - viewproj._21, viewproj._34 - viewproj._31, viewproj._44 - viewproj._41 };	// right
//		out[2] = { viewproj._14 - viewproj._12, viewproj._24 - viewproj._22, viewproj._34 - viewproj._32, viewproj._44 - viewproj._42 };	// top
//		out[3] = { viewproj._12 + viewproj._14, viewproj._22 + viewproj._24, viewproj._32 + viewproj._34, viewproj._42 + viewproj._44 };	// bottom
//		out[4] = { viewproj._13, viewproj._23, viewproj._33, viewproj._43 };																// near
//		out[5] = { viewproj._14 - viewproj._13, viewproj._24 - viewproj._23, viewproj._34 - viewproj._33, viewproj._44 - viewproj._43 };	// far
//
//		PlaneNormalize(out[0], out[0]);
//		PlaneNormalize(out[1], out[1]);
//		PlaneNormalize(out[2], out[2]);
//		PlaneNormalize(out[3], out[3]);
//		PlaneNormalize(out[4], out[4]);
//		PlaneNormalize(out[5], out[5]);
//	}
//
//	void FitToBoxPerspective(Vector2& outclip, const Vector3& refpoint, const Vector3& forward, const AABox& box)
//	{
//		// NOTE: for area lights you have to determine the projection matrix
//		Vector4 refplane;
//
//		refplane[0] = forward[0];
//		refplane[1] = forward[1];
//		refplane[2] = forward[2];
//		refplane[3] = -Vec3Dot(forward, refpoint);
//
//		PlaneNormalize(refplane, refplane);
//
//		outclip.x = box.Nearest(refplane) - 2e-3f;	// 2mm
//		outclip.y = box.Farthest(refplane) + 2e-3f;	// 2mm
//
//		outclip.x = Max<float>(outclip.x, 0.1f);
//		outclip.y = Max<float>(outclip.y, 0.2f);
//	}
//
//	void FitToBoxOrtho(Matrix& outproj, Vector2& outclip, const Matrix& view, const AABox& box)
//	{
//		Vector4 pright(1, 0, 0, 0);
//		Vector4 pbottom(0, 1, 0, 0);
//		Vector4 pnear(0, 0, -1, 0);
//
//		// transform planes to world space
//		Vec4TransformTranspose(pright, view, pright);
//		Vec4TransformTranspose(pbottom, view, pbottom);
//		Vec4TransformTranspose(pnear, view, pnear);
//
//		float left = box.Nearest(pright) - pright.w;
//		float right = box.Farthest(pright) - pright.w;
//		float bottom = box.Nearest(pbottom) - pbottom.w;
//		float top = box.Farthest(pbottom) - pbottom.w;
//
//		outclip[0] = box.Nearest(pnear) - 2e-3f;	// 2mm
//		outclip[1] = box.Farthest(pnear) + 2e-3f;	// 2mm
//
//		// NOTE: near/far are distances: [-near, -far] -> depth
//		MatrixOrthoOffCenterRH(outproj, left, right, bottom, top, outclip[0], outclip[1]);
//	}
//
//	void GetOrthogonalVectors(Vector3& out1, Vector3& out2, const Vector3& v)
//	{
//		// select dominant direction
//		int bestcoord = 0;
//
//		if (fabs(v[1]) > fabs(v[bestcoord]))
//			bestcoord = 1;
//
//		if (fabs(v[2]) > fabs(v[bestcoord]))
//			bestcoord = 2;
//
//		// ignore handedness
//		int other1 = (bestcoord + 1) % 3;
//		int other2 = (bestcoord + 2) % 3;
//
//		out1[bestcoord] = v[other1];
//		out1[other1] = -v[bestcoord];
//		out1[other2] = v[other2];
//
//		out2[bestcoord] = v[other2];
//		out2[other1] = v[other1];
//		out2[other2] = -v[bestcoord];
//	}
//
//	int FrustumIntersect(const Math::Vector4 frustum[6], const AABox& box)
//	{
//		Math::Vector3 center, halfsize;
//		float dist, maxdist;
//		int result = 2; // inside
//
//		box.GetCenter(center);
//		box.GetHalfSize(halfsize);
//
//		// NOTE: fast version, might give false negatives
//		for (int j = 0; j < 6; ++j) {
//			const Math::Vector4& plane = frustum[j];
//
//			dist = PlaneDotCoord(plane, center);
//			maxdist = fabs(plane[0] * halfsize[0]) + fabs(plane[1] * halfsize[1]) + fabs(plane[2] * halfsize[2]);
//
//			if (dist < -maxdist)
//				return 0;	// outside
//			else if (fabs(dist) < maxdist)
//				result = 1;	// intersect
//		}
//
//		return result;
//	}
//
//	// --- Utility functions impl -------------------------------------------------
//
//	float Gaussian(float x, float stddev)
//	{
//		return (ONE_OVER_SQRT_TWO_PI / stddev) * expf(-((x * x) / (2.0f * stddev * stddev)));
//	}
//
//	std::string& GetPath(std::string& out, const std::string& str)
//	{
//		size_t pos = str.find_last_of("\\/");
//
//		if (pos != std::string::npos) {
//#ifdef _WIN32
//			out = str.substr(0, pos) + '\\';
//#else
//			out = str.substr(0, pos) + '/';
//#endif
//		}
//		else {
//			out = "";
//		}
//
//		return out;
//	}
//
//	std::string& GetFile(std::string& out, const std::string& str)
//	{
//		size_t pos = str.find_last_of("\\/");
//
//		if (pos != std::string::npos)
//			out = str.substr(pos + 1);
//		else
//			out = str;
//
//		return out;
//	}
//
//	std::string& GetExtension(std::string& out, const std::string& str)
//	{
//		size_t pos = str.find_last_of(".");
//
//		if (pos == std::string::npos) {
//			out = "";
//			return out;
//		}
//
//		out = str.substr(pos + 1, str.length());
//		return ToLower(out, out);
//	}
//
//	std::string& ToLower(std::string& out, const std::string& str)
//	{
//		out.resize(str.size());
//
//		for (size_t i = 0; i < str.size(); ++i) {
//			if (str[i] >= 'A' && str[i] <= 'Z')
//				out[i] = str[i] + 32;
//			else
//				out[i] = str[i];
//		}
//
//		return out;
//	}
//
//}
//
//// --- Operators impl ---------------------------------------------------------
//
//Math::Vector2 operator *(float f, const Math::Vector2& v)
//{
//	return Math::Vector2(f * v.x, f * v.y);
//}
//
//Math::Vector3 operator *(float f, const Math::Vector3& v)
//{
//	return Math::Vector3(f * v.x, f * v.y, f * v.z);
//}
//
//Math::Vector4 operator *(float f, const Math::Vector4& v)
//{
//	return Math::Vector4(f * v.x, f * v.y, f * v.z, f * v.w);
//}
//
//Math::Vector4 operator /(float f, const Math::Vector4& v)
//{
//	return Math::Vector4(f / v.x, f / v.y, f / v.z, f / v.w);
//}
//
//class BasicCamera
//{
//	typedef Math::InterpolationArray<float, 1> CurveValue;
//	typedef Math::InterpolationArray<float, 3> CurveVector;
//
//private:
//	CurveVector		anglecurve;
//	CurveVector		pancurve;
//	CurveValue		zoomcurve;
//
//	Math::Vector3	targetangles;
//	Math::Vector3	smoothedangles;
//	Math::Vector3	targetpan;
//	Math::Vector3	position;		// orbit point
//	float			targetzoom;
//
//	float			distance;		// distance from lookat
//	float			nearplane;
//	float			farplane;
//	float			minzoom;
//	float			maxzoom;
//	float			minpitch;
//	float			maxpitch;
//	float			fov;
//	float			aspect;
//	bool			finished;
//
//	void GetViewAndEye(Math::Matrix& view, Math::Vector3& eye) const;
//
//public:
//	BasicCamera();
//
//	void OrbitRight(float angle);
//	void OrbitUp(float angle);
//	void PanRight(float offset);
//	void PanUp(float offset);
//	void Zoom(float offset);
//
//	void GetViewMatrix(Math::Matrix& out) const;
//	void GetProjectionMatrix(Math::Matrix& out) const;
//	void GetPosition(Math::Vector3& out) const;
//	void GetEyePosition(Math::Vector3& out) const;
//	void GetOrientation(Math::Vector3& out) const;
//
//	void Update(float dt);
//	void Animate(float alpha);
//	void SetOrientation(float yaw, float pitch, float roll);
//	void SetPosition(float x, float y, float z);
//	void SetDistance(float value);
//
//	inline void SetAspect(float value) { aspect = value; }
//	inline void SetFov(float value) { fov = value; }
//	inline void SetClipPlanes(float znear, float zfar) { nearplane = znear; farplane = zfar; }
//	inline void SetZoomLimits(float zmin, float zmax) { minzoom = zmin; maxzoom = zmax; }
//	inline void SetPitchLimits(float pmin, float pmax) { minpitch = pmin; maxpitch = pmax; }
//
//	inline float GetAspect() const { return aspect; }
//	inline float GetDistance() const { return distance; }
//	inline float GetNearPlane() const { return nearplane; }
//	inline float GetFarPlane() const { return farplane; }
//	inline float GetFov() const { return fov; }
//
//	inline bool IsAnimationFinished() const { return finished; }
//};
//
//
//#define ROTATIONAL_SPEED		0.75f	// rad/s
//#define ROTATIONAL_INVINTERTIA	5.0f
//#define POSITIONAL_INVINTERTIA	5.0f
//
//BasicCamera::BasicCamera()
//{
//	distance = 1.0f;
//	nearplane = 0.1f;
//	farplane = 50.0f;
//	fov = Math::PI / 3;
//	aspect = 4.0f / 3.0f;
//	minzoom = 0;
//	maxzoom = 30;
//	minpitch = -Math::HALF_PI;
//	maxpitch = Math::HALF_PI;
//	finished = true;
//
//	// NOTE: ignore XCode warning, the template handles promotion
//	anglecurve.Set(0, 0, 0);
//	pancurve.Set(0, 0, 0);
//	zoomcurve.Set(0);
//}
//
//void BasicCamera::OrbitRight(float angle)
//{
//	targetangles[0] += angle * ROTATIONAL_SPEED;
//	finished = false;
//}
//
//void BasicCamera::OrbitUp(float angle)
//{
//	targetangles[1] += angle * ROTATIONAL_SPEED;
//	targetangles[1] = Math::Clamp(targetangles[1], minpitch, maxpitch);
//
//	finished = false;
//}
//
//void BasicCamera::PanRight(float offset)
//{
//	Math::Matrix	view;
//	Math::Matrix	yaw;
//	Math::Matrix	pitch;
//	Math::Vector3	right;
//
//	Math::MatrixRotationAxis(yaw, smoothedangles[0], 0, 1, 0);
//	Math::MatrixRotationAxis(pitch, smoothedangles[1], 1, 0, 0);
//	Math::MatrixMultiply(view, yaw, pitch);
//
//	right = Math::Vector3(view._11, view._21, view._31);
//	Math::Vec3Mad(targetpan, targetpan, right, offset);
//
//	finished = false;
//}
//
//void BasicCamera::PanUp(float offset)
//{
//	Math::Matrix	view;
//	Math::Matrix	yaw;
//	Math::Matrix	pitch;
//	Math::Vector3	up;
//
//	Math::MatrixRotationAxis(yaw, smoothedangles[0], 0, 1, 0);
//	Math::MatrixRotationAxis(pitch, smoothedangles[1], 1, 0, 0);
//	Math::MatrixMultiply(view, yaw, pitch);
//
//	up = Math::Vector3(view._12, view._22, view._32);
//	Math::Vec3Mad(targetpan, targetpan, up, offset);
//
//	finished = false;
//}
//
//void BasicCamera::Zoom(float offset)
//{
//	targetzoom = Math::Clamp(distance - offset, minzoom, maxzoom);
//	finished = false;
//}
//
//void BasicCamera::GetViewAndEye(Math::Matrix& view, Math::Vector3& eye) const
//{
//	Math::Matrix	yaw;
//	Math::Matrix	pitch;
//	Math::Vector3	forward;
//
//	Math::MatrixRotationAxis(yaw, smoothedangles[0], 0, 1, 0);
//	Math::MatrixRotationAxis(pitch, smoothedangles[1], 1, 0, 0);
//	Math::MatrixMultiply(view, yaw, pitch);
//
//	// rotation matrix is right handed
//	forward = Math::Vector3(-view._13, -view._23, -view._33);
//
//	Math::Vec3Scale(forward, forward, distance);
//	Math::Vec3Subtract(eye, position, forward);
//}
//
//void BasicCamera::GetViewMatrix(Math::Matrix& out) const
//{
//	Math::Vector3 eye;
//	GetViewAndEye(out, eye);
//
//	out._41 = -(eye[0] * out._11 + eye[1] * out._21 + eye[2] * out._31);
//	out._42 = -(eye[0] * out._12 + eye[1] * out._22 + eye[2] * out._32);
//	out._43 = -(eye[0] * out._13 + eye[1] * out._23 + eye[2] * out._33);
//}
//
//void BasicCamera::GetProjectionMatrix(Math::Matrix& out) const
//{
//	return Math::MatrixPerspectiveFovRH(out, fov, aspect, nearplane, farplane);
//}
//
//void BasicCamera::GetPosition(Math::Vector3& out) const
//{
//	out = position;
//}
//
//void BasicCamera::GetEyePosition(Math::Vector3& out) const
//{
//	Math::Matrix view;
//	GetViewAndEye(view, out);
//}
//
//void BasicCamera::GetOrientation(Math::Vector3& out) const
//{
//	out = smoothedangles;
//}
//
//void BasicCamera::Update(float dt)
//{
//	float diff1[3];
//	float diff2[3];
//	float diff3;
//
//	// rotate
//	targetangles[1] = Math::Clamp(targetangles[1], -Math::HALF_PI, Math::HALF_PI);
//
//	diff1[0] = (targetangles[0] - anglecurve[0]) * dt * ROTATIONAL_INVINTERTIA;
//	diff1[1] = (targetangles[1] - anglecurve[1]) * dt * ROTATIONAL_INVINTERTIA;
//	diff1[2] = 0;
//
//	anglecurve.Advance(diff1);
//
//	// pan
//	diff2[0] = (targetpan[0] - pancurve[0]) * dt * POSITIONAL_INVINTERTIA;
//	diff2[1] = (targetpan[1] - pancurve[1]) * dt * POSITIONAL_INVINTERTIA;
//	diff2[2] = (targetpan[2] - pancurve[2]) * dt * POSITIONAL_INVINTERTIA;
//
//	pancurve.Advance(diff2);
//
//	// zoom
//	diff3 = (targetzoom - zoomcurve[0]) * dt * POSITIONAL_INVINTERTIA;
//	zoomcurve.Advance(&diff3);
//
//	if (Math::Vec3Dot(diff1, diff1) < 1e-4f && Math::Vec3Dot(diff2, diff2) < 1e-4f && diff3 < 1e-2f)
//		finished = true;
//}
//
//void BasicCamera::Animate(float alpha)
//{
//	anglecurve.Smooth(smoothedangles, alpha);
//	pancurve.Smooth(position, alpha);
//	zoomcurve.Smooth(&distance, alpha);
//}
//
//void BasicCamera::SetOrientation(float yaw, float pitch, float roll)
//{
//	targetangles = Math::Vector3(yaw, pitch, roll);
//	smoothedangles = Math::Vector3(yaw, pitch, roll);
//
//	anglecurve.Set(yaw, pitch, roll);
//	finished = true;
//}
//
//void BasicCamera::SetPosition(float x, float y, float z)
//{
//	targetpan = Math::Vector3(x, y, z);
//	position = Math::Vector3(x, y, z);
//
//	pancurve.Set(x, y, z);
//	finished = true;
//}
//
//void BasicCamera::SetDistance(float value)
//{
//	targetzoom = value;
//	distance = value;
//
//	zoomcurve.Set(value);
//	finished = true;
//}
//
//// helper macros
//#define TITLE				"Shader sample 39: HDR effects"
//#define MYERROR(x)			{ std::cout << "* Error: " << x << "!\n"; }
//#define SAFE_RELEASE(x)		{ if ((x)) { (x)->Release(); (x) = NULL; } }
//
//LPDIRECT3DDEVICE9		device = nullptr;
//LPDIRECT3DTEXTURE9		helptext = nullptr;
//
//LPDIRECT3DCUBETEXTURE9	environment = nullptr;		// HDR environment
//LPDIRECT3DCUBETEXTURE9	irradiance1 = nullptr;		// preintegrated diffuse irradiance
//LPDIRECT3DCUBETEXTURE9	irradiance2 = nullptr;		// preintegrated specular irradiance
//LPDIRECT3DTEXTURE9		brdfLUT = nullptr;		// preintegrated BRDF lookup texture
//
//LPDIRECT3DTEXTURE9		scenetarget = nullptr;
//LPDIRECT3DTEXTURE9		avgluminance = nullptr;
//LPDIRECT3DTEXTURE9		avglumsystemmem = nullptr;
//LPDIRECT3DTEXTURE9		dsampletargets[5] = { nullptr };
//LPDIRECT3DTEXTURE9		blurtargets[5] = { nullptr };
//LPDIRECT3DTEXTURE9		startargets[4][2] = { { nullptr } };
//LPDIRECT3DTEXTURE9		lensflaretargets[2] = { nullptr };
//LPDIRECT3DTEXTURE9		afterimagetargets[2] = { nullptr };
//LPDIRECT3DTEXTURE9		bloomresult = nullptr;
//LPDIRECT3DTEXTURE9		starresult = nullptr;
//
//LPDIRECT3DSURFACE9		scenesurface = nullptr;
//LPDIRECT3DSURFACE9		avglumsurfaces[4] = { nullptr };
//LPDIRECT3DSURFACE9		avglumsysmemsurface = nullptr;
//LPDIRECT3DSURFACE9		dsamplesurfaces[5] = { nullptr };
//LPDIRECT3DSURFACE9		blursurfaces[5] = { nullptr };
//LPDIRECT3DSURFACE9		starsurfaces[4][2] = { { nullptr } };
//LPDIRECT3DSURFACE9		lensflaresurfaces[2] = { nullptr };
//LPDIRECT3DSURFACE9		afterimagesurfaces[2] = { nullptr };
//LPDIRECT3DSURFACE9		bloomsurface = nullptr;
//LPDIRECT3DSURFACE9		starsurface = nullptr;
//
//LPD3DXMESH				teapot = nullptr;
//LPD3DXMESH				skull = nullptr;
//LPD3DXMESH				knot = nullptr;
//LPD3DXMESH				skymesh = nullptr;
//LPD3DXMESH				mesh = nullptr;
//
//LPD3DXEFFECT			skyeffect = nullptr;
//LPD3DXEFFECT			metaleffect = nullptr;
//LPD3DXEFFECT			insulatoreffect = nullptr;
//LPD3DXEFFECT			measureeffect = nullptr;
//LPD3DXEFFECT			hdreffects = nullptr;
//LPD3DXEFFECT			screenquad = nullptr;
//LPD3DXEFFECT			effect = nullptr;
//
//BasicCamera				camera;
//BasicCamera				objectrotator;
//D3DVIEWPORT9			oldviewport;
//D3DVIEWPORT9			viewport;
//D3DXVECTOR4				pixelsize(0, 0, 0, 1);
//D3DXVECTOR4				texelsize(0, 0, 0, 1);
//float					averageluminance = 0.1f;	// don't set this to zero!!!
//float					adaptedluminance = 0.1f;	// don't set this to zero!!!
//float					exposure = 0;
//uint8_t					mousebuttons = 0;
//int						currentafterimage = 0;
//bool					drawafterimage = false;
//bool					drawtext = true;
//
//// forward declarations
//void RenderScene(float alpha);
//void MeasureLuminance();
//void AdaptLuminance(float dt);
//void BrightPass();
//void DownSample();
//void Stars();
//void Bloom();
//void LensFlare();
//void ToneMap();
//
//bool InitScene()
//{
//	device = g_pDevice;
//
//	//  print Gaussian weights (kernel_length = 6*sigma - 1)
//	printf("static const float blurWeights[9] =\n{\n");
//
//	for (int i = -4; i < 5; ++i) {
//		printf("    %.6ff,\n", Math::Gaussian((float)i, (9.0f + 1.0f) / 6.0f));
//	}
//
//	printf("};\n");
//
//	// check for HDR support first
//	D3DDISPLAYMODE mode;
//	
//	LPDIRECT3D9 direct3d = (LPDIRECT3D9)g_pSDK;
//
//
//	if (FAILED(direct3d->GetAdapterDisplayMode(0, &mode))) {
//		MYERROR("Could not get adapter mode");
//		return false;
//	}
//
//	if (FAILED(direct3d->CheckDeviceFormat(0, D3DDEVTYPE_HAL, mode.Format, D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F))) {
//		MYERROR("No floating point rendertarget support");
//		return false;
//	}
//
//	if (FAILED(direct3d->CheckDepthStencilMatch(0, D3DDEVTYPE_HAL, mode.Format, D3DFMT_A16B16G16R16F, D3DFMT_D24S8))) {
//		MYERROR("D3DFMT_A16B16G16R16F does not support D3DFMT_D24S8");
//		return false;
//	}
//
//	// load resources
//	if (FAILED(D3DXCreateCubeTextureFromFile(device, L"../../Media/Textures/grace.dds", &environment)))
//		return false;
//
//	if (FAILED(D3DXCreateCubeTextureFromFile(device, L"../../Media/Textures/grace_diff_irrad.dds", &irradiance1)))
//		return false;
//
//	if (FAILED(D3DXCreateCubeTextureFromFile(device, L"../../Media/Textures/grace_spec_irrad.dds", &irradiance2)))
//		return false;
//
//	if (FAILED(D3DXCreateTextureFromFile(device, L"../../Media/Textures/brdf.dds", &brdfLUT)))
//		return false;
//
//	if (FAILED(D3DXLoadMeshFromX(L"../../Media/MeshesDX/knot.x", D3DXMESH_MANAGED, device, NULL, NULL, NULL, NULL, &knot)))
//		return false;
//
//	if (FAILED(D3DXLoadMeshFromX(L"../../Media/MeshesDX/skullocc3.x", D3DXMESH_MANAGED, device, NULL, NULL, NULL, NULL, &skull)))
//		return false;
//
//	if (FAILED(D3DXLoadMeshFromX(L"../../Media/MeshesDX/teapot.x", D3DXMESH_MANAGED, device, NULL, NULL, NULL, NULL, &teapot)))
//		return false;
//
//	if (FAILED(D3DXLoadMeshFromX(L"../../Media/MeshesDX/sky.x", D3DXMESH_MANAGED, device, NULL, NULL, NULL, NULL, &skymesh)))
//		return false;
//
//	if (FAILED(DXCreateEffect(device, L"../../Media/ShadersDX/sky.fx", &skyeffect)))
//		return false;
//
//	if (FAILED(DXCreateEffect(device, L"../../Media/ShadersDX/metal.fx", &metaleffect)))
//		return false;
//
//	if (FAILED(DXCreateEffect(device, L"../../Media/ShadersDX/insulator.fx", &insulatoreffect)))
//		return false;
//
//	if (FAILED(DXCreateEffect(device, L"../../Media/ShadersDX/measureluminance.fx", &measureeffect)))
//		return false;
//
//	if (FAILED(DXCreateEffect(device, L"../../Media/ShadersDX/hdreffects.fx", &hdreffects)))
//		return false;
//
//	if (FAILED(DXCreateEffect(device, L"../../Media/ShadersDX/screenquad.fx", &screenquad)))
//		return false;
//
//	// create render targets
//	UINT screenwidth = g_nWndCX;
//	UINT screenheight = g_nWndCY;
//
//	if (FAILED(device->CreateTexture(screenwidth, screenheight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &scenetarget, NULL)))
//		return false;
//
//	if (FAILED(device->CreateTexture(64, 64, 0, D3DUSAGE_RENDERTARGET, D3DFMT_R16F, D3DPOOL_DEFAULT, &avgluminance, NULL)))
//		return false;
//
//	if (FAILED(device->CreateTexture(1, 1, 0, 0, D3DFMT_R16F, D3DPOOL_SYSTEMMEM, &avglumsystemmem, NULL)))
//		return false;
//
//	for (int i = 0; i < 4; ++i) {
//		avgluminance->GetSurfaceLevel(i * 2, &avglumsurfaces[i]);
//	}
//
//	avglumsystemmem->GetSurfaceLevel(0, &avglumsysmemsurface);
//
//	if (FAILED(device->CreateTexture(screenwidth / 2, screenheight / 2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &bloomresult, NULL)))
//		return false;
//
//	if (FAILED(device->CreateTexture(screenwidth / 4, screenheight / 4, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &starresult, NULL)))
//		return false;
//
//	for (int i = 0; i < 5; ++i) {
//		if (FAILED(device->CreateTexture(screenwidth / (2 << i), screenheight / (2 << i), 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &dsampletargets[i], NULL)))
//			return false;
//
//		if (FAILED(device->CreateTexture(screenwidth / (2 << i), screenheight / (2 << i), 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &blurtargets[i], NULL)))
//			return false;
//
//		dsampletargets[i]->GetSurfaceLevel(0, &dsamplesurfaces[i]);
//		blurtargets[i]->GetSurfaceLevel(0, &blursurfaces[i]);
//	}
//
//	for (int i = 0; i < 4; ++i) {
//		for (int j = 0; j < 2; ++j) {
//			if (FAILED(device->CreateTexture(screenwidth / 4, screenheight / 4, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &startargets[i][j], NULL)))
//				return false;
//
//			startargets[i][j]->GetSurfaceLevel(0, &starsurfaces[i][j]);
//		}
//	}
//
//	for (int i = 0; i < 2; ++i) {
//		if (FAILED(device->CreateTexture(screenwidth / 2, screenheight / 2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &lensflaretargets[i], NULL)))
//			return false;
//
//		if (FAILED(device->CreateTexture(screenwidth / 2, screenheight / 2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &afterimagetargets[i], NULL)))
//			return false;
//
//		lensflaretargets[i]->GetSurfaceLevel(0, &lensflaresurfaces[i]);
//		afterimagetargets[i]->GetSurfaceLevel(0, &afterimagesurfaces[i]);
//	}
//
//	scenetarget->GetSurfaceLevel(0, &scenesurface);
//	bloomresult->GetSurfaceLevel(0, &bloomsurface);
//	starresult->GetSurfaceLevel(0, &starsurface);
//
//	// setup camera
//	camera.SetAspect((float)screenwidth / (float)screenheight);
//	camera.SetFov(Math::DegreesToRadians(60));
//	camera.SetClipPlanes(0.1f, 50);
//	camera.SetZoomLimits(3, 10);
//	camera.SetDistance(5);
//	camera.SetOrientation(Math::DegreesToRadians(146), 0.1f, 0);
//
//	objectrotator.SetDistance(0);
//
//	mesh = skull;
//	effect = insulatoreffect;
//
//	return true;
//}
//
//void UninitScene()
//{
//	SAFE_RELEASE(helptext);
//	SAFE_RELEASE(environment);
//	SAFE_RELEASE(irradiance1);
//	SAFE_RELEASE(irradiance2);
//	SAFE_RELEASE(brdfLUT);
//
//	SAFE_RELEASE(scenesurface);
//	SAFE_RELEASE(bloomsurface);
//	SAFE_RELEASE(starsurface);
//
//	SAFE_RELEASE(scenetarget);
//	SAFE_RELEASE(bloomresult);
//	SAFE_RELEASE(starresult);
//
//	for (int i = 0; i < 4; ++i) {
//		SAFE_RELEASE(avglumsurfaces[i]);
//	}
//
//	SAFE_RELEASE(avglumsysmemsurface);
//
//	SAFE_RELEASE(avgluminance);
//	SAFE_RELEASE(avglumsystemmem);
//
//	for (int i = 0; i < 5; ++i) {
//		SAFE_RELEASE(dsamplesurfaces[i]);
//		SAFE_RELEASE(blursurfaces[i]);
//
//		SAFE_RELEASE(dsampletargets[i]);
//		SAFE_RELEASE(blurtargets[i]);
//	}
//
//	for (int i = 0; i < 4; ++i) {
//		for (int j = 0; j < 2; ++j) {
//			SAFE_RELEASE(starsurfaces[i][j]);
//			SAFE_RELEASE(startargets[i][j]);
//		}
//	}
//
//	for (int i = 0; i < 2; ++i) {
//		SAFE_RELEASE(lensflaresurfaces[i]);
//		SAFE_RELEASE(afterimagesurfaces[i]);
//
//		SAFE_RELEASE(lensflaretargets[i]);
//		SAFE_RELEASE(afterimagetargets[i]);
//	}
//
//	SAFE_RELEASE(teapot);
//	SAFE_RELEASE(skull);
//	SAFE_RELEASE(knot);
//	SAFE_RELEASE(skymesh);
//
//	SAFE_RELEASE(metaleffect);
//	SAFE_RELEASE(insulatoreffect);
//	SAFE_RELEASE(skyeffect);
//	SAFE_RELEASE(measureeffect);
//	SAFE_RELEASE(hdreffects);
//	SAFE_RELEASE(screenquad);
//}
//
//
//void MouseScroll(int32_t x, int32_t y, int16_t dz)
//{
//	camera.Zoom(dz * 2.0f);
//}
//
//void Update(float dt)
//{
//	camera.Update(dt);
//	objectrotator.Update(dt);
//}
//
//void RenderScene(float alpha)
//{
//	D3DXMATRIX		identity;
//	D3DXMATRIX		world, worldinv;
//	D3DXMATRIX		rotation;
//	D3DXVECTOR4		silver = { 0.972f, 0.96f, 0.915f, 1.0f };
//	Math::Color		insulator = Math::Color::sRGBToLinear(255, 255, 255);
//
//	Math::Matrix	view, proj;
//	Math::Matrix	skyview, viewproj;
//	Math::Vector4	eye;
//	Math::Vector3	orient;
//
//	D3DXMatrixIdentity(&identity);
//
//	objectrotator.Animate(alpha);
//	objectrotator.GetOrientation(orient);
//	
//	// NOTE: camera is right-handed
//	camera.Animate(alpha);
//	camera.GetViewMatrix(view);
//	camera.GetProjectionMatrix(proj);
//	camera.GetEyePosition((Math::Vector3&)eye);
//
//	device->SetRenderTarget(0, scenesurface);
//	device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
//	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
//	{
//		// render sky
//		skyview = view;
//		skyview._41 = skyview._42 = skyview._43 = 0;
//
//		Math::MatrixMultiply(viewproj, skyview, proj);
//
//		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//		device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//		device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//
//		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
//		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
//
//		skyeffect->SetMatrix("matViewProj", (D3DXMATRIX*)&viewproj);
//		skyeffect->SetMatrix("matSkyRotation", &identity);
//
//		skyeffect->Begin(NULL, 0);
//		skyeffect->BeginPass(0);
//		{
//			device->SetTexture(0, environment);
//			skymesh->DrawSubset(0);
//		}
//		skyeffect->EndPass();
//		skyeffect->End();
//
//		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
//
//		// render object
//		if (mesh == teapot) {
//			D3DXMatrixScaling(&world, 1.5f, 1.5f, 1.5f);
//		}
//		else if (mesh == knot) {
//			D3DXMatrixScaling(&world, 0.8f, 0.8f, 0.8f);
//		}
//		else if (mesh == skull) {
//			D3DXMatrixScaling(&world, 0.4f, 0.4f, 0.4f);
//			world._42 = -1.5f;
//		}
//
//		D3DXMatrixRotationYawPitchRoll(&rotation, orient.x, orient.y, 0);
//		D3DXMatrixMultiply(&world, &world, &rotation);
//		D3DXMatrixInverse(&worldinv, NULL, &world);
//
//		Math::MatrixMultiply(viewproj, view, proj);
//
//		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
//
//		device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//		device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//		device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//
//		device->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//		device->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//		device->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//
//		effect->SetMatrix("matWorld", &world);
//		effect->SetMatrix("matWorldInv", &worldinv);
//		effect->SetMatrix("matViewProj", (D3DXMATRIX*)&viewproj);
//		effect->SetVector("eyePos", (D3DXVECTOR4*)&eye);
//
//		if (effect == metaleffect) {
//			effect->SetVector("baseColor", &silver);
//			effect->SetFloat("roughness", 0.0f);
//
//			device->SetTexture(0, irradiance2);
//			device->SetTexture(1, brdfLUT);
//		}
//		else {
//			effect->SetVector("baseColor", (D3DXVECTOR4*)&insulator);
//			effect->SetFloat("roughness", 0.2f);
//
//			device->SetTexture(0, irradiance1);
//			device->SetTexture(1, irradiance2);
//			device->SetTexture(2, brdfLUT);
//		}
//
//		effect->Begin(NULL, 0);
//		effect->BeginPass(0);
//		{
//			mesh->DrawSubset(0);
//		}
//		effect->EndPass();
//		effect->End();
//
//		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
//	}
//}
//
//void MeasureLuminance()
//{
//	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
//	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
//	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//
//	for (int i = 0; i < 4; ++i) {
//		viewport.Width = 64 >> (i * 2);
//		viewport.Height = 64 >> (i * 2);
//
//		pixelsize.x = 1.0f / (float)viewport.Width;
//		pixelsize.y = -1.0f / (float)viewport.Height;
//
//		if (i == 0) {
//			texelsize.x = 1.0f / (float)oldviewport.Width;
//			texelsize.y = 1.0f / (float)oldviewport.Height;
//
//			measureeffect->SetTechnique("avgluminital");
//			device->SetTexture(0, scenetarget);
//		}
//		else if (i == 3) {
//			texelsize.x = 1.0f / (float)(64 >> (2 * (i - 1)));
//			texelsize.y = 1.0f / (float)(64 >> (2 * (i - 1)));
//
//			measureeffect->SetTechnique("avglumfinal");
//			device->SetTexture(0, avgluminance);
//		}
//		else {
//			texelsize.x = 1.0f / (float)(64 >> (2 * (i - 1)));
//			texelsize.y = 1.0f / (float)(64 >> (2 * (i - 1)));
//
//			measureeffect->SetTechnique("avglumiterative");
//			device->SetTexture(0, avgluminance);
//		}
//
//		measureeffect->SetInt("prevLevel", (i - 1) * 2);
//		measureeffect->SetVector("pixelSize", &pixelsize);
//		measureeffect->SetVector("texelSize", &texelsize);
//
//		device->SetRenderTarget(0, avglumsurfaces[i]);
//		device->SetViewport(&viewport);
//
//		measureeffect->Begin(NULL, 0);
//		measureeffect->BeginPass(0);
//		{
//			device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//		}
//		measureeffect->EndPass();
//		measureeffect->End();
//	}
//
//	D3DLOCKED_RECT rect;
//
//	device->GetRenderTargetData(avglumsurfaces[3], avglumsysmemsurface);
//	avglumsystemmem->LockRect(0, &rect, NULL, D3DLOCK_READONLY);
//	{
//		averageluminance = ((Math::Float16*)rect.pBits)->operator float();
//	}
//	avglumsystemmem->UnlockRect(0);
//}
//
//void AdaptLuminance(float dt)
//{
//	adaptedluminance = adaptedluminance + (averageluminance - adaptedluminance) * (1.0f - powf(0.98f, 50.0f * dt));
//
//	// DICE's suggestion
//	float two_ad_EV = adaptedluminance * (100.0f / 12.5f);
//	exposure = 1.0f / (1.2f * two_ad_EV);
//}
//
//void BrightPass()
//{
//	viewport.Width = oldviewport.Width / 2;
//	viewport.Height = oldviewport.Height / 2;
//
//	pixelsize.x = 1.0f / (float)viewport.Width;
//	pixelsize.y = -1.0f / (float)viewport.Height;
//
//	device->SetRenderTarget(0, dsamplesurfaces[0]);
//	device->SetViewport(&viewport);
//
//	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
//	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
//	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//
//	hdreffects->SetTechnique("brightpass");
//	hdreffects->SetVector("pixelSize", &pixelsize);
//	hdreffects->SetFloat("exposure", exposure);
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		device->SetTexture(0, scenetarget);
//		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//
//	// generate afterimage here
//	device->SetRenderTarget(0, afterimagesurfaces[currentafterimage]);
//
//	if (drawafterimage) {
//		hdreffects->SetTechnique("afterimage");
//		hdreffects->SetVector("pixelSize", &pixelsize);
//
//		hdreffects->Begin(NULL, 0);
//		hdreffects->BeginPass(0);
//		{
//			device->SetTexture(0, afterimagetargets[1 - currentafterimage]);
//			device->SetTexture(1, dsampletargets[0]);
//
//			device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//		}
//		hdreffects->EndPass();
//		hdreffects->End();
//	}
//	else {
//		device->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
//	}
//
//	currentafterimage = 1 - currentafterimage;
//}
//
//void DownSample()
//{
//	viewport.Width = oldviewport.Width / 2;
//	viewport.Height = oldviewport.Height / 2;
//
//	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//
//	hdreffects->SetTechnique("downsample");
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		for (int i = 1; i < 5; ++i) {
//			texelsize.x = 1.0f / (float)viewport.Width;
//			texelsize.y = 1.0f / (float)viewport.Height;
//
//			viewport.Width /= 2;
//			viewport.Height /= 2;
//
//			pixelsize.x = 1.0f / (float)viewport.Width;
//			pixelsize.y = -1.0f / (float)viewport.Height;
//
//			hdreffects->SetVector("pixelSize", &pixelsize);
//			hdreffects->SetVector("texelSize", &texelsize);
//			hdreffects->CommitChanges();
//
//			device->SetRenderTarget(0, dsamplesurfaces[i]);
//			device->SetTexture(0, dsampletargets[i - 1]);
//			device->SetViewport(&viewport);
//
//			device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//		}
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//}
//
//void Stars()
//{
//	viewport.Width = oldviewport.Width / 4;
//	viewport.Height = oldviewport.Height / 4;
//
//	pixelsize.x = 1.0f / (float)viewport.Width;
//	pixelsize.y = -1.0f / (float)viewport.Height;
//
//	texelsize.x = 1.0f / (float)viewport.Width;
//	texelsize.y = 1.0f / (float)viewport.Height;
//
//	device->SetViewport(&viewport);
//
//	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
//
//	hdreffects->SetTechnique("star");
//	hdreffects->SetVector("pixelSize", &pixelsize);
//	hdreffects->SetVector("texelSize", &texelsize);
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		for (int i = 0; i < 4; ++i) {
//			hdreffects->SetInt("starDirection", i);
//
//			for (int j = 0; j < 3; ++j) {
//				hdreffects->SetInt("starPass", j);
//				hdreffects->CommitChanges();
//
//				device->SetRenderTarget(0, starsurfaces[i][j % 2]);
//				device->SetTexture(0, (j == 0 ? dsampletargets[1] : startargets[i][1 - j % 2]));
//
//				device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//			}
//		}
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//
//	// combine
//	for (int i = 1; i < 4; ++i) {
//		device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//		device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//		device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//	}
//
//	hdreffects->SetTechnique("starcombine");
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		device->SetRenderTarget(0, starsurface);
//
//		device->SetTexture(0, startargets[0][0]);
//		device->SetTexture(1, startargets[1][0]);
//		device->SetTexture(2, startargets[2][0]);
//		device->SetTexture(3, startargets[3][0]);
//
//		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//}
//
//void Bloom()
//{
//	viewport.Width = oldviewport.Width / 2;
//	viewport.Height = oldviewport.Height / 2;
//
//	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
//
//	hdreffects->SetTechnique("blur");
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		for (int i = 0; i < 5; ++i) {
//			device->SetViewport(&viewport);
//
//			pixelsize.x = 1.0f / (float)viewport.Width;
//			pixelsize.y = -1.0f / (float)viewport.Height;
//
//			texelsize.x = 1.0f / (float)viewport.Width;
//			texelsize.y = 1.0f / (float)viewport.Height;
//
//			hdreffects->SetVector("pixelSize", &pixelsize);
//			hdreffects->SetVector("texelSize", &texelsize);
//
//			// horizontal
//			hdreffects->SetInt("blurDirection", 0);
//			hdreffects->CommitChanges();
//
//			device->SetRenderTarget(0, blursurfaces[i]);
//			device->SetTexture(0, dsampletargets[i]);
//			device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//
//			// vertical
//			hdreffects->SetInt("blurDirection", 1);
//			hdreffects->CommitChanges();
//
//			device->SetRenderTarget(0, dsamplesurfaces[i]);
//			device->SetTexture(0, blurtargets[i]);
//			device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//
//			viewport.Width /= 2;
//			viewport.Height /= 2;
//		}
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//
//	// combine
//	viewport.Width = oldviewport.Width / 2;
//	viewport.Height = oldviewport.Height / 2;
//
//	pixelsize.x = 1.0f / (float)viewport.Width;
//	pixelsize.y = -1.0f / (float)viewport.Height;
//
//	for (int i = 1; i < 5; ++i) {
//		device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//		device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//		device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//	}
//
//	hdreffects->SetTechnique("blurcombine");
//	hdreffects->SetVector("pixelSize", &pixelsize);
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		device->SetRenderTarget(0, bloomsurface);
//		device->SetViewport(&viewport);
//
//		device->SetTexture(0, dsampletargets[0]);
//		device->SetTexture(1, dsampletargets[1]);
//		device->SetTexture(2, dsampletargets[2]);
//		device->SetTexture(3, dsampletargets[3]);
//		device->SetTexture(4, dsampletargets[4]);
//
//		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//}
//
//void LensFlare()
//{
//	viewport.Width = oldviewport.Width / 2;
//	viewport.Height = oldviewport.Height / 2;
//
//	pixelsize.x = 1.0f / (float)viewport.Width;
//	pixelsize.y = -1.0f / (float)viewport.Height;
//
//	for (int i = 0; i < 3; ++i) {
//		device->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//		device->SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
//		device->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
//		device->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
//	}
//
//	hdreffects->SetTechnique("lensflare");
//	hdreffects->SetVector("pixelSize", &pixelsize);
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		device->SetRenderTarget(0, lensflaresurfaces[0]);
//		device->SetViewport(&viewport);
//
//		device->SetTexture(0, dsampletargets[0]);
//		device->SetTexture(1, dsampletargets[1]);
//		device->SetTexture(2, dsampletargets[2]);
//
//		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//
//	// expand lens flares
//	hdreffects->SetTechnique("lensflare_expand");
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		device->SetRenderTarget(0, lensflaresurfaces[1]);
//		device->SetViewport(&viewport);
//
//		device->SetTexture(0, lensflaretargets[0]);
//		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//}
//
//void ToneMap()
//{
//	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
//	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
//
//	pixelsize.x = 1.0f / (float)oldviewport.Width;
//	pixelsize.y = -1.0f / (float)oldviewport.Height;
//
//	device->SetViewport(&oldviewport);
//	device->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);
//
//	hdreffects->SetTechnique("tonemap");
//	hdreffects->SetVector("pixelSize", &pixelsize);
//	hdreffects->SetFloat("exposure", exposure);
//
//	hdreffects->Begin(NULL, 0);
//	hdreffects->BeginPass(0);
//	{
//		device->SetTexture(0, scenetarget);
//		device->SetTexture(1, bloomresult);
//		device->SetTexture(2, starresult);
//		device->SetTexture(3, lensflaretargets[1]);
//		device->SetTexture(4, afterimagetargets[1 - currentafterimage]);
//
//		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//	}
//	hdreffects->EndPass();
//	hdreffects->End();
//}
//
//void RenderImpl(float alpha, float elapsedtime)
//{
//	static float time = 0;
//
//	if (SUCCEEDED(device->BeginScene())) {
//		LPDIRECT3DSURFACE9 backbuffer = nullptr;
//
//		device->GetRenderTarget(0, &backbuffer);
//		device->GetViewport(&oldviewport);
//
//		viewport = oldviewport;
//
//		// STEP 1: render scene
//		RenderScene(alpha);
//
//		device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);
//		device->SetRenderState(D3DRS_ZENABLE, FALSE);
//
//		// STEP 2: measure average luminance
//		MeasureLuminance();
//		AdaptLuminance(elapsedtime);
//
//		// STEP 3: extract high-luminance regions
//		BrightPass();
//
//		// STEP 4: downsample before blur
//		DownSample();
//
//		// STEP 5: render stars
//		Stars();
//
//		// STEP 6: render bloom
//		Bloom();
//
//		// STEP 7: lens flare
//		LensFlare();
//
//		// STEP 9: tonemap & final combine
//		device->SetRenderTarget(0, backbuffer);
//		backbuffer->Release();
//
//		ToneMap();
//
//		// render text
//		viewport.Width = 512;
//		viewport.Height = 512;
//		viewport.X = viewport.Y = 10;
//
//		if (drawtext) {
//			device->SetViewport(&viewport);
//			device->SetTexture(0, helptext);
//
//			device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);
//			device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
//			device->SetRenderState(D3DRS_ZENABLE, FALSE);
//			device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
//			device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
//
//			screenquad->Begin(NULL, 0);
//			screenquad->BeginPass(0);
//			{
//				device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, DXScreenQuadVertices, 6 * sizeof(float));
//			}
//			screenquad->EndPass();
//			screenquad->End();
//		}
//
//		// reset states
//		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
//		device->SetRenderState(D3DRS_ZENABLE, TRUE);
//		device->SetViewport(&oldviewport);
//
//		device->EndScene();
//	}
//
//	time += elapsedtime;
//	device->Present(NULL, NULL, NULL, NULL);
//}
//
//void ShaderTester::Initialize()
//{
//	InitScene();
//}
//void ShaderTester::Render()
//{
//	RenderImpl(1.0f, TimeSystem::GetInstance()->DeltaTime());
//}
//void ShaderTester::Clear()
//{
//	UninitScene();
//}