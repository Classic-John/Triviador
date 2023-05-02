#pragma once

#include <QWidget>
#include "ui_GameDisplay.h"

#include "net_common/net_client.h"
#include "model/Player.h"

#include "ChoiceQuestionWidget.h"
#include "NumericQuestionWidget.h"
#include "QPushButton"

class GameDisplay : public QWidget
{
	Q_OBJECT

public:
	GameDisplay(std::unique_ptr<net::ClientInterface> clientInterface, net::InitGameInfo gameInfo, QWidget *parent = nullptr);

	void displayNumericQuestionWidget(net::NumericQuestionInfo questionInfo);
	//TODO choiceQuestion
	void displayChoiceQuestionWidget(net::ChoiceQuestionInfo choiceQuestion);
	void removeChoiceQuestionWidget();
	void removeNumericQuestionWidget();

	~GameDisplay();

signals:
	void numericalQuestionRequested();
	void choiceQuestionRequested();
	void territorySelectionNeeded();
	void territoriesUpdated();

public slots:
	void initGame();
	void requestChoiceQuestion();
	void requestNumericalQuestion();
	void sendNumericalAnswer(numericAnswer answer);
	void sendChoiceAnswer(ChoiceQuestion::ArrayIndexType answerIndex);
	void sendTerritorySelection(model::Territory& territory);
	void requestTerritoryUpdate();
	void prepareTerritorySelection();

private:
	Ui::GameDisplayClass ui;
	ChoiceQuestionWidget* m_choiceQuestionWidget = nullptr;
	NumericQuestionWidget* m_numericQuestionWidget = nullptr;
	std::vector<uint64_t> m_selectableTerritories;
	bool m_receivedTerritorySelectionInfo{ false };

	std::unique_ptr<net::ClientInterface> m_interface;
	std::shared_ptr<model::Map> m_map;
	MapDisplayModel* m_mapDisplayModel;

	std::vector<std::string> m_playerNames;
	std::vector<Player> m_players;
};