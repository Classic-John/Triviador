#pragma once

#include <QWidget>
#include <vector>
#include <qlistview>
#include <QVBoxLayout>
#include <qstringlistmodel>
#include <thread>
#include <mutex>

#include <memory>

#include <cpr/cpr.h>

#include "ui_WaitingRoomPage.h"


class WaitingRoomPage : public QWidget
{
	Q_OBJECT

public:
	WaitingRoomPage(int roomCode, QWidget* parent = nullptr);

	void setRoomNumber(int roomNr);

	~WaitingRoomPage();

public slots:
	void onStartGameClicked();
	void onGoBackClicked();

	void updatePlayers(std::vector<std::string> players);
signals:
	void backClicked();
	void startClicked();
private:
	Ui::WaitingRoomPageClass ui;
	int m_roomNr;
};
