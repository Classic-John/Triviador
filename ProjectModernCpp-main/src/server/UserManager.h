#pragma once

#include <map>
#include <optional>
#include <memory>

#include "DatabaseUserManager.h"
#include "Timer.h"
#include "WaitingRoom.h"
#include "WaitingRoomManager.h"
#include "GameManager.h"
#include "User.h"

#include "net_common/ClientConnection.h"

#pragma warning(disable:4267)
#pragma warning(disable:4244)
#include <crow/http_request.h>
#pragma warning(default:4267)
#pragma warning(default:4244)

class UserManager
{
public:
	UserManager(std::string_view path, WaitingRoomManager& roomMng, GameManager& gameMng);

	std::optional<UserEntity> validateLogin(const crow::request& request);
	bool isRegistered(const std::string& username);

	//bool registered(const std::string& username, const std::string& email, const std::string& password);
	std::optional<std::shared_ptr<User>> getLoggedInUser(const crow::request& request, bool resetTimer = true);
	std::optional<std::shared_ptr<User>> getLoggedInUser(User::IdType id);

	User::IdType loginUser(UserEntity&& user);
	void registerUser(net::RegisterInfo registerInfo);

	void setConnectionState(std::shared_ptr<User> user, net::ConnectionStatus newState);
	void resetTimer(std::shared_ptr<User> user);

	void logoutUser(std::shared_ptr<User> user);
private:
	DatabaseUserManager m_database;

	WaitingRoomManager& m_roomMng;
	GameManager& m_gameMng;

	Timer m_timer;

	std::map<User::IdType, std::shared_ptr<User>> m_idToUsers;
};

