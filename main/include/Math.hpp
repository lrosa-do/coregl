#pragma once



#include "Config.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


static constexpr float smallFloat = 0.000001f;
static constexpr unsigned int MaxUInt32 = 0xFFFFFFFF;
static constexpr int MinInt32 = 0x80000000;
static constexpr int MaxInt32 = 0x7FFFFFFF;
static constexpr float MaxFloat = 3.402823466e+38F;
static constexpr float MinPosFloat = 1.175494351e-38F;
static constexpr float Pi = 3.141592654f;
static constexpr float TwoPi = 6.283185307f;
static constexpr float PiHalf = 1.570796327f;
static constexpr float Epsilon = 0.000001f;
static constexpr float M_INFINITY      = std::numeric_limits<float>::infinity();


#define powi(base,exp) (int)powf((float)(base), (float)(exp))

#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))


inline float Clamp(float value, float min, float max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}
inline int Clamp(int value, int min, int max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}



inline float Sin( float a ){	return sin( a * M_PI / 180 );}
inline float Cos( float a ){	return cos( a * M_PI / 180 );}
inline float Tan( float a ){	return tan( a * M_PI / 180 );}
inline float SinRad( float a ){	return sin( a );}
inline float CosRad( float a ){	return cos( a );}
inline float TanRad( float a ){	return tan( a );}
inline float ASin( float a ){	return asin( a ) * 180 / M_PI;}
inline float ACos( float a ){	return acos( a ) * 180 / M_PI;}
inline float ATan( float a ){	return atan( a ) * 180 / M_PI;}
inline float ATan2( float y, float x ){	return atan2( y, x ) * 180 / M_PI;}
inline float ASinRad( float a ){	return asin( a );}
inline float ACosRad( float a ){	return acos( a );}
inline float ATanRad( float a ){	return atan( a );}
inline float ATan2Rad( float y, float x ){	return atan2( y, x );}
inline int Floor( float a ){	return (int) ( floor( a ) );}
inline int Ceil( float a ){	return (int) ( ceil( a ) );}
inline int Trunc( float a ){	if ( a > 0 ) return Floor( a );	else return Ceil( a );}
inline int Round( float a ){	if ( a < 0 ) return (int) ( ceil( a-0.5f ) );	else return (int) ( floor( a+0.5f ) );}
inline float Sqrt( float a ){	if ( a > 0 ) return sqrt( a );	else return 0;}
inline float Abs( float a ){	if ( a < 0 ) a = -a;	return a;}
inline int Mod( int a, int b ){	if ( b == 0 ) return 0;	return a % b;}
inline float FMod( float a, float b ){	if ( b == 0 ) return 0;	return fmod( a, b );}
inline float Pow( float a, float b ){	return pow( a, b );}
inline int Sign( float a ){	if ( a < 0 ) return -1;	else if ( a > 0 ) return 1;	else return 0;}



template<typename T>
struct  Rectangle
{

T x;
T y;
T width;
T height;

Rectangle() : x(0), y(0), width(0), height(0) {}
Rectangle(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}
Rectangle(const Rectangle &rect) : x(rect.x), y(rect.y), width(rect.width), height(rect.height) {}

void Set(T x, T y, T width, T height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

void Merge(const Rectangle &rect)
{
	T right = x + width;
	T bottom = y + height;
	T rectRight = rect.x + rect.width;
	T rectBottom = rect.y + rect.height;
	x = glm::min(x, rect.x);
	y = glm::min(y, rect.y);
	right = glm::max(right, rectRight);
	bottom = glm::max(bottom, rectBottom);
	width = right - x;
	height = bottom - y;
}

void Merge(const glm::vec2 &point)
{
	T right = x + width;
	T bottom = y + height;
	x = glm::min(x, point.x);
	y = glm::min(y, point.y);
	right = glm::max(right, point.x);
	bottom = glm::max(bottom, point.y);
	width = right - x;
	height = bottom - y;
}

void Clear()
{
	x = 0;
	y = 0;
	width = 0;
	height = 0;
}

Rectangle& operator=(const Rectangle &rect)
{
	if (this == &rect)
		return *this;
	x = rect.x;
	y = rect.y;
	width = rect.width;
	height = rect.height;
	return *this;
}

};

template<typename T>
struct  Size
{
T width;
T height;

Size() : width(0), height(0) {}
Size(T w, T h) : width(w), height(h) {}
Size(const Size &size) : width(size.width), height(size.height) {}

Size& operator=(const Size &size)
{
	if (this == &size)
		return *this;
	width = size.width;
	height = size.height;
	return *this;
}


};

typedef Rectangle<int> IntRect;
typedef Rectangle<float> FloatRect;
typedef Size<int> IntSize;
typedef Size<float> FloatSize;



struct Vector2
{
	float x;
	float y;

	Vector2() : x(0), y(0) {};
	Vector2(float x, float y) : x(x), y(y) {};
	Vector2(const Vector2 &v) : x(v.x), y(v.y) {};

	Vector2& operator=(const Vector2 &v)
	{
		if (this == &v)
			return *this;
		x = v.x;
		y = v.y;
		return *this;
	}

	Vector2 operator+(const Vector2 &v) const
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 operator-(const Vector2 &v) const
	{
		return Vector2(x - v.x, y - v.y);
	}



};


struct Vector3
{
	float x;
	float y;
	float z;

	Vector3() : x(0), y(0), z(0) {};
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {};
	Vector3(const Vector3 &v) : x(v.x), y(v.y), z(v.z) {};

	Vector3& operator=(const Vector3 &v)
	{
		if (this == &v)
			return *this;
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	Vector3 operator+(const Vector3 &v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(const Vector3 &v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float scalar) const
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	Vector3 operator/(float scalar) const
	{
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	Vector3& operator+=(const Vector3 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& operator-=(const Vector3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vector3& operator/=(float scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	bool operator==(const Vector3 &v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator!=(const Vector3 &v) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	float Dot(const Vector3 &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vector3 Cross(const Vector3 &v) const
	{
		return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	float LengthSquared() const
	{
		return x * x + y * y + z * z;
	}

	Vector3 Normalized() const
	{
		float len = Length();
		if (len == 0)
			return Vector3(0, 0, 0);
		return *this / len;
	}

	void Normalize()
	{
		float len = Length();
		if (len == 0)
			return;
		*this /= len;
	}

	Vector3 Reflect(const Vector3 &normal) const
	{
		return *this - normal * 2.0f * Dot(normal);
	}

	Vector3 Refract(const Vector3 &normal, float eta) const
	{
		float dot = Dot(normal);
		float k = 1.0f - eta * eta * (1.0f - dot * dot);
		if (k < 0.0f)
			return Vector3(0, 0, 0);
		else
			return *this * eta - normal * (eta * dot + sqrt(k));
	}

	Vector3 Lerp(const Vector3 &v, float t) const
	{
		return *this * (1.0f - t) + v * t;
	}

	Vector3 Slerp(const Vector3 &v, float t) const
	{
		float dot = Dot(v);
		dot = Clamp(dot, -1.0f, 1.0f);
		float theta = acos(dot) * t;
		Vector3 relative = (v - *this * dot).Normalized();
		return *this * cos(theta) + relative * sin(theta);
	}

	Vector3 Rotate(const Vector3 &axis, float angle) const
	{
		float sinAngle = sin(angle);
		float cosAngle = cos(angle);
		return *this * cosAngle + axis.Cross(*this) * sinAngle + axis * axis.Dot(*this) * (1 - cosAngle);
	}



};




struct  BoundingBox
{
	glm::vec3 min;
	glm::vec3 max;
	BoundingBox()
	{
		min = glm::vec3(MaxFloat, MaxFloat, MaxFloat);
		max = glm::vec3(-MaxFloat, -MaxFloat, -MaxFloat);
	}
	BoundingBox(const glm::vec3 &min, const glm::vec3 &max)
	{
		this->min = min;
		this->max = max;
	}
	BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	{
		min = glm::vec3(minX, minY, minZ);
		max = glm::vec3(maxX, maxY, maxZ);
	}
	BoundingBox(const BoundingBox &box)
	{
		min = box.min;
		max = box.max;
	}
	BoundingBox(const glm::vec3* points, uint32_t numPoints)
	{
		SetFromPoints(points, numPoints);
	}

	~BoundingBox()
	{
	}
	void AddPoint(const glm::vec3 &point)
	{
		if (point.x < min.x)
			min.x = point.x;
		if (point.y < min.y)
			min.y = point.y;
		if (point.z < min.z)
			min.z = point.z;

		if (point.x > max.x)
			max.x = point.x;
		if (point.y > max.y)
			max.y = point.y;
		if (point.z > max.z)
			max.z = point.z;
	}
	void Reset()
	{
		min = glm::vec3(MaxFloat, MaxFloat, MaxFloat);
		max = glm::vec3(-MaxFloat, -MaxFloat, -MaxFloat);
	
	}

	void Reset(float x, float y, float z)
	{
			 min.x = x;
            min.y = y;
            min.z = z;
            max.x = x;
            max.y = y;
            max.z = z;
	}
	void Reset(const glm::vec3 &point)
	{
		min = point;
		max = point;
	}
	
	void Add(const BoundingBox &box)
	{
		AddPoint(box.min);
		AddPoint(box.max);
	}
	void Add(const glm::vec3 &min, const glm::vec3 &max)
	{
		AddPoint(min);
		AddPoint(max);
	}

  	void Clear();

	BoundingBox& operator=(const BoundingBox& other);
	BoundingBox& operator=(BoundingBox&& other);

	void Set(const glm::vec3& min, const glm::vec3& max);
	void Set(const glm::vec3* points, uint32_t numPoints);

	void SetFromPoints(const glm::vec3* points, uint32_t numPoints);
	void SetFromPoints(const glm::vec3* points, uint32_t numPoints, const glm::mat4& transform);
	void SetFromTransformedAABB(const BoundingBox& aabb, const glm::mat4& transform);

	void Translate(const glm::vec3& translation);
	void Translate(float x, float y, float z);

	void Scale(const glm::vec3& scale);
	void Scale(float x, float y, float z);

	void Rotate(const glm::mat3& rotation);

	void Transform(const glm::mat4& transform);
	BoundingBox Transformed(const glm::mat4& transform) const;

	void Merge(const BoundingBox& other);
	void Merge(const glm::vec3& point);

	void Merge(const BoundingBox& other, const glm::mat4& transform);
	void Merge(const glm::vec3& point, const glm::mat4& transform);

	void Merge(const BoundingBox& other, const glm::mat3& transform);
	void Merge(const glm::vec3& point, const glm::mat3& transform);

	bool IsInside(const glm::vec3& point) const;
	bool IsInside(const BoundingBox& box) const;

	bool IsPointInside(const glm::vec3 &point) const
	{
		return point.x >= min.x && point.x <= max.x &&
			point.y >= min.y && point.y <= max.y &&
			point.z >= min.z && point.z <= max.z;
	}

	bool IsBoundingBoxInside(const BoundingBox &box) const
	{
		return IsPointInside(box.min) && IsPointInside(box.max);
	}

	bool IsBoundingBoxInside(const glm::vec3 &min, const glm::vec3 &max) const
	{
		return IsPointInside(min) && IsPointInside(max);
	}


	

	glm::vec3 GetCenter() const
	{
		return (min + max) * 0.5f;
	}

	glm::vec3 GetExtents() const
	{
		return (max - min);
	}
	glm::vec3 GetSize() const
	{
		return max - min;
	}

	glm::vec3 GetHalfSize() const
	{
		return (max - min) * 0.5f;
	}

	void GetEdges(Vector3  *edges) const
	{
		Vector3 middle;
		middle.x = (min.x + max.x) * 0.5f;
		middle.y = (min.y + max.y) * 0.5f;
		middle.z = (min.z + max.z) * 0.5f;
		
		Vector3 diag;
		diag.x = middle.x - min.x;
		diag.y = middle.y - min.y;
		diag.z = middle.z - min.z;
		

		edges[0] = Vector3(middle.x + diag.x, middle.y + diag.y, middle.z + diag.z);
		edges[1] = Vector3(middle.x + diag.x, middle.y - diag.y, middle.z + diag.z);
		edges[2] = Vector3(middle.x + diag.x, middle.y + diag.y, middle.z - diag.z);
		edges[3] = Vector3(middle.x + diag.x, middle.y - diag.y, middle.z - diag.z);
		edges[4] = Vector3(middle.x - diag.x, middle.y + diag.y, middle.z + diag.z);
		edges[5] = Vector3(middle.x - diag.x, middle.y - diag.y, middle.z + diag.z);
		edges[6] = Vector3(middle.x - diag.x, middle.y + diag.y, middle.z - diag.z);
		edges[7] = Vector3(middle.x - diag.x, middle.y - diag.y, middle.z - diag.z);

	

	}

	void GetEdges(glm::vec3  *edges) const
	{
		glm::vec3 middle = (min + max) * 0.5f;
		glm::vec3 diag = middle - max;

		edges[0] = glm::vec3(middle.x + diag.x, middle.y + diag.y, middle.z + diag.z);
		edges[1] = glm::vec3(middle.x + diag.x, middle.y - diag.y, middle.z + diag.z);
		edges[2] = glm::vec3(middle.x + diag.x, middle.y + diag.y, middle.z - diag.z);
		edges[3] = glm::vec3(middle.x + diag.x, middle.y - diag.y, middle.z - diag.z);
		edges[4] = glm::vec3(middle.x - diag.x, middle.y + diag.y, middle.z + diag.z);
		edges[5] = glm::vec3(middle.x - diag.x, middle.y - diag.y, middle.z + diag.z);
		edges[6] = glm::vec3(middle.x - diag.x, middle.y + diag.y, middle.z - diag.z);
		edges[7] = glm::vec3(middle.x - diag.x, middle.y - diag.y, middle.z - diag.z);

	

	}
	float GetRadius() const
	{
		return glm::length(max - min) * 0.5f;
	}
	void Repair()
	{
			float temp;

            if (min.x > max.x)
            {
                temp = min.x;
                min.x = max.x;
                max.x = temp;
            }

            if (min.y > max.y)
            {
                temp = min.y;
                min.y = max.y;
                max.y = temp;
            }
	}
	bool IsEmpty() const
	{
		return min.x > max.x || min.y > max.y || min.z > max.z;
	}



	static BoundingBox Transform(const BoundingBox &box, const glm::mat4 &mat);


};

struct  Ray3D
{

	Ray3D();
	Ray3D(const glm::vec3& origin, const glm::vec3& direction);

	bool Intersects(const BoundingBox& bb) const;
	bool Intersects(const BoundingBox& bb, float& t) const;
	bool IntersectsTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& t) const;

	glm::vec3 Origin, Direction;
};

class  Plane3D
{
public:
	Plane3D();
	Plane3D(const glm::vec3& normal, float distance);
	Plane3D(const glm::vec3& point, const glm::vec3& normal);
	Plane3D(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3);
	Plane3D(const glm::vec4& plane);
	Plane3D(float a, float b, float c, float d);
	~Plane3D();

	void Set(const glm::vec3& normal, float distance);
	void Set(const glm::vec3& point, const glm::vec3& normal);
	void Set(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3);
	void Set(const glm::vec4& plane);
	void SetNormal(const glm::vec3& normal);
	void SetDistance(float distance);
	void Transform(const glm::mat4& transform);
	Plane3D Transformed(const glm::mat4& transform) const;

	void Normalise();

	float Distance(const glm::vec3& point) const;
	float Distance(const glm::vec4& point) const;

	bool IsPointOnPlane(const glm::vec3& point) const;
	bool IsPointOnPlane(const glm::vec4& point) const;

	glm::vec3 Project(const glm::vec3& point) const { return point - Distance(point) * m_Normal; }

	inline glm::vec3 Normal() const { return m_Normal; }
	inline float Distance() const { return m_Distance; }

	
static void Intersection(const Plane3D& p1, const Plane3D& p2,  const Plane3D& p3, glm::vec3* result);

private:
	glm::vec3 m_Normal;
	float m_Distance;
};

  enum FrustumPlane
        {
            PLANE_NEAR = 0,
            PLANE_LEFT,
            PLANE_RIGHT,
            PLANE_UP,
            PLANE_DOWN,
            PLANE_FAR,
        };

class   Frustum
{
public:
	Frustum();
	Frustum(const glm::mat4& transform);
	Frustum(const glm::mat4& projection, const glm::mat4& view);
	~Frustum();

	void Transform(const glm::mat4& transform);
	void Define(const glm::mat4& projection, const glm::mat4& view);
	void Define(const glm::mat4& transform);
	void DefineOrtho(float scale, float aspectRatio, float n, float f, const glm::mat4& viewMatrix);
	void Define(float fov, float aspectRatio, float n, float f, const glm::mat4& viewMatrix);

	bool IsInside(const glm::vec3& point) const;
	bool IsInside(const BoundingBox &box) const;
	bool IsInside(const glm::vec3 &min, const glm::vec3 &max) const;
	bool IsInside(float x, float y, float z,float radius) const;
	bool IsInside(const Plane3D& plane) const;
	bool IsBoxInside(const BoundingBox& box, const glm::mat4 &mat) const;


	const Plane3D& GetPlane(FrustumPlane plane) const;
	const Plane3D& GetPlane(int index) const { return m_Planes[index]; }
	glm::vec3* GetVerticies();

private:
	void CalculateVertices(const glm::mat4& transform);

	Plane3D m_Planes[6];
	glm::vec3 m_Verticies[8];
};

class  MatrixStack
{
public:
	constexpr static int MAX_STACK = 32;
	MatrixStack();
	~MatrixStack();

	void Push();
	void Pop();
	void Identity();
	void LoadMatrix(const glm::mat4 &matrix);
	void MultMatrix(const glm::mat4 &matrix);
	void MultMatrix(const float *matrix);
	void Translate(float x, float y, float z);
	void Translate(const glm::vec3 &v);
	void Scale(float x, float y, float z);
	void Scale(const glm::vec3 &v);
	void Rotate(float angle, float x, float y, float z);
	void Rotate(float angle, const glm::vec3 &v);
	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);
	const glm::mat4& Top() const;
private:
	glm::mat4 m_stack[MAX_STACK];
	glm::mat4 m_current;
	int count{ 0 };
};