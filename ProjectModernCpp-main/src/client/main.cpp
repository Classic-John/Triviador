#include "AuthWindow.h"
#include "GameWindow.h"
#include <QtWidgets/QApplication>

#include <iostream>
#include <string>
#include <vector>

#include "../common/Authentication.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	AuthWindow w;
	w.show();
	return a.exec();
}
