#include "User.h"

#include <iostream>
#include <cstdint>

User::User(UserEntity&& userEntity)
	: m_userId(userEntity.userId), m_name(std::move(userEntity.userName)), m_state(), m_unseenChanges(true)
{
}

const User::IdType& User::getId() const
{
	return m_userId;
}
const std::string& User::getName() const
{
	return m_name;
}

void User::setConnectionState(net::ConnectionStatus newState)
{
	m_state = newState;
}

net::ConnectionStatus User::getConnectionState() const
{
	return m_state;
}

void User::setTimerIds(Timer::timer_id lostConnectionTimerId, Timer::timer_id disconnectionTimerId)
{
	m_lostConnectionTimer = lostConnectionTimerId;
	m_disconnectionTimer = disconnectionTimerId;
}

std::pair<Timer::timer_id, Timer::timer_id> User::getTimerIds()
{
	return std::make_pair(m_lostConnectionTimer, m_disconnectionTimer);
}

void User::setRoom(std::shared_ptr<WaitingRoom> room)
{
	m_room = room;
}

std::shared_ptr<WaitingRoom> User::getRoom()
{
	return m_room;
}

void User::setGame(std::shared_ptr<Game> game)
{
	m_game = game;
}

std::shared_ptr<Game> User::getGame() const
{
	return m_game;
}

void User::setHasUnseenChanges(bool hasUnseenChanges)
{
	m_unseenChanges = hasUnseenChanges;
}

bool User::hasUnseenChanges()
{
	return m_unseenChanges;
}
