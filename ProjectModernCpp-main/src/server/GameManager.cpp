#include "GameManager.h"

#include <fstream>
#include "model/MapFactory.h"

uint64_t GameManager::NextID{ 100 };

std::shared_ptr<Game> GameManager::createGame()
{
	std::ifstream fin("res/maps/graphs/Romania.mapg");
	if (!fin.is_open())
		throw std::exception();

	auto mapFactory = model::MapFactory(fin);
    auto map = mapFactory.createMap();
    std::cout << map.size();
    std::shared_ptr<Game> game = std::make_shared<Game>(NextID++, std::move(map));
    m_games.insert({ game->getId(), game });
    return game;
}

void GameManager::addUserToGame(std::shared_ptr<User> user, std::shared_ptr<Game> game)
{
    game->addUser(user);
    user->setGame(game);
}

void GameManager::removeUserFromGame(std::shared_ptr<User> user)
{
    auto game = user->getGame();
    user->setGame(nullptr);

    game->removeUser(user);
}

std::optional<std::shared_ptr<Game>> GameManager::getGame(uint32_t gameId)
{
    if (auto it = m_games.find(gameId); it != m_games.end())
        return it->second;
    else
        return std::nullopt;
}
