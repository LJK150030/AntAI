#include"Math/MathUtils.hpp"
#include <cmath>
#include "Math/Vec2.hpp"
#include <vector>

typedef union {float f; int i;} IntOrFloat;

//Angle utilities
//--------------------------------------------------------------------------------------------------
float ConvertRadiansToDegrees(const float radians)
{
	return radians * RADIANS_TO_DEGREES;
}

//--------------------------------------------------------------------------------------------------
float ConvertDegreesToRadians(const float degrees)
{
	return degrees * DEGREES_TO_RADIANS;
}

//--------------------------------------------------------------------------------------------------
float CosDegrees(const float degrees)
{
	return cos(ConvertDegreesToRadians(degrees));
}

//--------------------------------------------------------------------------------------------------
float SinDegrees(const float degrees)
{
	return sin(ConvertDegreesToRadians(degrees));
}

float TanDegrees(const float radians)
{
	return tan(ConvertDegreesToRadians(radians));
}

float CosRadians(const float radians)
{
	return cos(radians);
}

float SinRadians(const float radians)
{
	return sin(radians);
}

float TanRadians(const float radians)
{
	return tan(radians);
}

//--------------------------------------------------------------------------------------------------
float Atan2Degrees(const float y, const float x)
{
	return ConvertRadiansToDegrees(atan2f(y, x));
}

float GetAngularDisplacement(const float start_degrees, const float end_degrees)
{
	float ang_disp = end_degrees - start_degrees;

	while (ang_disp > 180.f)
		ang_disp -= 360.f;

	while (ang_disp < -180.f)
		ang_disp += 360.f;

	return ang_disp;
}

float GetTurnedToward(const float current_degrees, const float goal_degrees, const float max_positive_delta_degrees)
{
	const float ang_disp = GetAngularDisplacement(current_degrees, goal_degrees);
	const float turn_degrees = ClampFloat(ang_disp, -max_positive_delta_degrees, max_positive_delta_degrees);
	return turn_degrees;
}

//Geometric utilities
//--------------------------------------------------------------------------------------------------
float GetDistance(const Vec2& position_a, const Vec2& position_b)
{
	const Vec2 direction = position_b - position_a;
	return direction.GetLength();
}


//-------------------------------------------------------------------------------------------------
float GetDistanceSquared(const Vec2& position_a, const Vec2& position_b)
{
	const Vec2 direction = position_b - position_a;
	return direction.GetLengthSquared();
}

//--------------------------------------------------------------------------------------------------
bool DoDiscsOverlap(const Vec2& center_a, const float radius_a, const Vec2& center_b, const float radius_b)
{
	const float i = GetDistanceSquared(center_a, center_b);
	const float j = (radius_a + radius_b) * (radius_a + radius_b);
	return i <= j;
}

bool DoesDiscOverlapLine2D(const Vec2& disc_center, float disc_radius, const Vec2& point_on_line,
	const Vec2& another_point_on_line)
{
	const Vec2 line = another_point_on_line - point_on_line;
	const Vec2 i_axis = line.GetNormalized();
	const Vec2 j_axis = i_axis.GetRotated90Degrees();
	const Vec2 displacement = disc_center - point_on_line;
	const Vec2 displacement_onto_j = GetProjectedVectorAlongAxis2D(displacement, j_axis);
	return displacement_onto_j.GetLengthSquared() < disc_radius * disc_radius;
}

bool DoesDiscOverlapLineSegment2D(const Vec2& disc_center, float disc_radius, const Vec2& line_start,
	const Vec2& line_end)
{
	const Vec2 start_to_end = line_end - line_start;
	const Vec2 start_to_center = disc_center - line_start;

	if (DotProduct(start_to_end, start_to_center) < 0.0f)
		return IsPointInDisc2D(line_start, disc_center, disc_radius);

	const Vec2 end_to_start = line_start - line_end;
	const Vec2 end_to_center = disc_center - line_end;

	if (DotProduct(end_to_start, end_to_center) < 0.0f)
		return IsPointInDisc2D(line_end, disc_center, disc_radius);

	const Vec2 i_axis = start_to_end.GetNormalized();
	const Vec2 displacement_onto_i = DotProduct(i_axis, start_to_center) * i_axis;
	return IsPointInDisc2D(line_start + displacement_onto_i, disc_center, disc_radius);
}

bool DoesDiscOverlapCapsule2D(const Vec2& disc_center, float disc_radius, const Vec2& capsule_start,
	const Vec2& capsule_end, float capsule_radius)
{
	const float disc_new_radius = disc_radius + capsule_radius;
	return DoesDiscOverlapLineSegment2D(disc_center, disc_new_radius, capsule_start, capsule_end);
}

//--------------------------------------------------------------------------------------------------
bool IsPointInSector(const Vec2& point, const Vec2& origin, const float orientation_degrees, const float max_dist,
	const float aperture_degrees)
{
	if (!IsPointInDisc2D(point, origin, max_dist)) return false;

	Vec2 displacement_to_point = point - origin;
	const float angle_to_point_degrees = displacement_to_point.GetAngleDegrees();
	const float angular_displacement_to_point = GetAngularDisplacement(orientation_degrees, angle_to_point_degrees);
	return (fabsf(angular_displacement_to_point) <= aperture_degrees * 0.5f);
}

Vec2 GetLocalVectorFromWorld(const Vec2& world_vector, const Vec2& i_basis, const Vec2& j_basis)
{
	const float world_project_i = DotProduct(world_vector, i_basis);
	const float world_project_j = DotProduct(world_vector, j_basis);
	return Vec2(world_project_i, world_project_j);
}

Vec2 GetLocalPositionFromWorld(const Vec2& world_position, const Vec2& i_basis, const Vec2& j_basis,
	const Vec2& t_basis)
{
	const Vec2 t_to_p = world_position - t_basis;
	const float displacement_project_i = DotProduct(t_to_p, i_basis);
	const float displacement_project_j = DotProduct(t_to_p, j_basis);
	return Vec2(displacement_project_i, displacement_project_j);
}

Vec2 GetWorldVectorFromLocal(const Vec2& local_vector, const Vec2& i_basis, const Vec2& j_basis)
{
	Vec2 world_vector = local_vector.x * i_basis + local_vector.y * j_basis;
	return world_vector;
}

Vec2 GetWorldPositionFromLocal(const Vec2& local_position, const Vec2& i_basis, const Vec2& j_basis,
	const Vec2& t_basis)
{
	Vec2 world_position = t_basis + local_position.x * i_basis + local_position.y * j_basis;
	return world_position;
}

float SmoothStart2(const float input_zero_to_one)
{
	return input_zero_to_one * input_zero_to_one;
}

float SmoothStart3(const float input_zero_to_one)
{
	return input_zero_to_one * input_zero_to_one * input_zero_to_one;
}

float SmoothStart4(const float input_zero_to_one)
{
	return input_zero_to_one * input_zero_to_one * input_zero_to_one * input_zero_to_one;
}

float SmoothStart5(const float input_zero_to_one)
{
	return input_zero_to_one * input_zero_to_one * input_zero_to_one * input_zero_to_one * input_zero_to_one;
}

float SmoothStop2(const float input_zero_to_one)
{
	return (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) + 1.0f;
}

float SmoothStop3(const float input_zero_to_one)
{
	return (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) + 1.0f;
}

float SmoothStop4(const float input_zero_to_one)
{
	return (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one -
		1.0f) + 1.0f;
}

float SmoothStop5(const float input_zero_to_one)
{
	return (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one -
		1.0f) * (input_zero_to_one - 1.0f) + 1.0f;
}

float SmoothStep3(const float input_zero_to_one)
{
	const float& t = input_zero_to_one;
	return t * t * (3.f - (2.f * t));
}

float SmoothStep5(const float input_zero_to_one)
{
	const float& t = input_zero_to_one;
	return t * t * t * (t * ((t * 6) - 15) + 10);
}


//--------------------------------------------------------------------------------------------------
// The order of the composite transformation is first scale, then rotate, then translate
Vec2 TransformPosition(const Vec2& position, const float uniform_scale, const float rotation_degree_about_z,
	const Vec2& translation_xy)
{
	Vec2 new_position = position * uniform_scale;
	new_position.RotateDegrees(rotation_degree_about_z);
	new_position += translation_xy;
	return new_position;
}

//--------------------------------------------------------------------------------------------------
void TransformLocalVectorToWorld()
{
	//TODO: TransformLocalVectorToWorld
}

void TransformLocalPositionToWorld()
{
	//TODO: TransformLocalPositionToWorld
}

void TransformWorldVectorToLocal()
{
	//TODO: TransformWorldVectorToLocal
}

void TransformWorldPositionToLocal()
{
	//TODO: TransformWorldPositionToLocal
}

Vec2 MaxVec2(const Vec2& a, const Vec2& b)
{
	const float max_x = (a.x > b.x) ? a.x : b.x;
	const float max_y = (a.y > b.y) ? a.y : b.y;
	return Vec2(max_x, max_y);
}

Vec2 MinVec2(const Vec2& a, const Vec2& b)
{
	const float min_x = (a.x < b.x) ? a.x : b.x;
	const float min_y = (a.y < b.y) ? a.y : b.y;
	return Vec2(min_x, min_y);
}

float RangeMapLinearFloat(const float in_value, const float in_start, const float in_end, const float out_start,
	const float out_end)
{
	if (in_end == in_start) return 0.5f * (out_start + out_end);

	const float in_range = in_end - in_start;
	const float out_range = out_end - out_start;
	const float in_distance = in_value - in_start;
	const float in_fraction = in_distance / in_range;
	const float out_distance = in_fraction * out_range;
	const float out_value = out_start + out_distance;
	return out_value;
}

Vec2 RangeMapLinearVec2(const Vec2& in_value, const Vec2& in_start, const Vec2& in_end, const Vec2& out_start,
	const Vec2& out_end)
{
	const float x_val = RangeMapLinearFloat(in_value.x, in_start.x, in_end.x, out_start.x, out_end.x);
	const float y_val = RangeMapLinearFloat(in_value.y, in_start.y, in_end.y, out_start.y, out_end.y);
	return Vec2(x_val, y_val);
}

float LinearInterpolationFloat(const float start_value, const float end_value, const float fraction)
{
	const float range = end_value - start_value;
	const float mapped_value = fraction * range;
	return (start_value + mapped_value);
}

Vec2 LinearInterpolationVec2(const Vec2& start_value, const Vec2& end_value, float fraction)
{
	const float range_x = end_value.x - start_value.x;
	const float mapped_value_x = fraction * range_x;
	const float result_x = start_value.x + mapped_value_x;

	const float range_y = end_value.y - start_value.y;
	const float mapped_value_y = fraction * range_y;
	const float result_y = start_value.y + mapped_value_y;

	return Vec2(result_x, result_y);
}

float GetFractionInRange(const float start_value, const float end_value, const float value)
{
	const float range = end_value - start_value;
	const float distance = value - start_value;
	return distance / range;
}

float Sign(const float value)
{
	return (value <= 0.0f) ? 1.0f : -1.0f;
}

bool IsBitFlagSet(const unsigned short bits, const unsigned short bit_flag)
{
	return (bits & bit_flag);
}

void SetBitFlag(unsigned short& bits, const unsigned short bit_flag)
{
	bits |= bit_flag;
}

unsigned short GetBitFlag(unsigned short& bits, const unsigned short bit_flag)
{
	const unsigned short set = bits;
	return set & bit_flag;
}

void ClearBitFlag(unsigned short& bits, const unsigned short bit_flag)
{
	bits &= ~bit_flag;
}

void ToggleBitFlag(unsigned short& bits, const unsigned short bit_flag)
{
	bits = bits ^ bit_flag;
}

bool IsBitFlagSet(const unsigned bits, const unsigned bit_flag)
{
	return (bits & bit_flag);
}

void SetBitFlag(unsigned& bits, const unsigned bit_flag)
{
	bits |= bit_flag;
}

unsigned GetBitFlag(unsigned& bits, const unsigned bit_flag)
{
	const unsigned int set = bits;
	return set & bit_flag;
}

void ClearBitFlag(unsigned& bits, const unsigned bit_flag)
{
	bits &= ~bit_flag;
}

float ClampFloat(const float value, const float min_value, const float max_value)
{
	if (value < min_value)
		return min_value;
	if (value > max_value)
		return max_value;
	return value;
}

double ClampDouble(const double value, const double min_value, const double max_value)
{
	if (value < min_value)
		return min_value;
	if (value > max_value)
		return max_value;
	return value;
}

float ClampFloatPositive(const float value, const float max_value)
{
	const float clamped_vale = ClampFloat(value, 0.0f, max_value);
	return clamped_vale;
}

float ClampFloatPositive(const float value)
{
	if( value < 0.f )
		return 0.f;
	else
		return value;
}

Vec2 ClampVec2(const Vec2& value, const Vec2& min_value, const Vec2& max_value)
{
	const float x_val = ClampFloat(value.x, min_value.x, max_value.x);
	const float y_val = ClampFloat(value.y, min_value.y, max_value.y);

	return Vec2(x_val, y_val);
}

int RoundToNearestInt(const float value)
{
	return static_cast<int>(floorf(value + 0.5f));
}

int RoundDownToNearestInt(const float value)
{
	return static_cast<int>(floorf(value));
}

int RoundUpToNearestInt(const float value)
{
	return static_cast<int>(ceilf(value));
}

int ModPositive(const int value, const int mod_by)
{
	int mod_value = value % mod_by;
	if (mod_value < 0)
		mod_value += mod_by;
	return mod_value;
}

float ModFloatPositive(float value, float mod_by)
{
	float mod_value = value;
	while(mod_value >= mod_by)
		mod_value -= mod_by;
	while(mod_value < 0.0f)
		mod_value += mod_by;

	return mod_value;
}

float ClampZeroToOne(const float value)
{
	return ClampFloat(value, 0.0f, 1.0f);
}

int ClampInt(const int value, const int min_value, const int max_value)
{
	if (value < min_value)
		return min_value;
	if (value > max_value)
		return max_value;
	return value;
}

int ClampIntPositive(const int value, const int max_value)
{
	const int clamped_vale = ClampInt(value, 0, max_value);
	return clamped_vale;
}

float Sqrt(const float value)
{
	/*ASSERT_RECOVERABLE(value >= 0, "Attempting to Sqrt a zero value");*/
	if(value <= 0.0f) return 0.0f;
	return sqrtf( value );
}

float RecipSqrt(const float value)
{
	/*ASSERT_RECOVERABLE(value >= 0, "Attempting to Sqrt a zero value");*/
	if(value <= 0.0f) return 0.0f;
	return 1.0f/sqrtf( value );
}

float Abs(const float f)
{
	return fabsf(f);
}

int Abs(const int i)
{
	return abs(i);
}

float Min(float a, float b)
{
	return a < b ? a : b;
}

float Max(float a, float b)
{
	return a > b ? a : b;
}

int Min(int a, int b)
{
	return a < b ? a : b;
}

int Max(int a, int b)
{
	return a > b ? a : b;
}

float DotProduct(const Vec2& a, const Vec2& b)
{
	return (a.x * b.x) + (a.y * b.y);
}

float GetProjectedLengthAlongAxis2D(const Vec2& source, const Vec2& normalized_axis)
{
	return DotProduct(source, normalized_axis);
}

Vec2 GetProjectedVectorAlongAxis2D(const Vec2& source, const Vec2& axis)
{
	// 	const float projected_length = GetProjectedLengthAlongAxis2D(source, normalized_axis);
	// 	return normalized_axis * projected_length;
	const float dot_axis = DotProduct(axis, axis);
	const Vec2 normalized_axis = axis / dot_axis;
	const float dot_product = DotProduct(source, axis);
	return normalized_axis * dot_product;
}

Vec2 GetReflectedVector(const Vec2& reflect_this, const Vec2& point_normal)
{
	const Vec2 projected_vector_against_normal = GetProjectedVectorAlongAxis2D(reflect_this, point_normal);
	const Vec2 projected_vector_against_wall = reflect_this - projected_vector_against_normal;
	return projected_vector_against_wall + (-1 * projected_vector_against_normal);
}

Vec2 ReflectVectorOffSurfaceNormal(const Vec2& incoming_vector, const Vec2& surface_normal)
{
	const Vec2 incoming_vector_n = GetProjectedVectorAlongAxis2D(incoming_vector, surface_normal);
	const Vec2 incoming_vector_t = incoming_vector - incoming_vector_n;
	return incoming_vector_t + (-1.0f * incoming_vector_n);
}

Vec2 GetClosestPointOnDisc(const Vec2& position, const Vec2& disc_center, const float disc_radius)
{
	Vec2 pc = position - disc_center;
	pc.ClampLength(disc_radius);
	return disc_center + pc;
}

bool IsPointInDisc2D(const Vec2& position, const Vec2& origin, float disc_radius)
{
	Vec2 displacement = origin - position;
	const float length_squared = displacement.GetLengthSquared();
	const float disc_radius_squared = disc_radius * disc_radius;
	return length_squared <= disc_radius_squared;
}

void PushDiscOutOfDisc(const Vec2& stationary_disc_center, const float stationary_disc_radius, Vec2& mobile_disc_center,
	const float mobile_disc_radius)
{
	if(DoDiscsOverlap(stationary_disc_center, stationary_disc_radius, mobile_disc_center, mobile_disc_radius))
	{
		Vec2 displacment = mobile_disc_center - stationary_disc_center;
		displacment.SetLength((mobile_disc_radius + stationary_disc_radius) - displacment.GetLength());
		mobile_disc_center += displacment;
	}
}

// assuming they have already overlapped
void PushDiscsOutOfEachOther(Vec2& disc_1_center, const float disc_1_radius, Vec2& disc_2_center,
	const float disc_2_radius)
{
	Vec2 displacement = Vec2(disc_2_center - disc_1_center);
	const float length_from_discs = displacement.GetLength();
	const float summation_of_disc_radii = disc_1_radius + disc_2_radius;
	const float overlap = summation_of_disc_radii - length_from_discs;

	if (overlap <= 0) return;
	displacement.Normalize();
	disc_1_center += displacement * (-0.5f * overlap);
	disc_2_center += displacement * (0.5f * overlap);
}

Vec2 GetClosestPointOnLine2D(const Vec2& reference_pos, const Vec2& point_on_line, const Vec2& another_point_on_line)
{
	//	Guildhall MP1
	// 	const Vec2 line = another_point_on_line - point_on_line;
	// 	const Vec2 i = line.GetNormalized();
	// 	const Vec2 sp = reference_pos - point_on_line;
	// 	const Vec2 p_i = DotProduct(i, sp) * i;
	// 	return point_on_line + p_i;

	//essential mathematics 3rd ed
	const Vec2 w = reference_pos  - point_on_line;
	const Vec2 line_direction = another_point_on_line - point_on_line;
	const float line_mag_sq = DotProduct(line_direction, line_direction);
	const float project = DotProduct(w, line_direction);
	return point_on_line + (project/line_mag_sq)*line_direction;
}

float GetDistanceSquaredFromLine2D(const Vec2& reference_pos, const Vec2& point_on_line, const Vec2& another_point_on_line)
{
	//essential mathematics 3rd ed
	const Vec2 origin_to_point = reference_pos - point_on_line;
	const Vec2 line_direction = another_point_on_line - point_on_line;
	const float line_mag_sq = DotProduct(line_direction, line_direction);
	const float o2p_line_mag_sq = DotProduct(origin_to_point, origin_to_point);
	const float projection = DotProduct(origin_to_point, line_direction);
	return o2p_line_mag_sq - projection * projection / line_mag_sq;
}

Vec2 GetClosestPointOnLineSegment2D(const Vec2& reference_pos, const Vec2& line_start, const Vec2& line_end)
{
	//	essential mathematics 3rd ed
	const Vec2 origin_to_point =  reference_pos - line_start;
	const Vec2 line_direction = line_end - line_start;
	const float projection = DotProduct(origin_to_point, line_direction);

	if( projection <= 0.0f )
	{
		return line_start;
	}

	const float line_mag_sq = DotProduct(line_direction, line_direction);

	if(projection >= line_mag_sq)
	{
		return line_end;
	}

	return line_start + (projection/line_mag_sq) * line_direction;
}

float GetDistanceSquaredFromLineSegment2D(const Vec2& reference_pos, const Vec2& line_start, const Vec2& line_end)
{
	//	essential mathematics 3rd ed
	const Vec2 origin_to_point =  reference_pos - line_start;
	const Vec2 line_direction = line_end - line_start;
	float projection = DotProduct(origin_to_point, line_direction);

	if( projection <= 0.0f )
	{
		return DotProduct(origin_to_point, origin_to_point);
	}

	const float line_mag_sq = DotProduct(line_direction, line_direction);

	if(projection >= line_mag_sq)
	{
		return DotProduct(origin_to_point, origin_to_point) - 2.0f * projection + line_mag_sq;
	}

	return DotProduct(origin_to_point, origin_to_point) - projection * projection / line_mag_sq;
}

void GetClosestPointsFromTwo2DLines(Vec2& point_1, Vec2& point_2, const Vec2& line_1_start,
	const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end)
{
	//	essential mathematics 3rd ed
	const Vec2 w_0 = line_1_start - line_2_start;
	const Vec2 line_1_direction = line_1_end - line_1_start;
	const Vec2 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;

	if(denom == 0.0f)
	{
		point_1 = line_1_start;
		point_2 = line_2_start + (e/c) * line_2_direction;
	}
	else
	{
		point_1 = line_1_start + ((b*e - c*d)/denom)*line_1_direction;
		point_2 = line_2_start + ((a*e - b*d)/denom)*line_2_direction;
	}
}

void GetClosestPointsFromTwo2DLineSegments(Vec2& point_1, Vec2& point_2, const Vec2& line_1_start,
	const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end)
{
	// compute intermediate parameters
	Vec2 w_0 = line_1_start - line_2_start;
	const Vec2 line_1_direction = line_1_end - line_1_start;
	const Vec2 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;
	// parameters to compute s_c, t_c
	float s_c, t_c;
	float sn, sd, tn, td;

	// if denom is zero, try finding closest point on segment1 to origin0
	if ( denom == 0.0f )
	{
		// clamp s_c to 0
		sd = td = c;
		sn = 0.0f;
		tn = e;
	}
	else
	{
		// clamp s_c within [0,1]
		sd = td = denom;
		sn = b*e - c*d;
		tn = a*e - b*d;

		// clamp s_c to 0
		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		// clamp s_c to 1
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	// clamp t_c within [0,1]
	// clamp t_c to 0
	if (tn < 0.0f)
	{
		t_c = 0.0f;
		// clamp s_c to 0
		if ( -d < 0.0f )
		{
			s_c = 0.0f;
		}
		// clamp s_c to 1
		else if ( -d > a )
		{
			s_c = 1.0f;
		}
		else
		{
			s_c = -d/a;
		}
	}
	// clamp t_c to 1
	else if (tn > td)
	{
		t_c = 1.0f;
		// clamp s_c to 0
		if ( (-d+b) < 0.0f )
		{
			s_c = 0.0f;
		}
		// clamp s_c to 1
		else if ( (-d+b) > a )
		{
			s_c = 1.0f;
		}
		else
		{
			s_c = (-d+b)/a;
		}
	}
	else
	{
		t_c = tn/td;
		s_c = sn/sd;
	}

	// compute closest points
	point_1 = line_1_start + s_c*line_1_direction;
	point_2 = line_2_start + t_c*line_2_direction;
}

float GetDistanceSquaredFromTwo2DLineSegments(float& length_sq_from_l1_start, float& length_sq_from_l2_start,
	const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end)
{
	// compute intermediate parameters
	const Vec2 w_0 = line_1_start - line_2_start;
	const Vec2 line_1_direction = line_1_end - line_1_start;
	const Vec2 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;
	// parameters to compute s_c, t_c
	float sn, sd, tn, td;

	// if denom is zero, try finding closest point on segment1 to origin0
	if ( denom == 0.0f )
	{
		// clamp s_c to 0
		sd = td = c;
		sn = 0.0f;
		tn = e;
	}
	else
	{
		// clamp s_c within [0,1]
		sd = td = denom;
		sn = b*e - c*d;
		tn = a*e - b*d;

		// clamp s_c to 0
		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		// clamp s_c to 1
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	// clamp t_c within [0,1]
	// clamp t_c to 0
	if (tn < 0.0f)
	{
		length_sq_from_l2_start = 0.0f;
		// clamp s_c to 0
		if ( -d < 0.0f )
		{
			length_sq_from_l1_start = 0.0f;
		}
		// clamp s_c to 1
		else if ( -d > a )
		{
			length_sq_from_l1_start = 1.0f;
		}
		else
		{
			length_sq_from_l1_start = -d/a;
		}
	}
	// clamp t_c to 1
	else if (tn > td)
	{
		length_sq_from_l2_start = 1.0f;
		// clamp s_c to 0
		if ( (-d+b) < 0.0f )
		{
			length_sq_from_l1_start = 0.0f;
		}
		// clamp s_c to 1
		else if ( (-d+b) > a )
		{
			length_sq_from_l1_start = 1.0f;
		}
		else
		{
			length_sq_from_l1_start = (-d+b)/a;
		}
	}
	else
	{
		length_sq_from_l2_start = tn/td;
		length_sq_from_l1_start = sn/sd;
	}

	// compute difference vector and distance squared
	const Vec2 wc = w_0 + length_sq_from_l1_start*line_1_direction - length_sq_from_l2_start*line_2_direction;
	return DotProduct(wc,wc);
}

float GetDistanceSquaredFromTwo2DLines(const Vec2& line_1_start, const Vec2& line_1_end,
	const Vec2& line_2_start, const Vec2& line_2_end)
{
	const Vec2 w_0 = line_1_start - line_2_start;
	const Vec2 line_1_direction = line_1_end - line_1_start;
	const Vec2 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;

	if( denom == 0.0f )
	{
		const Vec2 w_c = w_0 - (e/c) * line_2_direction;
		return DotProduct(w_c, w_c);
	}

	const Vec2 w_c = w_0 + ((b*e - c*d)/denom) * line_1_direction - ((a*e - b*d)/denom)*line_2_direction;
	return DotProduct(w_c, w_c);
}


////////////////////////////////////////////////////////////////////////////////////////////