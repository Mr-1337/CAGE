#pragma once

#include <CAGE.hpp>

namespace v2
{
    class V2 : public cage::Game
    {
        bool m_running;
    public:
        V2();
        void HandleCMDArgs(int argc, char** argv) override;
        void Init() override;
        void Run() override;
    };
}