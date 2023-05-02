#pragma once

#include <cstdint>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <map>
#include <thread>
#include <condition_variable>
#include <assert.h>
#include <algorithm>

#include "model/Player.h"
#include "User.h"
#include "QuestionManager.h"
#include "Timer.h"

#include "model/Map.h"
#include "model/ChoiceQuestion.h"
#include "model/NumericalQuestion.h"
#include "model/RandomGenerator.h"

namespace std
{
	template<>
	struct less<std::shared_ptr<User>>
	{
		bool operator()(const shared_ptr<User>& first, const shared_ptr<User>& second) const
		{
			return std::less()(first.get(), second.get());
		}
	};
}

struct AnswerOrderComparator
{
	NumericalQuestion* question;

	AnswerOrderComparator(NumericalQuestion* question) 
		: question(question)
	{}

	bool operator()(Player* first, Player* second) const
	{
		if (!first->hasGivenAnswer() && !second->hasGivenAnswer())
			return true;
		if (first->hasGivenAnswer() && !second->hasGivenAnswer())
			return true;
		if (!first->hasGivenAnswer() &&second->hasGivenAnswer())
			return false;
		if (question->answerScore(first->getNumericAnswer()) < question->answerScore(second->getNumericAnswer()))
			return true;
		if (question->answerScore(first->getNumericAnswer()) > question->answerScore(second->getNumericAnswer()))
			return false;
		return first->getIndex() < second->getIndex();
	}
};

class Game
{
	using ChoiceArrayType = ChoiceQuestion::ChoiceArrayType;
	using UserConstIterator = std::map<std::shared_ptr<User>, std::unique_ptr<Player>>::const_iterator;
public:
	enum class GameStage
	{
		BASE_SELECTION,
		EXPANSION,
		DUELING,
		RANKING
	};

	Game(uint32_t gameId, model::Map&& map);

	void runGame();

	void setGameId(uint32_t gameId);
	uint32_t getId() const;

	static std::array<ChoiceQuestion::ArrayIndexType, NUMBER_CHOICES / 2> halveOptions(const ChoiceQuestion& question) noexcept;
	static std::array<int, NUMBER_CHOICES> calculateProbabilitiesOfChoices(const ChoiceQuestion& question) noexcept;
	static numericAnswer approximateAnswer(const NumericalQuestion& question) noexcept;
	static std::array<numericAnswer, NUMBER_CHOICES> givePossibleAnswers(const NumericalQuestion& question) noexcept;
	static numericAnswer calculateApproximation(numericAnswer correctAnswer, int smallInterval, int mediumInterval, int largeInterval) noexcept;

	std::vector<std::string> getPlayerNames();

	void playerJoined();

	bool allPlayersJoined();

	UserConstIterator begin() const;
	UserConstIterator end() const;

	void setStage(GameStage stage);

	GameStage stage() const;

	void newChoiceQuestion();
	void newNumericalQuestion();

	// returns player with correct answer, nullptr if both have answered correctly
	Player* determineChoiceQuestionWinner(Player* attacker, Player* defender);

	ChoiceQuestion getCurrentChoiceQuestion();
	NumericalQuestion getCurrentNumericalQuestion();

	Player* getPlayer(std::shared_ptr<User> user);

	void startGame();

	void resetAllPlayerFlags();

	bool isNumericQuestionOngoing();
	void setNumericQuestionOngoing(bool state);

	bool isChoiceQuestionOngoing();
	void setChoiceQuestionOngoing(bool state);

	std::vector<Player*> determineAnswerCorrectnessOrder();
	std::vector<Player*> randomOrder();

	std::vector<uint64_t> selectableTerritories(Player* player);

	bool isStarted() const;

	void setResponseDeadline(uint64_t duration);

	uint64_t getResponseDeadline();

	void playerAnsweredNumericQuestion(Player* player, numericAnswer answer);
	void playerAnsweredChoiceQuestion(Player* player, ChoiceQuestion::ArrayIndexType answerIndex);

	void playerSelectedTerritory(Player* player, size_t territoryIdx);

	size_t nrPlayers() const;

	void setAllPlayersAction(ClientAction action);

	//void startBaseSelectionTimer();

	size_t randomBaseTerritoryIndex(Player* player);
	size_t randomUnoccupiedTerritoryIndex(Player* player);
	size_t randomNeighbouringTerritoryIndex(Player* player);

	void setBase(model::Territory& territory, Player* player);
	void updateTerritory(model::Territory& territory, Player* player, int32_t value);

	model::Territory* getUpdatedTerritory();

	size_t getOwnerIndex(model::Territory& territory);

	~Game();
private:
	bool m_gameStarted{ false };
	bool m_numericQuestionOngoing{ false };
	bool m_choiceQuestionOngoing{ false };
	bool m_mapOccupied{ false };

	ChoiceQuestion m_currentChoiceQuestion;
	NumericalQuestion m_currentNumericalQuestion;

	GameStage m_gameStage;
	QuestionManager m_qMng;

	void addUser(std::shared_ptr<User> user);
	void removeUser(std::shared_ptr<User> user);

	uint16_t m_nrPlayersJoined{ 0 };

	uint32_t m_gameId{ 0 };
	std::map<std::shared_ptr<User>, std::unique_ptr<Player>> m_userToPlayer;
	
	std::vector<Player*> m_players;
	std::map<Player*, size_t> m_playerToIndex;

	int32_t m_currentPlayerSelectIndex{ 0 };

	uint16_t m_numPlayersReceivedInfo{ 0 };
	uint16_t m_nrRoundSelectableTerritories{ 0 };

	uint16_t m_nrRemainingTerritories{ 0 };

	bool m_determinedBaseSelectionOrder{ false };

	uint64_t m_expireTime;
	Timer::timer_id m_currentTimerId;
	Timer m_timer;
	std::thread m_gameThread;
	std::condition_variable m_waiter;
	std::mutex m_waiterMutex;

	model::Map m_map;

	model::Territory* m_updatedTerritory;

	RandomGenerator m_generator;

	friend class GameManager;
};
