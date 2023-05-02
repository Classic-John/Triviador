#include "NumericalQuestion.h"

std::ostream& NumericalQuestion::print(std::ostream& out) const
{
	out << this->m_statement << '\n';
	out << "> " << this->m_correctAnswer;
	return out;

}

numericAnswer NumericalQuestion::getAnswer() const
{
	return m_correctAnswer;
}

void NumericalQuestion::setAnswer(numericAnswer answer)
{
	m_correctAnswer = answer;
}

int32_t NumericalQuestion::answerScore(numericAnswer answer)
{
	return labs(m_correctAnswer - answer);
}