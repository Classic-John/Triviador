#pragma once

#include <QWidget>
#include <QProgressBar>
#include "ui_TerritorySelectionWidget.h"
#include <QTimer>

class TerritorySelectionWidget : public QWidget
{
	Q_OBJECT

public:
	TerritorySelectionWidget(QWidget *parent = nullptr);
	~TerritorySelectionWidget();

	void startTimer();
	void stopTimer();
	bool isRunning();
signals:
	void timeExpired();

private:
	Ui::TerritorySelectionWidgetClass ui;
	QProgressBar* progressBar = new QProgressBar;
	QTimer timer;
};
