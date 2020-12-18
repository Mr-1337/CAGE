#include "ShrekMenu.hpp"
#include "Gameplay.hpp"
#include "Lobby.hpp"
#include "../CAGE/Core/StateMachine.hpp"
#include "../CAGE/Core/Game.hpp"

Gameplay::Gameplay(cage::Game& game, int w, int h) : 
	cage::GameState(game),
	BOARD_WIDTH(w),
	BOARD_HEIGHT(h),
	m_vrMode(false),
	m_spin(false),
	skybox(std::filesystem::current_path().append("Assets/skybox"))
{

	m_level = 1;
	m_score = 0;
	m_levelCounter = 0;
	levelTime = 1.5f - (m_level - 1) * 0.1f;
	totalTime = 0;
	m_frameCount = 0;
	m_fallTimer = 0.0;

	board = new char[BOARD_HEIGHT * BOARD_WIDTH];
	for (int i = 0; i < BOARD_HEIGHT * BOARD_WIDTH; i++)
		board[i] = 0;

	grid = new cage::Mesh<cage::Vertex3UVNormal>("grid");

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto size = getGame().GetWindow().GetSize();
	glViewport(0, 0, size.first, size.second);

#pragma region VR
	vr::EVRInitError eError = vr::VRInitError_None;
	if (m_vrMode)
	{
		m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

		m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

		getGame().GetWindow().SetSize(m_nRenderWidth, m_nRenderHeight);

		assert(eError == vr::VRInitError_None);

		std::cout << std::filesystem::current_path();

		auto error = vr::VRInput()->SetActionManifestPath((std::filesystem::current_path().string() + "\\Assets\\action_manifest.json").c_str());

		error = vr::VRInput()->GetActionHandle("/actions/main/in/Move", &m_move);

		error = vr::VRInput()->GetActionHandle("/actions/main/in/MoveBlockLeft", &m_moveBlockLeftAct);
		error = vr::VRInput()->GetActionHandle("/actions/main/in/MoveBlockRight", &m_moveBlockRightAct);
		error = vr::VRInput()->GetActionHandle("/actions/main/in/MoveBlockDown", &m_moveBlockDownAct);

		error = vr::VRInput()->GetActionHandle("/actions/main/in/Rotate", &m_rotateAct);

		error = vr::VRInput()->GetActionHandle("/actions/main/in/LeftHand", &m_leftPoseAct);

		error = vr::VRInput()->GetActionHandle("/actions/main/in/RightHand", &m_rightPoseAct);




		error = vr::VRInput()->GetInputSourceHandle("/user/hand/left", &m_leftHand);
		assert(error == vr::EVRInputError::VRInputError_None);

		error = vr::VRInput()->GetActionSetHandle("/actions/main", &m_main);
		assert(error == vr::EVRInputError::VRInputError_None);

		CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);
		CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, rightEyeDesc);

		projLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
		eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
		projRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
		eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
	}
	else
	{
		m_nRenderWidth = size.first;
		m_nRenderHeight = size.second;
	}
#pragma endregion
	
	m_spriteProgram = cage::ui::UIElement::shader;

	cage::Shader vertexShader(cage::Shader::ShaderType::VERTEX);
	cage::Shader fragShader(cage::Shader::ShaderType::FRAGMENT);

	vertexShader.CompileFromFile("Assets/generic3D.vert");
	fragShader.CompileFromFile("Assets/generic3D.frag");

	program = std::make_unique<cage::Generic3DShader>(vertexShader, fragShader);
	program->Use();
	program->Projection->value = glm::perspective(glm::quarter_pi<float>(), (float)m_nRenderWidth / (float)m_nRenderHeight, m_near, m_far);
	program->Projection->ForwardToShader();
	program->Tint->value = { 0.0, 0.0, 1.0, 0.9 };
	program->Tint->ForwardToShader();

	skyProgram = std::make_unique<cage::SkyboxShader>();

	skyProgram->Use();

	skyProgram->Projection->value = program->Projection->value;
	skyProgram->Projection->ForwardToShader();

	m_spriteProgram->Use();

	m_spriteProgram->Projection->value = glm::ortho(0.0f, (float)m_nRenderWidth, (float)m_nRenderHeight, 0.0f);
	m_spriteProgram->Projection->ForwardToShader();
	

	CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);

	shrek.LoadModel("Assets/shrek.obj");

	populateGrid();

	m_font = new cage::Font("Assets/sans.ttf", 32);

	std::srand(std::time(nullptr));

	music = Mix_LoadWAV("Assets/music.mp3");

	Mix_VolumeChunk(music, 10);

	layers1 = Mix_LoadWAV("Assets/layers1.ogg");
	layers2 = Mix_LoadWAV("Assets/layers2.ogg");
	layers3 = Mix_LoadWAV("Assets/layers3.ogg");
	bigLayers = Mix_LoadWAV("Assets/biglayers.ogg");
	donkey = Mix_LoadWAV("Assets/donkey.ogg");

	levels[0] = Mix_LoadWAV("Assets/level1.ogg");
	levels[1] = Mix_LoadWAV("Assets/level2.ogg");
	levels[2] = Mix_LoadWAV("Assets/level3.ogg");
	levels[3] = Mix_LoadWAV("Assets/level4.ogg");

	Mix_PlayChannel(2, music, -1);

	currentPiece.shape = 0;
	currentPiece.y = 1;
	currentPiece.x = 5;

	scoreText = std::make_shared<cage::ui::Text>(*m_font);
	scoreText->SetColor(fontColor);
	levelText = std::make_shared<cage::ui::Text>(*m_font);
	levelText->SetColor(fontColor);

	m_rightPanel = std::make_shared<cage::ui::LayoutGroup>(new cage::ui::FlowLayout(glm::vec2{ 30.f, 50.f }, cage::ui::FlowLayout::Orientation::VERTICAL, false));

	m_rightPanel->Add(scoreText);
	m_rightPanel->Add(levelText);
	m_rightPanel->SetColor(glm::vec4{ 0x78, 0x51, 0xa9, 255 } /= 255.f);
	m_rightPanel->SetParentMounting(cage::ui::MountPoint::CENTER_RIGHT);
	m_rightPanel->SetLocalMounting(cage::ui::MountPoint::CENTER_RIGHT);

	m_rootNode.Resize(glm::vec2{ m_nRenderWidth, m_nRenderHeight });
	m_rootNode.SetLocalMounting(cage::ui::MountPoint::TOP_LEFT);

	m_rootNode.Add(m_rightPanel);

	scoreText->SetText("Score: 0 ");
	levelText->SetText("Level: 1");
	m_rightPanel->Compress();
	m_rightPanel->Resize({ m_rightPanel->GetSize().x, m_nRenderHeight });

	m_wireframe = false;

}

void Gameplay::populateGrid()
{
	auto buffer = grid->GetBuffer();

	std::vector<cage::Vertex3UVNormal> verts;

	auto addVert = [&verts](float x, float y)
	{
		verts.emplace_back(glm::vec3{ x, y, 0 }, glm::vec2{ x, y }, glm::vec3{ x, y, 0 });
	};

	for (int i = 0; i <= BOARD_WIDTH; i++)
	{
		addVert(i - 0.5, 0);
		addVert(i - 0.5, BOARD_HEIGHT);
	}

	for (int i = 0; i <= BOARD_HEIGHT; i++)
	{
		addVert(-0.5, i);
		addVert(BOARD_WIDTH - 0.5, i);
	}

	grid->SetGeometry(verts);
}

void Gameplay::ProcessEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			quit();
		if (e.type == SDL_KEYDOWN)
		{
			if (e.key.keysym.scancode == SDL_SCANCODE_TAB && e.key.repeat == false)
				if (SDL_GetRelativeMouseMode() == SDL_TRUE)
					SDL_SetRelativeMouseMode(SDL_FALSE);
				else
					SDL_SetRelativeMouseMode(SDL_TRUE);
			else if (e.key.keysym.scancode == SDL_SCANCODE_F && e.key.repeat == false)
			{
				m_wireframe = !m_wireframe;
				glPolygonMode(GL_FRONT_AND_BACK, m_wireframe ? GL_LINE : GL_FILL);
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_P && e.key.repeat == false)
			{
				m_spin = !m_spin;
				position = glm::vec3(0, 2, 20), target = glm::vec3(0, 0, -1);
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_B && e.key.repeat == false)
			{
				m_inputHack = !m_inputHack;
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_G && e.key.repeat == false)
			{
				memset(board, 0, sizeof(board));
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_A)
			{
				movePieceLR(currentPiece, -1);
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_D)
			{
				movePieceLR(currentPiece, 1);
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_S)
			{
				if (!movePieceDown(currentPiece))
					m_fallTimer = levelTime;
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_E)
			{
				rotate(currentPiece);
			}
		}
		if (e.type == SDL_WINDOWEVENT)
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				m_nRenderWidth = e.window.data1;
				m_nRenderHeight = e.window.data2;
				glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
				program->Use();
				program->Projection->value = glm::perspective(glm::quarter_pi<float>(), (float)m_nRenderWidth / (float)m_nRenderHeight, m_near, m_far);
				program->Projection->ForwardToShader();

				skyProgram->Use();

				skyProgram->Projection->value = program->Projection->value;
				skyProgram->Projection->ForwardToShader();

				m_spriteProgram->Use();

				m_spriteProgram->Projection->value = glm::ortho(0.0f, (float)m_nRenderWidth, (float)m_nRenderHeight, 0.0f);
				m_spriteProgram->Projection->ForwardToShader();

				m_rootNode.Resize({ m_nRenderWidth, m_nRenderHeight });
				m_rightPanel->Resize({ m_rightPanel->GetSize().x, m_nRenderHeight });
			}
		}
	}
}

void Gameplay::gameOver()
{
	Mix_HaltChannel(2);
	Mix_HaltChannel(5);
	Mix_PlayChannel(4, donkey, 0);
	SDL_Delay(7000);
	quit();
}

void Gameplay::logic()
{
	visualizer = currentPiece;
	m_frameCount++;

	if (m_frameCount >= 100)
	{
		//scoreText->SetText(std::string("FPS: ").append(std::to_string(fps)));
		m_frameCount = 0;
	}

	while (checkCollision(visualizer))
	{
		visualizer.y++;
	}
	visualizer.y--;

	if (m_fallTimer >= levelTime)
	{
		m_fallTimer = 0.f;
		m_levelCounter++;
		if (m_levelCounter > 60)
		{
			m_level++;
			levelTime -= 0.1f;
			m_levelCounter = 0;
			Mix_PlayChannel(5, levels[rand() % 4], 0);
			levelText->SetText(std::string("Level: ").append(std::to_string(m_level)));
		}
		//std::cout << "MOVE" << std::endl;
		if (!movePieceDown(currentPiece))
		{

			setBoardValOverPiece(currentPiece, 1);

			if (currentPiece.y == 0) // You lost :(
			{
				//gameOver();
			}

			//Clear any rows we should

			int breakTotal = 0;
			for (size_t i = BOARD_HEIGHT - 1; i > 0; i--)
			{

				int yesCount = 0;
				for (int j = 0; j < BOARD_WIDTH; j++)
				{
					if (board[i * BOARD_WIDTH + j] != 0)
						yesCount++;
				}
				if (yesCount == BOARD_WIDTH)
				{
					// we cleared the row

					// copy the board into itself but 1 row lower
					memcpy(&board[BOARD_WIDTH + 0], board, (i * BOARD_WIDTH * sizeof(char)));
					// zero the now empty top
					memset(board, 0, (BOARD_WIDTH * sizeof(char)));
					i++;
					breakTotal++;
				}

			}

			if (breakTotal == 1)
			{
				m_score += 40 * m_level;
				Mix_PlayChannel(3, layers1, 0);
			}
			else if (breakTotal == 2)
			{
				m_score += 100 * m_level;
				Mix_PlayChannel(3, layers2, 0);
			}
			else if (breakTotal == 3)
			{
				m_score += 300 * m_level;
				Mix_PlayChannel(3, layers3, 0);
			}
			else if (breakTotal == 4)
			{
				m_score += 1200 * m_level;
				Mix_PlayChannel(3, bigLayers, 0);
			}

			if (breakTotal > 0)
			{
				scoreText->SetText(std::string("Score: ").append(std::to_string(m_score)));
			}

			// make a new piece
			nextPiece.x = BOARD_WIDTH / 2;
			nextPiece.y = 0;
			nextPiece.orientation = 0;
			currentPiece = nextPiece;
			nextPiece.shape = (std::rand() % 7) * 16;

			//setBoardValOverPiece(currentPiece, 1);
		}
	}
}

void Gameplay::Update(float delta)
{
	logic();

	totalTime += delta;
	m_fallTimer += delta;
	fps = 1.0f / delta;

	// direct keyboard polling for camera motion

	velocity = { 0.f, 0.f, 0.f };

	auto keys = SDL_GetKeyboardState(nullptr);
	const float speed = 6;

	if (!m_spin)
	{

		if (keys[SDL_SCANCODE_J])
		{
			velocity.x = -speed;
		}

		if (keys[SDL_SCANCODE_K])
		{
			velocity.z = speed;
		}

		if (keys[SDL_SCANCODE_L])
		{
			velocity.x = speed;
		}

		if (keys[SDL_SCANCODE_I])
		{
			velocity.z = -speed;
		}

		if (keys[SDL_SCANCODE_U])
		{
			velocity.y = speed;
		}

		if (keys[SDL_SCANCODE_O])
		{
			velocity.y = -speed;
		}
	}

	position += velocity * delta;

	// vr input and events

	if (m_vrMode)
	{

		// vr events
		vr::VREvent_t event;
		while (m_pHMD->PollNextEvent(&event, sizeof(event)))
		{
			switch (event.eventType)
			{
			case vr::VREvent_TrackedDeviceDeactivated:
			{
				std::cout << "Device %u detached.\n" << event.trackedDeviceIndex;
			}
			break;
			case vr::VREvent_TrackedDeviceUpdated:
			{
				std::cout << "Device %u updated.\n" << event.trackedDeviceIndex;
			}
			break;
			}
		}

		// vr action based input
		if (!m_spin)
		{
			// action set updating
			vr::VRActiveActionSet_t activeSet = { 0 };
			activeSet.ulActionSet = m_main;

			vr::VRInput()->UpdateActionState(&activeSet, sizeof(activeSet), 1);

			// getting pose data

			vr::InputPoseActionData_t poseData;

			auto error = vr::EVRInputError::VRInputError_None;

			error = vr::VRInput()->GetPoseActionDataForNextFrame(m_leftPoseAct, vr::ETrackingUniverseOrigin::TrackingUniverseSeated, &poseData, sizeof(poseData), vr::k_ulInvalidInputValueHandle);
			//assert(error == vr::EVRInputError::VRInputError_None);
			if (poseData.pose.bPoseIsValid)
				m_leftPose = poseData.pose;
			error = vr::VRInput()->GetPoseActionDataForNextFrame(m_rightPoseAct, vr::ETrackingUniverseOrigin::TrackingUniverseSeated, &poseData, sizeof(poseData), vr::k_ulInvalidInputValueHandle);
			//assert(error == vr::EVRInputError::VRInputError_None);

			if (poseData.pose.bPoseIsValid)
				m_rightPose = poseData.pose;


			// data structs to store input into
			vr::InputAnalogActionData_t aData = { 0 };
			vr::InputDigitalActionData_t dData;

			// joystick motion

			error = vr::VRInput()->GetAnalogActionData(m_move, &aData, sizeof(aData), vr::k_ulInvalidInputValueHandle);
			//assert(error == vr::EVRInputError::VRInputError_None);

			velocity.x += aData.x;
			velocity.z -= aData.y;

			// buttons

			error = vr::VRInput()->GetDigitalActionData(m_rotateAct, &dData, sizeof(dData), vr::k_ulInvalidInputValueHandle);
			if (error == vr::EVRInputError::VRInputError_None && (dData.bState && dData.bChanged))
			{
				rotate(currentPiece);
			}

			error = vr::VRInput()->GetDigitalActionData(m_moveBlockLeftAct, &dData, sizeof(dData), vr::k_ulInvalidInputValueHandle);
			if (dData.bState && dData.bChanged)
				movePieceLR(currentPiece, -1);

			error = vr::VRInput()->GetDigitalActionData(m_moveBlockRightAct, &dData, sizeof(dData), vr::k_ulInvalidInputValueHandle);
			if (dData.bState && dData.bChanged)
				movePieceLR(currentPiece, 1);

			error = vr::VRInput()->GetDigitalActionData(m_moveBlockDownAct, &dData, sizeof(dData), vr::k_ulInvalidInputValueHandle);
			if (dData.bState && dData.bChanged)
				movePieceDown(currentPiece);

			velocity = velocity * speed;

			velocity = glm::vec4(velocity, 0.0) * hmdPose;
			velocity.y = 0;
		}
	}
}

void Gameplay::drawGrid()
{
	program->Model->value = glm::translate(glm::identity<glm::mat4>(), { -BOARD_WIDTH / 2, -BOARD_HEIGHT / 2 + 1, 0.f });
	//program->Model->value = glm::identity<glm::mat4>();
	program->Model->ForwardToShader();
	grid->Draw(GL_LINES);
}

void Gameplay::drawScene(vr::EVREye eye)
{
	program->Use();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.5f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!m_vrMode)
	{
		if (m_spin)
			program->View->value = glm::lookAt(glm::vec3(0.f, 2.f, 20.f), { 0.f, 2.f, -1.f }, { 0.f, 1.f, 0.f }) * glm::rotate(glm::identity<glm::mat4>(), 0.5f * 6.28f * totalTime, glm::vec3(0.f, 1.f, 0.f));
		else
			program->View->value = glm::lookAt(position, position + target, { 0.f, 1.f, 0.f });
		program->View->ForwardToShader();
	}
	else
	{
		program->View->value = glm::identity<glm::mat4>();
		program->View->ForwardToShader();

		program->Projection->value = GetCurrentViewProjection(eye);
		program->Projection->ForwardToShader();
	}

	glDepthMask(GL_FALSE);
	skyProgram->Use();
	if (!m_vrMode)
		skyProgram->View->value = program->View->value;
	else
	{
		skyProgram->View->value = GetCurrentView(eye);
		skyProgram->Projection->value = GetCurrentProjection(eye);
		skyProgram->Projection->ForwardToShader();
	}

	skyProgram->View->ForwardToShader();
	skybox.Draw();
	glDepthMask(GL_TRUE);

	program->Use();
	program->Tint->value = { 0.0, 0.0, 0.0, 0.0 };
	program->Tint->ForwardToShader();

	// draw the current piece

	std::array<int, 16> box;
	auto squares = getSquares(currentPiece);

	for (int i = 0; i < 16; i++)
	{
		box[i] = 0;

		for (int j = 0; j < 4; j++)
		{
			box[squares[j]] = 1;
		}

		if (box[i] != 0)
		{
			int x = currentPiece.x + (i % 4);
			int y = currentPiece.y + (i / 4);
			program->Model->value = glm::translate(glm::identity<glm::mat4>(), { x - BOARD_WIDTH / 2, -(y - BOARD_HEIGHT / 2), 0.f });
			program->Model->ForwardToShader();
			shrek.Draw();
		}
	}

	// draw the main game board

	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			if (board[y * BOARD_WIDTH + x] != 0)
			{
				program->Model->value = glm::translate(glm::identity<glm::mat4>(), { x - BOARD_WIDTH / 2, BOARD_HEIGHT / 2 - y, 0.f });
				program->Model->ForwardToShader();
				shrek.Draw();
			}
		}
	}


	// draw the next piece
	squares = getSquares(nextPiece);

	for (int i = 0; i < 16; i++)
	{
		box[i] = 0;

		for (int j = 0; j < 4; j++)
		{
			box[squares[j]] = 1;
		}

		if (box[i] != 0)
		{
			program->Model->value = glm::translate(glm::identity<glm::mat4>(), { BOARD_WIDTH / 2 + 1 + (i % 4), -((i / 4) - 5.f), 0.f });
			program->Model->ForwardToShader();
		}
		shrek.Draw();
	}

	// draw the visualizer

	squares = getSquares(visualizer);

	program->Tint->value = { 0.0, 0.0, 0.0, 0.6 };
	program->Tint->ForwardToShader();

	for (int i = 0; i < 16; i++)
	{
		box[i] = 0;

		for (int j = 0; j < 4; j++)
		{
			box[squares[j]] = 1;
		}

		if (box[i] != 0)
		{
			int x = visualizer.x + (i % 4);
			int y = visualizer.y + (i / 4);
			program->Model->value = glm::translate(glm::identity<glm::mat4>(), { x - BOARD_WIDTH / 2, -(y - BOARD_HEIGHT / 2), 0.f });
			program->Model->ForwardToShader();
			shrek.Draw();
		}
	}

	// draw my hands

	if (m_vrMode)
	{
		auto identity = glm::identity<glm::mat4>();
		program->Model->value = glm::translate(identity, position) * (ConvertSteamVRMatrixToMatrix4(m_leftPose.mDeviceToAbsoluteTracking)) * glm::scale(identity, { 0.25, 0.25, 0.25 }) * glm::rotate(identity, -glm::half_pi<float>(), { 1.0, 0.0, 0.0 });
		program->Model->ForwardToShader();
		shrek.Draw();

		program->Model->value = glm::translate(identity, position) * (ConvertSteamVRMatrixToMatrix4(m_rightPose.mDeviceToAbsoluteTracking)) * glm::scale(identity, { 0.25, 0.25, 0.25 }) * glm::rotate(identity, -glm::half_pi<float>(), { 1.0, 0.0, 0.0 });
		program->Model->ForwardToShader();
		shrek.Draw();
	}

	// draw the grid
	drawGrid();

	m_spriteProgram->Use();
	glDisable(GL_DEPTH_TEST);
	m_rootNode.Draw();
}

void Gameplay::Draw()
{
	if (m_vrMode)
	{
		if (vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0) != vr::EVRCompositorError::VRCompositorError_None)
		{
			std::cout << "THROTTLING\n";
		}

		if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
		{
			auto steamMat = m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;

			hmdPose = ConvertSteamVRMatrixToMatrix4(steamMat);
			hmdPose[3][0] += position.x;
			hmdPose[3][1] += position.y;
			hmdPose[3][2] += position.z;

			hmdPose = glm::inverse(hmdPose);
		}

		glEnable(GL_MULTISAMPLE);

		// Left Eye
		glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
		glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
		drawScene(vr::Eye_Left);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_MULTISAMPLE);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId);

		glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glEnable(GL_MULTISAMPLE);

		// Right Eye

		glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
		glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
		drawScene(vr::Eye_Right);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_MULTISAMPLE);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId);

		glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		drawScene(vr::Eye_Right);

		vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

		vr::VRCompositor()->PostPresentHandoff();
	}
	else
	{
		drawScene(vr::Eye_Left);
	}
}


void Gameplay::OnRevealed()
{

}

void Gameplay::OnHidden()
{
	Mix_HaltChannel(2);
	Mix_HaltChannel(5);
}

Gameplay::~Gameplay()
{
	Mix_HaltChannel(2);
	Mix_HaltChannel(5);
	delete[] board;
}