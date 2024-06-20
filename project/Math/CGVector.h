#pragma once
#include <assert.h>
#include<cmath>
#include<stdio.h>
#include<limits>


typedef struct Matrix4x4 {
	float m[4][4];
} Matrix4x4;

typedef struct Matrix3x3 {
	float m[3][3];
} Matrix3x3;

typedef struct Vector2 {
	float x;
	float y;
} Vector2;

typedef struct Vector3 {
	float x;
	float y;
	float z;
} Vector3;

struct Vector4 final {
	float x;
	float y;
	float z;
	float a;
};

//struct TransformMatrix {
//	Vector3 scale;
//	Vector3 rotate;
//	Vector3 translate;
//};
//
struct Line//直線
{
	Vector3 origin;//始点
	Vector3 diff;//終点
};

struct Ray//半直線
{
	Vector3 origin;
	Vector3 diff;
};

struct Segment//線分
{
	Vector3 origin;
	Vector3 diff;
};

struct Plane {//平面
	Vector3 normal;//法線
	float distance;//距離
};

struct Triangle
{
	Vector3 vertices[3];
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct OBB {
	Vector3 center;//中心点
	Vector3 orientations[3];//座標軸　正規化・直交必須 0=x 1=y 2=z
	Vector3 size;//中心から面までの距離　座標軸方向の長さの半分
};

struct EulerTransform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct QuaternionTransform
{
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};
constexpr double epsilon = std::numeric_limits<double>::epsilon();

//Matrix2x2 Add(Matrix2x2 a, Matrix2x2 b) {
//	Matrix2x2 add;
//	for (int x = 0; x < 2; x++) {
//		for (int y = 0; y < 2; y++) {
//			add.m[x][y] = a.m[x][y] + b.m[x][y];
//		}
//	}
//	return add;
//}
//Matrix2x2 Subtract(Matrix2x2 a, Matrix2x2 b) {
//	Matrix2x2 subtract;
//	for (int x = 0; x < 2; x++) {
//		for (int y = 0; y < 2; y++) {
//			subtract.m[x][y] = a.m[x][y] - b.m[x][y];
//		}
//	}
//	return subtract;
//}
//Matrix2x2 Multiply(Matrix2x2 a, Matrix2x2 b) {
//	Matrix2x2 multiply;
//	multiply.m[0][0] = (a.m[0][0] * b.m[0][0]) + (a.m[0][1] * b.m[1][0]);
//	multiply.m[0][1] = (a.m[0][0] * b.m[0][1]) + (a.m[0][1] * b.m[1][1]);
//	multiply.m[1][0] = (a.m[1][0] * b.m[0][0]) + (a.m[1][1] * b.m[1][0]);
//	multiply.m[1][1] = (a.m[1][0] * b.m[0][1]) + (a.m[1][1] * b.m[1][1]);
//
//	return multiply;
//}
//
//Vector2 Multiply(Vector2 v, Matrix2x2 b) {
//	Vector2 multiply;
//	multiply.x = (v.x * b.m[0][0]) + (v.y * b.m[1][0]);
//	multiply.y = (v.x * b.m[0][1]) + (v.y * b.m[1][1]);
//	return multiply;
//}
//
//Matrix3x3 Multiply(Matrix3x3 a, Matrix3x3 b) {
//	Matrix3x3 multiply;
//	multiply.m[0][0] = (a.m[0][0] * b.m[0][0]) + (a.m[0][1] * b.m[1][0]);
//	multiply.m[0][1] = (a.m[0][0] * b.m[0][1]) + (a.m[0][1] * b.m[1][1]);
//	multiply.m[0][2] = (a.m[0][0] * b.m[0][1]) + (a.m[0][1] * b.m[1][1]);
//	multiply.m[1][0] = (a.m[1][0] * b.m[0][0]) + (a.m[1][1] * b.m[1][0]);
//	multiply.m[1][1] = (a.m[1][0] * b.m[0][1]) + (a.m[1][1] * b.m[1][1]);
//	multiply.m[1][2] = (a.m[1][0] * b.m[0][1]) + (a.m[1][1] * b.m[1][1]);
//	multiply.m[2][0] = (a.m[1][0] * b.m[0][0]) + (a.m[1][1] * b.m[1][0]);
//	multiply.m[2][1] = (a.m[1][0] * b.m[0][1]) + (a.m[1][1] * b.m[1][1]);
//	multiply.m[2][2] = (a.m[1][0] * b.m[0][1]) + (a.m[1][1] * b.m[1][1]);
//
//	return multiply;
//}
//
//static const int kRowHeight = 20;
//static const int kcolumnWidth = 54;
//void MatrixScreenPrintf(int x, int y, Matrix2x2 z) {
//	for (int row = 0; row < 2; row++) {
//		for (int column = 0; column < 2; column++) {
//			Novice::ScreenPrintf(
//				x + column * kcolumnWidth, y + row * kRowHeight, "%.02f", z.m[row][column]);
//		}
//	}
//}
//
//void VectorScreenPrintf(int x, int y, Vector2 z) {
//	Novice::ScreenPrintf(x, y, "%.02f", z.x);
//	Novice::ScreenPrintf(x + kcolumnWidth, y, "%.02f", z.y);
//}
//
//float theta = (1.0 / 16.0f) * M_PI;
//
//int kWorld = 500;
//
//static const int kColumnWidth = 60;
//void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
//	Novice::ScreenPrintf(x, y, "%s", label);
//	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.x);
//	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.y);
//	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%.02f", vector.z);
//}
//
//void MatrixScreenPrintf(int x, int y, Matrix4x4& z) {
//	for (int row = 0; row < 4; row++) {
//		for (int column = 0; column < 4; column++) {
//			Novice::ScreenPrintf(
//				x + column * kcolumnWidth, y + row * kRowHeight, "%6.02f", z.m[row][column]);
//		}
//	}
//}
//
//
//Matrix2x2 MakeRotateMatrix(float theta) {
//
//	Matrix2x2 rotate;
//	rotate.m[0][0] = std::cos(theta);
//	rotate.m[0][1] = std::sin(theta);
//	rotate.m[1][0] = -std::sin(theta);
//	rotate.m[1][1] = std::cos(theta);
//
//	return rotate;
//};
//
//Matrix3x3 makeRotateMatrix(float theta) {
//
//	Matrix3x3 rotate;
//	rotate.m[0][0] = std::cos(theta);
//	rotate.m[1][0] = -std::sin(theta);
//	rotate.m[2][0] = 0;
//	rotate.m[0][1] = std::sin(theta);
//	rotate.m[1][1] = std::cos(theta);
//	rotate.m[2][1] = 0;
//	rotate.m[0][2] = 0;
//	rotate.m[1][2] = 0;
//	rotate.m[2][2] = 1;
//
//	return rotate;
//};
//
//Matrix3x3 MakeTranslateMatrix(Vector2 translate);
//
//Vector2 Transform(Vector2 vector, Matrix3x3 matrix);
//
//
//Matrix3x3 MakeTranslateMatrix(Vector2 translate) {
//	Matrix3x3 result;
//	result.m[0][0] = 1.0f;
//	result.m[0][1] = 0.0f;
//	result.m[0][2] = 0.0f;
//	result.m[1][0] = 0.0f;
//	result.m[1][1] = 1.0f;
//	result.m[1][2] = 0.0f;
//	result.m[2][0] = translate.x;
//	result.m[2][1] = translate.y;
//	result.m[2][2] = 1.0f;
//
//	return result;
//}
//
//Vector2 Transform(Vector2 vector, Matrix3x3 matrix) {
//	Vector2 result;
//	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
//	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
//	float w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
//	assert(w != 0.0f);
//	result.x /= w;
//	result.y /= w;
//	return result;
//}
//
//
//Matrix2x2 Inverse(Matrix2x2 a) {
//	Matrix2x2 result = {0};
//	float x = a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0];
//	if (x != 0) {
//		result.m[0][0] = 1 / x * a.m[1][1];
//		result.m[0][1] = 1 / x * -(a.m[0][1]);
//		result.m[1][0] = 1 / x * -(a.m[1][0]);
//		result.m[1][1] = 1 / x * a.m[0][0];
//	}
//	return result;
//}
//
//Matrix3x3 Inverse(Matrix3x3 a) {
//	Matrix3x3 result;
//	float x = a.m[0][0] * a.m[1][1] * a.m[2][2] + a.m[0][1] * a.m[1][2] * a.m[2][1] +
//		a.m[0][2] * a.m[1][0] * a.m[2][1] - a.m[2][0] * a.m[1][1] * a.m[2][0] -
//		a.m[0][1] * a.m[2][1] * a.m[2][2] - a.m[0][0] * a.m[1][0] * a.m[2][1];
//	if (x != 0) {
//		result.m[0][0] = 1 / x * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]);
//		result.m[0][1] = 1 / x * -(a.m[0][1] * a.m[2][2] - a.m[0][2] * a.m[2][1]);
//		result.m[0][2] = 1 / x * (a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1]);
//		result.m[1][0] = 1 / x * -(a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0]);
//		result.m[1][1] = 1 / x * (a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0]);
//		result.m[1][2] = 1 / x * -(a.m[0][0] * a.m[1][2] - a.m[0][2] * a.m[1][0]);
//		result.m[2][0] = 1 / x * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]);
//		result.m[2][1] = 1 / x * -(a.m[0][0] * a.m[2][1] - a.m[0][1] * a.m[2][0]);
//		result.m[2][2] = 1 / x * (a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0]);
//	}
//	return result;
//}
//
//Matrix2x2 TransposeM1(Matrix2x2 a) {
//	Matrix2x2 result;
//	result.m[0][0] = a.m[0][0];
//	result.m[1][1] = a.m[1][1];
//	result.m[0][1] = a.m[1][0];
//	result.m[1][0] = a.m[0][1];
//	return result;
//}
//
//Matrix3x3 TransposeM2(Matrix3x3 a) {
//	Matrix3x3 result;
//	result.m[0][0] = a.m[0][0];
//	result.m[1][0] = a.m[0][1];
//	result.m[2][0] = a.m[0][2];
//	result.m[0][1] = a.m[1][0];
//	result.m[1][1] = a.m[1][1];
//	result.m[2][1] = a.m[1][2];
//	result.m[0][2] = a.m[2][0];
//	result.m[1][2] = a.m[2][1];
//	result.m[2][2] = a.m[2][2];
//	return result;
//}
//
//
//Matrix3x3 MakeOrthographicMatrix(float left, float top, float right, float bottom) {//正射影
//	Matrix3x3 result;
//	result.m[0][0] = 2.0f / (right - left);
//	result.m[1][0] = 0;
//	result.m[2][0] = (left + right) / (left - right);
//	result.m[0][1] = 0;
//	result.m[1][1] = 2.0f / (top - bottom);
//	result.m[2][1] = (top + bottom) / (bottom - top);
//	result.m[0][2] = 0;
//	result.m[1][2] = 0;
//	result.m[2][2] = 1;
//	return result;
//}
//Matrix3x3 MakeViewportMatrix(float left, float top, float width, float height) { //ビューポート
//	Matrix3x3 result;
//	result.m[0][0] = width / 2;
//	result.m[1][0] = 0;
//	result.m[2][0] = left + width / 2;
//	result.m[0][1] = 0;
//	result.m[1][1] = -height / 2;
//	result.m[2][1] = top + height / 2;
//	result.m[0][2] = 0;
//	result.m[1][2] = 0;
//	result.m[2][2] = 1;
//	return result;
//}
//Matrix3x3 Inverse3x3(Matrix3x3 a) {
//	Matrix3x3 result;
//	result.m[2][0] = -a.m[2][0];
//	result.m[2][1] = -a.m[2][1];
//	return result;
//};
//
Vector2 Lerp(const Vector2& a, const Vector2& b, float t);

Vector2 Bezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, float t);

Vector3 Lerp(const Vector3& a, const Vector3& b, float t);

Vector3 Add(const Vector3& v1, const Vector3& v2);

Vector3 Subtract(const Vector3& v1, const Vector3& v2);

Vector3 Multiply(float scalar, const Vector3& v);

float Dot(const Vector3& v1, const Vector3& v2);

float Length(const Vector3& v);

Vector3 Normalize(const Vector3& v);

Matrix4x4 Add(Matrix4x4 a, Matrix4x4 b);

Matrix4x4 Subtract(Matrix4x4 a, Matrix4x4 b);

Matrix4x4 Multiply(Matrix4x4 a, Matrix4x4 b);

Matrix4x4 Inverse(Matrix4x4 a);

Matrix4x4 Transpose(Matrix4x4 a);

Matrix4x4 MakeIdentity4x4();

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

Matrix4x4 MakerotateXMatrix(float radian);

Matrix4x4 MakerotateYMatrix(float radian);

Matrix4x4 MakerotateZMatrix(float radian);

Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate);
//透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
//正射影
Matrix4x4 MakeOrthographicMatrix(
    float left, float top, float right, float bottom, float nearClip, float farClip);
//ビューポート
Matrix4x4 MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth);

Vector3 Cross(const Vector3& v1, const Vector3& v2);

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);

Matrix4x4 DirectionTodirection(const Vector3& from, const Vector3& to);

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

Quaternion identityQuaternion();

Quaternion Conjugate(const Quaternion& quaternion);

float Norm(const Quaternion& quaternion);

Quaternion Normalize(const Quaternion& quaternion);//正規化・単位Quaternion

Quaternion Inverse(const Quaternion& quaternion);

Quaternion MakerotateAxisQuaternion(const Vector3& axis, float angle);

Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

Matrix4x4 MakeAffineMatrix(Vector3 scale, Quaternion rotate, Vector3 translate);
