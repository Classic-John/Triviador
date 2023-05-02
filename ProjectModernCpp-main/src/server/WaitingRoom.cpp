#include "WaitingRoom.h"

WaitingRoom::WaitingRoom(const CodeType& code)
	: m_roomCode(code)
	, m_players()
{

}

WaitingRoom::WaitingRoom(const WaitingRoom& other)
	: m_roomCode(other.m_roomCode)
	, m_players(other.m_players)
{
}

WaitingRoom& WaitingRoom::operator=(const WaitingRoom& other)
{
	m_roomCode = other.m_roomCode;
	m_players = other.m_players;

	return *this;
}

void WaitingRoom::addUser(std::shared_ptr<User> player)
{
	m_players.push_back(player);
}

void WaitingRoom::removeUser(std::shared_ptr<User> user)
{
	std::erase(m_players, user);
}

void WaitingRoom::notifyAll()
{
	for (auto user : m_players)
	{
		user->setHasUnseenChanges(true);
	}
}

const WaitingRoom::UserContainerType& WaitingRoom::getUserList() const
{
	return m_players;
}

const WaitingRoom::CodeType& WaitingRoom::getRoomCode() const
{
	return m_roomCode;
}

WaitingRoom::UserConstIterator WaitingRoom::begin() const 
{
	return m_players.begin();
}

WaitingRoom::UserConstIterator WaitingRoom::end() const
{
	return m_players.end();
}

WaitingRoom::UserContainerType::size_type WaitingRoom::size() const
{
	return m_players.size();
}

bool WaitingRoom::isGameStartable() const
{
	return m_players.size() >= 2 && m_players.size() <= 4;
}
