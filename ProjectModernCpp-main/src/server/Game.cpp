#include "Game.h"

Game::Game(uint32_t gameId, model::Map&& map)
	: m_gameId(gameId)
	, m_nrPlayersJoined()
	, m_qMng(R"(db.sqlite)")
	, m_gameStage(GameStage::BASE_SELECTION)
	, m_map(std::move(map))
	, m_generator()
{
	m_nrRemainingTerritories = m_map.size();
}

void Game::startGame()
{
	for (const auto& [user, player] : m_userToPlayer)
	{
		m_players.push_back(player.get());
	}

	if (m_gameStarted)
		return;
	m_gameThread = std::thread(std::bind(&Game::runGame, this));
	m_gameThread.detach();
}

void Game::resetAllPlayerFlags()
{
	m_numPlayersReceivedInfo = 0;
	m_nrRoundSelectableTerritories = (nrPlayers() * (nrPlayers() - 1)) / 2;
	for (auto& [player, _] : m_playerToIndex) {
		player->setGaveAnswer(false);
		player->setHasSelectedTerritory(false);
	}
}

void Game::runGame()
{
	std::unique_lock<std::mutex> lock(m_waiterMutex);
	std::cout << m_map.size() << '\n';
	int j = 0;

	resetAllPlayerFlags();
	// *** BASE_SELECTION

	// Ask the numerical question which determines the order in which
	// players will select their bases
	m_gameStarted = true;
	newNumericalQuestion();
	setNumericQuestionOngoing(true);

	setResponseDeadline(15);
	setAllPlayersAction(ClientAction::REQUEST_NUMERICAL_QUESTION);
	m_waiter.wait_for(lock, std::chrono::seconds(16));
	setNumericQuestionOngoing(false);

	// Order the players according to their answers and time it took for them to respond
	auto playerOrder = determineAnswerCorrectnessOrder();
	// Ask each player in order to select a base, and let other players know which
	// territory was selected

	for (auto& currentPlayer : playerOrder)
	{
		setAllPlayersAction(ClientAction::WAIT);
		currentPlayer->setAction(ClientAction::SELECT_TERRITORY);
		m_waiter.wait_for(lock, std::chrono::seconds(11));

		size_t territoryIdx{};
		if (!currentPlayer->hasSelectedTerritory())
			territoryIdx = randomBaseTerritoryIndex(currentPlayer);
		else
			territoryIdx = currentPlayer->getSelectedTerritory();
		std::cout << ++j << '\n';

		setBase(m_map[territoryIdx], currentPlayer);
		m_nrRemainingTerritories--;

		setAllPlayersAction(ClientAction::UPDATE_TERRITORIES);
		m_waiter.wait_for(lock, std::chrono::seconds(3));
	}
	setAllPlayersAction(ClientAction::WAIT);

	m_gameStage = GameStage::EXPANSION;

	while (!m_mapOccupied)
	{
		resetAllPlayerFlags();

		newNumericalQuestion();
		setNumericQuestionOngoing(true);

		setResponseDeadline(15);
		setAllPlayersAction(ClientAction::REQUEST_NUMERICAL_QUESTION);
		m_waiter.wait_for(lock, std::chrono::seconds(16));
		std::cout << "Questions arrived" << '\n';
		//all questions arrived
		setNumericQuestionOngoing(false);

		// Order the players according to their answers and time it took for them to respond
		auto playerOrder = determineAnswerCorrectnessOrder();
		// Ask each player in order to select a base, and let other players know which
		// territory was selected
		resetAllPlayerFlags();
		for (size_t i = 0; i < playerOrder.size(); ++i)
		{
			setAllPlayersAction(ClientAction::WAIT);

			auto currentPlayer = playerOrder[i];
			int16_t playerNrSelectableTerritories = m_userToPlayer.size() - i - 1;
			while (!m_mapOccupied && playerNrSelectableTerritories--)
			{
				std::cout << "Remaining territories " << m_nrRemainingTerritories << '\n';
				currentPlayer->setAction(ClientAction::SELECT_TERRITORY);
				m_waiter.wait_for(lock, std::chrono::seconds(11));
				std::cout << "Territories arrived" << '\n';
				size_t territoryIdx{};
				if (!currentPlayer->hasSelectedTerritory())
					territoryIdx = randomUnoccupiedTerritoryIndex(currentPlayer);
				else
					territoryIdx = currentPlayer->getSelectedTerritory();
				updateTerritory(m_map[territoryIdx], currentPlayer, 100);
				--m_nrRemainingTerritories;
				if (!m_nrRemainingTerritories)
					m_mapOccupied = true;

				setAllPlayersAction(ClientAction::UPDATE_TERRITORIES);
				m_waiter.wait_for(lock, std::chrono::seconds(3));
			}
		}
	}
	setAllPlayersAction(ClientAction::WAIT);

	m_gameStage = GameStage::DUELING;

	uint16_t roundIndex = 0;
	while (++roundIndex != NR_DUELING_ROUNDS)
	{
		resetAllPlayerFlags();

		setAllPlayersAction(ClientAction::WAIT);
		// create a random order of players
		std::vector<Player*> randomPlayerOrder = randomOrder();
		// loop through the players in the random order
		for (Player* attacker : randomPlayerOrder)
		{
			resetAllPlayerFlags();

			attacker->setAction(ClientAction::SELECT_TERRITORY);
			m_waiter.wait_for(lock, std::chrono::seconds(11));

			size_t territoryIdx{};
			if (!attacker->hasSelectedTerritory())
				territoryIdx = randomNeighbouringTerritoryIndex(attacker);
			else
				territoryIdx = attacker->getSelectedTerritory();
			//territory was selected, choiceQuestion incoming
			std::cout << "Territory selected " << '\n';

			auto& territory = m_map[territoryIdx];

			Player* defender = territory.owner();
			newChoiceQuestion();
			setChoiceQuestionOngoing(true);

			setResponseDeadline(15);
			attacker->setAction(ClientAction::REQUEST_CHOICE_QUESTION);
			defender->setAction(ClientAction::REQUEST_CHOICE_QUESTION);
			m_waiter.wait_for(lock, std::chrono::seconds(16));
			//all questions arrived
			setChoiceQuestionOngoing(false);
			std::cout << "All questions arrived\n";

			Player* winner = determineChoiceQuestionWinner(attacker, defender);
			if (winner)
			{
				if (winner == attacker)
				{
					if (territory.score() == 100)
						updateTerritory(territory, attacker, 0);
					else
						updateTerritory(territory, defender, -100);
				}
				else if (winner == defender)
				{
					updateTerritory(territory, defender, 100);
				}
			}
			else {
				resetAllPlayerFlags();

				newNumericalQuestion();
				setNumericQuestionOngoing(true);

				setResponseDeadline(15);
				attacker->setAction(ClientAction::REQUEST_NUMERICAL_QUESTION);
				defender->setAction(ClientAction::REQUEST_NUMERICAL_QUESTION);
				m_waiter.wait_for(lock, std::chrono::seconds(16));
				//all questions arrived
				setAllPlayersAction(ClientAction::WAIT);
				setNumericQuestionOngoing(false);

				auto playerOrder = determineAnswerCorrectnessOrder();

				Player* winner = playerOrder[0];
				if (winner == attacker)
				{
					if (territory.score() == 100)
						updateTerritory(territory, attacker, 0);
					else
						updateTerritory(territory, defender, -100);
				}
				else if (winner == defender)
				{
					updateTerritory(territory, defender, 100);
				}
			}
			setAllPlayersAction(ClientAction::UPDATE_TERRITORIES);
			m_waiter.wait_for(lock, std::chrono::seconds(3));
		}
	}
	setAllPlayersAction(ClientAction::WAIT);
}

void Game::playerAnsweredNumericQuestion(Player* player, numericAnswer answer)
{
	size_t nrPlayersQuestion{};
	if (m_gameStage == GameStage::DUELING)
		nrPlayersQuestion = 2;
	else
		nrPlayersQuestion = nrPlayers();

	std::unique_lock<std::mutex> lock(m_waiterMutex);
	if (player->hasGivenAnswer())
		return;
	player->setAction(ClientAction::WAIT);

	player->setGaveAnswer(true);
	player->setNumericAnswer(answer);
	player->setIndex(m_numPlayersReceivedInfo++);
	
	if (m_numPlayersReceivedInfo == nrPlayersQuestion) {
		m_waiter.notify_all();
	}
}

void Game::playerAnsweredChoiceQuestion(Player* player, ChoiceQuestion::ArrayIndexType answerIndex)
{
	std::unique_lock<std::mutex> lock(m_waiterMutex);
	if (player->hasGivenAnswer())
		return;
	player->setAction(ClientAction::WAIT);

	player->setChoiceAnswerIndex(answerIndex);
	player->setIndex(m_numPlayersReceivedInfo++);
	player->setGaveAnswer(true);

	if (m_numPlayersReceivedInfo == 2) {
		m_waiter.notify_all();
	}
}

void Game::playerSelectedTerritory(Player* player, size_t territoryIdx)
{
	std::unique_lock<std::mutex> lock(m_waiterMutex);

	if (player->hasSelectedTerritory())
		return;
	player->setAction(ClientAction::WAIT);

	player->setHasSelectedTerritory(true);
	player->setSelectedTerritory(territoryIdx);

	m_waiter.notify_all();
}

void Game::setNumericQuestionOngoing(bool state)
{
	m_numericQuestionOngoing = state;
}

bool Game::isChoiceQuestionOngoing()
{
	return m_choiceQuestionOngoing;
}

void Game::setChoiceQuestionOngoing(bool state)
{
	m_choiceQuestionOngoing = state;
}

bool Game::isNumericQuestionOngoing()
{
	return m_numericQuestionOngoing;
}

void Game::setGameId(uint32_t gameId)
{
	m_gameId = gameId;
}

uint32_t Game::getId() const
{
	return m_gameId;
}

std::vector<std::string> Game::getPlayerNames()
{
	std::vector<std::string> names;
	names.resize(m_userToPlayer.size());
	for (auto& [user, player] : m_userToPlayer)
	{
		auto index = m_playerToIndex.at(player.get());
		names[index] = user->getName();
	}
	return names;
}

std::array<ChoiceQuestion::ArrayIndexType, NUMBER_CHOICES / 2> Game::halveOptions(const ChoiceQuestion& question) noexcept
{
	srand(time(NULL));
	ChoiceQuestion::ArrayIndexType correctAnswerIndex = question.getCorrectChoiceIndex();
	std::unordered_set< ChoiceQuestion::ArrayIndexType> incorrectAnswersIndexes;
	for (int i = 0; i < NUMBER_CHOICES; ++i)
	{
		incorrectAnswersIndexes.insert(i);
	}
	incorrectAnswersIndexes.erase(correctAnswerIndex);
	std::array<ChoiceQuestion::ArrayIndexType, NUMBER_CHOICES / 2> remainingChoices;
	remainingChoices[0] = correctAnswerIndex;
	for (int i = 1; i < NUMBER_CHOICES / 2; ++i)
	{
		ChoiceQuestion::ArrayIndexType position = rand() % incorrectAnswersIndexes.size();
		remainingChoices[i] = *std::next(incorrectAnswersIndexes.begin(), position);
		incorrectAnswersIndexes.erase(remainingChoices[i]);
	}
	return remainingChoices;
}

numericAnswer Game::approximateAnswer(const NumericalQuestion& question) noexcept
{
	srand(time(NULL));
	int difficulty = question.getDifficultyValue();
	numericAnswer estimatedAnswer{};
	numericAnswer correctAnswer = question.getAnswer();
	int smallInterval{};
	int mediumInterval{};
	int largeInterval{};
	const int EASY_SMALL_INTERVAL = 500;
	const int EASY_MEDIUM_INTERVAL = 300;
	const int EASY_LARGE_INTERVAL = 150;
	const int MEDIUM_SMALL_INTERVAL = 200;
	const int MEDIUM_MEDIUM_INTERVAL = 150;
	const int MEDIUM_LARGE_INTERVAL = 100;
	const int HARD_SMALL_INTERVAL = 100;
	const int HARD_MEDIUM_INTERVAL = 50;
	const int HARD_LARGE_INTERVAL = 30;

	switch (difficulty)
	{
	case static_cast<int>(ChoiceQuestion::Difficulty::Easy):
		smallInterval = (correctAnswer / EASY_SMALL_INTERVAL + 1) * 2;
		mediumInterval = (correctAnswer / EASY_MEDIUM_INTERVAL + 1) * 2;
		largeInterval = (correctAnswer / EASY_LARGE_INTERVAL + 1) * 2;
		estimatedAnswer = calculateApproximation(correctAnswer, smallInterval, mediumInterval, largeInterval);
		break;
	case static_cast<int>(ChoiceQuestion::Difficulty::Medium):
		smallInterval = (correctAnswer / MEDIUM_SMALL_INTERVAL + 1) * 2;
		mediumInterval = (correctAnswer / MEDIUM_MEDIUM_INTERVAL + 1) * 2;
		largeInterval = (correctAnswer / MEDIUM_LARGE_INTERVAL + 1) * 2;
		estimatedAnswer = calculateApproximation(correctAnswer, smallInterval, mediumInterval, largeInterval);
		break;
	case static_cast<int>(ChoiceQuestion::Difficulty::Hard):
		smallInterval = (correctAnswer / HARD_SMALL_INTERVAL + 1) * 2;
		mediumInterval = (correctAnswer / HARD_MEDIUM_INTERVAL + 1) * 2;
		largeInterval = (correctAnswer / HARD_LARGE_INTERVAL + 1) * 2;
		estimatedAnswer = calculateApproximation(correctAnswer, smallInterval, mediumInterval, largeInterval);
		break;
	}
	return estimatedAnswer;
}

numericAnswer Game::calculateApproximation(numericAnswer correctAnswer, int smallestInterval, int mediumInterval, int largeInterval) noexcept
{
	srand(time(NULL));
	int probability = rand() % 100 + 1;
	if (probability < QUESTION_FIRST_THRESHOLD) [[likely]] {
		return std::labs(correctAnswer + rand() % (smallestInterval + 1) - (smallestInterval / 2));
	}
	else if (probability < QUESTION_SECOND_THRESHOLD) [[unlikely]] {
		return std::labs(correctAnswer + rand() % (mediumInterval + 1) - (mediumInterval / 2));
	}
	else [[unlikely]] {
		return std::labs(correctAnswer + rand() % (largeInterval + 1) - (largeInterval / 2));
	}
}

void Game::playerJoined()
{
	++m_nrPlayersJoined;
}

bool Game::allPlayersJoined()
{
	return m_nrPlayersJoined == m_userToPlayer.size();
}

Game::UserConstIterator Game::begin() const
{
	return m_userToPlayer.cbegin();
}

Game::UserConstIterator Game::end() const
{
	return m_userToPlayer.cend();
}

void Game::setStage(GameStage stage)
{
	m_gameStage = stage;
}

Game::GameStage Game::stage() const
{
	return m_gameStage;
}

void Game::newChoiceQuestion()
{
	m_currentChoiceQuestion = m_qMng.getChoiceQuestion();
}

void Game::newNumericalQuestion()
{
	m_currentNumericalQuestion = m_qMng.getNumericalQuestion(false);
}

Player* Game::determineChoiceQuestionWinner(Player* attacker, Player* defender)
{
	if (!attacker->hasGivenAnswer())
		return defender;

	bool attackerCorrect = attacker->getChoiceAnswerIndex() == m_currentChoiceQuestion.getCorrectChoiceIndex();
	bool defenderCorrect = defender->getChoiceAnswerIndex() == m_currentChoiceQuestion.getCorrectChoiceIndex();
	if (!defender->hasGivenAnswer() && attackerCorrect)
		return attacker;
	if (attackerCorrect && defenderCorrect)
		return nullptr;
	else if (attackerCorrect)
		return attacker;
	else if (defenderCorrect)
		return defender;
	else
		return nullptr;
}

ChoiceQuestion Game::getCurrentChoiceQuestion()
{
	return m_currentChoiceQuestion;
}

NumericalQuestion Game::getCurrentNumericalQuestion()
{
	return m_currentNumericalQuestion;
}

Player* Game::getPlayer(std::shared_ptr<User> user)
{
	return m_userToPlayer.at(user).get();
}

std::array<numericAnswer, POSSIBLE_ANSWERS> Game::givePossibleAnswers(const NumericalQuestion& question) noexcept
{
	srand(time(NULL));
	numericAnswer correctAnswer = question.getAnswer();
	std::unordered_set<numericAnswer> setOfAnswers;
	std::array<numericAnswer, POSSIBLE_ANSWERS> possibleAnswers;
	numericAnswer possibleAnswer = correctAnswer;
	size_t i = 0;
	setOfAnswers.insert(possibleAnswer);
	int difficulty = question.getDifficultyValue() + 1;
	while (possibleAnswer > 0 && i < NEGATIVE_ERROR * difficulty)
	{
		--possibleAnswer;
		setOfAnswers.insert(possibleAnswer);
		++i;
	}
	i = 0;
	possibleAnswer = correctAnswer;
	while (i < POSITIVE_ERROR * difficulty)
	{
		++possibleAnswer;
		setOfAnswers.insert(possibleAnswer);
		++i;
	}
	for (int i = 0; i < possibleAnswers.size(); ++i)
	{
		size_t position = rand() % setOfAnswers.size();
		possibleAnswers[i] = *std::next(setOfAnswers.begin(), position);
		setOfAnswers.erase(possibleAnswers[i]);
	}
	return possibleAnswers;
}

std::array<int, NUMBER_CHOICES> Game::calculateProbabilitiesOfChoices(const ChoiceQuestion& question) noexcept
{
	constexpr int PROBABILITY_INCREMENT_EASY = 20;
	constexpr int PROBABILITY_INCREMENT_MEDIUM = 10;
	std::array<int, NUMBER_CHOICES> probabilities;
	ChoiceQuestion::ArrayIndexType correctAnswerIndex = question.getCorrectChoiceIndex();
	std::array<int, NUMBER_CHOICES - 1> probabilitiesIncorrectAnswers;
	int probability = 100;
	float probabilityCorrectAnswer{};
	int difficulty = question.getDifficultyValue();
	switch (difficulty)
	{
		int probabilityEasyQuestion, probabilityMediumQuestion;
	case static_cast<int>(NumericalQuestion::Difficulty::Easy):
		probabilityEasyQuestion = MIN_PROBABILITY_OF_CORRECT_ANSWER + PROBABILITY_INCREMENT_EASY;
		probabilityCorrectAnswer = rand() % (probability - probabilityEasyQuestion) + probabilityEasyQuestion;
		break;
	case static_cast<int>(NumericalQuestion::Difficulty::Medium):
		probabilityMediumQuestion = MIN_PROBABILITY_OF_CORRECT_ANSWER + PROBABILITY_INCREMENT_MEDIUM;
		probabilityCorrectAnswer = rand() % (probability - probabilityMediumQuestion) + probabilityMediumQuestion;
		break;
	case static_cast<int>(NumericalQuestion::Difficulty::Hard):
		probabilityCorrectAnswer = rand() % (probability - MIN_PROBABILITY_OF_CORRECT_ANSWER) + MIN_PROBABILITY_OF_CORRECT_ANSWER;
		break;
	}
	probabilities[correctAnswerIndex] = probabilityCorrectAnswer;
	probability -= probabilityCorrectAnswer;
	float sumIncorrectAnswers{};
	for (int i = 0; i < probabilitiesIncorrectAnswers.size(); ++i)
	{
		probabilitiesIncorrectAnswers[i] = rand() % probability;
		probability -= probabilitiesIncorrectAnswers[i];
		sumIncorrectAnswers += probabilitiesIncorrectAnswers[i];
	}
	probability = 100;
	for (int i = 0; i < probabilitiesIncorrectAnswers.size(); ++i)
	{
		probabilitiesIncorrectAnswers[i] = probabilitiesIncorrectAnswers[i] / sumIncorrectAnswers * (probability - probabilityCorrectAnswer);
	}
	int j = 0;
	for (int i = 0; i < probabilities.size(); ++i)
	{
		if (i == correctAnswerIndex)
		{
			continue;
		}
		probabilities[i] = probabilitiesIncorrectAnswers[j];
		++j;
	}
	return probabilities;
}

void Game::setAllPlayersAction(ClientAction action)
{
	for (auto& [user, player] : m_userToPlayer)
		player->setAction(action);
}

std::vector<Player*> Game::determineAnswerCorrectnessOrder()
{
	std::vector<Player*> players;
	// put players associated with user in the vector (unsorted)
	for (const auto& [user, player] : m_userToPlayer)
		players.push_back(player.get());

	// sort players with a custom comparator
	std::sort(players.begin(), players.end(), AnswerOrderComparator(&m_currentNumericalQuestion));

	return players;
}

std::vector<Player*> Game::randomOrder()
{
	std::vector<Player*> shuffledPlayers = m_players;
	std::shuffle(shuffledPlayers.begin(), shuffledPlayers.end(), m_generator.engine());
	return shuffledPlayers;
}

std::vector<uint64_t> Game::selectableTerritories(Player* player)
{
	std::vector<uint64_t> territories;
	if (stage() == GameStage::BASE_SELECTION)
	{
		auto v = m_map.selectableBaseTerritories(player);
		territories.assign(v.begin(), v.end());
	}
	else if (stage() == GameStage::EXPANSION) {
		auto v = m_map.selectableUnoccupiedTerritories(player);
		territories.assign(v.begin(), v.end());
	}
	else if (stage() == GameStage::DUELING) {
		auto v = m_map.attackableTerritories(player);
		territories.assign(v.begin(), v.end());
	}
	return territories;
}

bool Game::isStarted() const
{
	return m_gameStarted;
}

void Game::setResponseDeadline(uint64_t duration)
{

	auto curTime = std::chrono::system_clock::now();
	auto answerTime = std::chrono::seconds::duration(duration);

	auto expireTimeStamp = curTime + answerTime;

	m_expireTime = std::chrono::system_clock::to_time_t(expireTimeStamp);
}

uint64_t Game::getResponseDeadline()
{
	return m_expireTime;
}

size_t Game::nrPlayers() const
{
	return m_userToPlayer.size();
}

size_t Game::randomBaseTerritoryIndex(Player* player)
{
	auto unoccupiedTerritories = m_map.selectableBaseTerritories(player);

	uint64_t randomIdx = m_generator.generate(0, unoccupiedTerritories.size() - 1);

	return unoccupiedTerritories[randomIdx];
}

size_t Game::randomUnoccupiedTerritoryIndex(Player* player)
{
	auto selectableTerritories = m_map.selectableUnoccupiedTerritories(player);

	uint64_t randomIdx = m_generator.generate(0, selectableTerritories.size() - 1);

	std::set<size_t>::iterator idxIterator = selectableTerritories.begin();
	std::advance(idxIterator, randomIdx);

	return *idxIterator;
}

size_t Game::randomNeighbouringTerritoryIndex(Player* player)
{
	auto neighbouringTerritories = m_map.attackableTerritories(player);

	uint64_t randomIdx = m_generator.generate(0, neighbouringTerritories.size() - 1);

	std::set<size_t>::iterator idxIterator = neighbouringTerritories.begin();
	std::advance(idxIterator, randomIdx);

	return *idxIterator;
}

void Game::setBase(model::Territory& territory, Player* player)
{
	updateTerritory(territory, player, BASE_SCORE);
	player->setBase(&territory);
}

void Game::updateTerritory(model::Territory& territory, Player* player, int32_t value)
{
	m_updatedTerritory = &territory;
	m_map.setOwnerOf(territory, player);
	m_map.setScoreOf(territory, territory.score() + value);
	player->updatePoints(value);
}

model::Territory* Game::getUpdatedTerritory()
{
	return m_updatedTerritory;
}

size_t Game::getOwnerIndex(model::Territory& territory)
{
	Player* player = territory.m_owner;
	return m_playerToIndex.at(player);
}

Game::~Game()
{

}

void Game::addUser(std::shared_ptr<User> user)
{
	std::unique_ptr<Player> player = std::make_unique<Player>();
	m_playerToIndex.insert({ player.get(), m_userToPlayer.size() });
	m_userToPlayer.insert({ user, std::move(player) });
}

void Game::removeUser(std::shared_ptr<User> user)
{
	auto player = getPlayer(user);
	m_userToPlayer.erase(user);
	m_playerToIndex.erase(player);
}
