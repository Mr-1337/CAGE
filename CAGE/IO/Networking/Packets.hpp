#pragma once

namespace cage
{
	namespace networking
	{
		enum class PacketType : unsigned char
		{
			QUERY = 1,
			QUERY_RESPONSE,
			CONNECTION_REQUEST,
			CONNECTION_ACCEPT,
			ROSTER_SYNC,
			GAME_START,
			PLAYER_POS,
			PLAYER_PIECE_POS,
			BOARD_SYNC,
			PLAYER_SYNC,
		};

		namespace packets
		{
			struct Query
			{
				PacketType type = PacketType::QUERY;
			};

			struct QueryResponse
			{
				PacketType type = PacketType::QUERY_RESPONSE;
				char name[16];
			};

			struct ConnectionRequest
			{
				PacketType type = PacketType::CONNECTION_REQUEST;
				char message[14] = "Shrek is love";
			};

			struct ConnectionAccept
			{
				PacketType type = PacketType::CONNECTION_ACCEPT;
				int playerID;
				char message[14] = "Shrek is life";
			};

			struct RosterSync
			{
				PacketType type = PacketType::ROSTER_SYNC;
				bool players[8];
			};

			struct GameStart
			{
				PacketType type = PacketType::GAME_START;
				int boardW, boardH;
			};

			struct PlayerPos
			{
				PacketType type = PacketType::PLAYER_POS;
				int playerID;
				float x, y, z;
			};

			struct PlayerPiecePos
			{
				PacketType type = PacketType::PLAYER_PIECE_POS;
				int playerID;
				int x, y, orientation;
			};

			struct BoardSync
			{
				PacketType type = PacketType::BOARD_SYNC;
				char board[1600];
			};

			struct PlayerSync
			{
				PacketType type = PacketType::PLAYER_SYNC;
				int px[8];
				int py[8];
				int po[8];
				float x[8];
				float y[8];
				float z[8];
			};
		}
	}
}