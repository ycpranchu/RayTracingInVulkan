#pragma once

#include "Procedural.hpp"
#include "Utilities/Glm.hpp"

namespace Assets
{
	
	class Sphere final : public Procedural
	{
	public:

		Sphere(const glm::vec3& center, const float radius) :
			Center(center), Radius(radius)
		{
		}

		const glm::vec3 Center;
		const float Radius;

		std::pair<glm::vec3, glm::vec3> BoundingBox() const override
		{
			return std::make_pair(Center - Radius, Center + Radius);
		}

	};

	class Cube final : public Procedural
	{
	public:

		Cube(const glm::vec3& center, const float radius) :
			Center(center), Radius(radius)
		{
		}

		const glm::vec3 Center;
		const float Radius;

		std::pair<glm::vec3, glm::vec3> BoundingBox() const override
		{
			return std::make_pair(Center - Radius, Center + Radius);
		}

	};

	class Cylinder final : public Procedural
	{
	public:

		Cylinder(const glm::vec3& center, const float radius) :
			Center(center), Radius(radius)
		{
		}

		const glm::vec3 Center;
		const float Radius;

		std::pair<glm::vec3, glm::vec3> BoundingBox() const override
		{
			return std::make_pair(Center - Radius, Center + Radius);
		}

	};

	class Mandelbulb final : public Procedural
	{
	public:
		
		Mandelbulb(const glm::vec3 &center, const float radius) :
			Center(center), Radius(radius)
		{
		}

		const glm::vec3 Center;
		const float Radius;

		std::pair<glm::vec3, glm::vec3> BoundingBox() const override
		{
			return std::make_pair(Center - Radius, Center + Radius);
		}
	};

}
