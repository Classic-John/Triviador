#include "DatabaseQuestionManager.h"

DatabaseQuestionManager::DatabaseQuestionManager(std::string_view databasePath)
	: DatabaseManager(databasePath), m_databaseStorage(QuestionStorageInitializer()(databasePath))
{
}

DatabaseQuestionManager::Storage& DatabaseQuestionManager::getStorage()
{
	return m_databaseStorage;
}


