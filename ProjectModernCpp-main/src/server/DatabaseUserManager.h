#pragma once

#include "DatabaseManager.h"
#include "User.h"
#include "Entities.h"


struct UserStorageInitializer
{
	inline auto operator()(std::string_view path)
	{
		using namespace sqlite_orm;
		auto storage = make_storage(path.data(),
			make_table<UserEntity>("user",
				make_column("id", &UserEntity::userId, autoincrement(), primary_key()),
				make_column("username", &UserEntity::userName),
				make_column("email", &UserEntity::email),
				make_column("password", &UserEntity::password)));
		return storage;
	}
};

class DatabaseUserManager : public DatabaseManager
{
public:
	using Storage = decltype(UserStorageInitializer()(std::string_view()));

	DatabaseUserManager(std::string_view databasePath);

	std::optional<UserEntity> getUserFromLogin(const std::string& username, const std::string& password);

	std::optional<UserEntity> getUserFromUsername(const std::string& username);

	bool addUser(const std::string& username, const std::string& email, const std::string& password);


	Storage& getStorage();
private:
	Storage m_databaseStorage;
};
