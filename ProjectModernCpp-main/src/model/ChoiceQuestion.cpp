#include "ChoiceQuestion.h"

std::ostream& ChoiceQuestion::print(std::ostream& out) const
{
	out << this->m_statement << '\n';
	for (int i = 0; i < this->m_choices.size(); ++i)
	{
		out << '(' << i << "). " << this->m_choices[i] << '\n';
	}
	return out;

}

std::string ChoiceQuestion::getSerializedChoices() const
{
	return serializeString(this->m_choices);
}

const ChoiceQuestion::ChoiceArrayType& ChoiceQuestion::getChoices() const
{
	return this->m_choices;
}

ChoiceQuestion::ArrayIndexType ChoiceQuestion::getCorrectChoiceIndex() const
{
	return this->m_correctChoiceIndex;
}

const ChoiceQuestion::ChoiceAnswerType& ChoiceQuestion::getCorrectAnswer() const
{
	return m_choices[m_correctChoiceIndex];
}

void ChoiceQuestion::setChoices(std::string choicesString)
{
	this->m_choices = deserializeString(choicesString);
}

void ChoiceQuestion::setCorrectChoiceIndex(ArrayIndexType index)
{
	this->m_correctChoiceIndex = index;
}

ChoiceQuestion::ChoiceArrayType deserializeString(const std::string& string)
{
	std::istringstream parse(string);
	ChoiceQuestion::ChoiceArrayType choices;
	size_t i = 0;
	for (std::string token; std::getline(parse, token, '|'); ++i)
	{
		choices[i] = token;
	}
	return choices;
}

std::string serializeString(const ChoiceQuestion::ChoiceArrayType& array)
{
	// exception handling
	std::string serialized;
	for (int i = 0; i < array.size() - 1; ++i)
	{
		serialized += array[i] + '|';
	}
	serialized += array.back();
	return serialized;
}
