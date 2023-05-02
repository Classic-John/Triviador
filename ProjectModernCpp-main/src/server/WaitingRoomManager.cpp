#include "WaitingRoomManager.h"

WaitingRoomManager::WaitingRoomManager()
{
}

std::shared_ptr<WaitingRoom> WaitingRoomManager::createRoom()
{
	// TODO check that an existing room code is not generated
	std::srand(time(NULL));
	WaitingRoom::CodeType roomCode = (std::rand() % 900'000) + 100'000;
	auto room = std::make_shared<WaitingRoom>(roomCode);
	m_idToRoom.insert({ roomCode, room });
	return room;
}

bool WaitingRoomManager::addUserToRoom(std::shared_ptr<User> user, std::shared_ptr<WaitingRoom> room)
{
	if (room->size() >= 4)
		return false;

	room->addUser(user);
	user->setRoom(room);
	room->notifyAll();
	return true;
}

void WaitingRoomManager::removeUserFromRoom(std::shared_ptr<User> user)
{
	auto room = user->getRoom();
	if (room)
	{
		ROOM_LEFT(user->getId(), room->getRoomCode());
		user->setRoom(nullptr);
		room->removeUser(user);
		if (room->getUserList().empty())
			m_idToRoom.erase(room->getRoomCode());
		else
			room->notifyAll();
	}
}

std::optional<std::shared_ptr<WaitingRoom>> WaitingRoomManager::getRoom(WaitingRoom::CodeType roomCode)
{
	if (auto it = m_idToRoom.find(roomCode); it != m_idToRoom.end())
		return it->second;
	else
		return std::nullopt;
}
