#include "DatabaseUserManager.h"

DatabaseUserManager::DatabaseUserManager(std::string_view databasePath)
	:DatabaseManager(databasePath), m_databaseStorage(UserStorageInitializer()(databasePath))
{
}

std::optional<UserEntity> DatabaseUserManager::getUserFromLogin(const std::string& username, const std::string& password)
{
	using namespace sqlite_orm;

	// TODO: Don't allow same usernames
	auto user = getStorage().get_all<UserEntity>(where(c(&UserEntity::userName) == username and c(&UserEntity::password) == password));
	if (!user.empty())
		return user.front();
	return std::nullopt;
}

std::optional<UserEntity> DatabaseUserManager::getUserFromUsername(const std::string& username)
{
	using namespace sqlite_orm;

	// TODO: Don't allow same usernames
	auto user = getStorage().get_all<UserEntity>(where(c(&UserEntity::userName) == username));
	if (!user.empty())
		return user.front();
	return std::nullopt;
}

bool DatabaseUserManager::addUser(const std::string& username, const std::string& email, const std::string& password)
{
	UserEntity user{ .userName = username, .password = password, .email = email };
	getStorage().insert(user);
	return true;
}

DatabaseUserManager::Storage& DatabaseUserManager::getStorage()
{
	return m_databaseStorage;
}

