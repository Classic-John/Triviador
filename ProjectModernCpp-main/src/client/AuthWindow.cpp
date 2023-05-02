#include "AuthWindow.h"
#include "GameWindow.h"

#include "LoginPage.h"
#include "RegistrationPage.h"

#include "Definitions.h"

#include <iostream>

#include "QPushButton"
#include "QStringList"
#include <qmessagebox.h>
#include <QInputDialog>

#include "../net_common/net_client.h"

AuthWindow::AuthWindow(QWidget* parent)
	: QMainWindow(parent), m_stackedWidget(new QStackedWidget), m_enteredAddress(defaultIpAndPort), m_changeServerInformation(new QPushButton("Change IP and port", this))
{
	ui.setupUi(this);
	this->setGeometry(500, 70, 450, 450);

	connect(m_changeServerInformation, &QPushButton::clicked, this, &AuthWindow::onChangeServerInformationClicked);

	QPixmap bkgnd("res/background.jpg");
	bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatioByExpanding);
	QPalette palette;
	palette.setBrush(QPalette::Window, bkgnd);
	this->setPalette(palette);

	LoginPage* loginPage = new LoginPage(&m_pages);
	m_pages["login"] = loginPage;
	connect(loginPage, &LoginPage::clickedLogin, this, &AuthWindow::sendLoginInfo);

	RegistrationPage* registrationPage = new RegistrationPage(&m_pages);
	m_pages["registration"] = registrationPage;
	connect(registrationPage, &RegistrationPage::registerClicked, this, &AuthWindow::sendRegistrationInfo);

	for (auto it = m_pages.begin(); it != m_pages.end(); ++it)
	{
		m_stackedWidget->addWidget(it.value());
	}
	m_stackedWidget->setCurrentWidget(m_pages["login"]);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_stackedWidget);
	layout->addWidget(m_changeServerInformation);
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);
	m_changeServerInformation->setWindowModality(Qt::WindowModal);
}

void AuthWindow::onChangeServerInformationClicked()
{
	bool ok;

	QString text = QInputDialog::getText(this, tr("Change server information"),
		tr("Ip and port"), QLineEdit::Normal,
		tr(m_enteredAddress.c_str()), &ok);
	
	if(ok)
		m_enteredAddress = text.toUtf8().data();
}

void AuthWindow::sendLoginInfo(std::string username, std::string password)
{
	qDebug() << "Login info sent:";
	std::cout << username << '\n' << password << '\n';

	std::unique_ptr<net::ClientInterface> clientInterface = std::make_unique<net::ClientInterface>();

	if (clientInterface->AuthenticateAndConnect(m_enteredAddress, net::LoginInfo{ username, password }))
	{
		QMessageBox::information(this, "Success", QString("Welcome back"));

		GameWindow* gameWindow = new GameWindow(std::move(clientInterface), std::move(username));
		this->close();
		gameWindow->show();

		return;
	}

	QMessageBox::critical(this, "Error", "Failed to log in");
}

AuthWindow::~AuthWindow()
{

}

void AuthWindow::sendRegistrationInfo(std::string username, std::string email, std::string password)
{
	qDebug() << "Registration info sent:";
	std::cout << username << email << password;

	std::unique_ptr<net::ClientInterface> clientInterface = std::make_unique<net::ClientInterface>();

	if (clientInterface->Register(m_enteredAddress, net::RegisterInfo{std::move(username), std::move(email), std::move(password) }))
	{
		QMessageBox::information(this, "Success", QString("Successfully registered"));
		return;
	}

	QMessageBox::critical(this, "Error", "Failed to register");
}
