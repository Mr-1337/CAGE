#pragma once

#include "Node.hpp"
#include "../Textures/VkTexture.hpp"
#include "../Models/VkMesh.hpp"
#include "../Materials/Material.hpp"
#include "../Camera/Camera.hpp"
#include "../Descriptors/DescriptorManager.hpp"

namespace cage
{
	namespace graphics
	{
		/* 
		* A Scene in CAGE is the all encompassing container for everything that you see or interact with. All game elements are children of the root node in the CAGE scene graph.
		* The scene also acts as a buffer storing all the meshes, materials, and per-object shader resources needed to render them.
		* 
		*/
		class Scene final
		{
		public:
			Scene(Device& device);
			~Scene();

			/*
			 * Loads a mesh into the scene mesh buffer, does not actually add it to the scene graph.
			 * To add a mesh to draw, add a DrawCmd with AddDrawObject()
			 */
			void LoadMesh(std::unique_ptr<VkMesh>&& mesh);

			/*
			 * Loads a material into the scene material buffer, drawn objects reference this by ID
			 * TODO : Actually implement materials proper, not just have this be an array of textures
			 */
			void LoadMaterial(std::unique_ptr<VkTexture>&& material);

			// Adds a draw object to the scene. ID of passed DrawCmd will be updated
			void AddDrawObject(DrawCmd& draw);

			Camera m_MainCam;

			inline NodeRef Root() { return m_root; }

			// Walks the scene graph and computes object transforms.
			void ComputeTransforms();
		private:
			friend class Renderer;

			struct
			{
				std::vector<VkTexture*> Textures;
			} m_transferQueue;

			// Stores the object and material descriptors
			DescriptorManager m_descriptorManager;

			std::vector<std::unique_ptr<VkMesh>> m_meshes;
			std::vector<std::unique_ptr<Material>> m_materials;
			std::vector <std::unique_ptr<VkTexture>> m_textures;
			std::vector<DrawCmd> m_drawObjects;

			// Error texture, mesh
			void loadDefaultAssets();

			// Writes transform to memory mapped shader buffers
			void writeTransforms();


			// Generates a list of draw commands to feed to the renderer. Culling is performed here.
			std::vector<DrawCmd> generateDraws();

			NodeRef m_root;
			glm::vec4 m_bgColor;
		};
	}
}