#include "Shrektris.hpp"
#include <Glad/glad/glad.h>
#include <ctime>
#include <chrono>
#include <GLM/glm/gtc/type_ptr.hpp>
#include <filesystem>

Shrektris::Shrektris(int argc, char** argv) : 
	Game("Shrektris", argc, argv),
	m_running(true), 
	m_vrMode(true)
{
	auto size = m_window->GetSize();

	std::string shrekStr = R"(

                      _____
                   ,-'     `._
                 ,'           `.        ,-.
               ,'               \       ),.\
     ,.       /                  \     /(  \;
    /'\\     ,o.        ,ooooo.   \  ,'  `-')
    )) )`. d8P"Y8.    ,8P"""""Y8.  `'  .--"'
   (`-'   `Y'  `Y8    dP       `'     /
    `----.(   __ `    ,' ,---.       (
           ),--.`.   (  ;,---.        )
          / \O_,' )   \  \O_,'        |
         ;  `-- ,'       `---'        |
         |    -'         `.           |
        _;    ,            )          :
     _.'|     `.:._   ,.::" `..       |
  --'   |   .'     """         `      |`.
        |  :;      :   :     _.       |`.`.-'--.
        |  ' .     :   :__.,'|/       |  \
        `     \--.__.-'|_|_|-/        /   )
         \     \_   `--^"__,'        ,    |
         ;  `    `--^---'          ,'     |
          \  `                    /      /
           \   `    _ _          /
            \           `       /
             \           '    ,'
              `.       ,   _,'
                `-.___.---'
	)";

	std::cout << std::endl << shrekStr << std::endl;

	m_context = m_window->GetGLContext();
	m_window->MakeContextCurrent(m_context);

	m_level = 1;
	m_score = 0;
	m_levelCounter = 0;
	levelTime = 1.5f - (m_level - 1) * 0.1f;
	totalTime = 0;

	m_rootNode = new cage::ui::UIElement();
	grid = new cage::Mesh<cage::Vertex3UVNormal>("grid");

	cage::Shader vertexShader(cage::Shader::ShaderType::VERTEX);
	cage::Shader fragShader(cage::Shader::ShaderType::FRAGMENT);
	cage::Shader spriteVS(cage::Shader::ShaderType::VERTEX);
	cage::Shader spriteFS(cage::Shader::ShaderType::FRAGMENT);

	glEnable(GL_DEPTH_TEST);
	
	glViewport(0, 0, size.first, size.second);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cage::Texture::MissingTexture = new cage::Texture(IMG_Load("Assets/missing.png"));

	glViewport(0, 0, 2560, 1440);

	vr::EVRInitError eError = vr::VRInitError_None;

	if (m_vrMode)
	{
		m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

		m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

		m_window->SetSize(m_nRenderWidth, m_nRenderHeight);

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

		m_nRenderWidth = 2560;
		m_nRenderHeight = 1440;
	}

	CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);

	shrek.LoadModel("Assets/shrek.obj");
	thanos.LoadModel("Assets/thanos.obj");

	populateGrid();

	m_font = TTF_OpenFont("Assets/sans.ttf", 32);

	std::srand(std::time(nullptr));

	vertexShader.CompileFromFile("Assets/generic3D.vert");
	fragShader.CompileFromFile("Assets/generic3D.frag");

	program = std::make_unique<cage::Generic3DShader>(vertexShader, fragShader);

	program->Use();

	program->Projection->value = glm::perspective(glm::quarter_pi<float>(), (float)m_nRenderWidth / (float)m_nRenderHeight, m_near, m_far);
	program->Projection->ForwardToShader();
	program->Model->value = glm::scale(program->Model->value, { 1.f, 0.6f, 1.f });
	program->Model->ForwardToShader();

	spriteVS.CompileFromFile("Assets/sprite.vert");
	spriteFS.CompileFromFile("Assets/sprite.frag");

	spriteProgram = std::make_shared<cage::SpriteShader>(spriteVS, spriteFS);
	spriteProgram->Use();

	spriteProgram->Projection->value = glm::ortho(0.0f, (float)m_nRenderWidth, (float)m_nRenderHeight, 0.0f);
	spriteProgram->Projection->ForwardToShader();
	cage::ui::UIElement::shader = spriteProgram;

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

	scoreText = std::make_shared<cage::ui::UIElement>();
	levelText = std::make_shared<cage::ui::UIElement>();

	scoreText->SetLocalMounting(cage::ui::MountPoint::CENTER_RIGHT);
	levelText->SetLocalMounting(cage::ui::MountPoint::CENTER_RIGHT);

	scoreText->SetParentMounting(cage::ui::MountPoint::CENTER_RIGHT);
	levelText->SetParentMounting(cage::ui::MountPoint::CENTER_RIGHT);

	scoreText->MoveTo({ -200.f, 0.f });
	levelText->MoveTo({ -200.f, 50.f });

	m_rootNode->Add(scoreText);
	m_rootNode->Add(levelText);

	m_rootNode->Resize(glm::vec2{ m_nRenderWidth, m_nRenderHeight });
	m_rootNode->MoveTo(0.5f * glm::vec2{ m_nRenderWidth, m_nRenderHeight });

	scoreText->LoadTexture(TTF_RenderText_Blended(m_font, "Score: 0 ", fontColor));
	levelText->LoadTexture(TTF_RenderText_Blended(m_font, "Level: 1", fontColor));

	m_wireframe = false;

}

bool spin = false;

void Shrektris::populateGrid()
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

void Shrektris::handleInput(float& t)
{
	// normal events

	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			m_running = false;
		}
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
				spin = !spin;
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
					t = levelTime;
			}
			else if (e.key.keysym.scancode == SDL_SCANCODE_E)
			{
				rotate(currentPiece);
			}
		}

	}

	// direct keyboard polling for camera motion

	velocity = { 0.f, 0.f, 0.f };

	auto keys = SDL_GetKeyboardState(nullptr);
	const float speed = 6;

	if (!spin)
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
		if (!spin)
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

void Shrektris::Run()
{
	auto startTime = std::chrono::system_clock::now();
	
	float t = 0;

	std::chrono::time_point<std::chrono::high_resolution_clock> currTime;
	std::chrono::duration<float> frameTime(1.0/60.0);

	while (m_running)
	{
		currTime = std::chrono::high_resolution_clock::now();		

		float ft = frameTime.count();
		t += ft;
		totalTime += ft;

		handleInput(t);
		logic(t);
		draw(ft);

		position += velocity * ft;
		frameTime = std::chrono::high_resolution_clock::now() - currTime;
		fps = 1.0f / ft;
		SDL_Delay(2);
	}

	auto duration = std::chrono::system_clock::now() - startTime;
	long long seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
	int minutes = seconds / 60;
	seconds = seconds % 60;
	std::string z = ":";
	if (seconds < 10)
		z = ":0";
	std::cout << "You survived Shrek's wrath for " << minutes << z << seconds % 60 << std::endl;
}

void Shrektris::gameOver()
{
	Mix_HaltChannel(2);
	Mix_HaltChannel(5);
	Mix_PlayChannel(4, donkey, 0);
	SDL_Delay(7000);
	m_running = false;
	return;
}

static int frameCount = 0;

void Shrektris::logic(float& t)
{
	visualizer = currentPiece;
	visualizer.y += 4;
	frameCount++;

	if (frameCount >= 100)
	{
		scoreText->LoadTexture(TTF_RenderText_Blended(m_font, std::string("FPS: ").append(std::to_string(fps)).c_str(), { 255, 0, 0, 255 }));
		frameCount = 0;
	}

	while (checkCollision(visualizer))
	{
		visualizer.y++;
	}

	while (!checkCollision(visualizer) && visualizer.y > currentPiece.y)
	{
		visualizer.y--;
	}

	if (t >= levelTime)
	{
		t = 0.f;
		m_levelCounter++;
		if (m_levelCounter > 60)
		{
			m_level++;
			levelTime -= 0.1f;
			m_levelCounter = 0;
			Mix_PlayChannel(5, levels[rand() % 4], 0);
			levelText->LoadTexture(TTF_RenderText_Blended(m_font, std::string("Level: ").append(std::to_string(m_level)).c_str(), fontColor));
		}
		//std::cout << "MOVE" << std::endl;
		if (!movePieceDown(currentPiece))
		{

			if (currentPiece.y == 0) // You lost :(
			{
				//gameOver();
			}

			std::cout << "FAILURE!!!" << std::endl;
			//SDL_Delay(500);

			//Clear any rows we should

			int breakTotal = 0;
			for (size_t i = BOARD_HEIGHT - 1; i > 0; i--)
			{

				int yesCount = 0;
				for (int j = 0; j < BOARD_WIDTH; j++)
				{
					if (board[i][j] != 0)
						yesCount++;
				}
				if (yesCount == BOARD_WIDTH)
				{
					// we cleared the row

					// copy the board into itself but 1 row lower
					memcpy(&board[1][0], board, (i * BOARD_WIDTH * sizeof(int)));
					// zero the now empty top
					memset(board, 0, (BOARD_WIDTH * sizeof(int)));
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
				//scoreText->LoadTexture(TTF_RenderText_Blended(m_font, std::string("Score: ").append(std::to_string(m_score)).c_str(), fontColor));
			}

			// make a new piece
			nextPiece.x = 5;
			nextPiece.y = 0;
			nextPiece.orientation = 0;
			currentPiece = nextPiece;
			nextPiece.shape = (std::rand() % 7) * 16;

			setBoardValOverPiece(currentPiece, 1);
		}
	}
}

void Shrektris::drawGrid()
{
	program->Model->value = glm::translate(glm::identity<glm::mat4>(), { - 5, - 4, 0.f });
	//program->Model->value = glm::identity<glm::mat4>();
	program->Model->ForwardToShader();
	grid->Draw(GL_LINES);
}

void Shrektris::drawScene(vr::EVREye eye, float t)
{
	program->Use();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.3f, 0.5f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (!m_vrMode)
	{
		if (spin)
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

	// draw the main game board
	
	for (int y = 0; y < 15; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (board[y][x] != 0)
			{
				program->Model->value = glm::translate(glm::identity<glm::mat4>(), { x - 5, 10 - y, 0.f });
				program->Model->ForwardToShader();
				shrek.Draw();
			}
		}
	}
	

	// draw the next piece
	std::array<int, 16> box;

	auto squares = getSquares(nextPiece);
	
	for (int i = 0; i < 16; i++)
	{
		box[i] = 0;

		for (int j = 0; j < 4; j++)
		{
			box[squares[j]] = 1;
		}

		if (box[i] != 0)
		{
			program->Model->value = glm::translate(glm::identity<glm::mat4>(), { 6.f + (i % 4), -((i / 4) - 5.f), 0.f });
			program->Model->ForwardToShader();
		}
		shrek.Draw();
	}
	
	// draw the visualizer

	squares = getSquares(visualizer);
	
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
			program->Model->value = glm::translate(glm::identity<glm::mat4>(), { x - 5, -(y - 10), 0.f });
			program->Model->ForwardToShader();
			thanos.Draw();
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

	spriteProgram->Use();
	glDisable(GL_DEPTH_TEST);
	m_rootNode->Draw();
}

void Shrektris::draw(float t)
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
		drawScene(vr::Eye_Left, t);
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
		drawScene(vr::Eye_Right, t);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_MULTISAMPLE);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId);

		glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		//drawScene(vr::Eye_Right, t);

		vr::Texture_t leftEyeTexture = { (void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		vr::Texture_t rightEyeTexture = { (void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

		m_window->SwapBuffers();
		vr::VRCompositor()->PostPresentHandoff();
	}
	else
	{
		/*
		m_win2.SetPosition(1000 + 200 * sin(totalTime), 1000 + 200 * cos(totalTime));
		m_win2.SetOpacity(0.5 * (cos(totalTime) + 1.0));
		m_win3.SetSize(400 + 200 * sin(totalTime), 400 + 200 * cos(totalTime));

		
		glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
		glViewport(0, 0, 2560, 1440);
		drawScene(vr::Eye_Left, t);

		auto size = m_window->GetSize();
		auto pos = m_window->GetPosition();
		
		const int WIDTH = 2560;
		const int HEIGHT = 1440;

		auto interp = GL_LINEAR;

		m_window->MakeContextCurrent(m_context);
		glViewport(0, 0, size.first, size.second);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(pos.first, HEIGHT - (pos.second + size.second), pos.first + size.first, HEIGHT-pos.second, 0, 0, size.first, size.second, GL_COLOR_BUFFER_BIT, interp);
		m_window->SwapBuffers();

		size = m_win2.GetSize();
		pos = m_win2.GetPosition();

		m_win2.MakeContextCurrent(m_context);
		glViewport(0, 0, size.first, size.second);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(pos.first, HEIGHT - (pos.second + size.second), pos.first + size.first, HEIGHT - pos.second, 0, 0, size.first, size.second, GL_COLOR_BUFFER_BIT, interp);
		m_win2.SwapBuffers();

		size = m_win3.GetSize();
		pos = m_win3.GetPosition();

		m_win3.MakeContextCurrent(m_context);
		glViewport(0, 0, size.first, size.second);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(pos.first, HEIGHT - (pos.second + size.second), pos.first + size.first, HEIGHT - pos.second, 0, 0, size.first, size.second, GL_COLOR_BUFFER_BIT, interp);
		m_win3.SwapBuffers();
		*/

		drawScene(vr::Eye_Left, t);
		m_window->SwapBuffers();

	}

}