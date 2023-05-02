#pragma once

#include <memory>
#include <unordered_set>
#include <vector>
#include <random>
#include <type_traits>

#include "DatabaseQuestionManager.h"

// QuestionManager class is responsible for every interaction with Questions (these include read from database, choose random question (depending on the stage of Game), send question to client(s))


/*
QuestionManager class
	This Manager class is part of the server, and is responsible for reading data from database and store this data into vector fields (readQuestions() method),
*/

class QuestionManager
{
public:
	QuestionManager(std::string_view);

	ChoiceQuestion getChoiceQuestion(bool unique_only = true);

	NumericalQuestion getNumericalQuestion(bool unique_only = true);

	~QuestionManager();
private:
	DatabaseQuestionManager m_database;
	std::unordered_set<int> m_uniqueChoices;
	std::unordered_set<int> m_uniqueNumericals;
};
