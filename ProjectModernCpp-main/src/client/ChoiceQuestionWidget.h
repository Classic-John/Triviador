#pragma once

#include <QWidget>
#include "ui_ChoiceQuestionWidget.h"
#include "model/ChoiceQuestion.h"
#include "net_common/MessageTypes.h"

class ChoiceQuestionWidget : public QWidget
{
	Q_OBJECT

public:
	ChoiceQuestionWidget(net::ChoiceQuestionInfo info, QWidget *parent = nullptr);

	void halveOptions(std::array<ChoiceQuestion::ArrayIndexType, NUMBER_CHOICES / 2> remainingOptions);

	~ChoiceQuestionWidget();
public slots:
	void onHalveOptionsClicked();
	void onSendOptionClicked(int option);
signals:
	void halveOptionsClicked();
	void sendOptionClicked(int option);
private:
	Ui::ChoiceQuestionWidgetClass ui;
	QMap<int, QPushButton*> options;
	QProgressBar* progressBar = new QProgressBar;
};
