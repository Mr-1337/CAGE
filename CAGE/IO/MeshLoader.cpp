#include <SDL2/SDL_image.h>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <iomanip>
#include <unordered_map>

#include "MeshLoader.hpp"
#include "../Graphics/Models/Model.hpp"
#include "../Graphics/Animation/Animation.hpp"

namespace cage
{

	static std::vector<std::string> split(const std::string& string, char delim)
	{
		std::stringstream stream(string);
		std::string item;
		std::vector<std::string> items;
		while (std::getline(stream, item, delim))
		{
			if (item != "")
				items.emplace_back(item);
		}
		return items;
	}

	std::vector<Vertex3UVNormal> LoadObjVertices(const std::string& path)
	{

		std::ifstream file(path);
		if (!file.is_open())
			throw std::runtime_error("File not found!");

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		std::vector<Vertex3UVNormal> verticesOut;
		std::string currLine;

		while (std::getline(file, currLine))
		{
			std::stringstream temp(currLine);
			std::string lineType;
			temp >> lineType;

			if (lineType == "#")
				continue;
			else if (lineType == "v")
			{
				// Position
				float x, y, z;
				temp >> x >> y >> z;
				positions.emplace_back(x, y, z);
			}
			else if (lineType == "vt")
			{
				// UVs
				float u, v;
				temp >> u >> v;
				uvs.emplace_back(u, v);
			}
			else if (lineType == "vn")
			{
				// Normals
				float x, y, z;
				temp >> x >> y >> z;
				normals.emplace_back(x, y, z);
			}
			else if (lineType == "f")
			{
				// Construct the face
				auto vertices = split(temp.str().substr(2), ' ');
				for (auto& s : vertices)
				{
					auto indicesStr = split(s, '/');
					int posIndex, uvIndex, normIndex;

					if (!uvs.empty())
					{
						posIndex = std::stoi(indicesStr[0]) - 1;
						uvIndex = std::stoi(indicesStr[1]) - 1;
						normIndex = std::stoi(indicesStr[2]) - 1;

						verticesOut.emplace_back(positions[posIndex], uvs[uvIndex], normals[normIndex]);
					}
					else
					{
						posIndex = std::stoi(indicesStr[0]) - 1;
						normIndex = std::stoi(indicesStr[1]) - 1;

						verticesOut.emplace_back(positions[posIndex], glm::vec2(0.f), normals[normIndex]);
					}
				}
			}
		}
		return verticesOut;
	}

	void LoadObjMeshes(const std::string& path, std::vector<Mesh<Vertex3UVNormal>*>& meshesOut)
	{

		std::ifstream file(path);
		if (!file.is_open())
			throw std::runtime_error("File not found!");

		int meshCount = 0;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		std::vector<Vertex3UVNormal> verticesOut;
		std::string currLine;

		while (std::getline(file, currLine))
		{
			std::stringstream temp(currLine);
			std::string lineType;
			temp >> lineType;

			if (lineType == "#" || lineType == "mtllib" || lineType == "g")
				continue;
			else if (lineType == "v")
			{
				// Position
				float x, y, z;
				temp >> x >> y >> z;
				positions.emplace_back(x, y, z);
			}
			else if (lineType == "vt")
			{
				// UVs
				float u, v;
				temp >> u >> v;
				uvs.emplace_back(u, v);
			}
			else if (lineType == "vn")
			{
				// Normals
				float x, y, z;
				temp >> x >> y >> z;
				normals.emplace_back(x, y, z);
			}
			else if (lineType == "f")
			{
				// Construct the face
				auto vertices = split(temp.str().substr(2), ' ');
				for (auto& s : vertices)
				{
					auto indicesStr = split(s, '/');
					int posIndex, uvIndex, normIndex;

					if (!uvs.empty())
					{
						posIndex = std::stoi(indicesStr[0]) - 1;
						uvIndex = std::stoi(indicesStr[1]) - 1;
						normIndex = std::stoi(indicesStr[2]) - 1;

						verticesOut.emplace_back(positions[posIndex], uvs[uvIndex], normals[normIndex]);
					}
					else
					{
						posIndex = std::stoi(indicesStr[0]) - 1;
						normIndex = std::stoi(indicesStr[1]) - 1;

						verticesOut.emplace_back(positions[posIndex], glm::vec2(0.f), normals[normIndex]);
					}
				}
			}
			else if (lineType == "usemtl")
			{
				std::string mtlName, fileName = "Assets/";
				temp >> mtlName;
				fileName.append(mtlName).append(".png");
				if (!meshesOut.empty())
				{
					// finalize the previous mesh
					(meshesOut[meshCount-1])->SetGeometry(verticesOut);
					verticesOut.clear();
				}
				meshesOut.push_back(new Mesh<Vertex3UVNormal>(mtlName, true));
				SDL_Surface* surface = IMG_Load(fileName.c_str());
				if (surface != nullptr)
					(meshesOut[meshCount])->LoadTexture(surface);
				meshCount++;
			}
		}
		(meshesOut[meshCount - 1])->SetGeometry(verticesOut);
	}

	void recurseScene(aiNode* node, const aiScene* scene, Model* model, scene::Node* parent, int depth, std::unordered_map<std::string, aiBone*>& bones, std::vector<animation::Bone>& bonesOut)
	{
		std::cout << "|" << std::setw(depth) << " " << "Node Name: " << node->mName.C_Str() << std::endl;

		scene::Node* cageNode = new scene::Node(node->mName.C_Str());
		
		for (int x = 0; x < 4; x++)
			for (int y = 0; y < 4; y++)
				cageNode->m_LocalTransform[x][y] = node->mTransformation[y][x];

		if (parent)
			parent->Add(cageNode);

		if (auto bone = bones.find(node->mName.C_Str()); bone != bones.end())
		{
			// This node is a bone, make a CAGE bone
			std::cout << "Found bone " << node->mName.C_Str() << std::endl;

			animation::Bone cageBone;
			cageBone.m_Name = node->mName.C_Str();

			// bone transform data
			glm::mat4 boneMat;
			for (int x = 0; x < 4; x++)
				for (int y = 0; y < 4; y++)
					boneMat[y][x] = bone->second->mOffsetMatrix[x][y];
			cageBone.m_LocalTransform = cageNode->m_LocalTransform;
			cageBone.m_Orientation = glm::identity<glm::quat>();
			cageBone.m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
			cageBone.m_Offset = boneMat;
			cageBone.m_Parent = 0;
			
			// figure out parent bone index
			auto parentBone = bones.find(node->mParent->mName.C_Str());
			if (parentBone != bones.cend())
			{
				// parent node is a known bone, find its index in bonesOut
				for (int i = 0; i < bonesOut.size(); i++)
				{
					if (bonesOut[i].m_Name == parentBone->first)
					{
						// we found the bone and the index
						cageBone.m_Parent = i;
						break;
					}
				}
			}

			// cage bone now has all data and knows its parent, add to output vector
			bonesOut.push_back(cageBone);
		}

		for (int i = 0; i < node->mNumMeshes; i++)
		{
			std::cout << "Mesh" << std::endl;
			// Get assimp mesh
			auto mesh = scene->mMeshes[node->mMeshes[i]];

			// Construct engine mesh
			Mesh<Vertex3UVNormal>* cageMesh = new Mesh<Vertex3UVNormal>(node->mName.C_Str());

			std::vector<Vertex3UVNormal> verts;
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

			for (int j = 0; j < mesh->mNumBones; j++)
			{
				for (int w = 0; w < mesh->mBones[j]->mNumWeights; w++)
				{
					
					auto weightInfo = mesh->mBones[j]->mWeights[w];

					for (int vi = 0; vi < 4; vi++)
					{
						if (verts[weightInfo.mVertexId].boneIDs[vi] == -1)
						{
							verts[weightInfo.mVertexId].weights[vi] = weightInfo.mWeight;
							verts[weightInfo.mVertexId].boneIDs[vi] = j;
							break;
						}
					}
				}
			}

			std::vector<unsigned int> indices;

			for (int f = 0; f < mesh->mNumFaces; f++)
			{
				aiFace face = mesh->mFaces[f];
				for (int v = 0; v < face.mNumIndices; v++)
					indices.push_back(face.mIndices[v]);
			}

			cageMesh->SetGeometry(verts, indices);
			auto material = scene->mMaterials[mesh->mMaterialIndex];
			aiString path;
			if (!material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path) && scene->HasTextures())
			{
				auto texture = scene->GetEmbeddedTexture(path.C_Str());
				auto b = texture->CheckFormat("png");

				auto src = SDL_RWFromMem(texture->pcData, texture->mWidth);
				auto sdlImg = IMG_Load_RW(src, 1);
				cageMesh->LoadTexture(sdlImg);
				SDL_FreeSurface(sdlImg);
			}

			cageMesh->m_TransformNode = cageNode;
			model->AddMesh(cageMesh);
		}
		for (int i = 0; i < node->mNumChildren; i++)
			recurseScene(node->mChildren[i], scene, model, cageNode, depth+1, bones, bonesOut);
	}

	void findBones(const aiScene* scene, aiNode* node, std::unordered_map<std::string, aiBone*>& bones)
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			auto mesh = scene->mMeshes[node->mMeshes[i]];
			for (int j = 0; j < mesh->mNumBones; j++)
			{
				auto meshBone = mesh->mBones[j];
				bones.insert({ meshBone->mName.C_Str(), meshBone });
			}
		}

		for (int i = 0; i < node->mNumChildren; i++)
			findBones(scene, node->mChildren[i], bones);
	}

	Model* LoadModelFromFile(const std::string& path)
	{
		Assimp::Importer importer;
		std::unordered_map<std::string, aiBone*> bones;
		std::vector<animation::Bone> bonesOut;

		const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace);
		findBones(scene, scene->mRootNode, bones);		

		Model* m = new Model();

		recurseScene(scene->mRootNode, scene, m, nullptr, 1, bones, bonesOut);

		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			std::cout << "Loading animation: " << scene->mAnimations[i]->mName.C_Str() << std::endl;
			auto anim = scene->mAnimations[i];
			animation::Animation* animation;
			std::vector<std::vector<animation::Keyframe>> channels;
			channels.resize(bonesOut.size());
			for (int i = 0; i < anim->mNumChannels; i++)
			{
				std::cout << anim->mChannels[i]->mNodeName.C_Str() << std::endl;
				auto itr = std::find_if(bonesOut.cbegin(), bonesOut.cend(), [anim, i](const auto& b) { return b.m_Name == anim->mChannels[i]->mNodeName.C_Str(); });
				if (itr != bonesOut.cend())
				{
					int index = itr - bonesOut.cbegin();
					for (int j = 0; j < anim->mChannels[i]->mNumRotationKeys; j++)
					{
						auto rot = anim->mChannels[i]->mRotationKeys[j];
						auto pos = anim->mChannels[i]->mPositionKeys[j];

						// std::cout << rot.mTime << ", " << pos.mTime << std::endl;
						glm::quat orientation(rot.mValue.w, rot.mValue.x, rot.mValue.y, rot.mValue.z);
						float timestamp = rot.mTime / (1000);

						animation::Bone b = bonesOut[index];
						b.m_Orientation = orientation;
						b.m_Position = { pos.mValue.x, pos.mValue.y, pos.mValue.z };
						channels[index].push_back(animation::Keyframe(b, timestamp));
					}
				}

			}
			animation = new animation::Animation(channels, bonesOut, anim->mDuration / 1000.0f);
			m->LoadAnimation(animation);
		}

		m->LoadBones(bonesOut);
		return m;
	}
}