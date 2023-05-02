#include "algorithm"

#include "ChoiceQuestionWidget.h"
#include "QTimer"

ChoiceQuestionWidget::ChoiceQuestionWidget(net::ChoiceQuestionInfo choiceQuestion, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	options[0] = ui.option1;
	options[1] = ui.option2;
	options[2] = ui.option3;
	options[3] = ui.option4;

	connect(ui.halveOptions, &QPushButton::clicked, this, &ChoiceQuestionWidget::onHalveOptionsClicked);
	connect(ui.option1, &QPushButton::clicked, [this]() { onSendOptionClicked(0); });
	connect(ui.option2, &QPushButton::clicked, [this]() { onSendOptionClicked(1); });
	connect(ui.option3, &QPushButton::clicked, [this]() { onSendOptionClicked(2); });
	connect(ui.option4, &QPushButton::clicked, [this]() { onSendOptionClicked(3); });

	progressBar = ui.timerProgressBar;
	ChoiceQuestion::ChoiceArrayType choices = choiceQuestion.answers;
	std::string statement = choiceQuestion.question;
	for (int i = 0; i < NUMBER_CHOICES; ++i)
	{
		options[i]->setText(QString::fromLatin1(choices[i]));
	}
	ui.questionText->setText(QString::fromLatin1(statement));

	progressBar->setRange(0, 100);
	progressBar->setValue(100);

	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [this]()
		{
			int newValue = progressBar->value() - 1;
	progressBar->setValue(newValue);
	if (newValue == 0) {
		qDebug() << "Nagydarab egy buzi fasz";
	}
		});
	timer->start(100);
}

void ChoiceQuestionWidget::halveOptions(std::array<ChoiceQuestion::ArrayIndexType, NUMBER_CHOICES / 2> remainingOptions)
{
	std::array<ChoiceQuestion::ArrayIndexType, NUMBER_CHOICES> allOptions;
	for (int i = 0; i < NUMBER_CHOICES; ++i)
	{
		allOptions[i] = i;
	}
	std::vector<uint16_t> missingElements;
	std::sort(remainingOptions.begin(), remainingOptions.end());
	std::set_difference(allOptions.begin(), allOptions.end(), remainingOptions.begin(), remainingOptions.end(),
		std::inserter(missingElements, missingElements.begin()));
	for (int i = 0; i < missingElements.size(); ++i)
	{
		options[missingElements[i]]->setEnabled(false);
	}
}



void ChoiceQuestionWidget::onHalveOptionsClicked()
{
	emit halveOptionsClicked();
}

ChoiceQuestionWidget::~ChoiceQuestionWidget()
{

}

void ChoiceQuestionWidget::onSendOptionClicked(int option)
{
	emit sendOptionClicked(option);
}
