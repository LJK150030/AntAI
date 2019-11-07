#pragma once

struct Vec2;

//-----------------------------------------------------------------------------------------------
struct IntVec2
{
	const static IntVec2 ZERO;
	const static IntVec2 ONE;

public:
	// Construction/Destruction
	~IntVec2() = default; // destructor: do nothing (for speed)
	IntVec2(): x(0), y(0)
	{
	} // default constructor: do nothing (for speed)
	IntVec2(const IntVec2& copy); // copy constructor (from another vec2)
	explicit IntVec2(const Vec2& copy); // copy constructor (from another vec2)
	explicit IntVec2(int initial_x, int initial_y); // explicit constructor (from x, y)

	// Accessors
	//int GetLength() const;
	int GetLengthSquared() const;
	int GetArea() const;
	const IntVec2 GetRotated90Degrees() const;
	const IntVec2 GetRotatedMinus90Degrees() const;

	// Operators
	const IntVec2 operator+(const IntVec2& vec_to_add) const; // vec2 + vec2
	const IntVec2 operator-(const IntVec2& vec_to_subtract) const; // vec2 - vec2
	const IntVec2 operator*(int uniform_scale) const; // vec2 * int
	const IntVec2 operator/(int inverse_scale) const; // vec2 / int
	void operator+=(const IntVec2& vec_to_add); // vec2 += vec2
	void operator-=(const IntVec2& vec_to_subtract); // vec2 -= vec2
	void operator*=(int uniform_scale); // vec2 *= int
	void operator/=(int uniform_divisor); // vec2 /= int
	void operator=(const IntVec2& copy_from); // vec2 = vec2
	bool operator==(const IntVec2& compare) const; // vec2 == vec2
	bool operator!=(const IntVec2& compare) const; // vec2 != vec2

	friend const IntVec2 operator*(int uniform_scale, const IntVec2& vec_to_scale); // int * vec2

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x;
	int y;
};
