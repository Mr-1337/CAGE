#include "V2.hpp"
#include "Menu.hpp"

namespace v2
{

    V2::V2() : cage::Game("V2")
    {
        m_running = true;
    }

    void V2::HandleCMDArgs(int argc, char** argv)
    {

    }

    void V2::Init()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_stateMachine.Push(new Menu(*this));
    }

    void V2::Run()
    {
        using namespace std::chrono;
        using Clock = high_resolution_clock;
        duration<float> frameTime(1.0f / 60.f);
        time_point<Clock> startTime;
        while (m_running)
        {
            startTime = Clock::now();
            m_stateMachine.ProcessEvents();
            m_stateMachine.Update(frameTime.count());
            m_stateMachine.Draw();
            m_running = !m_stateMachine.Quit();
            m_window->SwapBuffers();
            frameTime = Clock::now() - startTime;
        }
    }
}