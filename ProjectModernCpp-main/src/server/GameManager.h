#pragma once
#include "Game.h"

class GameManager
{
public:
	std::shared_ptr<Game> createGame();
	void addUserToGame(std::shared_ptr<User> user, std::shared_ptr<Game> game);
	void removeUserFromGame(std::shared_ptr<User> user);
	std::optional<std::shared_ptr<Game>> getGame(uint32_t gameId);
private:
	static uint64_t NextID;

	std::unordered_map<uint32_t, std::shared_ptr<Game>> m_games;
};