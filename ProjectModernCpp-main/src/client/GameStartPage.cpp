#include "GameStartPage.h"
#include "QInputDialog"
#include <QStackedWidget>
#include <qmessagebox.h>
#include <qstackedwidget.h>
#include <qdialog.h>
#include "NumericQuestionWidget.h"
#include "ChoiceQuestionWidget.h"
#include "qdialog"

#include <cpr/cpr.h>

MainMenuPage::MainMenuPage(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.joinGamePushButton, &QPushButton::clicked, this, &MainMenuPage::onJoinGameClicked);
	connect(ui.createGamePushButton, &QPushButton::clicked, this, &MainMenuPage::onCreateGameClicked);
	connect(ui.viewHistoryPushButton, &QPushButton::clicked, this, &MainMenuPage::onViewHistoryClicked);
	connect(ui.logOutPushButton, &QPushButton::clicked, this, &MainMenuPage::onLogOutClicked);
}

void MainMenuPage::onJoinGameClicked()
{
	bool ok;

	QString text = QInputDialog::getText(
		parentWidget(),
		tr("New game"),
		tr("Please enter the code of the room you wish to join"),
		QLineEdit::Normal,
		QString(),
		&ok);

	if (ok)
	{
		auto roomCode = text.toUInt();
		emit joinClicked(roomCode);
	}
}

void MainMenuPage::onCreateGameClicked()
{
	emit createClicked();
}

void MainMenuPage::onViewHistoryClicked()
{
	emit historyClicked();
}

void MainMenuPage::onLogOutClicked()
{
	emit logOut();
}

MainMenuPage::~MainMenuPage()
{

}

//cpr::Response GameStartPage::joinGame(const std::string& roomCode)
//{
//	auto response = cpr::Post(
//		cpr::Url{ "http://localhost:18080/game/join/" },
//		cpr::Payload{ {"room", roomCode}, {"player", m_username} });
//
//	return response;
//}
