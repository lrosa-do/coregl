#include "Math.hpp"
#include "Utils.hpp"

//

// Array<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
// {
// 	const auto inv = glm::inverse(proj * view);

// 	Array<glm::vec4> frustumCorners;
// 	for (unsigned int x = 0; x < 2; ++x)
// 	{
// 		for (unsigned int y = 0; y < 2; ++y)
// 		{
// 			for (unsigned int z = 0; z < 2; ++z)
// 			{
// 				const glm::vec4 pt =inv * glm::vec4(
// 						2.0f * x - 1.0f,
// 						2.0f * y - 1.0f,
// 						2.0f * z - 1.0f,
// 						1.0f);
// 				frustumCorners.push_back(pt / pt.w);
// 			}
// 		}
// 	}

// 	return frustumCorners;
// }

float getLengthSQ(const glm::vec3& v)
{
	return  v.x * v.x + v.y * v.y + v.z * v.z;
}

float GetDistanceFromSQ(const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 v = v1 - v2;
	return getLengthSQ(v);
}

glm::vec3 GetAngleWeight(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	float a = GetDistanceFromSQ(v2, v3);
	float asqrt = sqrt(a);
	float b = GetDistanceFromSQ(v1, v3);
	float bsqrt = sqrt(b);
	float c = GetDistanceFromSQ(v1, v2);
	float csqrt = sqrt(c);
	return glm::vec3(
		acosf((b + c - a) / (2.f * bsqrt * csqrt)),
		acosf((-b + c + a) / (2.f * asqrt * csqrt)),
		acosf((b - c + a) / (2.f * bsqrt * asqrt)));

}

//glm::vec3 ScreenToWorld(const glm::vec2& screenPos, const glm::mat4& mvp, float width, float height)
//{
//	glm::vec4 viewport = glm::vec4(0.0f, 0.0f, width, height);
//	glm::vec4 win = glm::vec4(screenPos.x, height - screenPos.y, 0.0f, 1.0f);
//	const auto world = glm::unProject(win, glm::mat4(1.0f), mvp, viewport);
//	return glm::vec3(world);
//}

glm::vec2 WorldToScreen(const glm::vec3& worldPos, const glm::mat4& mvp, float width, float height, float winPosX, float winPosY)
{
	glm::vec4 trans = mvp * glm::vec4(worldPos, 1.0f);
	trans *= 0.5f / trans.w;
	trans += glm::vec4(0.5f, 0.5f, 0.0f, 0.0f);
	trans.y = 1.f - trans.y;
	trans.x *= width;
	trans.y *= height;
	trans.x += winPosX;
	trans.y += winPosY;
	return glm::vec2(trans.x, trans.y);
}
glm::vec3 GetScale(const glm::mat4& transform)
{
	glm::vec3 scale = glm::vec3(transform[0][0], transform[1][1], transform[2][2]);
	return scale;
}

glm::vec3 GetRotation(const glm::mat4& transform)
{
	glm::vec3 rotation = glm::eulerAngles(glm::quat_cast(transform));
	return rotation;
}

//*************************************************************************************
// BoundingBox
//*************************************************************************************
void BoundingBox::Clear()
{
	min = glm::vec3(FLT_MAX);
	max = glm::vec3(-FLT_MAX);
}

BoundingBox& BoundingBox::operator=(const BoundingBox& other)
{
	min = other.min;
	max = other.max;

	return *this;
}

BoundingBox& BoundingBox::operator=(BoundingBox&& other)
{
	min = other.min;
	max = other.max;

	return *this;
}

void BoundingBox::Set(const glm::vec3& min, const glm::vec3& max)
{
	this->min = min;
	this->max = max;
}

void BoundingBox::Set(const glm::vec3* points, uint32_t numPoints)
{
	min = glm::vec3(FLT_MAX);
	max = glm::vec3(-FLT_MAX);

	for(uint32_t i = 0; i < numPoints; i++)
	{
		min = glm::min(min, points[i]);
		max = glm::max(max, points[i]);
	}
}

void BoundingBox::SetFromPoints(const glm::vec3* points, uint32_t numPoints)
{
	min = glm::vec3(FLT_MAX);
	max = glm::vec3(-FLT_MAX);

	for(uint32_t i = 0; i < numPoints; i++)
	{
		min = glm::min(min, points[i]);
		max = glm::max(max, points[i]);
	}
}

void BoundingBox::SetFromPoints(const glm::vec3* points, uint32_t numPoints, const glm::mat4& transform)
{
	
	glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(uint32_t i = 0; i < numPoints; i++)
	{
		glm::vec3 transformed = transform * glm::vec4(points[i], 1.0f);

		min.x = glm::min(min.x, transformed.x);
		min.y = glm::min(min.y, transformed.y);
		min.z = glm::min(min.z, transformed.z);

		max.x = glm::max(max.x, transformed.x);
		max.y = glm::max(max.y, transformed.y);
		max.z = glm::max(max.z, transformed.z);
	}

	min = min;
	max = max;
}

void BoundingBox::SetFromTransformedAABB(const BoundingBox& aabb, const glm::mat4& transform)
{
	
	glm::vec3 min = aabb.min;
	glm::vec3 max = aabb.max;

	glm::vec3 minTransformed = transform * glm::vec4(min, 1.0f);
	glm::vec3 maxTransformed = transform * glm::vec4(max, 1.0f);

	min = glm::min(minTransformed, maxTransformed);
	max = glm::max(minTransformed, maxTransformed);
}

void BoundingBox::Translate(const glm::vec3& translation)
{
	
	min += translation;
	max += translation;
}

void BoundingBox::Translate(float x, float y, float z)
{
	
	Translate(glm::vec3(x, y, z));
}

void BoundingBox::Scale(const glm::vec3& scale)
{
	
	min *= scale;
	max *= scale;
}

void BoundingBox::Scale(float x, float y, float z)
{
	
	min.x *= x;
	min.y *= y;
	min.z *= z;

	max.x *= x;
	max.y *= y;
	max.z *= z;
}

void BoundingBox::Rotate(const glm::mat3& rotation)
{
	
	glm::vec3 center  = GetCenter();
	glm::vec3 extents = GetExtents();

	glm::vec3 rotatedExtents = glm::vec3(rotation * glm::vec4(extents, 1.0f));

	min = center - rotatedExtents;
	max = center + rotatedExtents;
}

void BoundingBox::Transform(const glm::mat4& transform)
{
	
	glm::vec3 newCenter = transform * glm::vec4(GetCenter(), 1.0f);
	glm::vec3 oldEdge   = GetSize() * 0.5f;
	glm::vec3 newEdge   = glm::vec3(
		glm::abs(transform[0][0]) * oldEdge.x + glm::abs(transform[1][0]) * oldEdge.y + glm::abs(transform[2][0]) * oldEdge.z,
		glm::abs(transform[0][1]) * oldEdge.x + glm::abs(transform[1][1]) * oldEdge.y + glm::abs(transform[2][1]) * oldEdge.z,
		glm::abs(transform[0][2]) * oldEdge.x + glm::abs(transform[1][2]) * oldEdge.y + glm::abs(transform[2][2]) * oldEdge.z);

	min = newCenter - newEdge;
	max = newCenter + newEdge;
}

BoundingBox BoundingBox::Transformed(const glm::mat4& transform) const
{
	
	BoundingBox box(*this);
	box.Transform(transform);
	return box;
}

BoundingBox BoundingBox::Transform(const BoundingBox &box, const glm::mat4 &mat)
{
	BoundingBox result;
	result.SetFromTransformedAABB(box, mat);
	return result;

}


void BoundingBox::Merge(const BoundingBox& other)
{
	
	if(other.min.x < min.x)
		min.x = other.min.x;
	if(other.min.y < min.y)
		min.y = other.min.y;
	if(other.min.z < min.z)
		min.z = other.min.z;
	if(other.max.x > max.x)
		max.x = other.max.x;
	if(other.max.y > max.y)
		max.y = other.max.y;
	if(other.max.z > max.z)
		max.z = other.max.z;
}

void BoundingBox::Merge(const glm::vec3& point)
{
	
	if(point.x < min.x)
		min.x = point.x;
	if(point.y < min.y)
		min.y = point.y;
	if(point.z < min.z)
		min.z = point.z;
	if(point.x > max.x)
		max.x = point.x;
	if(point.y > max.y)
		max.y = point.y;
	if(point.z > max.z)
		max.z = point.z;
}

void BoundingBox::Merge(const BoundingBox& other, const glm::mat4& transform)
{
	
	Merge(other.Transformed(transform));
}
void BoundingBox::Merge(const glm::vec3& point, const glm::mat4& transform)
{
	
	glm::vec3 transformed = transform * glm::vec4(point, 1.0f);
	Merge(transformed);
}

void BoundingBox::Merge(const BoundingBox& other, const glm::mat3& transform)
{
	
	Merge(other.Transformed(glm::mat4(transform)));
}
void BoundingBox::Merge(const glm::vec3& point, const glm::mat3& transform)
{
	
	glm::vec3 transformed = transform * glm::vec4(point, 1.0f);
	Merge(transformed);
}

bool BoundingBox::IsInside(const glm::vec3& point) const
{
	
	if(point.x < min.x || point.x > max.x || point.y < min.y || point.y > max.y || point.z < min.z || point.z > max.z)
		return false;
	else
		return true;
}

bool BoundingBox::IsInside(const BoundingBox& box) const
{
	
	if(box.max.x < min.x || box.min.x > max.x || box.max.y < min.y || box.min.y > max.y || box.max.z < min.z || box.min.z > max.z)
		return false;
	else if(box.min.x < min.x || box.max.x > max.x || box.min.y < min.y || box.max.y > max.y || box.min.z < min.z || box.max.z > max.z)
		return true;
	else
		return true;
}

//*************************************************************************************
// Plane3D
//*************************************************************************************
  Plane3D::Plane3D()
    {
        m_Normal   = glm::vec3(0.0f, 1.0f, 0.0f);
        m_Distance = 0.0f;
    }

    Plane3D::Plane3D(const glm::vec3& normal, float distance)
    {
        m_Normal   = glm::normalize(normal);
        m_Distance = distance;
    }

    Plane3D::Plane3D(const glm::vec3& point, const glm::vec3& normal)
    {
        m_Normal   = glm::normalize(normal);
        m_Distance = glm::dot(normal, point);
    }

    Plane3D::Plane3D(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3)
    {
        glm::vec3 edge1 = point2 - point1;
        glm::vec3 edge2 = point3 - point1;
        m_Normal        = glm::normalize(glm::cross(edge1, edge2));
        m_Distance      = glm::dot(m_Normal, point1);
    }

    Plane3D::Plane3D(const glm::vec4& plane)
    {
        m_Normal   = glm::vec3(plane.x, plane.y, plane.z);
        m_Distance = plane.w;
    }

    Plane3D::Plane3D(float a, float b, float c, float d)
    {
        m_Normal   = glm::vec3(a, b, c);
        m_Distance = d;
    }

    Plane3D::~Plane3D()
    {
    }

    void Plane3D::Set(const glm::vec3& normal, float distance)
    {
        m_Normal   = normal;
        m_Distance = distance;
    }

    void Plane3D::Set(const glm::vec3& point, const glm::vec3& normal)
    {
        m_Normal   = normal;
        m_Distance = glm::dot(m_Normal, point);
    }

    void Plane3D::Set(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3)
    {
        glm::vec3 vec1 = point2 - point1;
        glm::vec3 vec2 = point3 - point1;
        m_Normal       = glm::cross(vec1, vec2);
        m_Normal       = glm::normalize(m_Normal);
        m_Distance     = glm::dot(m_Normal, point1);
    }

    void Plane3D::Set(const glm::vec4& plane)
    {
        m_Normal   = glm::vec3(plane.x, plane.y, plane.z);
        m_Distance = plane.w;
    }

    void Plane3D::Normalise()
    {
        float magnitude = glm::length(m_Normal);
        m_Normal /= magnitude;
        m_Distance /= magnitude;
    }

    float Plane3D::Distance(const glm::vec3& point) const
    {
        return glm::dot(point, m_Normal) + m_Distance;
    }

    float Plane3D::Distance(const glm::vec4& point) const
    {
        return glm::dot(glm::vec3(point), m_Normal) + m_Distance;
    }

    bool Plane3D::IsPointOnPlane(const glm::vec3& point) const
    {
        return Distance(point) >= -0.0001f;
    }

    bool Plane3D::IsPointOnPlane(const glm::vec4& point) const
    {
        return Distance(point) >= -Epsilon;
    }

    void Plane3D::SetNormal(const glm::vec3& normal)
    {
        m_Normal = normal;
    }

    void Plane3D::SetDistance(float distance)
    {
        m_Distance = distance;
    }

    void Plane3D::Transform(const glm::mat4& matrix)
    {
        glm::vec4 plane = glm::vec4(m_Normal, m_Distance);
        plane           = matrix * plane;
        m_Normal        = glm::vec3(plane.x, plane.y, plane.z);
        m_Distance      = plane.w;
    }

    Plane3D Plane3D::Transformed(const glm::mat4& matrix) const
    {
        glm::vec4 plane = glm::vec4(m_Normal, m_Distance);
        plane           = matrix * plane;
        return Plane3D(glm::vec3(plane.x, plane.y, plane.z), plane.w);
    }

void Plane3D::Intersection(const Plane3D& p1, const Plane3D& p2, const Plane3D& p3, glm::vec3* result)
{
	
	
}
//*************************************************************************************
// Ray3D
//*************************************************************************************


Ray3D::Ray3D()
{
	Origin    = glm::vec3(0.0f);
	Direction = glm::vec3(0.0f);
}

Ray3D::Ray3D(const glm::vec3& origin, const glm::vec3& direction)
{
	Origin    = origin;
	Direction = direction;
}

bool Ray3D::Intersects(const BoundingBox& bb, float& t) const
{

	t = 0.0f;
	// Check for ray origin being inside the bb
	if(bb.IsInside(Origin))
		return true;

	float dist = M_INFINITY;

	// Check for intersecting in the X-direction
	if(Origin.x < bb.min.x && Direction.x > 0.0f)
	{
		float x = (bb.min.x - Origin.x) / Direction.x;
		if(x < dist)
		{
			glm::vec3 point = Origin + x * Direction;
			if(point.y >= bb.min.y && point.y <= bb.max.y && point.z >= bb.min.z && point.z <= bb.max.z)
				dist = x;
		}
	}
	if(Origin.x > bb.max.x && Direction.x < 0.0f)
	{
		float x = (bb.max.x - Origin.x) / Direction.x;
		if(x < dist)
		{
			glm::vec3 point = Origin + x * Direction;
			if(point.y >= bb.min.y && point.y <= bb.max.y && point.z >= bb.min.z && point.z <= bb.max.z)
				dist = x;
		}
	}
	// Check for intersecting in the Y-direction
	if(Origin.y < bb.min.y && Direction.y > 0.0f)
	{
		float x = (bb.min.y - Origin.y) / Direction.y;
		if(x < dist)
		{
			glm::vec3 point = Origin + x * Direction;
			if(point.x >= bb.min.x && point.x <= bb.max.x && point.z >= bb.min.z && point.z <= bb.max.z)
				dist = x;
		}
	}
	if(Origin.y > bb.max.y && Direction.y < 0.0f)
	{
		float x = (bb.max.y - Origin.y) / Direction.y;
		if(x < dist)
		{
			glm::vec3 point = Origin + x * Direction;
			if(point.x >= bb.min.x && point.x <= bb.max.x && point.z >= bb.min.z && point.z <= bb.max.z)
				dist = x;
		}
	}
	// Check for intersecting in the Z-direction
	if(Origin.z < bb.min.z && Direction.z > 0.0f)
	{
		float x = (bb.min.z - Origin.z) / Direction.z;
		if(x < dist)
		{
			glm::vec3 point = Origin + x * Direction;
			if(point.x >= bb.min.x && point.x <= bb.max.x && point.y >= bb.min.y && point.y <= bb.max.y)
				dist = x;
		}
	}
	if(Origin.z > bb.max.z && Direction.z < 0.0f)
	{
		float x = (bb.max.z - Origin.z) / Direction.z;
		if(x < dist)
		{
			glm::vec3 point = Origin + x * Direction;
			if(point.x >= bb.min.x && point.x <= bb.max.x && point.y >= bb.min.y && point.y <= bb.max.y)
				dist = x;
		}
	}

	t = dist;
	return dist < M_INFINITY;
	;
}

bool Ray3D::Intersects(const BoundingBox& bb) const
{
	float distance;
	return Intersects(bb, distance);
}

bool Ray3D::IntersectsTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& t) const
{

	const glm::vec3 E1  = b - a;
	const glm::vec3 E2  = c - a;
	const glm::vec3 N   = cross(E1, E2);
	const float det     = -glm::dot(Direction, N);
	const float invdet  = 1.f / det;
	const glm::vec3 AO  = Origin - a;
	const glm::vec3 DAO = glm::cross(AO, Direction);
	const float u       = glm::dot(E2, DAO) * invdet;
	const float v       = -glm::dot(E1, DAO) * invdet;
	t                   = glm::dot(AO, N) * invdet;
	return (det >= 1e-6f && t >= 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f);
}

//*************************************************************************************
// Frustrum
//*************************************************************************************

Frustum::Frustum()
{
Define(glm::mat4(1.0f));
}

Frustum::Frustum(const glm::mat4& transform)
{
Define(transform);
}

Frustum::Frustum(const glm::mat4& projection, const glm::mat4& view)
{
glm::mat4 m = projection * view;
Define(m);
}

Frustum::~Frustum() {
}

void Frustum::Define(const glm::mat4& projection, const glm::mat4& view)
{

glm::mat4 m = projection * view;
Define(m);
}

void Frustum::Transform(const glm::mat4& transform)
{

for(int i = 0; i < 6; i++)
{
	m_Planes[i].Transform(transform);
}

for(int i = 0; i < 6; i++)
{
	m_Planes[i].Normalise();
}

CalculateVertices(transform);
}

void Frustum::Define(const glm::mat4& transform)
{
	
	auto& m               = transform;
	m_Planes[PLANE_LEFT]  = Plane3D(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0]);
	m_Planes[PLANE_RIGHT] = Plane3D(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0]);
	m_Planes[PLANE_DOWN]  = Plane3D(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1]);
	m_Planes[PLANE_UP]    = Plane3D(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1]);
	m_Planes[PLANE_NEAR]  = Plane3D(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2]);
	m_Planes[PLANE_FAR]   = Plane3D(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2]);

	for(int i = 0; i < 6; i++)
	{
		m_Planes[i].Normalise();
	}

	CalculateVertices(transform);
}

void Frustum::DefineOrtho(float scale, float aspectRatio, float n, float f, const glm::mat4& viewMatrix)
{
	
	glm::mat4 m = glm::ortho(-scale * aspectRatio, scale * aspectRatio, -scale, scale, n, f);
	m           = m * viewMatrix;
	Define(m);
}
void Frustum::Define(float fov, float aspectRatio, float n, float f, const glm::mat4& viewMatrix)
{
	
	float tangent = tan(fov * 0.5f);
	float height  = n * tangent;
	float width   = height * aspectRatio;

	glm::mat4 m = glm::frustum(-width, width, -height, height, n, f);
	m           = m * viewMatrix;
	Define(m);
}

bool Frustum::IsInside(const glm::vec3& point) const
{
	
	for(int i = 0; i < 6; i++)
	{
		if(m_Planes[i].Distance(point) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool Frustum::IsInside(float x, float y, float z,float radius) const
{
	
	for(int i = 0; i < 6; i++)
	{
		if(m_Planes[i].Distance(glm::vec3(x,y,z)) < -radius)
		{
			return false;
		}
	}

	return true;
}
bool Frustum::IsInside(const BoundingBox &box) const
{
	return IsInside(box.min, box.max);	
}
bool Frustum::IsInside(const glm::vec3 &min, const glm::vec3 &max) const
{
	
	for(int i = 0; i < 6; i++)
	{
		glm::vec3 p = min, n = max;
		glm::vec3 N = m_Planes[i].Normal();
		if(N.x >= 0)
		{
			p.x = max.x;
			n.x = min.x;
		}
		if(N.y >= 0)
		{
			p.y = max.y;
			n.y = min.y;
		}
		if(N.z >= 0)
		{
			p.z = max.z;
			n.z = min.z;
		}

		if(m_Planes[i].Distance(p) < 0)
		{
			return false;
		}
	}
	return true;
}

bool Frustum::IsBoxInside(const BoundingBox& box, const glm::mat4& mat) const
{
	BoundingBox transformed(box);
	transformed.Transform(mat);
	return IsInside(transformed.min, transformed.max);

}



// bool Frustum::IsInside(const Ray& ray) const
// {
	
// 	for(int i = 0; i < 6; i++)
// 	{
// 		if(m_Planes[i].Distance(ray.Origin) < 0.0f)
// 		{
// 			return false;
// 		}
// 	}

// 	return true;
// }

bool Frustum::IsInside(const Plane3D& plane) const
{
	
	for(int i = 0; i < 6; i++)
	{
		if(m_Planes[i].Distance(plane.Normal()) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

const Plane3D& Frustum::GetPlane(FrustumPlane plane) const
{
	
	return m_Planes[plane];
}

glm::vec3* Frustum::GetVerticies()
{
	
	return m_Verticies;
}

void Frustum::CalculateVertices(const glm::mat4& transform)
{
	
	static const bool zerotoOne = false;
	static const bool leftHand  = true;

	glm::mat4 transformInv = glm::inverse(transform);

	m_Verticies[0] = glm::vec4(-1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
	m_Verticies[1] = glm::vec4(1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
	m_Verticies[2] = glm::vec4(1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
	m_Verticies[3] = glm::vec4(-1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);

	m_Verticies[4] = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	m_Verticies[5] = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
	m_Verticies[6] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Verticies[7] = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);

	glm::vec4 temp;
	for(int i = 0; i < 8; i++)
	{
		temp           = transformInv * glm::vec4(m_Verticies[i], 1.0f);
		m_Verticies[i] = temp / temp.w;
	}
}




//*************************************************************************************
// MatrixStack
//*************************************************************************************

MatrixStack::MatrixStack()
{
	count = 0;
	m_current =glm::mat4(1.0f);
	

}

MatrixStack::~MatrixStack()
{
}

void MatrixStack::Push()
{
	if (count == MAX_STACK)
	{
		Log(0,"MatrixStack::Push() - Stack overflow");
		return;
	}
	if (count < MAX_STACK)
	{
		m_stack[count] = m_current;
		count++;
	}
}

void MatrixStack::Pop()
{
	if (count == 0)
	{
		Log(0,"MatrixStack::Pop() - Stack underflow");
		return;
	}
	if (count > 0)
	{
		count--;
		m_current = m_stack[count];
	}
}	

void MatrixStack::Identity()
{
	m_current = glm::mat4(1.0f);
}

const glm::mat4& MatrixStack::Top() const
{
	return m_current;
}

void MatrixStack::Translate(const glm::vec3& v)
{
	m_current = glm::translate(m_current, v);
}	

void MatrixStack::Translate(float x, float y, float z)
{
	m_current = glm::translate(m_current, glm::vec3(x, y, z));
}

void MatrixStack::Scale(const glm::vec3& v)
{
	m_current = glm::scale(m_current, v);
}
void MatrixStack::Scale(float x, float y, float z)
{
	m_current = glm::scale(m_current, glm::vec3(x, y, z));
}

void MatrixStack::Rotate(float angle, float x, float y, float z)
{
	m_current = glm::rotate(m_current, angle, glm::vec3(x, y, z));
}
void MatrixStack::Rotate(float angle, const glm::vec3& v)
{
	m_current = glm::rotate(m_current, angle, v);
}

void MatrixStack::RotateX(float angle)
{
	m_current = glm::rotate(m_current, angle, glm::vec3(1.0f, 0.0f, 0.0f));
}

void MatrixStack::RotateY(float angle)
{
	m_current = glm::rotate(m_current, angle, glm::vec3(0.0f, 1.0f, 0.0f));
}

void MatrixStack::RotateZ(float angle)
{
	m_current = glm::rotate(m_current, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}



void MatrixStack::MultMatrix(const glm::mat4& m)
{
	m_current = m_current * m;
}

void MatrixStack::MultMatrix(const float* m)
{
	m_current = m_current * glm::make_mat4(m);
}

void MatrixStack::LoadMatrix(const glm::mat4& m)
{
	m_current = m;
}
