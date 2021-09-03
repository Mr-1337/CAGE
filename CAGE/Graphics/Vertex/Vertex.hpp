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

	struct Vertex2
	{
		Vertex2(float x, float y) : position{ x, y } {};
		glm::vec2 position;
	};

	template<>
	inline std::vector<VertexAttribute> GetLayout<Vertex2>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(2, VertexAttribute::Type::FLOAT, false, offsetof(Vertex2, position));

		return layout;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//

	struct Vertex2UV
	{
		Vertex2UV(float x, float y, float u, float v) : 
			Vertex2UV({ x, y }, { u, v }) 
		{};
		Vertex2UV(const glm::vec2&& pos, const glm::vec2&& uv) : 
			position(pos), 
			uv(uv) 
		{};

		glm::vec2 position;
		glm::vec2 uv;

	};

	template<>
	inline std::vector<VertexAttribute> GetLayout<Vertex2UV>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(2, VertexAttribute::Type::FLOAT, false, offsetof(Vertex2UV, position));
		layout.emplace_back(2, VertexAttribute::Type::FLOAT, false, offsetof(Vertex2UV, uv));

		return layout;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//

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

	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//

	struct Vertex3Color
	{
		Vertex3Color(const glm::vec3&& pos, const glm::vec4&& col) : position(pos), color(col) {};
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

	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//

	struct Vertex3UV
	{
		Vertex3UV(const glm::vec3&& pos, const glm::vec2&& uv) : position(pos), uv(uv) {};
		Vertex3UV(float x, float y, float z, float u, float v) :
			Vertex3UV({ x, y, z }, { u, v })
		{};

		glm::vec3 position;
		glm::vec2 uv;
	};

	template<>
	inline std::vector<VertexAttribute> GetLayout<Vertex3UV>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UV, position));
		layout.emplace_back(2, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UV, uv));

		return layout;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//

	struct Vertex3UVNormal
	{

		Vertex3UVNormal(const glm::vec3& position, const glm::vec2& uv, const glm::vec3& normal) : position(position), uv(uv), normal(normal) 
		{
			boneIDs = { -1, -1, -1, -1 };
			weights = { 0, 0, 0, 0 };
		};

		Vertex3UVNormal(const glm::vec3& position, const glm::vec2& uv, const glm::vec3& normal, const glm::ivec4& boneIDs, const glm::vec4& weights) : position(position), uv(uv), normal(normal), boneIDs(boneIDs), weights(weights)
		{

		};

		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		glm::vec4 boneIDs;
		glm::vec4 weights;
	};

	template<>
	inline std::vector<VertexAttribute> GetLayout<Vertex3UVNormal>()
	{
		std::vector<VertexAttribute> layout;

		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UVNormal, position));
		layout.emplace_back(2, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UVNormal, uv));
		layout.emplace_back(3, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UVNormal, normal));
		layout.emplace_back(4, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UVNormal, boneIDs));
		layout.emplace_back(4, VertexAttribute::Type::FLOAT, false, offsetof(Vertex3UVNormal, weights));

		return layout;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
	//---------------------------------------------------------------------------------------------------------------------------------------//
}