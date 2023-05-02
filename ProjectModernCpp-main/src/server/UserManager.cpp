#include "UserManager.h"

#define USER_LOGGED_OUT(sessionID) CROW_LOG_WARNING << "Player with SessionID [" << sessionID << "] logged out"

UserManager::UserManager(std::string_view path, WaitingRoomManager& roomMng, GameManager& gameMng)
	:m_database(path), m_roomMng(roomMng), m_gameMng(gameMng)
{
}

std::optional<UserEntity> UserManager::validateLogin(const crow::request& request)
{
	auto loginInfo = net::LoginInfo::Deserialize(request.body);
	return m_database.getUserFromLogin(loginInfo.username, loginInfo.password);
}

bool UserManager::isRegistered(const std::string& username)
{
	return bool(m_database.getUserFromUsername(username));
}

//bool UserManager::registered(const std::string& username, const std::string& email, const std::string& password)
//{
//	using namespace sqlite_orm;
//	auto validUsers = m_database.getStorage().get_all<User>(where(c(&User::getName) == username and c(&User::getEmail) == email));
//	if (!validUsers.empty())
//		return false;
//
//	User user(username, email, password);
//	m_database.getStorage().insert(user);
//	return true;
//}

std::optional<std::shared_ptr<User>> UserManager::getLoggedInUser(const crow::request& request, bool resetTimer)
{
	crow::query_string query = request.url_params;
	uint32_t sessionId = std::stoi(query.get("session_token"));
	auto user = getLoggedInUser(sessionId);
	if (user && resetTimer)
		this->resetTimer(*user);
	return user;
}

std::optional<std::shared_ptr<User>> UserManager::getLoggedInUser(User::IdType id)
{
	if (auto it = m_idToUsers.find(id); it != m_idToUsers.end())
		return it->second;
	else
		return std::nullopt;
}

User::IdType UserManager::loginUser(UserEntity&& userEntity)
{
	std::shared_ptr<User> user = std::make_shared<User>(std::move(userEntity));
	User::IdType userId = user->getId();
	m_idToUsers.insert({ userId, user });
	resetTimer(user);

	return userId;
}

void UserManager::registerUser(net::RegisterInfo registerInfo)
{
	m_database.addUser(registerInfo.username, registerInfo.email, registerInfo.password);
}

void UserManager::setConnectionState(std::shared_ptr<User> user, net::ConnectionStatus newState)
{
	if (newState == net::ConnectionStatus::DISCONNECTED)
	{
		logoutUser(user);
	}
	user->setConnectionState(newState);
}

void UserManager::resetTimer(std::shared_ptr<User> user)
{
	m_timer.destroy(user->getTimerIds().first);
	m_timer.destroy(user->getTimerIds().second);

	auto connectionLostId = m_timer.create(5000, 0, std::bind(&UserManager::setConnectionState, this, user, net::ConnectionStatus::CONNECTION_LOST));

	auto disconnectionId = m_timer.create(30000, 0, std::bind(&UserManager::setConnectionState, this, user, net::ConnectionStatus::DISCONNECTED));

	user->setTimerIds(connectionLostId, disconnectionId);
}

void UserManager::logoutUser(std::shared_ptr<User> user)
{
	if(user->getRoom())
		m_roomMng.removeUserFromRoom(user);
	USER_LOGGED_OUT(user->getId());
	m_idToUsers.erase(user->getId());
}
