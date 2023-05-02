#pragma once
#include <iostream>

#include "DatabaseManager.h"
#include "model/NumericalQuestion.h"
#include "model/ChoiceQuestion.h"

struct QuestionStorageInitializer
{
	inline auto operator()(std::string_view path)
	{
		using namespace sqlite_orm;
		return make_storage(path.data(),
			make_table<ChoiceQuestion>("choiceQuestion",
				make_column("id", &ChoiceQuestion::getId, &ChoiceQuestion::setId, autoincrement(), primary_key()),
				make_column("difficulty", &ChoiceQuestion::setDifficultyValue, &ChoiceQuestion::getDifficultyValue),
				make_column("statement", &ChoiceQuestion::getStatement, &ChoiceQuestion::setStatement),
				make_column("choices", &ChoiceQuestion::setChoices, &ChoiceQuestion::getSerializedChoices),
				make_column("correctIdx", &ChoiceQuestion::setCorrectChoiceIndex, &ChoiceQuestion::getCorrectChoiceIndex)),
			make_table<NumericalQuestion>("numericalQuestion",
				make_column("id", &NumericalQuestion::m_questionId, autoincrement(), primary_key()),
				make_column("id", &NumericalQuestion::setDifficultyValue, &NumericalQuestion::getDifficultyValue),
				make_column("statement", &NumericalQuestion::m_statement),
				make_column("answer", &NumericalQuestion::m_correctAnswer)));
	}
};

class DatabaseQuestionManager : public DatabaseManager
{
public:
	using Storage = decltype(QuestionStorageInitializer()(std::string_view()));

	DatabaseQuestionManager(std::string_view databasePath);

	Storage& getStorage();
private:
	Storage m_databaseStorage;
};


