#pragma once

#include <vector>

#include "Mesh.hpp"
#include "../../IO/MeshLoader.hpp"
#include "../ShaderProgram/Generic3DShader.hpp"
#include "../Animation/Animation.hpp"

namespace cage
{
	class Model
	{

	protected:
		std::vector<Mesh<Vertex3UVNormal>*> m_meshes;
		std::vector<Mesh<Vertex3UVNormal>*> m_boneMeshes;
		std::vector<animation::Animation*> m_animations;
		std::vector<animation::Bone> m_bones;

		int m_currentAnimation;
	public:

		Model() : m_TransformNode(nullptr), m_currentAnimation(0)
		{
			m_TransformNode = new scene::Node("Model");
		};

		Model(const Model& other) : m_TransformNode(nullptr)
		{
			m_TransformNode = other.m_TransformNode;	
		}

		scene::Node* m_TransformNode;

		~Model()
		{
			// delete m_transformNode;
		};

		void LoadModel(const std::string& path)
		{
			m_meshes.reserve(50);
			LoadObjMeshes(path, m_meshes);
		}

		void LoadBones(std::vector<animation::Bone> bones)
		{
			m_bones = bones;
			for (const auto& b : m_bones)
			{
				auto newMesh = new Mesh<Vertex3UVNormal>("Bone");
				newMesh->SetGeometry(LoadObjVertices("Assets/bone.obj"));
				newMesh->m_TransformNode = new scene::Node("Bone");
				newMesh->m_TransformNode->m_LocalTransform = b.m_LocalTransform;
				m_boneMeshes.push_back(newMesh);
			}

			for (int i = 1; i < m_bones.size(); i++)
			{
				m_boneMeshes[m_bones[i].m_Parent]->m_TransformNode->Add(m_boneMeshes[i]->m_TransformNode);
			}
		}

		void LoadAnimation(animation::Animation* anim)
		{
			m_animations.push_back(anim);
			//m_animation->Play();
		}

		void SetAnimation(int id)
		{
			if (id >= 0 && id < m_animations.size())
				m_currentAnimation = id;
		}

		void CycleAnimation()
		{
			m_currentAnimation++;
			m_currentAnimation %= m_animations.size();
		}

		void AddMesh(Mesh<Vertex3UVNormal>* mesh)
		{
			m_meshes.push_back(mesh);
		}

		virtual void Draw()
		{
			for (auto& m : m_meshes)
			{
				m->Draw();
			}
		}

		void Update(float dt)
		{
			if (m_animations[m_currentAnimation])
			{
				m_animations[m_currentAnimation]->Update(dt);
				m_bones = m_animations[m_currentAnimation]->GetPose();
				
				for (int i = 0; i < m_boneMeshes.size(); i++)
				{
					m_boneMeshes[i]->m_TransformNode->m_LocalTransform = m_bones[i].m_LocalTransform;
					m_boneMeshes[i]->m_TransformNode->m_InvTransform = m_bones[i].m_Offset;
				}
			}
		}

		void Play()
		{
			m_animations[m_currentAnimation]->Play();
		}

		void Stop()
		{
			m_animations[m_currentAnimation]->Stop();
		}

		virtual void Draw(std::shared_ptr<Generic3DShader> shader)
		{

			std::vector<glm::mat4> modelMats(m_bones.size());		

			modelMats[0] = m_bones[0].m_LocalTransform;

			for (int i = 1; i < m_bones.size(); i++)
			{
				int parent = m_bones[i].m_Parent;
				modelMats[i] = modelMats[parent] * m_bones[i].m_LocalTransform;
			}

			for (int i = 0; i < m_bones.size(); i++)
			{
				shader->Bones->value[i] = modelMats[i] * m_bones[i].m_Offset;
			}

			shader->Bones->ForwardToShader();
			for (auto& m : m_meshes)
			{
				m->DrawIndexed(shader);
			}
			for (auto& b : m_boneMeshes)
			{
				b->DrawIndexed(shader);
			}
		}
	};
}