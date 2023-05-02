#pragma once

#include <QWidget>
#include "ui_RegistrationPage.h"
#include "qstackedwidget.h"

class RegistrationPage : public QWidget
{
	Q_OBJECT

public:
	RegistrationPage(QMap<QString, QWidget*>* m_pages, QWidget *parent = nullptr);

	QString getEmail() const noexcept;
	QString getUsername() const noexcept;
	QString getPassword() const noexcept;
	QString getPasswordConfirmation() const noexcept;

	~RegistrationPage();

signals:
	void registerClicked(std::string, std::string, std::string);

public slots:
	void onBackClicked();
	void onSignUpClicked();

private:
	Ui::RegistrationPageClass ui;
	QMap<QString, QWidget*>* m_pages;
};
