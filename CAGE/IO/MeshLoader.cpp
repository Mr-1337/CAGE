#include <SDL2/SDL_image.h>

#include "MeshLoader.hpp"

namespace cage
{

	static std::vector<std::string> split(std::string& string, char delim)
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
			throw std::exception("File not found!");

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

	void LoadObjMeshes(const std::string& path, std::vector<Mesh<Vertex3UVNormal>>& meshesOut)
	{

		std::ifstream file(path);
		if (!file.is_open())
			throw std::exception("File not found!");

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
			else if (lineType == "usemtl")
			{
				std::string mtlName, fileName = "Assets/";
				temp >> mtlName;
				fileName.append(mtlName).append(".png");
				if (!meshesOut.empty())
				{
					// finalize the previous mesh
					(meshesOut[meshCount-1]).SetGeometry(verticesOut);
					verticesOut.clear();
				}
				meshesOut.emplace_back(mtlName, true);
				(meshesOut[meshCount]).LoadTexture(IMG_Load(fileName.c_str()));
				meshCount++;
			}
		}
		(meshesOut[meshCount - 1]).SetGeometry(verticesOut);
	}
}