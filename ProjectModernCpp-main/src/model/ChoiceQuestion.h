#pragma once
#include "DEFINITIONS.h"
#include <string>
#include <sstream>
#include <array>
#include <stdint.h>
#include "../model/Question.h"

struct QuestionStorageInitializer;

class ChoiceQuestion : public Question 
{
private:
	std::ostream& print(std::ostream& out) const;

	std::string getSerializedChoices() const;
public:
	static const std::size_t NumberChoices = NUMBER_CHOICES;

	using ChoiceAnswerType = std::string;
	using ChoiceArrayType = std::array<ChoiceAnswerType, NumberChoices>;
	using ArrayIndexType = std::size_t;

	const ChoiceArrayType& getChoices() const;
	ArrayIndexType getCorrectChoiceIndex() const;
	const ChoiceAnswerType& getCorrectAnswer() const; 

	void setChoices(std::string choicesString);
	void setCorrectChoiceIndex(ArrayIndexType index);

	friend QuestionStorageInitializer;
private:
	ChoiceArrayType m_choices;
	ArrayIndexType m_correctChoiceIndex;
};

// this function parses a string, it splits the whole string into substrings using , (comma) as the delimiter
ChoiceQuestion::ChoiceArrayType deserializeString(const std::string& string);
std::string serializeString(const ChoiceQuestion::ChoiceArrayType& array);


