#include <vector>
#include "QListView"
#include "QStringListModel"
#include "QPushButton"
#include <QVBoxLayout>
#include <QThread>
#include <QListWidget>
#include <qstackedwidget>

#include "WaitingRoomPage.h"
#include "GameDisplay.h"

#include <crow.h>

WaitingRoomPage::WaitingRoomPage(int roomCode, QWidget* parent)
	: QWidget(parent), m_roomNr(roomCode)
{
	ui.setupUi(this);
	connect(ui.startGame, &QPushButton::clicked, this, &WaitingRoomPage::onStartGameClicked);
	connect(ui.goBack, &QPushButton::clicked, this, &WaitingRoomPage::onGoBackClicked);

	QString text = ui.codRoomLabel->text();
	text += QString::number(roomCode);
	ui.codRoomLabel->setText(text);
}

void WaitingRoomPage::setRoomNumber(int roomNr)
{
	m_roomNr = roomNr;
}

void WaitingRoomPage::onStartGameClicked()
{
	emit startClicked();
}

void WaitingRoomPage::onGoBackClicked()
{
	emit backClicked();
}

void WaitingRoomPage::updatePlayers(std::vector<std::string> players)
{
	QListWidget* playerListWidget = ui.playersList;
	playerListWidget->clear();

	for (auto& player : players)
	{
		playerListWidget->addItem(QString::fromLatin1(std::move(player)));
	}
}

WaitingRoomPage::~WaitingRoomPage()
{

}
