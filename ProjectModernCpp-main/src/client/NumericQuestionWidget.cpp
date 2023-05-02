#include "NumericQuestionWidget.h"
#include "QIntValidator"
#include "QTimer"

#include <chrono>

NumericQuestionWidget::NumericQuestionWidget(net::NumericQuestionInfo questionInfo, QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QIntValidator* intValidator = new QIntValidator(0, INT_MAX, this);
	ui.answerLineEdit->setValidator(intValidator);
	connect(ui.sendAnswer, &QPushButton::clicked, [this]() { onSendAnswerClicked(ui.answerLineEdit->text().toUShort()); });
	connect(ui.approximateAnswer, &QPushButton::clicked, this, &NumericQuestionWidget::onApproximateAnswerClicked);
	connect(ui.givePossibleAnswers, &QPushButton::clicked, this, &NumericQuestionWidget::onGivePossibleAnswersClicked);

	ui.questionStatement->setText(QString::fromLatin1(questionInfo.question));
	progressBar = ui.timerProgressBar;


	auto clockLambda = [this]()
	{
		int newValue = progressBar->value() - 1;
		progressBar->setValue(newValue);
		if (newValue == 0)
		{
			emit timeExpired();
		}
	};


	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, clockLambda);
	auto currentTime = std::chrono::system_clock::now();

	auto timeDifference = static_cast<int>(questionInfo.expireTime - std::chrono::system_clock::to_time_t(currentTime));

	qDebug() << timeDifference;

	progressBar->setRange(0, timeDifference * 10);
	progressBar->setValue(timeDifference * 10);

	timer->start(100);

}

void NumericQuestionWidget::approximateAnswer(numericAnswer approximateAnswer)
{
	ui.answerLineEdit->setText(QString::number(approximateAnswer));
}

void NumericQuestionWidget::givePossibleAnswers(const std::array<numericAnswer, POSSIBLE_ANSWERS>& possibleOptions)
{
	QLabel* label = new QLabel(this);
	QString str = "Possible answers: ";
	for (int i = 0; i < POSSIBLE_ANSWERS - 1; ++i)
	{
		str += QString::number(possibleOptions[i]);
		str += ", ";
	}
	str += QString::number(possibleOptions[POSSIBLE_ANSWERS - 1]);
	label->setText(str);
	label->setStyleSheet("QLabel { color: white; }");
	ui.verticalLayout->addWidget(label);
}

void NumericQuestionWidget::onSendAnswerClicked(numericAnswer answer)
{
	emit sendAnswerClicked(answer);
}

void NumericQuestionWidget::onApproximateAnswerClicked()
{
	emit approximateAnswerClicked();
}

void NumericQuestionWidget::onGivePossibleAnswersClicked()
{
	emit givePossibleAnswersClicked();
}

NumericQuestionWidget::~NumericQuestionWidget()
{

}
