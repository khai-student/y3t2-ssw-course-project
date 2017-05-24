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
	connect(dialog, SIGNAL(eventMessage(QString)), this, SLOT(showMessage(QString)));
	connect(this, SIGNAL(queryChart()), dialog, SLOT(queryChart()));
	connect(dialog, SIGNAL(sendChart(QChartView*)), this, SLOT(receiveChart(QChartView*)));

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
	if (ui->viewTypeBinaryTree->isChecked())
	{
		svgToLabel();
	}
	else
	{
		emit queryChart();
	}
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

QResultStatus WindowMain::svgToLabel()
{
	try
	{
		if (!QFile(svgPath).exists()) {
			dialog->printMessage("Cannot open SVG with graph image.");
			throw QResult_Failure;
		}
		// Load your SVG
		QSvgRenderer renderer(svgPath);
		// Prepare a QImage with desired characteritisc
		QImage image(ui->label->width(), ui->label->height(), QImage::Format_ARGB32);
		image.fill(0xfffffffaa);  // partly transparent red-ish background

		// Get QPainter that paints to the image
		QPainter painter(&image);
		renderer.render(&painter);

		QPixmap pixmap;
		pixmap = pixmap.fromImage(image);
		viewLabel->setPixmap(pixmap.scaled(viewLabel->width(), viewLabel->height(), Qt::KeepAspectRatio));
	}
	catch (QResultStatus resultStatus)
	{
		return resultStatus;
	}
	return QResult_Success;
}

void WindowMain::showMessage(const QString& string)
{
	ui->statusBar->showMessage(string, MESSAGE_TIMEOUT);
}

void WindowMain::receiveChart(QChartView* chartView)
{
	if (viewChart != nullptr) delete viewChart;
	this->viewChart = chartView;
}
