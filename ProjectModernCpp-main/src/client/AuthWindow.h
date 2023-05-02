#pragma once


#include "ui_AuthWindow.h"

#include <QtWidgets/QMainWindow>
#include <QMap>
#include "QPushButton"
#include <qstackedwidget.h>

class AuthWindow : public QMainWindow
{
    Q_OBJECT

public:
    AuthWindow(QWidget *parent = nullptr);

    void onChangeServerInformationClicked();

    ~AuthWindow();

public slots:
    void sendLoginInfo(std::string, std::string);

    // TODO: fix QInputDialog
    void sendRegistrationInfo(std::string, std::string, std::string);

private:
    std::string m_enteredAddress;

    QPushButton* m_changeServerInformation;
    Ui::AuthWindowClass ui;
    QMap<QString, QWidget*> m_pages;
    QStackedWidget* m_stackedWidget;
};
