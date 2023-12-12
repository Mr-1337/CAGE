#include "MeshFactory.hpp"
#include "../Vertex/Vertex.hpp"
#include <GLM/glm/glm.hpp>

namespace cage
{
	namespace graphics
	{
		VkMesh* MeshFactory::BuildCube(Device& dev, float size)
		{
			std::vector<Vertex3UVNormal> finalGeo;
			glm::vec3 dirs[6] =
			{
				{ +0.0f, +0.0f, -1.0f },
				{ +0.0f, +0.0f, +1.0f },
				{ -1.0f, +0.0f, +0.0f },
				{ +1.0f, +0.0f, +0.0f },
				{ +0.0f, +1.0f, +0.0f },
				{ +0.0f, -1.0f, +0.0f }
			};

			glm::vec3 pos[8] =
			{
				{ -0.5f, +0.5f, +0.5f },
				{ +0.5f, +0.5f, +0.5f },
				{ -0.5f, -0.5f, +0.5f },
				{ +0.5f, -0.5f, +0.5f },
				{ -0.5f, +0.5f, -0.5f },
				{ +0.5f, +0.5f, -0.5f },
				{ -0.5f, -0.5f, -0.5f },
				{ +0.5f, -0.5f, -0.5f },
			};

			/*
			*    4------5
			*    |\     |\
			*    | 0----+-1
			*    | |    | |
			*    6-+----7 |
			*     \|     \|
			*      2------3
			*/

			int indices[36] =
			{
				//back
				5,7,6,
				5,6,4,
				//font
				0,2,3,
				0,3,1,
				//left
				4,6,2,
				4,2,0,
				//right
				1,3,7,
				1,7,5,
				//top
				4,0,1,
				4,1,5,
				//bottom
				2,6,7,
				2,7,3
			};
			for (int face = 0; face < 6; face++)
			{
				/*
				* Quad layout
				* A
				* | \
				* |  \     
				* |   \  
				* B----C
				* 
				* D---F
				*  \  |
				*   \ |   
				*    \| 
				*     E
				*/
				Vertex3UVNormal a(size * pos[indices[face*6 + 0]], { 0.0f, 0.0f }, dirs[face]);
				Vertex3UVNormal b(size * pos[indices[face*6 + 1]], { 0.0f, 1.0f }, dirs[face]);
				Vertex3UVNormal c(size * pos[indices[face*6 + 2]], { 1.0f, 1.0f }, dirs[face]);
				Vertex3UVNormal d(size * pos[indices[face*6 + 3]], { 0.0f, 0.0f }, dirs[face]);
				Vertex3UVNormal e(size * pos[indices[face*6 + 4]], { 1.0f, 1.0f }, dirs[face]);
				Vertex3UVNormal f(size * pos[indices[face*6 + 5]], { 1.0f, 0.0f }, dirs[face]);
				finalGeo.push_back(a);
				finalGeo.push_back(b);
				finalGeo.push_back(c);
				finalGeo.push_back(d);
				finalGeo.push_back(e);
				finalGeo.push_back(f);
			}
			VkMesh* mesh = new VkMesh(dev);
			mesh->m_VertexBuffer.Fill(finalGeo.data(), finalGeo.size() * sizeof(Vertex3UVNormal));
			mesh->m_VertexCount = finalGeo.size();
			return mesh;
		}

		VkMesh* MeshFactory::BuildSphere(Device& dev, float radius, unsigned int resolution)
		{

			std::vector<Vertex3UVNormal> spherePoints, finalGeo;

			Vertex3UVNormal top({ 0.0f, radius, 0.0f }, { 1.f, 1.f }, { 0.0f, 1.0f, 0.0f });
			Vertex3UVNormal bottom({ 0.0f, -radius, 0.0f }, { 0.f, 0.f }, { 0.0f, -1.0f, 0.0f });

			// generate points on sphere from spherical coordinates that we'll later build quads from
			for (int latitude = 1; latitude < resolution; latitude++)
			{
				float phi = (M_PI / resolution) * latitude;
				for (int longitude = 0; longitude < resolution; longitude++)
				{
					float theta = (2.f * M_PI / resolution) * longitude;
					glm::vec3 pos = { std::sinf(phi) * std::cosf(theta), std::cosf(phi), -std::sinf(phi) * std::sinf(theta) };
					pos *= radius;
					spherePoints.push_back(Vertex3UVNormal(pos, { 0.0f, 0.0f }, pos / radius));
				}
			}

			// generate triangle fans on top / bottom
			for (int tri = 0; tri < resolution; tri++)
			{
				// top
				auto a = top;
				auto b = spherePoints[tri];
				auto c = spherePoints[(tri + 1) % resolution];
				const float phi = (1.0f / resolution);
				a.uv = { (float)tri / resolution,0.f};
				b.uv = { (float)tri / resolution, phi};
				c.uv = { (float)(tri + 1) / resolution, phi};

				finalGeo.push_back(a);
				finalGeo.push_back(b);
				finalGeo.push_back(c);
				// bottom
				a = spherePoints[resolution * (resolution - 2) + tri];
				b = bottom;
				c = spherePoints[resolution * (resolution - 2) + (tri + 1) % resolution];
				const float phi2 = (1.0f / resolution) * (resolution - 1);
				a.uv = { (float)tri / resolution, phi2 };
				b.uv = { (float)(tri) / resolution, 1.0f };
				c.uv = { (float)(tri + 1) / resolution, phi2 };
				finalGeo.push_back(a);
				finalGeo.push_back(b);
				finalGeo.push_back(c);
			}

			// generate quads on strips
			for (int latitude = 0; latitude < resolution - 2; latitude++)
			{
				for (int longitude = 0; longitude < resolution; longitude++)
				{
					/*
					* Quad layout
					* A-----D
					* |     |
					* |     |
					* B-----C
					*/
					auto a = spherePoints[latitude * resolution + (longitude + 0) % resolution];
					auto b = spherePoints[(latitude+1) * resolution + (longitude + 0) % resolution];
					auto c = spherePoints[(latitude+1) * resolution + (longitude + 1) % resolution];
					auto d = spherePoints[latitude * resolution + (longitude + 1) % resolution];
					a.uv = { (float)(longitude+0) / (resolution), (float)(latitude+0+1) / (resolution) };
					b.uv = { (float)(longitude+0) / (resolution), (float)(latitude+1+1) / (resolution) };
					c.uv = { (float)(longitude+1) / (resolution), (float)(latitude+1+1) / (resolution) };
					d.uv = { (float)(longitude+1) / (resolution), (float)(latitude+0+1) / (resolution) };


					finalGeo.push_back(a);
					finalGeo.push_back(b);
					finalGeo.push_back(c);
					finalGeo.push_back(a);
					finalGeo.push_back(c);
					finalGeo.push_back(d);
				}
			}

			VkMesh* mesh = new VkMesh(dev);
			mesh->m_VertexBuffer.Fill(finalGeo.data(), finalGeo.size() * sizeof(Vertex3UVNormal));
			mesh->m_VertexCount = finalGeo.size();
			return mesh;
		}
	}
}