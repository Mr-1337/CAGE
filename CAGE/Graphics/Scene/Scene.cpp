#include <stack>
#include "Scene.hpp"
#include "../../IO/Logging/Logger.hpp"

namespace cage
{
	namespace graphics
	{
		Scene::Scene(Device& device) : m_MainCam(), m_descriptorManager(device)
		{
			m_root = Node::Make("Scene Root");
			m_bgColor = { 0.f, 0.f, 0.f, 1.f };
			loadDefaultAssets();
		}

		Scene::~Scene()
		{

		}

		void Scene::loadDefaultAssets()
		{
			SDL_Surface* nullTex = SDL_CreateRGBSurface(0, 1024, 1024, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0);
			SDL_Rect* rects = new SDL_Rect[128];
			int rectIndex = 0;

			for (int y = 0; y < 16; y++)
			{
				for (int x = (y%2); x < 16; x+=2, rectIndex++)
				{
					rects[rectIndex].x = x * 64;
					rects[rectIndex].y = y * 64;
					rects[rectIndex].w = 64;
					rects[rectIndex].h = 64;
				}
			}
			SDL_FillRects(nullTex, rects, 128 ,0xFF00FF);
			LoadMaterial(std::make_unique<VkTexture>(nullTex));
			delete[] rects;
		}

		void Scene::LoadMesh(std::unique_ptr<VkMesh>&& mesh)
		{
			mesh->m_MeshID = m_meshes.size();
			if (mesh->m_Texture)
			{
				LoadMaterial(std::make_unique<VkTexture>(mesh->m_Texture));
			}
			m_meshes.push_back(std::move(mesh));
		}

		void Scene::LoadMaterial(std::unique_ptr<VkTexture>&& material)
		{
			m_transferQueue.Textures.push_back(material.get());
			m_textures.push_back(std::move(material));
		}

		void Scene::AddDrawObject(DrawCmd& draw)
		{
			draw.ID = m_drawObjects.size();
			m_drawObjects.push_back(draw);
			try
			{
				m_descriptorManager.AllocObjectDescriptor(draw, m_textures.at(draw.MaterialID).get());
			}
			catch (std::out_of_range e)
			{
				io::Logger::LogError("Texture of id " + std::to_string(draw.MaterialID) + " not found", io::Logger::Category::Graphics);
			}
		}

		void Scene::ComputeTransforms()
		{
			std::stack<Node*> nodeStack;
			nodeStack.push(m_root.get());
			while (!nodeStack.empty())
			{
				Node* current = nodeStack.top();
				nodeStack.pop();
				if (current->GetParent())
					current->m_GlobalTransform = current->GetParent()->m_GlobalTransform * current->m_LocalTransform;
				auto& children = current->GetChildren();
				for (const auto& child : children)
				{
					nodeStack.push(child.get());
				}
			}
			writeTransforms();
		}

		void Scene::writeTransforms()
		{
			for (const auto& draw : m_drawObjects)
			{
				m_descriptorManager.UpdateUniforms(draw, m_MainCam.GetMatrix());
			}
		}

		std::vector<DrawCmd> Scene::generateDraws()
		{
			std::vector<DrawCmd> cmds = m_drawObjects;

			for (const auto& draw : m_drawObjects)
			{
				// in the future do culling stuff here maybe
				// if (!shouldCull(draw)) cmds.push_back(draw)
			}
			return cmds;
		}
	}
}