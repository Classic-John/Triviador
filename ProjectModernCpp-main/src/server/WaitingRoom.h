#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

#include "User.h"

class WaitingRoom
{
public:
	using CodeType = uint32_t;
	using UserContainerType = std::vector<std::shared_ptr<User>>;
	using UserConstIterator = UserContainerType::const_iterator;
	using UserIterator = UserContainerType::iterator;
public:
	bool m_gameStarted{ false };

	WaitingRoom(const CodeType& code);
	WaitingRoom(const WaitingRoom& other);
	WaitingRoom& operator=(const WaitingRoom& other);

	const UserContainerType& getUserList() const;

	const CodeType& getRoomCode() const;

	UserConstIterator begin() const;
	UserConstIterator end() const;
	UserContainerType::size_type size() const;

	bool isGameStartable() const;
private:
	void addUser(std::shared_ptr<User> player);
	//UserConstIterator getUser(const User::IdType& id) const;
	void removeUser(std::shared_ptr<User> user);

	void notifyAll();

	CodeType m_roomCode;
	UserContainerType m_players;

	friend class WaitingRoomManager;
};

