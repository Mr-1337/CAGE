#include <string>
#include <vector>

#include "../Buffers/VertexArray.hpp"

namespace cage
{

	// A Mesh in CAGE represents a single collection of vertices associated with a texture and / or material
	template <class VertexType>
	class Mesh final
	{
		std::string m_name;
		VertexArray<VertexType> m_vao;
	public:
		Mesh(std::string name);

		inline std::string GetName() const { return m_name; }
	};
}