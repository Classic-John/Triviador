#pragma once
#include "WaitingRoom.h"

#include <optional>

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#include <crow/logging.h>
#pragma warning(default:4267)
#pragma warning(default:4244)

#define ROOM_LEFT(sessionID, room_code) CROW_LOG_WARNING << "Player with SessionID [" << sessionID << "] left Room with code [" << room_code << ']'

class WaitingRoomManager
{
public:
	WaitingRoomManager();

	std::shared_ptr<WaitingRoom> createRoom();
	bool addUserToRoom(std::shared_ptr<User> user, std::shared_ptr<WaitingRoom> room);
	void removeUserFromRoom(std::shared_ptr<User> user);
	std::optional<std::shared_ptr<WaitingRoom>> getRoom(WaitingRoom::CodeType roomCode);
private:
	std::unordered_map<WaitingRoom::CodeType, std::shared_ptr<WaitingRoom>> m_idToRoom;
};
