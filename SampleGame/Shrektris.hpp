#pragma once

#include <array>
#include <string>
#include <memory>
#include <SDL2/SDL_mixer.h>
#include <GLM/glm/glm.hpp>
#include <GLM/glm/gtc/matrix_transform.hpp>
#include "../CAGE/CAGE.hpp"

#include "../CAGE/Graphics/Models/Mesh.hpp"
#include "../CAGE/Graphics/ShaderProgram/ShaderProgram.hpp"
#include "../CAGE/IO/MeshLoader.hpp"

class Shrektris : public cage::Game
{
public:
	Shrektris(int argc, char** argv);
	void Run() override;
private:

	struct Tetromino
	{
		int x = 0, y = 0;
		int shape = 0;
		int orientation = 0;
	};

	int board[15][10] = { 0 };

	/*
		0 1 2 3
		4 5 6 7
		8 9 A B
		C D E F
	*/

	const std::array<int, 4 * 4 * 7> shapes = {
		0, 1, 4, 5,    0, 1, 4, 5,    0, 1, 4, 5,   0, 1, 4, 5,   //square
		0, 1, 5, 6,    8, 4, 5, 1,    0, 1, 5, 6,   8, 4, 5, 1,   //z
		4, 5, 1, 2,    0, 4, 5, 9,    4, 5, 1, 2,   0, 4, 5, 9,   //s
		0, 4, 8, 9,    4, 5, 6, 2,    0, 1, 5, 9,   0, 4, 1, 2,   //L
		8, 1, 5, 9,    0, 4, 5, 6,    8, 4, 0, 1,   0, 1, 2, 6,   //backward L
		0, 4, 8, 0xC,  0, 1, 2, 3,    0, 4, 8, 0xC, 0, 1, 2, 3,   //line
		0, 1, 2, 5,    0, 4, 8, 5,    1, 4, 5, 6,   4, 1, 5, 9	  //T
	};

	auto getSquares(const Tetromino & t)
	{
		std::vector<int> squares = { shapes[t.shape + 4 * t.orientation], shapes[t.shape + 4 * t.orientation + 1], shapes[t.shape + 4 * t.orientation + 2], shapes[t.shape + 4 * t.orientation + 3] };
		return squares;
	};

	auto checkCollision(Tetromino & t, const std::vector<int> & squares)
	{
		for (int i = 0; i < 4; i++)
		{
			// check on board first
			if (t.x + squares[i] % 4 > 9 || t.x + squares[i] % 4 < 0)
				return false;
			if (t.y + squares[i] / 4 > 14 || t.y + squares[i] / 4 < 0)
				return false;
			// check if cell is occupied
			if (board[t.y + squares[i] / 4][t.x + squares[i] % 4] != 0)
				return false;
		}
		return true;
	};

	auto movePieceDown(Tetromino & t)
	{
		// indices of all the 'cells' this tetromino fills
		auto squares = getSquares(t);

		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 0;
		}

		t.y++;

		if (!checkCollision(t, squares))
		{
			t.y--;
			for (int i = 0; i < 4; i++)
			{
				board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 1;
			}
			return false;
		}

		for (int i = 0; i < 4; i++)
		{
			board[t.y + (squares[i] / 4)][t.x + squares[i] % 4] = 1;
		}
		return true;
	};

	auto movePieceLR(Tetromino & t, int dx)
	{
		// indices of all the 'cells' this tetromino fills
		auto squares = getSquares(t);

		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 0;
		}

		t.x += dx;

		if (!checkCollision(t, squares))
		{
			t.x -= dx;
			for (int i = 0; i < 4; i++)
			{
				board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 1;
			}
			return false;
		}

		for (int i = 0; i < 4; i++)
		{
			board[t.y + (squares[i] / 4)][t.x + squares[i] % 4] = 1;
		}
		return true;
	};

	auto rotate(Tetromino & t)
	{
		auto squares = getSquares(t);

		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 0;
		}

		t.orientation++;
		t.orientation %= 4;

		squares = getSquares(t);

		if (!checkCollision(t, squares))
		{
			t.orientation--;
			t.orientation %= 4;
			squares = getSquares(t);
			for (int i = 0; i < 4; i++)
			{
				board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 1;
			}
			return false;
		}

		for (int i = 0; i < 4; i++)
		{
			board[(t.y + (squares[i] / 4))][t.x + squares[i] % 4] = 1;
		}

		return true;

	};

	void draw();
	void logic(float& t);

	glm::vec3 position = glm::vec3(0, 2, 20), target = glm::vec3(0, 0, -1);
	Tetromino currentPiece;
	Mix_Chunk *music, *layers1, *layers2, *bigLayers, *donkey;
	cage::Mesh<cage::Vertex3UVNormal> shrek;
	cage::Shader vertexShader, fragShader;
	std::unique_ptr<cage::ShaderProgram> program;
	bool m_running;
};