#include <qmessagebox.h>

#include "RegistrationPage.h"
#include "../common/Authentication.h"

#include <cpr/cpr.h>

RegistrationPage::RegistrationPage(QMap<QString, QWidget*>* pages, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.back, &QPushButton::clicked, this, &RegistrationPage::onBackClicked);
	connect(ui.toolButtonSignUp, &QPushButton::clicked, this, &RegistrationPage::onSignUpClicked);

	m_pages = pages;
}

QString RegistrationPage::getEmail() const noexcept
{
	QString email = ui.emailLineEdit->text();
	return email;
}

QString RegistrationPage::getUsername() const noexcept
{
	QString username = ui.usernameLineEdit->text();
	return username;
}

QString RegistrationPage::getPassword() const noexcept
{
	QString password = ui.passwordLineEdit->text();
	return password;
}

QString RegistrationPage::getPasswordConfirmation() const noexcept
{
	QString password = ui.passwordConfirmationLineEdit->text();
	return password;
}

void RegistrationPage::onBackClicked()
{
	QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(parentWidget());
	stackedWidget->setCurrentWidget(m_pages->value("login"));
}

void RegistrationPage::onSignUpClicked()
{
	std::string username = getUsername().toUtf8().constData();
	std::string email = getEmail().toUtf8().constData();
	std::string password = getPassword().toUtf8().constData();

	if (!Authentication::checkEmail(email.c_str()))
	{
		QMessageBox::critical(this, "Error", "Incorrect email");
		return;
	}
	if (!Authentication::checkPassword(password.c_str()))
	{
		QMessageBox::critical(this, "Error", "Incorrect password. Please make sure that your password fulfils all requirements. " 
			"You can check the requirements if you move the mouse over the password field.");
		return;
	}
	if (getPassword() != getPasswordConfirmation())
	{
		QMessageBox::critical(this, "Error", "Passwords do not match");
		return;
	}

	emit registerClicked(username, email, password);

	/*cpr::Response response = cpr::Post(cpr::Url{ "http://localhost:18080/register/" },
		cpr::Payload{ {"username", username.toUtf8().data()}, {"email", email.toUtf8().data()}, {"password", password.toUtf8().data()} },
		cpr::Redirect{ 10L, true, false, cpr::PostRedirectFlags::NONE });

	if (!response.status_code)
	{
		QMessageBox::critical(this, "Server error", "Unable to communicate with server.");
		return;
	}

	if (response.status_code >= 400)
	{
		QString errorMsg = "Register failed: " + QString::fromUtf8(response.text.data(), response.text.size());
		QMessageBox::critical(this, "Error", errorMsg);
		return;
	}

	QMessageBox::information(this, "Success", QString("Welcome, please log in!"));*/
}

RegistrationPage::~RegistrationPage()
{

}