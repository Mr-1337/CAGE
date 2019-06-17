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
		T t;
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
	std::vector<VertexAttribute> GetLayout<Vertex3>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, 0);

		return layout;
	}

	struct Vertex3Color
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	template<>
	std::vector<VertexAttribute> GetLayout<Vertex3Color>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, 0);
		layout.emplace_back(4, VertexAttribute::Type::FLOAT, false, 12);

		return layout;
	}

	struct Vertex3UVNormal
	{
		glm::vec3 positon;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	template<>
	std::vector<VertexAttribute> GetLayout<Vertex3UVNormal>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, 0);
		layout.emplace_back(2, VertexAttribute::Type::FLOAT, false, 12);
		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, 20);

		return layout;
	}


}