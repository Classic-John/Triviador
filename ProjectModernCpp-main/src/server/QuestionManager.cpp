#include "QuestionManager.h"

QuestionManager::QuestionManager(std::string_view path)
	: m_database(DatabaseQuestionManager(path))
{
}

ChoiceQuestion QuestionManager::getChoiceQuestion(bool unique_only)
{
	int id{};
	do
	{
		id = std::rand() % m_database.getStorage().count<ChoiceQuestion>() + 1;
	} while (unique_only && !m_uniqueChoices.insert(id).second);

	return m_database.getStorage().get<ChoiceQuestion>(id);
}

NumericalQuestion QuestionManager::getNumericalQuestion(bool unique_only)
{
	std::srand(time(0));
	int id{};
	do
	{
		id = std::rand() % m_database.getStorage().count<NumericalQuestion>() + 1;
	} while (unique_only && !m_uniqueNumericals.insert(id).second);

	return m_database.getStorage().get<NumericalQuestion>(id);
}

QuestionManager::~QuestionManager()
{
}
