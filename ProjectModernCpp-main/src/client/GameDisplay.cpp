#include "GameDisplay.h"
#include "../model/Map.h"
#include "../model/MapFactory.h"
#include "MapGraphics.h"

#include "model/ClientActions.h"

#include <fstream>

GameDisplay::GameDisplay(std::unique_ptr<net::ClientInterface> clientInterface, net::InitGameInfo gameInfo, QWidget* parent)
	: QWidget(parent), m_interface(std::move(clientInterface))
{
	ui.setupUi(this);
	qDebug() << QDir(".").absolutePath();

	std::string mapName = gameInfo.mapName;
	m_playerNames = std::move(gameInfo.players);
	for (size_t i = 0; i < m_playerNames.size(); ++i)
		m_players.emplace_back();

	std::ifstream fin("res/maps/graphs/" + mapName + ".mapg");
	if (!fin.is_open())
		throw std::exception();

	//ui.gameDisplay->setViewport(new QOpenGLWidget);

	auto mapFactory = model::MapFactory(fin);
	m_map = std::make_shared<model::Map>(mapFactory.createMap());

	fin = std::ifstream("res/maps/graphics/defs/" + mapName + ".mapv");
	auto mapGraphics = std::make_shared<MapGraphics>(fin, "res/maps/graphics");
	mapGraphics->colors = { Qt::red, Qt::green, Qt::blue, Qt::yellow };
	m_mapDisplayModel = new MapDisplayModel;
	m_mapDisplayModel->setMap(m_map, mapGraphics);
	m_mapDisplayModel->setSelectionEnabled(true);
	ui.gameDisplay->setMapDisplayModel(m_mapDisplayModel);

	ui.territoryInfo->hide();

	connect(this, &GameDisplay::numericalQuestionRequested, this, &GameDisplay::requestNumericalQuestion);
	connect(this, &GameDisplay::choiceQuestionRequested, this, &GameDisplay::requestChoiceQuestion);
	connect(this, &GameDisplay::territorySelectionNeeded, this, &GameDisplay::prepareTerritorySelection);
	connect(this, &GameDisplay::territoriesUpdated, this, &GameDisplay::requestTerritoryUpdate);
	// FIXME dont connect until necessary, or check whether send territory is actually needed/possible
	connect(m_mapDisplayModel, &MapDisplayModel::territorySelected, this, &GameDisplay::sendTerritorySelection);

	auto checkAction = [this](std::optional<net::GameActionInfo> info) {
		if (!info)
			return;
		switch (static_cast<ClientAction>(info->action))
		{
		case ClientAction::REQUEST_NUMERICAL_QUESTION:
			//ui.territoryInfo->stopTimer();
			m_receivedTerritorySelectionInfo = false;
			emit numericalQuestionRequested();
			break;
		case ClientAction::REQUEST_CHOICE_QUESTION:
			//ui.territoryInfo->stopTimer();

			m_receivedTerritorySelectionInfo = false;
			emit choiceQuestionRequested();
			break;
		case ClientAction::SELECT_TERRITORY:
			emit territorySelectionNeeded();
			break;
		case ClientAction::UPDATE_TERRITORIES:
			//ui.territoryInfo->stopTimer();
			m_receivedTerritorySelectionInfo = false;
			emit territoriesUpdated();
			break;
		case ClientAction::WAIT:
			break;
		}
	};
	m_interface->StartRequestingClientAction(checkAction);
}

void GameDisplay::displayNumericQuestionWidget(net::NumericQuestionInfo questionInfo)
{
	m_numericQuestionWidget = new NumericQuestionWidget(questionInfo, this);
	ui.formLayout->setWidget(0, QFormLayout::SpanningRole, m_numericQuestionWidget);
	ui.formLayout->setAlignment(m_numericQuestionWidget, Qt::AlignCenter);

	connect(m_numericQuestionWidget, &NumericQuestionWidget::sendAnswerClicked, this, &GameDisplay::sendNumericalAnswer);
	connect(m_numericQuestionWidget, &NumericQuestionWidget::sendAnswerClicked, this, &GameDisplay::removeNumericQuestionWidget);
	connect(m_numericQuestionWidget, &NumericQuestionWidget::timeExpired, this, &GameDisplay::removeNumericQuestionWidget);
}

void GameDisplay::displayChoiceQuestionWidget(net::ChoiceQuestionInfo choiceQuestion)
{
	m_choiceQuestionWidget = new ChoiceQuestionWidget(choiceQuestion, this);
	ui.formLayout->setWidget(0, QFormLayout::SpanningRole, m_choiceQuestionWidget);
	ui.formLayout->setAlignment(m_choiceQuestionWidget, Qt::AlignCenter);

	connect(m_choiceQuestionWidget, &ChoiceQuestionWidget::sendOptionClicked, this, &GameDisplay::sendChoiceAnswer);
	connect(m_choiceQuestionWidget, &ChoiceQuestionWidget::sendOptionClicked, this, &GameDisplay::removeChoiceQuestionWidget);
	connect(m_numericQuestionWidget, &NumericQuestionWidget::timeExpired, this, &GameDisplay::removeChoiceQuestionWidget);
}

void GameDisplay::removeChoiceQuestionWidget()
{
	ui.formLayout->removeWidget(m_choiceQuestionWidget);
	delete m_choiceQuestionWidget;
	m_choiceQuestionWidget = nullptr;
}

void GameDisplay::removeNumericQuestionWidget()
{
	ui.formLayout->removeWidget(m_numericQuestionWidget);
	delete m_numericQuestionWidget;
	m_numericQuestionWidget = nullptr;
}

void GameDisplay::sendTerritorySelection(model::Territory& territory)
{
	m_mapDisplayModel->setSelectionEnabled(false);
	qDebug() << "Selected territory with index " << territory.index();
	m_interface->SendTerritorySelection(territory.index());
	ui.territoryInfo->stopTimer();
}

void GameDisplay::requestTerritoryUpdate()
{
	std::optional<net::TerritoryUpdatedInfo> info = m_interface->RequestTerritoryUpdate();
	if (!info)
		return;

	qDebug() << "Updated " << info->index << " territory with owner " << info->ownerIndex << ", score " << info->score;
	model::Territory& territory = (*m_map)[info->index];
	m_map->setOwnerOf(territory, &m_players[info->ownerIndex]);
	m_map->setScoreOf(territory, info->score);
	m_mapDisplayModel->updateTerritory(territory, info->ownerIndex);
}

void GameDisplay::prepareTerritorySelection()
{
	// TODO show user that they need to select territory
	if (m_receivedTerritorySelectionInfo)
		return;
	m_receivedTerritorySelectionInfo = true;
	auto selectableTerritoriesVec = m_interface->RequestSelectableTerritories();
	if (!selectableTerritoriesVec)
		return;

	for (auto i : *selectableTerritoriesVec)
		qDebug() << i << ' ';
	std::set<size_t> selectableTerritories(selectableTerritoriesVec->begin(), selectableTerritoriesVec->end());
	for (auto i : selectableTerritories)
		qDebug() << i << ' ';
	m_mapDisplayModel->setSelectableTerritories(selectableTerritories);
	m_mapDisplayModel->setSelectionEnabled(true);
	// TODO disconnect after selection
	ui.territoryInfo->show();
	ui.territoryInfo->startTimer();
}

void GameDisplay::initGame()
{
	qDebug() << "Game started...";
}

GameDisplay::~GameDisplay()
{

}

void GameDisplay::requestChoiceQuestion()
{
	auto questionInfo = m_interface->RequestChoiceQuestion();

	//TODO Log Error
	if (!questionInfo)
		return;

	displayChoiceQuestionWidget(*questionInfo);
}

void GameDisplay::requestNumericalQuestion()
{
	auto questionInfo = m_interface->RequestNumericalQuestion();

	//TODO Log Error
	if (!questionInfo)
		return;

	displayNumericQuestionWidget(*questionInfo);
}

void GameDisplay::sendNumericalAnswer(numericAnswer answer)
{
	m_interface->PostNumericalAnswer(net::NumericAnswerInfo{ .answer = answer });
}

void GameDisplay::sendChoiceAnswer(ChoiceQuestion::ArrayIndexType answerIndex)
{
	m_interface->PostChoiceAnswer(net::ChoiceAnswerInfo{ .index = answerIndex });
}
