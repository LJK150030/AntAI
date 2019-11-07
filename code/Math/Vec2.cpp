#include "Math/Vec2.hpp"
#include "Blackboard.hpp"
#include "Math/IntVec2.hpp"
#include "Math/MathUtils.hpp"

const STATIC Vec2 Vec2::ZERO(0.f, 0.f);
const STATIC Vec2 Vec2::ONE(1.f, 1.f);

//-----------------------------------------------------------------------------------------------
Vec2::Vec2(const Vec2& copy) = default;

Vec2::Vec2(const IntVec2& copy)
{
	x = static_cast<float>(copy.x);
	y = static_cast<float>(copy.y);
}

//-----------------------------------------------------------------------------------------------
Vec2::Vec2(const float initial_x, const float initial_y): x(initial_x), y(initial_y)
{
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLength() const
{
	return Sqrt(x * x + y * y);
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetLengthSquared() const
{
	return x * x + y * y;
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetAngleDegrees() const
{
	return Atan2Degrees(y, x);
}

//-----------------------------------------------------------------------------------------------
float Vec2::GetAngleRadians() const
{
	return ConvertDegreesToRadians(GetAngleDegrees());
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotated90Degrees() const
{
	const Vec2 new_vec = Vec2(-y, x);
	return new_vec;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	const Vec2 new_vec = Vec2(y, -x);
	return new_vec;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedDegrees(const float degrees_to_rotate) const
{
	const float c = CosDegrees(degrees_to_rotate);
	const float s = SinDegrees(degrees_to_rotate);

	const Vec2 new_vec = Vec2((x * c) - (y * s), (x * s) + (y * c));
	return new_vec;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetRotatedRadians(const float radians_to_rotate) const
{
	const Vec2 new_vec = GetRotatedDegrees(ConvertRadiansToDegrees(radians_to_rotate));
	return new_vec;
}

const Vec2 Vec2::GetRotationAroundZAxis(float theta_degrees) const
{
	Vec2 tempVec(x, y);
	tempVec.RotateAroundZAxis(theta_degrees);
	return tempVec;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetClamped(float max_length) const
{
	const float current_length = GetLength();

	if (current_length > max_length)
	{
		const float scale = max_length / current_length;
		const Vec2 new_vec = Vec2(x * scale, y * scale);
		return new_vec;
	}
	const Vec2 new_vec = Vec2(x, y);
	return new_vec;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::GetNormalized() const
{
	Vec2 tempVec(x, y);
	tempVec.Normalize();
	return tempVec;
}

//-----------------------------------------------------------------------------------------------
void Vec2::ClampLength(const float max_length)
{
	const float current_length = GetLength();
	if (current_length > max_length)
		SetLength(max_length);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetLength(const float new_length)
{
	Normalize();
	*this = new_length * *this;
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetAngleDegrees(const float new_angle_degrees)
{
	const float current_length = GetLength();
	x = current_length * CosDegrees(new_angle_degrees);
	y = current_length * SinDegrees(new_angle_degrees);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetAngleRadians(const float new_angle_radians)
{
	this->SetAngleDegrees(ConvertRadiansToDegrees(new_angle_radians));
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarDegrees(const float new_angle_degrees, const float new_length)
{
	this->SetLength(new_length);
	this->SetAngleDegrees(new_angle_degrees);
}

//-----------------------------------------------------------------------------------------------
void Vec2::SetPolarRadians(const float new_angle_radians, const float new_length)
{
	this->SetPolarDegrees(ConvertRadiansToDegrees(new_angle_radians), new_length);
}

void Vec2::RotateDegrees(float additional_angle_degrees)
{
	SetAngleDegrees(GetAngleDegrees() + additional_angle_degrees);
}

void Vec2::RotateRadians(float additional_angle_radians)
{
	SetAngleRadians(GetAngleRadians() + additional_angle_radians);
}

void Vec2::Rotate90Degrees()
{
	SetAngleDegrees(GetRotated90Degrees().GetAngleDegrees());
}

void Vec2::RotateMinus90Degrees()
{
	SetAngleDegrees(GetRotatedMinus90Degrees().GetAngleDegrees());
}

void Vec2::RotateAroundZAxis(float theta_degrees)
{
	x = x * CosDegrees(theta_degrees) - y * SinDegrees(theta_degrees);
	y = x * SinDegrees(theta_degrees) + x * CosDegrees(theta_degrees);
}

//-----------------------------------------------------------------------------------------------
void Vec2::Normalize()
{
	const float lengthsq = x*x + y*y;
	const bool IsZero = lengthsq == 0.0f;

	//ASSERT_RECOVERABLE(!IsZero, "attempting to normalize a zero vector");
	if(IsZero)
	{
		x = y = 0.0f;
		return;
	}

	float recip = RecipSqrt( lengthsq );
	x *= recip;
	y *= recip;
}

//-----------------------------------------------------------------------------------------------
float Vec2::NormalizeAndGetOldLength()
{
	const float old_length = GetLength();
	this->Normalize();
	return old_length;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator +(const Vec2& vec_to_add) const
{
	return Vec2(
		x + vec_to_add.x,
		y + vec_to_add.y
	);
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-(const Vec2& vec_to_subtract) const
{
	return Vec2(
		x - vec_to_subtract.x,
		y - vec_to_subtract.y
	);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*(const float uniform_scale) const
{
	return Vec2(
		x * uniform_scale,
		y * uniform_scale
	);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/(const float inverse_scale) const
{
	return Vec2(
		x / inverse_scale,
		y / inverse_scale
	);
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=(const Vec2& vec_to_add)
{
	x += vec_to_add.x;
	y += vec_to_add.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=(const Vec2& vec_to_subtract)
{
	x -= vec_to_subtract.x;
	y -= vec_to_subtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=(const float uniform_scale)
{
	x *= uniform_scale;
	y *= uniform_scale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=(const float uniform_divisor)
{
	x /= uniform_divisor;
	y /= uniform_divisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=(const Vec2& copy_from)
{
	x = copy_from.x;
	y = copy_from.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*(const float uniform_scale, const Vec2& vec_to_scale)
{
	return Vec2(
		vec_to_scale.x * uniform_scale,
		vec_to_scale.y * uniform_scale
	);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==(const Vec2& compare) const
{
	return (
		x == compare.x &&
		y == compare.y
	);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=(const Vec2& compare) const
{
	return (
		x != compare.x ||
		y != compare.y
	);
}
