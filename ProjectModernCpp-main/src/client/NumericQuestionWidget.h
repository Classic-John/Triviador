#pragma once

#include <QWidget>
#include "ui_NumericQuestionWidget.h"
#include "../model/NumericalQuestion.h"

#include "net_common/MessageTypes.h"
#include <QProgressBar>

class NumericQuestionWidget : public QWidget
{
	Q_OBJECT
public:
	NumericQuestionWidget(net::NumericQuestionInfo questionInfo, QWidget *parent = nullptr);

	void approximateAnswer(numericAnswer approximateAnswer);
	void givePossibleAnswers(const std::array<numericAnswer, POSSIBLE_ANSWERS>& possibleAnswers);

	~NumericQuestionWidget();
public slots:
	void onSendAnswerClicked(numericAnswer answer);
	void onApproximateAnswerClicked();
	void onGivePossibleAnswersClicked();
signals:
	void sendAnswerClicked(numericAnswer answer);
	void approximateAnswerClicked();
	void givePossibleAnswersClicked();
	void timeExpired();
private:
	Ui::NumericQuestionWidgetClass ui;
	QProgressBar* progressBar = new QProgressBar;
};
