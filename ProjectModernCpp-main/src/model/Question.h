#pragma once
#include <string>
#include <iostream>

class Question
{
protected:
	void setDifficultyValue(int value);
	virtual std::ostream& print(std::ostream& out) const = 0;
public:
	enum class Difficulty
	{
		Easy,
		Medium,
		Hard
	};

	int getId() const;
	const std::string& getStatement() const;
	int getDifficultyValue() const;

	void setId(int id);
	void setStatement(std::string statement);
	void setDifficulty(Difficulty difficulty);

	friend std::ostream& operator<<(std::ostream& out, const Question& question);
protected:
	int m_questionId;
	Difficulty m_difficulty;
	std::string m_statement;
};

std::ostream& operator<<(std::ostream& out, const Question& question);
