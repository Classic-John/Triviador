#pragma once

#include <stdint.h>
#include "DEFINITIONS.h"
#include "ClientActions.h"
#include "ChoiceQuestion.h"

#include "Map.h"

class Player
{
public:
	using ScoreType = uint16_t;

	void updatePoints(ScoreType points);
	void losePoints(ScoreType points);

	void setReceivedQuestion(bool received);
	bool hasReceivedQuestion();

	void setGaveAnswer(bool gaveAnswer);
	bool hasGivenAnswer() const;

	void setHasToSelectTerritory(bool hasToSelectTerritory);
	bool hasToSelectTerritory() const;

	void setHasSelectedTerritory(bool hasSelectedTerritory);
	bool hasSelectedTerritory() const;

	void setSelectedTerritory(size_t selectedTerritory);
	size_t getSelectedTerritory() const;

	ScoreType getScore() const;

	uint16_t getBaseIndex();
	void setBase(model::Territory* base);

	void setNumericAnswer(numericAnswer answer);
	numericAnswer getNumericAnswer() const;

	void setChoiceAnswerIndex(ChoiceQuestion::ArrayIndexType index);
	ChoiceQuestion::ArrayIndexType getChoiceAnswerIndex() const;

	void setAction(ClientAction action);
	ClientAction getAction() const;

	void setIndex(size_t index);
	size_t getIndex() const;
private:
	ScoreType m_score;
	model::Territory* m_base;

	bool m_receivedQuestion{false};
	bool m_gaveAnswer{ false };
	numericAnswer m_numericAnswer;
	ChoiceQuestion::ArrayIndexType m_choiceIndex;
	size_t m_index; // order in which answers arrived

	bool m_hasToSelectTerritory{ false };
	bool m_hasSelectedTerritory{ false };
	int64_t m_selectedTerritory{ -1 };

	ClientAction m_action{ClientAction::WAIT};
};

