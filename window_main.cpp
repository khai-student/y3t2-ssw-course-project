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
	connect(this, SIGNAL(querySvg(QString)), dialog, SLOT(querySvg(QString)));
	connect(dialog, SIGNAL(sendSvg(QString)), this, SLOT(receiveSvg(QString)));
	connect(this, SIGNAL(queryInfo()), dialog, SLOT(queryInfo()));
	connect(dialog, SIGNAL(redraw()), this, SLOT(on_redrawButton_clicked()));

	ui->view->addWidget(viewLabel);
	emit queryChart();
	ui->view->addWidget(viewChart);
	ui->view->setLayout(new QGridLayout());

	on_redrawButton_clicked();
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
		emit querySvg(svgPath);
	}
	else
	{
		emit queryChart();
	}
	emit queryInfo();
	showInfo();
}

void WindowMain::on_actionCommandWindow_triggered()
{
	dialog->setHidden(false);
	dialog->raise();
	emit chooseDialogTab(DialogTab::Commands);
}

void WindowMain::on_actionSettingsWindow_triggered()
{
	dialog->setHidden(false);
	dialog->raise();
	emit chooseDialogTab(DialogTab::Settings);
}

void WindowMain::on_actionLogWindow_triggered()
{
	dialog->setHidden(false);
	dialog->raise();
	emit chooseDialogTab(DialogTab::Log);
}

void WindowMain::on_viewTypeBinaryTree_clicked()
{
	ui->view->setCurrentIndex(ui->view->indexOf(viewLabel));
	on_redrawButton_clicked();
}

void WindowMain::on_viewTypeHorizontalBar_clicked()
{
	ui->view->setCurrentIndex(ui->view->indexOf(viewChart));
	on_redrawButton_clicked();
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
		QImage image(viewLabel->width(), viewLabel->height(), QImage::Format_ARGB32);
		image.fill(0xfffffffaa);  // partly transparent red-ish background

		// Get QPainter that paints to the image
		QPainter painter(&image);
		renderer.render(&painter);

		QPixmap pixmap;
		pixmap = pixmap.fromImage(image);
		viewLabel->setPixmap(pixmap.scaled(ui->view->width(), ui->view->height(), Qt::KeepAspectRatio));
	}
	catch (QResultStatus resultStatus)
	{
		return resultStatus;
	}
	return QResult_Success;
}

void WindowMain::redraw()
{
	if (ui->viewTypeBinaryTree->isChecked())
	{
		svgToLabel();
	}
}

void WindowMain::showMessage(const QString& string)
{
	ui->statusBar->showMessage(string, MESSAGE_TIMEOUT);
}

void WindowMain::receiveChart(QChartView* chartView)
{
	ui->view->removeWidget(viewChart);
	if (viewChart != nullptr) delete viewChart;
	viewChart = chartView;
	ui->view->addWidget(viewChart);
	ui->view->setCurrentIndex(ui->view->indexOf(viewChart));
	ui->view->setLayout(new QGridLayout());
	redraw();
}

void WindowMain::receiveSvg(const QString& filePath)
{
	if (filePath != svgPath)
	{
		viewLabel->setText("SVG creation failed.");
	}
	else
	{
		redraw();
	}
}

void WindowMain::on_actionAboutQt_triggered()
{
	QMessageBox::aboutQt(this, "About Qt5");
}

void WindowMain::on_actionAboutProgram_triggered()
{
	QMessageBox::information(this, "About the program", "Buddy Memory Management Algorithm Simulator is written by Yuriy Vlasov as the course project in 2017.");
}

void WindowMain::on_actionExit_triggered()
{
	exit(0);
}

void WindowMain::showInfo()
{
	ui->progressBar->setRange(0, 10000);
	ui->progressBar->setValue(MemoryInfo::usedPercent * 10000);
	ui->spaceInUseLabel->setText(MemoryInfo::usedMemory);
	ui->blockMinSizeLabel->setText(MemoryInfo::smallestBlockSize);
	ui->blocksQuantityLabel->setText(QString::number(MemoryInfo::blocksQuantity));
}
