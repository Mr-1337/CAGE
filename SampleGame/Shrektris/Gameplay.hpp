#pragma once

#include <utility>
#include <array>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include <cstring>
#include <OpenVR/openvr.h>
#include <SDL2/SDL_mixer.h>

#include "../CAGE/Core/GameState.hpp"
#include "../MenuButton.hpp"
#include "../CAGE/Graphics/UI/UIElement.hpp"
#include "../CAGE/Graphics/UI/FlowLayout.hpp"
#include "../CAGE/Graphics/UI/LayoutGroup.hpp"
#include "../CAGE/Graphics/UI/Text.hpp"
#include "../CAGE/Graphics/ShaderProgram/Generic3DShader.hpp"
#include "../CAGE/Graphics/ShaderProgram/SpriteShader.hpp"
#include "../CAGE/Graphics/ShaderProgram/SkyboxShader.hpp"
#include "../CAGE/IO/MeshLoader.hpp"
#include "../CAGE/Graphics/Models/Skybox.hpp"
#include "../CAGE/Graphics/Models/Model.hpp"
#include "../CAGE/IO/Networking/Packets.hpp"

#include "Client.hpp"
#include "Server.hpp"

namespace cage
{
	class Window;
}

class Gameplay : public cage::GameState
{
	using Ref = std::shared_ptr<cage::ui::UIElement>;
public:

	enum Mode
	{
		OFFLINE,
		CLIENT,
		SERVER
	};

	Gameplay(cage::Game& game, int w, int h, Mode mode = OFFLINE);

	Gameplay(cage::Game& game, cage::networking::packets::GameStart startPacket, std::unique_ptr<Client> client);
	Gameplay(cage::Game& game, cage::networking::packets::GameStart startPacket, std::unique_ptr<Server> client);

	~Gameplay();

	void OnRevealed() override;
	void OnHidden() override;

	void ProcessEvents() override;
	void Update(float delta) override;
	void Draw() override;

private:

	friend class Client;
	friend class Server;

	std::unique_ptr<Client> m_client;
	std::unique_ptr<Server> m_server;

	//cage::Window &m_w2, &m_w3;
	cage::Window &m_window;

	SDL_GLContext m_context;

	void boardSync(cage::networking::packets::BoardSync& sync);
	void playerSync(cage::networking::packets::PlayerPiecePos& sync);
	void compress(char* compressed);

	int m_numPlayers;
	int m_playerID;
	const int BOARD_WIDTH;
	const int BOARD_HEIGHT;

	Mode m_mode;
	bool m_vrMode;
	float m_near = 0.1, m_far = 100.f;
	float fps;
	uint32_t m_nRenderWidth, m_nRenderHeight;

	char* board;

	struct Tetromino
	{
		int x = 0, y = 0;
		int shape = 0;
		int orientation = 0;
	};

	/*
	 *	The master shapes array, each row represents the 4 orientations of a given shape
	 *	The numbers represent the indices of a 16 element array that a shape occupies as shown by the grid below
	 *	All tetrominos have only 4 cells, so each piece can be represented with only 4 indices
	 */
	 /*
		 0 1 2 3
		 4 5 6 7
		 8 9 A B
		 C D E F
	 */

	const std::array<int, 4 * 4 * 7> shapes =
	{
		0, 1, 4, 5,    0, 1, 4, 5,    0, 1, 4, 5,   0, 1, 4, 5,   //square
		0, 1, 5, 6,    8, 4, 5, 1,    0, 1, 5, 6,   8, 4, 5, 1,   //z
		4, 5, 1, 2,    0, 4, 5, 9,    4, 5, 1, 2,   0, 4, 5, 9,   //s
		0, 4, 8, 9,    4, 5, 6, 2,    0, 1, 5, 9,   0, 4, 1, 2,   //L
		8, 1, 5, 9,    0, 4, 5, 6,    8, 4, 0, 1,   0, 1, 2, 6,   //backward L
		0, 4, 8, 12,   0, 1, 2, 3,    0, 4, 8, 12,  0, 1, 2, 3,   //line
		0, 1, 2, 5,    0, 4, 8, 5,    1, 4, 5, 6,   4, 1, 5, 9	  //T
	};

	// returns the list of indices for a given tetromino
	auto getSquares(const Tetromino& t)
	{
		std::vector<int> squares = { shapes[t.shape + 4 * t.orientation], shapes[t.shape + 4 * t.orientation + 1], shapes[t.shape + 4 * t.orientation + 2], shapes[t.shape + 4 * t.orientation + 3] };
		return squares;
	};

	// sets the board cells to a value in the cells a tetromino is occupying
	void setBoardValOverPiece(const Tetromino& t, int value)
	{
		auto squares = getSquares(t);
		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4)) * BOARD_WIDTH + (t.x + squares[i] % 4)] = value;
		}
	}

	// determines if the tetromino is colliding with occupied cells, returns false on collisions
	auto checkCollision(Tetromino& t)
	{
		auto squares = getSquares(t);
		for (int i = 0; i < 4; i++)
		{
			// check on board first
			if (t.x + squares[i] % 4 > BOARD_WIDTH - 1 || t.x + squares[i] % 4 < 0)
				return false;
			if (t.y + squares[i] / 4 > BOARD_HEIGHT - 1 || t.y + squares[i] / 4 < 0)
				return false;
			// check if cell is occupied
			if (board[(t.y + squares[i] / 4) * BOARD_WIDTH + (t.x + squares[i] % 4)] != 0)
				return false;
		}
		return true;
	};

	void networkPieceSync();

	auto movePieceDown(Tetromino& t)
	{
		// removes current piece from board
		//setBoardValOverPiece(t, 0);

		// move down
		t.y++;

		// see if we're colliding, and if so, put the piece back where it was before
		if (!checkCollision(t))
		{
			t.y--;
			//setBoardValOverPiece(t, 1);
			return false;
		}

		// the piece successfully moved down, put it into the board
		//setBoardValOverPiece(t, 1);
		networkPieceSync();
		return true;
	};

	void slamBlock(Tetromino& t);

	auto movePieceLR(Tetromino& t, int dx)
	{
		// removes current piece from board
		//setBoardValOverPiece(t, 0);

		// move piece
		t.x += dx;

		// see if we're colliding, and if so, put the piece back where it was before
		if (!checkCollision(t))
		{
			t.x -= dx;
			//setBoardValOverPiece(t, 1);
			Mix_PlayChannel(-1, sfx[2], 0);
			return false;
		}

		// the piece successfully moved, put it into the board
		//setBoardValOverPiece(t, 1);
		networkPieceSync();
		return true;
	};

	auto rotate(Tetromino& t)
	{
		// remove the existing piece
		//setBoardValOverPiece(t, 0);

		// update the orientation int
		t.orientation++;
		t.orientation %= 4;

		// see if we're colliding with something and if so, revert back to previous rotation
		if (!checkCollision(t))
		{
			t.orientation--;
			t.orientation %= 4;

			//setBoardValOverPiece(t, 1);
			Mix_PlayChannel(-1, sfx[2], 0);
			return false;
		}

		// place new piece onto the board
		//setBoardValOverPiece(t, 1);
		Mix_PlayChannel(-1, sfx[3], 0);
		networkPieceSync();

		return true;

	};

	

#pragma region VR
	vr::IVRSystem* m_pHMD;

	glm::mat4 projLeft, projRight, eyePosLeft, eyePosRight, hmdPose;

	vr::VRActionHandle_t m_move, m_leftPoseAct, m_rightPoseAct, m_rotateAct, m_moveBlockLeftAct, m_moveBlockRightAct, m_moveBlockDownAct;
	vr::VRActionSetHandle_t m_main;
	vr::VRInputValueHandle_t m_leftHand;

	vr::TrackedDevicePose_t m_leftPose, m_rightPose;

	bool m_inputHack = false;

	struct FramebufferDesc
	{
		GLuint m_nDepthBufferId;
		GLuint m_nRenderTextureId;
		GLuint m_nRenderFramebufferId;
		GLuint m_nResolveTextureId;
		GLuint m_nResolveFramebufferId;
	};

	glm::mat4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t& matPose)
	{
		glm::mat4 matrixObj(
			matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
			matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
			matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
			matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
		);
		return matrixObj;
	}

	glm::mat4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
	{

		vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(nEye, m_near, m_far);

		return glm::mat4(
			mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
			mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
			mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
			mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
		);
	}

	glm::mat4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
	{
		vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform(nEye);
		glm::mat4 matrixObj(
			matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
			matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
			matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
			matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
		);


		return glm::inverse(matrixObj);
	}

	vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];

	FramebufferDesc leftEyeDesc, rightEyeDesc;

	glm::mat4 GetCurrentProjection(vr::Hmd_Eye eye)
	{
		glm::mat4 p;
		if (eye == vr::Eye_Left)
		{
			p = projLeft;
			//mvp = hmdPose * eyePosLeft * projLeft;
		}
		else if (eye == vr::Eye_Right)
		{
			p = projRight;
			//mvp = hmdPose * eyePosRight * projRight;
		}
		return p;
	}

	glm::mat4 GetCurrentView(vr::Hmd_Eye eye)
	{
		glm::mat4 v;
		if (eye == vr::Eye_Left)
		{
			v = eyePosLeft * hmdPose;
			//mvp = hmdPose * eyePosLeft * projLeft;
		}
		else if (eye == vr::Eye_Right)
		{
			v = eyePosRight * hmdPose;
			//mvp = hmdPose * eyePosRight * projRight;
		}
		return v;
	}

	glm::mat4 GetCurrentViewProjection(vr::Hmd_Eye eye)
	{
		glm::mat4 mvp;
		if (eye == vr::Eye_Left)
		{
			mvp = projLeft * eyePosLeft * hmdPose;
			//mvp = hmdPose * eyePosLeft * projLeft;
		}
		else if (eye == vr::Eye_Right)
		{
			mvp = projRight * eyePosRight * hmdPose;
			//mvp = hmdPose * eyePosRight * projRight;
		}
		return mvp;
	}



	bool CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc& framebufferDesc)
	{
		glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

		glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

		glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

		glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

		glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
		glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

		// check FBO status
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}
#pragma endregion

	void populateGrid();

	void drawScene(vr::EVREye eye);
	void shake(float strength, float duration);
	void applyShake();
	void drawGrid();

	void logic();
	void gameOver();

	float levelTime;
	float m_fallTimer;

	float m_shakeTimer;
	float m_shakeDuration;
	float m_shakeStrength;
	bool m_shaking;
	glm::vec3 m_shakeOffset;

	float totalTime;
	bool m_wireframe, m_spin;
	int m_frameCount;

	glm::vec3 position = glm::vec3(0, 2, 25), 
		target = glm::vec3(0, 0, -1),
		velocity = glm::vec3(0, 0, 0);
	Tetromino currentPiece, nextPiece, visualizer;
	Tetromino playerParts[8];
	Mix_Chunk* music, * layers1, * layers2, * layers3, * bigLayers, * donkey;
	Mix_Chunk* levels[4];
	Mix_Chunk* sfx[5];
	cage::ui::UIElement m_rootNode;
	std::shared_ptr<cage::ui::Text> scoreText, levelText;
	std::shared_ptr<cage::ui::LayoutGroup> m_rightPanel;
	std::shared_ptr<cage::ui::Button> m_dab, m_dab2;
	cage::Font* m_font;
	unsigned int m_score, m_level, m_levelCounter;
	glm::vec4 fontColor = { 12 / 255.f, 160 / 255.f, 18 / 255.f, 1.0 };
	cage::Model shrek;
	cage::Model* newShrek, *newShrek2;
	cage::Skybox skybox;
	cage::Mesh<cage::Vertex3UVNormal>* grid;
	std::shared_ptr<cage::Generic3DShader> program;
	std::unique_ptr<cage::SkyboxShader> skyProgram;
	std::shared_ptr<cage::SpriteShader> m_spriteProgram;

};