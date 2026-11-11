#pragma once

namespace bdt
{
	class AABB
	{
	public:
		explicit AABB(const vec3& min, const vec3& max);
		~AABB();

		void Expand(const vec3& point);
		void Expand(const AABB& other);

		vec3 GetCenter() const;
		vec3 GetExtent() const;

		const vec3& GetMin() const;
		const vec3& GetMax() const;

		bool Intersect
		(
			const vec3& rayOrigin,
			const vec3& rayDirection,
			float tMin,
			float tMax
		) const;

	private:
		vec3 m_min;
		vec3 m_max;
	};
}
