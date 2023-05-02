#include "GameWindow.h"
#include "GameStartPage.h"
#include "AuthWindow.h"
#include "GameDisplay.h"
#include "WaitingRoomPage.h"
#include <qmessagebox.h>

GameWindow::GameWindow(std::unique_ptr<net::ClientInterface> clientInterface, std::string username, QWidget* parent)
	: QMainWindow(parent), m_stackedWidget(new QStackedWidget), m_interface(std::move(clientInterface)), m_username(std::move(username))
{
	ui.setupUi(this);
	this->setGeometry(300, 30, 900, 800);
	QPixmap bkgnd("res/Romania.png");
	bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
	QPalette palette;
	palette.setBrush(QPalette::Window, bkgnd);
	this->setPalette(palette);

	MainMenuPage* mainMenu = new MainMenuPage;
	connect(mainMenu, &MainMenuPage::joinClicked, this, &GameWindow::sendRoomInfo);
	connect(mainMenu, &MainMenuPage::createClicked, this, &GameWindow::requestRoom);
	connect(mainMenu, &MainMenuPage::historyClicked, this, &GameWindow::requestHistory);
	connect(mainMenu, &MainMenuPage::logOut, this, &GameWindow::logOut);
	connect(this, &GameWindow::serverError, this, &GameWindow::returnToAuth);

	m_pages["mainMenu"] = mainMenu;

	m_stackedWidget->addWidget(mainMenu);
	m_stackedWidget->setCurrentWidget(mainMenu);
	setCentralWidget(m_stackedWidget);

	auto returnLambda = [this]() {emit serverError(); };
	m_interface->setOnDisconnectCallback(returnLambda);
}

void GameWindow::startWaitingRoom(uint32_t roomCode)
{
	WaitingRoomPage* waitingRoom = new WaitingRoomPage(roomCode);
	connect(waitingRoom, &WaitingRoomPage::backClicked, this, &GameWindow::leaveRoom);
	connect(this, &GameWindow::updatePlayerList, waitingRoom, &WaitingRoomPage::updatePlayers);
	connect(waitingRoom, &WaitingRoomPage::startClicked, this, &GameWindow::sendStartRequest);

	connect(this, &GameWindow::gameStarted, this, &GameWindow::initGame);

	m_pages["waitingRoom"] = waitingRoom;

	m_stackedWidget->addWidget(waitingRoom);
	m_stackedWidget->setCurrentWidget(waitingRoom);

	auto requestLambda = [this](std::optional<net::WaitingRoomInfo> roomInfo) 
	{
		if (roomInfo)
		{
			if (roomInfo->gameStarted)
			{
				emit gameStarted();
			}
			else if (roomInfo->changed)
			{
				emit updatePlayerList(std::move(roomInfo->players));
			}
		}
	};

	m_interface->RequestRoomInfo(requestLambda);

}

void GameWindow::returnToAuth()
{
	QMessageBox::critical(this, "Error", "Connection lost: Returning to Login page...");
	AuthWindow* authWindow = new AuthWindow;
	this->close();
	authWindow->show();
}

void GameWindow::requestRoom()
{
	std::optional<net::InitRoomInfo> roomInfo = m_interface->CreateRoom();
	if (roomInfo)
	{
		startWaitingRoom(roomInfo->roomCode);
		qDebug() << "Creating Room...";
	}
	else
		;// TODO handle this case
}

void GameWindow::requestHistory()
{
	auto history = m_interface->RequestHistory(m_username);
	// TODO populate history page with data

	ProfilePage* profilePage = new ProfilePage(&m_pages);

	m_pages["profilePage"] = profilePage;

	m_stackedWidget->addWidget(profilePage);
	m_stackedWidget->setCurrentWidget(profilePage);
}

void GameWindow::leaveRoom()
{
	m_interface->LeaveRoom();
	m_stackedWidget->removeWidget(m_pages["waitingRoom"]);
}

void GameWindow::sendStartRequest()
{ 
	bool gameStartable = m_interface->StartGame();
	if (!gameStartable)
	{
		QMessageBox::critical(this, "Error", "Room could not be started");
		return;
	}
}

void GameWindow::initGame()
{

	std::optional<net::InitGameInfo> gameInfo = m_interface->RequestGameInfo();

	if (!gameInfo)
	{
		QMessageBox::critical(this, "Error", "Game could not be started");
		return;
	}

	GameDisplay* gameDisplay = new GameDisplay(std::move(m_interface), *gameInfo);

	m_pages["gameDisplay"] = gameDisplay;

	m_stackedWidget->addWidget(gameDisplay);
	m_stackedWidget->setCurrentWidget(gameDisplay);
}

void GameWindow::sendRoomInfo(uint32_t roomCode)
{
	if (m_interface->JoinRoom(roomCode))
	{
		startWaitingRoom(roomCode);
		return;
	}
	QMessageBox::critical(this, "Error", "Some error on join");
}

void GameWindow::logOut()
{
	this->close();
}

GameWindow::~GameWindow()
{

}
