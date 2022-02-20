#pragma once

#include <IO/Assets/TextureManager.hpp>
#include <Graphics/Models/Mesh.hpp>

namespace v2
{
    class Entity
    {
    protected:
        cage::Mesh<cage::Vertex2UV> m_mesh;
        int m_startFrame;
    public:
        float w, h, x, y;
        bool Added;
        bool Dead;
        Entity(cage::TextureManager& tm, const std::string& name);
        virtual ~Entity();
        void Init(float x, float y, int startFrame);
        virtual void Update(float dt) = 0;
        void Draw();
        int GetFrame();
    };
}