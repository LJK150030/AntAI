#pragma once
#include "Blackboard.hpp"

//--------------------------------------------------------------------------------------------------
struct Vec2;

//--------------------------------------------------------------------------------------------------
// Angle utilities
//
float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);
float CosDegrees(float degrees);
float SinDegrees(float degrees);
float TanDegrees(float degrees);
float CosRadians(float radians);
float SinRadians(float radians);
float TanRadians(float radians);
float Atan2Degrees(float y, float x);
float GetAngularDisplacement(float start_degrees, float end_degrees);
float GetTurnedToward(float current_degrees, float goal_degrees, float max_positive_delta_degrees);

//--------------------------------------------------------------------------------------------------
// Geometric utilities
//
float GetDistance(const Vec2& position_a, const Vec2& position_b);
float GetDistanceSquared(const Vec2& position_a, const Vec2& position_b);

//--------------------------------------------------------------------------------------------------
// Transform utilities
//
Vec2 TransformPosition(const Vec2& position, float uniform_scale, float rotation_degree_about_z,
                       const Vec2& translation_xy);
void TransformLocalVectorToWorld();
void TransformLocalPositionToWorld();
void TransformWorldVectorToLocal();
void TransformWorldPositionToLocal();
Vec2 MaxVec2(const Vec2& a, const Vec2& b);
Vec2 MinVec2(const Vec2& a, const Vec2& b);

//--------------------------------------------------------------------------------------------------
// Conversion
//
constexpr float MATH_PI = 3.1415926535897932384626433832795f;
constexpr float RADIANS_TO_DEGREES = 180.0f / MATH_PI;
constexpr float DEGREES_TO_RADIANS = MATH_PI / 180.0f;
float RangeMapLinearFloat(float in_value, float in_start, float in_end, float out_start, float out_end);
Vec2 RangeMapLinearVec2(const Vec2& in_value, const Vec2& in_start, const Vec2& in_end, const Vec2& out_start, const Vec2& out_end);
float LinearInterpolationFloat(float start_value, float end_value, float fraction);
Vec2 LinearInterpolationVec2(const Vec2& start_value, const Vec2& end_value, float fraction);
float GetFractionInRange(float start_value, float end_value, float value);
float Sign( float value );

//--------------------------------------------------------------------------------------------------
// Bit operations
//
bool IsBitFlagSet(unsigned short bits, unsigned short bit_flag);
void SetBitFlag(unsigned short& bits, unsigned short bit_flag);
unsigned short GetBitFlag(unsigned short& bits, unsigned short bit_flag);
void ClearBitFlag(unsigned short& bits, unsigned short bit_flag);
void ToggleBitFlag(unsigned short& bits, unsigned short bit_flag);

bool IsBitFlagSet(unsigned int bits, unsigned int bit_flag);
void SetBitFlag(unsigned int& bits, unsigned int bit_flag);
unsigned int GetBitFlag(unsigned int& bits, unsigned int bit_flag);
void ClearBitFlag(unsigned int& bits, unsigned int bit_flag);
void ToggleBitFlag(unsigned int& bits, unsigned int bit_flag);

//--------------------------------------------------------------------------------------------------
// number operations
//
float ClampFloat(float value, float min_value, float max_value);
double ClampDouble(double value, double min_value, double max_value);
float ClampFloatPositive(float value, float max_value);
float ClampFloatPositive(const float value);
Vec2 ClampVec2(const Vec2& value, const Vec2& min_value, const Vec2& max_value);
int RoundToNearestInt(float value);
int RoundDownToNearestInt(float value);
int RoundUpToNearestInt(float value);
int ModPositive(int value, int mod_by);
float ModFloatPositive(float value, float mod_by);
float ClampZeroToOne(float value);
int ClampInt(int value, int min_value, int max_value);
int ClampIntPositive(int value, int max_value);
float Sqrt(float value);
float RecipSqrt(float value);
float Abs(float f);
int Abs(int i);
float Min(float a, float b);
float Max(float a, float b);
int Min(int a, int b);
int Max(int a, int b);
unsigned int QuadraticFormula(float* out, float A, float B, float C);

//--------------------------------------------------------------------------------------------------
// Matrix Operations
//
float DotProduct(const Vec2& a, const Vec2& b);
float GetProjectedLengthAlongAxis2D(const Vec2& source, const Vec2& normalized_axis);
Vec2 GetReflectedVector(const Vec2& reflect_this, const Vec2& point_normal);
Vec2 GetProjectedVectorAlongAxis2D(const Vec2& source, const Vec2& axis);
Vec2 ReflectVectorOffSurfaceNormal(const Vec2& incoming_vector, const Vec2& surface_normal);
float GetRayImpactFractionVsDisc2D(const Vec2& ray_start, const Vec2& ray_normalized_dir, float ray_length,
                                   const Vec2& disc_center, float disc_radius);

//--------------------------------------------------------------------------------------------------
// Collision Detection and resolution
//

// line vs point
Vec2 GetClosestPointOnLine2D(const Vec2& reference_pos, const Vec2& point_on_line, const Vec2& another_point_on_line);
float GetDistanceSquaredFromLine2D(const Vec2& reference_pos, const Vec2& point_on_line, const Vec2& another_point_on_line);

Vec2 GetClosestPointOnLineSegment2D(const Vec2& reference_pos, const Vec2& line_start, const Vec2& line_end);
float GetDistanceSquaredFromLineSegment2D(const Vec2& reference_pos, const Vec2& line_start, const Vec2& line_end);

void GetClosestPointsFromTwo2DLines(Vec2& point_1, Vec2& point_2, const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end);
void GetClosestPointsFromTwo2DLineSegments(Vec2& point_1, Vec2& point_2, const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end);
float GetDistanceSquaredFromTwo2DLineSegments(float& length_sq_from_l1_start, float& length_sq_from_l2_start, const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end);
float GetDistanceSquaredFromTwo2DLines(const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end);

// Disc checks
Vec2 GetClosestPointOnDisc(const Vec2& position, const Vec2& disc_center, float disc_radius);
bool IsPointInDisc2D(const Vec2& position, const Vec2& origin, float disc_radius);
bool DoDiscsOverlap(const Vec2& center_a, float radius_a, const Vec2& center_b, float radius_b);
bool DoesDiscOverlapLine2D(const Vec2& disc_center, float disc_radius, const Vec2& point_on_line, const Vec2& another_point_on_line);
bool DoesDiscOverlapLineSegment2D(const Vec2& disc_center, float disc_radius, const Vec2& line_start, const Vec2& line_end);
bool DoesDiscOverlapCapsule2D(const Vec2& disc_center, float disc_radius, const Vec2& capsule_start, const Vec2& capsule_end, float capsule_radius);

void PushDiscOutOfPoint(const Vec2& position, Vec2& disc_center, float disc_radius);
void PushDiscOutOfDisc(const Vec2& stationary_disc_center, float stationary_disc_radius, Vec2& mobile_disc_center, float mobile_disc_radius);
void PushDiscsOutOfEachOther(Vec2& disc_1_center, float disc_1_radius, Vec2& disc_2_center, float disc_2_radius);

Vec2 GetClosestPointOnCapsule2D(const Vec2& reference_pos, const Vec2& capsule_start, const Vec2& capsule_end, float capsule_radius);
bool IsPointInCapsule2D(const Vec2& point, const Vec2& capsule_start, const Vec2& capsule_end, float capsule_radius);

bool IsPointInSector(const Vec2& point, const Vec2& origin, float orientation_degrees, float max_dist,
                     float aperture_degrees);

//--------------------------------------------------------------------------------------------------
// Local and World conversions
//
Vec2 GetLocalVectorFromWorld(const Vec2& world_vector, const Vec2& i_basis, const Vec2& j_basis);
Vec2 GetLocalPositionFromWorld(const Vec2& world_position, const Vec2& i_basis, const Vec2& j_basis,
                               const Vec2& t_basis);
Vec2 GetWorldVectorFromLocal(const Vec2& local_vector, const Vec2& i_basis, const Vec2& j_basis);
Vec2 GetWorldPositionFromLocal(const Vec2& local_position, const Vec2& i_basis, const Vec2& j_basis,
                               const Vec2& t_basis);

//--------------------------------------------------------------------------------------------------
// Easing functions 
//

float SmoothStart2(float input_zero_to_one);
float SmoothStart3(float input_zero_to_one);
float SmoothStart4(float input_zero_to_one);
float SmoothStart5(float input_zero_to_one);

float SmoothStop2(float input_zero_to_one);
float SmoothStop3(float input_zero_to_one);
float SmoothStop4(float input_zero_to_one);
float SmoothStop5(float input_zero_to_one);

float SmoothStep3(float input_zero_to_one);
float SmoothStep5(float input_zero_to_one);
