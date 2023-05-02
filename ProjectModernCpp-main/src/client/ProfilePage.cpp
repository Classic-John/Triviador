#include "ProfilePage.h"

ProfilePage::ProfilePage(QMap<QString, QWidget*>* pages, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.goBack, &QPushButton::clicked, this, &ProfilePage::onGoBackClicked);
	m_pages = pages;
}

void ProfilePage::onGoBackClicked()
{
	QStackedWidget* stackedWidget = qobject_cast<QStackedWidget*>(parentWidget());
	stackedWidget->removeWidget(m_pages->value("profilePage"));
}

ProfilePage::~ProfilePage()
{

}
