#include "Player.h"

void Player::updatePoints(ScoreType points)
{
}

void Player::losePoints(ScoreType points)
{
}

void Player::setReceivedQuestion(bool received)
{
	m_receivedQuestion = received;
}

bool Player::hasReceivedQuestion()
{
	return m_receivedQuestion;
}

void Player::setGaveAnswer(bool gaveAnswer)
{
	m_gaveAnswer = gaveAnswer;
}

bool Player::hasGivenAnswer() const
{
	return m_gaveAnswer;
}

void Player::setHasToSelectTerritory(bool hasToSelectTerritory)
{
	m_hasToSelectTerritory = hasToSelectTerritory;
}
bool Player::hasToSelectTerritory() const
{
	return m_hasToSelectTerritory;
}

void Player::setHasSelectedTerritory(bool hasSelectedTerritory) {
	m_hasSelectedTerritory = hasSelectedTerritory;
}
bool Player::hasSelectedTerritory() const {
	return m_hasSelectedTerritory;
}

void Player::setSelectedTerritory(size_t selectedTerritory) {
	m_selectedTerritory = selectedTerritory;
}
size_t Player::Player::getSelectedTerritory() const {
	return m_selectedTerritory;
}

Player::ScoreType Player::getScore() const
{
	return m_score;
}

uint16_t Player::getBaseIndex()
{
	return m_base->index();
}

void Player::setBase(model::Territory* base)
{
	m_base = base;
}

void Player::setNumericAnswer(numericAnswer answer)
{
	m_numericAnswer = answer;
}

numericAnswer Player::getNumericAnswer() const
{
	return m_numericAnswer;
}

void Player::setChoiceAnswerIndex(ChoiceQuestion::ArrayIndexType index)
{
	m_choiceIndex = index;
}

ChoiceQuestion::ArrayIndexType Player::getChoiceAnswerIndex() const
{
	return m_choiceIndex;
}

void Player::setAction(ClientAction action)
{
	m_action = action;
}

ClientAction Player::getAction() const
{
	return m_action;
}

void Player::setIndex(size_t index)
{
	m_index = index;
}

size_t Player::getIndex() const
{
	return m_index;
}