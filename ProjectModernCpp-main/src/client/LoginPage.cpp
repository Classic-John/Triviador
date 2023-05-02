#include "LoginPage.h"
#include "GameStartPage.h"
#include <cpr/cpr.h>
#include "NumericQuestionWidget.h"
#include "GameWindow.h"

LoginPage::LoginPage(QMap<QString, QWidget*>* pages, QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.toolButtonLogin, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
	connect(ui.usernameLineEdit, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);
	connect(ui.passwordLineEdit, &QLineEdit::returnPressed, this, &LoginPage::onLoginClicked);

	connect(ui.checkBoxShowPassword, &QCheckBox::clicked, this, &LoginPage::onShowPasswordClicked);
	connect(ui.dontHaveAccount, &QPushButton::clicked, this, &LoginPage::onDontHaveAccountClicked);
	connect(ui.forgotPassword, &QPushButton::clicked, this, &LoginPage::onForgotPasswordClicked);

	m_pages = pages;
}

QString LoginPage::getUsername() const noexcept
{
	QString username = ui.usernameLineEdit->text();
	return username;
}

QString LoginPage::getPassword() const noexcept
{
	QString password = ui.passwordLineEdit->text();
	return password;
}

void LoginPage::onLoginClicked()
{
	std::string username = getUsername().toUtf8().data();
	std::string password = getPassword().toUtf8().data();

	emit clickedLogin(username, password);
}

void LoginPage::onShowPasswordClicked()
{
	ui.passwordLineEdit->setEchoMode(ui.checkBoxShowPassword->checkState() == Qt::Checked ? QLineEdit::Normal : QLineEdit::Password);
}

void LoginPage::onDontHaveAccountClicked()
{
	QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(parentWidget());
	stackedWidget->setCurrentWidget(m_pages->value("registration"));
}

void LoginPage::onForgotPasswordClicked()
{
	
}

LoginPage::~LoginPage()
{

}
