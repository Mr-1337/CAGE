#pragma once
#include <GLM/glm/glm.hpp>
#include <initializer_list>
#include <vector>
#include "../VertexLayouts/VertexAttribute.hpp"

namespace cage
{

	template <class T>
	std::vector<VertexAttribute> GetLayout()
	{
	}

	// Each vertex must be able to return its layout
	// The prototype for a vertex attribute is as follows:
	// VertexAttribute(int dimensions, Type type, bool normalized, unsigned int offset)

	struct Vertex3
	{
		Vertex3(float x, float y, float z) : position{ x, y, z } {};
		glm::vec3 position;
	};

	template<>
	inline std::vector<VertexAttribute> GetLayout<Vertex3>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3, position));

		return layout;
	}

	struct Vertex3Color
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	template<>
	inline std::vector<VertexAttribute> GetLayout<Vertex3Color>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3Color, position));
		layout.emplace_back(4, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3Color, color));

		return layout;
	}

	struct Vertex3UVNormal
	{

		Vertex3UVNormal(const glm::vec3& position, const glm::vec2& uv, const glm::vec3& normal) : position(position), uv(uv), normal(normal) {};

		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	template<>
	inline std::vector<VertexAttribute> GetLayout<Vertex3UVNormal>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UVNormal, position));
		layout.emplace_back(2, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UVNormal, uv));
		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UVNormal, normal));

		return layout;
	}

}