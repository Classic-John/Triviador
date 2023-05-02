#pragma once

#include <QWidget>
#include "ui_ProfilePage.h"
#include "qstackedwidget"

class ProfilePage : public QWidget
{
	Q_OBJECT

public:
	ProfilePage(QMap<QString, QWidget*>* pages, QWidget *parent = nullptr);

    ~ProfilePage();

public slots:
	void onGoBackClicked();


private:
	Ui::ProfilePageClass ui;
	QMap<QString, QWidget*>* m_pages;
};
