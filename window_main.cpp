#include "window_main.h"
#include "ui_window_main.h"

WindowMain::WindowMain(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::WindowMain),
	dialog(new DialogSettings(this->window())),
	viewLabel(new QLabel(this->window()))
{
	ui->setupUi(this);
	dialog->show();

	connect(this, SIGNAL(chooseDialogTab(DialogTab)), dialog, SLOT(changeTab(DialogTab)));
	connect(dialog, SIGNAL(eventMessage(QString)), this, SLOT(showMessage(QString));

	ui->view->addWidget(viewLabel);
	ui->view->addWidget(viewChart);

	viewLabel->setText("asdf");
}

WindowMain::~WindowMain()
{
	delete ui;
	delete dialog;
	delete viewLabel;
	delete viewChart;
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

}

void WindowMain::on_actionCommandWindow_triggered()
{
	emit chooseDialogTab(DialogTab::Commands);
}

void WindowMain::on_actionSettingsWindow_triggered()
{
	emit chooseDialogTab(DialogTab::Settings);
}

void WindowMain::on_actionLogWindow_triggered()
{
	emit chooseDialogTab(DialogTab::Log);
}

void WindowMain::on_viewTypeBinaryTree_clicked()
{
	ui->view->setCurrentIndex(ui->view->indexOf(viewLabel));
}

void WindowMain::on_viewTypeHorizontalBar_clicked()
{
	ui->view->setCurrentIndex(ui->view->indexOf(viewChart));
}

void WindowMain::showMessage(const QString& string)
{
	ui->statusBar->showMessage(string, MESSAGE_TIMEOUT);
}
