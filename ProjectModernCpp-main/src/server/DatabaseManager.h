#pragma once

#include  <string_view>
#include <memory>
#include <sqlite_orm/sqlite_orm.h>

class DatabaseManager
{
public:
	DatabaseManager(std::string_view databasePath);
protected:
	std::string_view m_databasePath;
};

