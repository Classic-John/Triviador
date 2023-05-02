#pragma once

#include <iostream>
#include <string>
#include <cstdint>
#include <memory>

#include "net_common/net_common.h"
#include "Entities.h"
#include "Timer.h"

class WaitingRoom;
class Game;

class User
{
public:
	using IdType = uint32_t;
	
	User(UserEntity&& userEntity);

	const IdType& getId() const;
	const std::string& getName() const;

	void setConnectionState(net::ConnectionStatus newState);
	net::ConnectionStatus getConnectionState() const;

	void setTimerIds(Timer::timer_id lostConnectionTimerId, Timer::timer_id disconnectionTimerId);
	
	std::pair<Timer::timer_id, Timer::timer_id> getTimerIds();

	void setRoom(std::shared_ptr<WaitingRoom> room);
	std::shared_ptr<WaitingRoom> getRoom();

	void setGame(std::shared_ptr<Game> game);
	std::shared_ptr<Game> getGame() const;

	void setHasUnseenChanges(bool hasUnseenChanges);
	bool hasUnseenChanges();
private:
	Timer::timer_id m_lostConnectionTimer, m_disconnectionTimer;

	IdType m_userId;
	std::string m_name;

	bool m_unseenChanges;

	std::shared_ptr<WaitingRoom> m_room;
	std::shared_ptr<Game> m_game;

	net::ConnectionStatus m_state;
};
