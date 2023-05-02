#pragma once

#include <QWidget>
#include "ui_GameStartPage.h"
#include "WaitingRoomPage.h"
#include "ProfilePage.h"
#include "QMap"

class MainMenuPage : public QWidget
{
	Q_OBJECT

public:
	MainMenuPage(QWidget *parent = nullptr);
	~MainMenuPage();

public slots:
	void onJoinGameClicked();
	void onCreateGameClicked();
	void onViewHistoryClicked();
	void onLogOutClicked();

signals:
	void joinClicked(uint32_t);
	void createClicked();
	void historyClicked();
	void logOut();

private:
	Ui::GameStartPageClass ui;
};
