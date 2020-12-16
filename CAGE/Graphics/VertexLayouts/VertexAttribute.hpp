#pragma once
#include <Glad/glad/glad.h>

namespace cage
{
	struct VertexAttribute
	{
		// Moves the OpenGL type enum constants into a nice C++ style enum
		enum class Type
		{
			BYTE = GL_BYTE,
			UBYTE = GL_UNSIGNED_BYTE,
			SHORT = GL_SHORT,
			USHORT = GL_UNSIGNED_SHORT,
			INT = GL_INT,
			UINT = GL_UNSIGNED_INT,
			FLOAT = GL_FLOAT,
			DOUBLE = GL_DOUBLE,
			HALF_FLOAT = GL_HALF_FLOAT,
			FIXED = GL_FIXED
		};

		VertexAttribute(int dimensions, Type type, bool normalized, unsigned int offset) 
			: dimensions(dimensions), type(type), offset(offset), normalized(normalized)
		{

		}

		// The amount of dimensions for this component. Vec3 has 3, Vec4 has 4, etc.
		int dimensions;
		
		// The type of this attribute
		Type type;

		// The offset of this attribute in bytes from the start of the Vertex
		unsigned int offset;

		// Is this value normalized
		bool normalized;
	};
}