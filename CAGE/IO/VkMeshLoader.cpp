#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Graphics/Vertex/Vertex.hpp"
#include "VkMeshLoader.hpp"
#include <SDL2/SDL_image.h>

namespace cage
{
	namespace io
	{

		std::vector<Vertex3UVNormal> toCageVerts(aiMesh* mesh)
		{

			std::vector<Vertex3UVNormal> verts, vertsOut;
			verts.reserve(mesh->mNumVertices);
			// Construct vertex data
			for (int j = 0; j < mesh->mNumVertices; j++)
			{
				glm::vec3 pos = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };
				glm::vec2 uv = { 0.0, 0.0 };
				if (mesh->mTextureCoords[0])
					uv = { mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y };

				glm::vec3 normal = { mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };

				verts.emplace_back(pos, uv, normal);
			}

			std::vector<unsigned int> indices;

			for (int f = 0; f < mesh->mNumFaces; f++)
			{
				aiFace face = mesh->mFaces[f];
				for (int v = 0; v < face.mNumIndices; v++)
					indices.push_back(face.mIndices[v]);
			}

			for (auto i : indices)
			{
				vertsOut.push_back(verts[i]);
			}

			return vertsOut;
		}

		void recurseScene(aiNode* node, const aiScene* scene, graphics::VkMesh* cageMesh)
		{
			// Get assimp mesh

			if (node->mNumMeshes)
			{
				auto mesh = scene->mMeshes[node->mMeshes[0]];				

				auto vertsOut = toCageVerts(mesh);

				cageMesh->m_VertexBuffer.Fill(vertsOut.data(), vertsOut.size() * sizeof(Vertex3UVNormal));
				cageMesh->m_VertexCount = vertsOut.size();

				auto material = scene->mMaterials[mesh->mMaterialIndex];
				aiString path;
				if (!material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path) && scene->HasTextures())
				{
					auto texture = scene->GetEmbeddedTexture(path.C_Str());
					auto b = texture->CheckFormat("png");

					auto src = SDL_RWFromMem(texture->pcData, texture->mWidth);
					auto sdlImg = IMG_Load_RW(src, 1);
					cageMesh->m_Texture = sdlImg;
				}
			}
			else
			{
				recurseScene(node->mChildren[0], scene, cageMesh);
			}
		}

		graphics::VkMesh* LoadMesh(const std::string& path, graphics::Device& device)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

			graphics::VkMesh* m = new graphics::VkMesh(device);
			recurseScene(scene->mRootNode, scene, m);
			return m;
		}

		void recurseSceneModel(aiNode* node, const aiScene* scene, std::vector<graphics::VkMesh*>& meshes, graphics::Device& device)
		{
			for (int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				auto verts = toCageVerts(mesh);
				graphics::VkMesh* cageMesh = new graphics::VkMesh(device);
				cageMesh->m_VertexBuffer.Fill(verts.data(), sizeof(Vertex3UVNormal) * verts.size());
				cageMesh->m_VertexCount = verts.size();
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				aiString path;
				if (!material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path))
				{
					if (scene->HasTextures())
					{
						auto texture = scene->GetEmbeddedTexture(path.C_Str());
						auto src = SDL_RWFromMem(texture->pcData, texture->mWidth);
						cageMesh->m_Texture = IMG_Load_RW(src, 1);
					}
					else
					{
						cageMesh->m_Texture = IMG_Load(path.C_Str());
					}

				}
				meshes.push_back(cageMesh);
			}

			for (int i = 0; i < node->mNumChildren; i++)
			{
				recurseSceneModel(node->mChildren[i], scene, meshes, device);
			}
		}

		std::vector<graphics::VkMesh*> LoadModel(const std::string& path, graphics::Device& device)
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
			if (!scene)
			{
				throw std::runtime_error("Failed to load scene!");
			}
			std::vector<graphics::VkMesh*> meshes;
			recurseSceneModel(scene->mRootNode, scene, meshes, device);
			return meshes;
		}
	}
}