#include "SampleGame.hpp"

#include <IO/VkMeshLoader.hpp>
#include <SDL2/SDL_image.h>
#include <GLM/glm/gtx/string_cast.hpp>
#include <Graphics/Models/MeshFactory.hpp>

SampleGame::SampleGame() : cage::Game("Sample Game")
{

}

SampleGame::~SampleGame()
{

}

void SampleGame::Init()
{
	using cage::io::InputManager;
	using namespace cage::graphics;
	InputManager::InputSrc space;
	space.m_Type = InputManager::InputSrc::Type::Key;
	space.m_ButtonIDs[0] = SDL_SCANCODE_SPACE;

	InputManager::InputSrc movement;
	movement.m_Type = InputManager::InputSrc::Type::KeyAxes;
	movement.m_ButtonIDs = { SDL_SCANCODE_D, SDL_SCANCODE_A, SDL_SCANCODE_SPACE, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_S, SDL_SCANCODE_W };

	InputManager::InputSrc controller;
	controller.m_Type = InputManager::InputSrc::Type::ControllerStickXZ;
	controller.m_ButtonIDs = { SDL_CONTROLLER_AXIS_LEFTX };

	m_input->RegisterControl("Jump", space);
	m_input->RegisterControl("Movement", movement);
	m_input->RegisterControl("Controller", controller);

	using namespace cage::io;
	std::unique_ptr<VkMesh> saul(LoadMesh("Assets/Models/saul_goodman.glb", *m_graphicsContext->m_device));
	m_scene->LoadMesh(std::move(saul));
	m_scene->LoadMesh(std::unique_ptr<VkMesh>(MeshFactory::BuildCube(*m_graphicsContext->m_device, 0.4f)));
	m_scene->LoadMesh(std::unique_ptr<VkMesh>(MeshFactory::BuildSphere(*m_graphicsContext->m_device, 0.3f)));
	m_scene->LoadMesh(std::unique_ptr<VkMesh>(LoadMesh("Assets/Models/protomap.glb", *m_graphicsContext->m_device)));
	m_scene->LoadMesh(std::unique_ptr<VkMesh>(MeshFactory::BuildSphere(*m_graphicsContext->m_device, 0.06f)));
	m_scene->LoadMesh(std::unique_ptr<VkMesh>(LoadMesh("Assets/Models/sus2.glb", *m_graphicsContext->m_device)));
	m_scene->LoadMaterial(std::make_unique<VkTexture>(IMG_Load("Assets/Textures/cage.png")));
	m_scene->LoadMaterial(std::make_unique<VkTexture>(IMG_Load("Assets/Textures/saul.png")));
	m_scene->LoadMaterial(std::make_unique<VkTexture>(IMG_Load("Assets/Textures/finger.bmp")));
	auto& root = *m_scene->Root();
	saulNode = Node::Make("Saul");
	pivot = Node::Make("Pivot");
	auto node2 = Node::Make("SaulBox");
	node3 = Node::Make("SaulBall");
	auto node4 = Node::Make("Finger");
	node4->m_LocalTransform = glm::translate(glm::identity<glm::mat4>(), { 0.7f, 0.0f, 0.0f });
	node2->m_LocalTransform = glm::translate(glm::identity<glm::mat4>(), { -2.0f, 0.0f, 0.0f });
	node3->m_LocalTransform = glm::translate(glm::identity<glm::mat4>(), { 2.0f, 0.0f, 0.0f });
	root.Add(saulNode);
	saulNode->Add(pivot);
	pivot->Add(node2);
	pivot->Add(node3);
	node3->Add(node4);
	DrawCmd draw{}, draw2{}, draw3{}, draw4{}, draw5{};
	draw.MeshID = 0;
	draw.SceneNode = saulNode.get();
	draw.MaterialID = 1;
	draw2.MeshID = 1;
	draw2.SceneNode = node2.get();
	draw2.MaterialID = 2;
	draw3.MeshID = 2;
	draw3.SceneNode = node3.get();
	draw3.MaterialID = 3;
	draw4.MeshID = 3;
	draw4.MaterialID = 0;
	draw4.SceneNode = &root;
	draw5.MeshID = 5;
	draw5.MaterialID = 4;
	draw5.SceneNode = node4.get();
	m_scene->AddDrawObject(draw);
	m_scene->AddDrawObject(draw2);
	m_scene->AddDrawObject(draw3);
	m_scene->AddDrawObject(draw4);
	m_scene->AddDrawObject(draw5);

	using namespace cage::io;
	InputManager::InputSrc wasd{ .m_Type = InputManager::InputSrc::Type::KeyAxes, .m_ButtonIDs =
	{
		SDL_SCANCODE_D, SDL_SCANCODE_A, SDL_SCANCODE_SPACE, SDL_SCANCODE_LCTRL, SDL_SCANCODE_S, SDL_SCANCODE_W
	} };
	InputManager::InputSrc arrows{ .m_Type = InputManager::InputSrc::Type::KeyAxes, .m_ButtonIDs =
	{
		SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, -1, -1, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN
	} };
	InputManager::InputSrc look{ .m_Type = InputManager::InputSrc::Type::MouseDelta };
	m_input->RegisterControl("Directionals", wasd);
	m_input->RegisterControl("Saul", arrows);
	m_input->RegisterControl("Look", look);
	m_input->Subscribe(this);
	
}

bool SampleGame::HandleEvent(cage::Event& e)
{
	if (auto key = std::get_if<cage::KeyDownEvent>(&e))
	{
		if (key->keyCode == SDLK_g)
		{
			SDL_SetRelativeMouseMode(static_cast<SDL_bool>(!SDL_GetRelativeMouseMode()));
		}
	}
	return true;
}

void SampleGame::Update(float frameTime)
{
	static float totalTime = 0;
	const float moveSpeed = 4.f;
	totalTime += frameTime;
	auto moveMat = glm::inverse(m_scene->m_MainCam.GetViewMatrix());
	moveMat[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
	auto moveVec = moveMat * glm::vec4{ (*m_input)["Directionals"], 0.0f };
	moveVec *= moveSpeed;
	m_scene->m_MainCam.Move(moveVec * frameTime);
	const float sens = 0.25f;
	glm::vec2 look = -(*m_input)["Look"] * sens;
	m_scene->m_MainCam.Rotate(look.x, look.y, 0.0f);
	pivot->m_LocalTransform = glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3{ 0.0f, 1.0f, 0.0f }), totalTime * 0.0f, { 0.f, 1.f, 0.f });
	saulNode->m_LocalTransform[3] += glm::vec4{ (*m_input)["Saul"] * frameTime * 3.0f, 0.0f };
}

void SampleGame::Run()
{
	SDL_Delay(5000);
}