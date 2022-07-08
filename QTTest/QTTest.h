#pragma once

#include <QtWidgets/QWidget>
#include "ui_QTTest.h"

class QTTest : public QWidget
{
	Q_OBJECT

public:
	QTTest(QWidget *parent = Q_NULLPTR);

private:
	Ui::QTTestClass ui;
};
