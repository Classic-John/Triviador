#pragma once

#include <QWidget>
#include <qmessagebox.h>
#include "ui_LoginPage.h"
#include "qstackedwidget.h"
#include "QMap"

class LoginPage : public QWidget
{
	Q_OBJECT

public:
	LoginPage(QMap<QString, QWidget*>* pages, QWidget *parent = nullptr);
	~LoginPage();

signals:
	void clickedLogin(std::string, std::string);

public slots:
	void onLoginClicked();
	void onShowPasswordClicked();
	void onDontHaveAccountClicked();
	void onForgotPasswordClicked();
	
	QString getUsername() const noexcept;
	QString getPassword() const noexcept;
private:
	Ui::LoginPageClass ui;
	QMap<QString, QWidget*>* m_pages;
};
