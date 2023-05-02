#pragma once

#include "ui_GameWindow.h"

#include "../net_common/net_client.h"

#include <QtWidgets/QMainWindow>
#include "QMap"
#include "qstackedwidget.h"

#include <cpr/cpr.h>

#include <thread>

class GameWindow : public QMainWindow
{
	Q_OBJECT

public:
	GameWindow(std::unique_ptr<net::ClientInterface> clientInterface, std::string username, QWidget *parent = nullptr);
	~GameWindow();

	void startWaitingRoom(uint32_t roomCode);
	void returnToAuth();

public slots:
	void sendRoomInfo(uint32_t);
	void requestRoom();
	void requestHistory();
	void logOut();

	void leaveRoom();
	void sendStartRequest();
	void initGame();
signals:
	void serverError();
	void updatePlayerList(std::vector<std::string>);
	void gameStarted();
private:
	std::unique_ptr<net::ClientInterface> m_interface;
	std::string m_username;

	Ui::GameWindowClass ui;
	QMap<QString, QWidget*> m_pages;
	QStackedWidget* m_stackedWidget;

};
