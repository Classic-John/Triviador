#include "TerritorySelectionWidget.h"
#include "QTimer"

TerritorySelectionWidget::TerritorySelectionWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void TerritorySelectionWidget::startTimer()
{
	progressBar = ui.timerProgressBar;

	auto clockLambda = [this]() {
		int newValue = progressBar->value() - 1;
		progressBar->setValue(newValue);
		if (newValue == 0) {
			qDebug() << "Territory selection time expired";
			stopTimer();
			emit timeExpired();
		}
	};

	connect(&timer, &QTimer::timeout, clockLambda);

	progressBar->setRange(0, 100);
	progressBar->setValue(100);

	timer.start(100);
}

void TerritorySelectionWidget::stopTimer()
{
	hide();
	timer.stop();
}

bool TerritorySelectionWidget::isRunning()
{
	return timer.isActive();
}

TerritorySelectionWidget::~TerritorySelectionWidget()
{

}
