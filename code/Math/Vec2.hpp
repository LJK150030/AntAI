#pragma once
#include <string>

struct IntVec2;

//-----------------------------------------------------------------------------------------------
struct Vec2
{
public:
	const static Vec2 ZERO;
	const static Vec2 ONE;

public:
	// Construction/Destruction
	~Vec2() = default; // destructor: do nothing (for speed)

	Vec2(): x(0.f), y(0.f) {} // default constructor: do nothing (for speed)
	Vec2(const Vec2& copy); // copy constructor (from another vec2)
	explicit Vec2(const IntVec2& copy);
	explicit Vec2(float initial_x, float initial_y); // explicit constructor (from x, y)

	// Accessors
	float GetLength() const;
	float GetLengthSquared() const;
	float GetAngleDegrees() const;
	float GetAngleRadians() const;
	const Vec2 GetRotated90Degrees() const;
	const Vec2 GetRotatedMinus90Degrees() const;
	const Vec2 GetRotatedDegrees(float degrees_to_rotate) const;
	const Vec2 GetRotatedRadians(float radians_to_rotate) const;
	const Vec2 GetRotationAroundZAxis(float theta_degrees) const;
	const Vec2 GetClamped(float max_length) const;
	const Vec2 GetNormalized() const;

	// Mutilators
	void ClampLength(float max_length);
	void SetLength(float new_length);
	void SetAngleDegrees(float new_angle_degrees);
	void SetAngleRadians(float new_angle_radians);
	void SetPolarDegrees(float new_angle_degrees, float new_length);
	void SetPolarRadians(float new_angle_radians, float new_length);
	void RotateDegrees(float additional_angle_degrees);
	void RotateRadians(float additional_angle_radians);
	void Rotate90Degrees();
	void RotateMinus90Degrees();
	void RotateAroundZAxis( float theta_degrees );
	void Normalize();
	float NormalizeAndGetOldLength();

	// Operators
	const Vec2 operator+(const Vec2& vec_to_add) const; // vec2 + vec2
	const Vec2 operator-(const Vec2& vec_to_subtract) const; // vec2 - vec2
	const Vec2 operator*(float uniform_scale) const; // vec2 * float
	const Vec2 operator/(float inverse_scale) const; // vec2 / float
	void operator+=(const Vec2& vec_to_add); // vec2 += vec2
	void operator-=(const Vec2& vec_to_subtract); // vec2 -= vec2
	void operator*=(float uniform_scale); // vec2 *= float
	void operator/=(float uniform_divisor); // vec2 /= float
	void operator=(const Vec2& copy_from); // vec2 = vec2
	bool operator==(const Vec2& compare) const; // vec2 == vec2
	bool operator!=(const Vec2& compare) const; // vec2 != vec2

	friend const Vec2 operator*(float uniform_scale, const Vec2& vec_to_scale); // float * vec2
public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
};
