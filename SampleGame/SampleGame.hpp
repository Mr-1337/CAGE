#pragma once

#include <Core/Game.hpp>

class SampleGame : public cage::Game, public cage::EventListener
{
	cage::graphics::NodeRef pivot, saulNode, node3;
public:
	SampleGame();
	~SampleGame();

	bool HandleEvent(cage::Event& e) override;

	void Init() override;
	void Run() override;
	void Update(float frameTime) override;
};