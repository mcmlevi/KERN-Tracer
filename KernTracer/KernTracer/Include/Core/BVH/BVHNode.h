#pragma once
#include <glm/vec3.hpp>
#include <Core/Ray.h>
#include <array>
#include <utility>

namespace RT
{
	// 32 Bytes
	struct BVHNode
	{
		//6 * 4 = 24;
		std::array<glm::vec3, 2> Bounds{};
		// 4
		union 
		{
			uint32_t first;
			uint32_t left;
		};
		
		// 4
		uint32_t count{};
		bool Intersect(const RT::Ray& ray, float& t) const;
	};

	inline bool BVHNode::Intersect(const RT::Ray& ray, float& t) const
	{
		float divX{ 1.f / ray.direction.x };
		float divY{ 1.f / ray.direction.y };
		float divZ{ 1.f / ray.direction.z };
		int sign[3]{};
		sign[0] = (divX < 0);
		sign[1] = (divY < 0);
		sign[2] = (divZ < 0);

		float dMinX{};
		float dMaxX{};
		float dMinY{};
		float dMaxY{};
		float dMinZ{};
		float dMaxZ{};

		dMinX = (Bounds[sign[0]].x - ray.origin.x) * divX;
		dMaxX = (Bounds[1 - sign[0]].x - ray.origin.x) * divX;
		if (dMinX > dMaxX)
			std::swap(dMinX, dMaxX);

		dMinY = (Bounds[sign[1]].y - ray.origin.y) * divY;
		dMaxY = (Bounds[1 - sign[1]].y - ray.origin.y) * divY;

		if (dMinY > dMaxY) std::swap(dMinY, dMaxY);

		if (dMinX > dMaxY || dMinY > dMaxX)
			return false;

		dMinZ = (Bounds[sign[2]].z - ray.origin.z) * divZ;
		dMaxZ = (Bounds[1 - sign[2]].z - ray.origin.z) * divZ;

		if (dMinZ > dMaxZ) std::swap(dMinZ, dMaxZ);

		float dMinXY{ dMinY > dMinX ? dMinY : dMinX };
		float dMaxXY{ dMaxY < dMaxX ? dMaxY : dMaxX };

		if (dMinXY > dMaxZ || dMinZ > dMaxXY)
			return false;

		float dMin{ dMinZ > dMinXY ? dMinZ : dMinXY };
		float dMax{ dMaxZ < dMaxXY ? dMaxZ : dMaxXY };
		if (dMin > dMax) std::swap(dMin, dMax);



		if (dMin < 0 && dMax >= 0)
		{
			t = dMin;
			return true;
		}

		if (dMin < 0)
		{
			dMin = dMax;
			if (dMin < 0)
				return false;
		}

		t = dMin;
		return true;
	}
}
