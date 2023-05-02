#include "Question.h"

int Question::getDifficultyValue() const
{
	return static_cast<int>(m_difficulty);
}

void Question::setDifficultyValue(int value)
{
	m_difficulty = static_cast<Difficulty>(value);
}

void Question::setId(int id)
{
	m_questionId = id;
}

void Question::setStatement(std::string statement)
{
	m_statement = statement;
}

void Question::setDifficulty(Difficulty difficulty)
{
	m_difficulty = difficulty;
}

int Question::getId() const
{
	return m_questionId;
}

const std::string& Question::getStatement() const
{
	return m_statement;
}

std::ostream& operator<<(std::ostream& out, const Question& question)
{
	return question.print(out);
}
