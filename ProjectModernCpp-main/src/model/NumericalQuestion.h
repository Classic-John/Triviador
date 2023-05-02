#pragma once
#include <iostream>
#include "../model/Question.h"
#include <string>
#include "DEFINITIONS.h"

#include <stdint.h>

struct QuestionStorageInitializer;

class NumericalQuestion : public Question
{
private:
	std::ostream& print(std::ostream& out) const;
public:

	int32_t answerScore(numericAnswer answer);

	numericAnswer getAnswer() const;
	void setAnswer(numericAnswer answer);

	friend QuestionStorageInitializer;
private:	
	numericAnswer m_correctAnswer;
};

