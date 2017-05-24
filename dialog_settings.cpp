#include "dialog_settings.h"
#include "ui_dialog_settings.h"

DialogSettings::DialogSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogSettings),
	memorySettings(new MemorySettings())
{
	ui->setupUi(this);
}

DialogSettings::~DialogSettings()
{
	delete ui;
	delete memorySettings;
}

void DialogSettings::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}

void DialogSettings::updateSettingsFromObject()
{
	uint8_t minBlockDegree = memorySettings->GetMinBlockDegree();
	uint8_t totalMemoryDegree = memorySettings->GetTotalMemoryDegree();

	ui->minBlockDegreeSlider->setMinimum(1);
	ui->minBlockDegreeSlider->setMaximum(memorySettings->MAX_TOTAL_MEMORY_DEGREE);
	ui->minBlockDegreeSpinBox->setMinimum(1);
	ui->minBlockDegreeSpinBox->setMaximum(memorySettings->MAX_TOTAL_MEMORY_DEGREE);

	ui->totalMemorySlider->setMinimum(1);
	ui->totalMemorySlider->setMaximum(memorySettings->GetTotalMemoryDegree());
	ui->totalMemorySpinBox->setMinimum(1);
	ui->totalMemorySpinBox->setMaximum(memorySettings->GetTotalMemoryDegree());

	ui->stepsExecSpeedSlider->setMinimum(1);
	ui->stepsExecSpeedSlider->setMaximum(uint16_t(memorySettings->MAX_STEPS_EXECUTION_SPEED));
	ui->stepsExecSpeedSpinBox->setMinimum(0.5);
	ui->stepsExecSpeedSpinBox->setMaximum(memorySettings->MAX_STEPS_EXECUTION_SPEED);

	ui->autoSave->setChecked(memorySettings->GetAutoSaveCmds());

	updateLabels();
}

void DialogSettings::updateLabels()
{
	uint8_t minBlockDegree = memorySettings->GetMinBlockDegree();
	uint8_t totalMemoryDegree = memorySettings->GetTotalMemoryDegree();

	ui->minBlockSizeLabel->setText(MemorySettings::DegreeToString(minBlockDegree));
	ui->totalMemoryLabel->setText(MemorySettings::DegreeToString(totalMemoryDegree));
}

void DialogSettings::printMessage(const QString& string)
{
	emit eventMessage(string);
	emit writeLog(string);
}

void DialogSettings::changeTab(DialogTab tab)
{
	QTabWidget *tabWidget = ui->tabWidget;
	switch (tab) {
		case DialogTab::Commands:
			tabWidget->setCurrentIndex(tabWidget->indexOf(ui->tabCommands));
			break;
		case DialogTab::Settings:
			tabWidget->setCurrentIndex(tabWidget->indexOf(ui->tabSettings));
			break;
		case DialogTab::Log:
			tabWidget->setCurrentIndex(tabWidget->indexOf(ui->tabLog));
			break;
		default:
			break;
	}
}

void DialogSettings::writeLog(const QString& string)
{
	ui->log->append(string);
}

void DialogSettings::on_minBlockDegreeSlider_sliderMoved(int position)
{
	if (memorySettings->SetMinBlockDegree(position) != QResult_Success)
	{
		qDebug() << "Cannot update min block degree value.";
	}

	updateSettingsFromObject();
}

void DialogSettings::on_minBlockDegreeSpinBox_valueChanged(int value)
{
	if (memorySettings->SetMinBlockDegree(value) != QResult_Success)
	{
		qDebug() << "Cannot update total memory degree value.";
	}

	updateSettingsFromObject();
}

void DialogSettings::on_totalMemorySlider_sliderMoved(int position)
{
	if (memorySettings->SetTotalMemoryDegree(position) != QResult_Success)
	{
		qDebug() << "Cannot update total memory degree value.";
	}

	updateSettingsFromObject();
}

void DialogSettings::on_totalMemorySpinBox_valueChanged(int value)
{
	if (memorySettings->SetTotalMemoryDegree(value) != QResult_Success)
	{
		qDebug() << "Cannot update total memory degree value.";
	}

	updateSettingsFromObject();
}

void DialogSettings::on_restoreDefaultSettings_clicked()
{
	delete memorySettings;
	memorySettings = new MemorySettings();
	updateSettingsFromObject();
}

void DialogSettings::on_clearLog_clicked()
{
	ui->log->clear();
}

void DialogSettings::on_exportLog_clicked()
{
	// choosing file to export
	QString fileName = QFileDialog::getSaveFileName(this,
								 "Choose file for writing log",
								 QString("Log %1.txt", QDateTime::toString("YYYY-MM-dd HH:mm:ss")),
								 "Text files (*.txt) ;; All files (*.*)");
	// trying to open file
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		printMessage("Cannot open file for writing");
		return;
	}
	// writing table to file
	QTextStream wfstream(&file);
	wfstream << ui->log->toPlainText();
	wfstream.flush();
	// closing file
	file.flush();
	file.close();
}
