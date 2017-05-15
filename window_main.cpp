#include "window_main.h"
#include "ui_window_main.h"

WindowMain::WindowMain(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WindowMain)
{
	ui->setupUi(this);
}

WindowMain::~WindowMain()
{
	delete ui;
}

void WindowMain::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void WindowMain::on_redrawButton_clicked()
{
	this->event = new QEvent();
}
