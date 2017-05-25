#include "dialog_settings.h"
#include "ui_dialog_settings.h"

DialogSettings::DialogSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogSettings),
	memorySettings(new MemorySettings()),
	processor(new CommandProcessor(memorySettings)),
	execTimer(new QTimer()),
	saveTimer(new QTimer()),
	updateInProgress(false)
{
	ui->setupUi(this);

	hotkeyDeleteCmd = new QShortcut(QKeySequence("Del"), this);
	connect(hotkeyDeleteCmd, SIGNAL(activated()), this, SLOT(on_deleteCmd()));
	connect(execTimer, SIGNAL(timeout()), this, SLOT(on_execNextCmd_clicked()));

	updateSettingsFromObject();
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
	updateInProgress = true;

	ui->minBlockDegreeSlider->setMinimum(1);
	ui->minBlockDegreeSlider->setMaximum(memorySettings->getTotalMemoryDegree());
	ui->minBlockDegreeSlider->setValue(memorySettings->getMinBlockDegree());

	ui->minBlockDegreeSpinBox->setMinimum(1);
	ui->minBlockDegreeSpinBox->setMaximum(memorySettings->getTotalMemoryDegree());
	ui->minBlockDegreeSpinBox->setValue(memorySettings->getMinBlockDegree());

	ui->totalMemorySlider->setMinimum(memorySettings->getMinBlockDegree());
	ui->totalMemorySlider->setMaximum(memorySettings->MAX_TOTAL_MEMORY_DEGREE);
	ui->totalMemorySlider->setValue(memorySettings->getTotalMemoryDegree());

	ui->totalMemorySpinBox->setMinimum(memorySettings->getMinBlockDegree());
	ui->totalMemorySpinBox->setMaximum(memorySettings->MAX_TOTAL_MEMORY_DEGREE);
	ui->totalMemorySpinBox->setValue(memorySettings->getTotalMemoryDegree());

	ui->stepsExecSpeedSlider->setMinimum(1);
	ui->stepsExecSpeedSlider->setMaximum(uint16_t(memorySettings->MAX_STEPS_EXECUTION_SPEED));
	ui->stepsExecSpeedSlider->setValue(memorySettings->getStepsExecutionSpeed());

	ui->stepsExecSpeedSpinBox->setMinimum(0.5);
	ui->stepsExecSpeedSpinBox->setMaximum(memorySettings->MAX_STEPS_EXECUTION_SPEED);
	ui->stepsExecSpeedSpinBox->setValue(memorySettings->getStepsExecutionSpeed());

	ui->autoSave->setChecked(memorySettings->getAutoSaveCmds());

	updateLabels();

	resetProcessor();

	updateInProgress = false;
}

void DialogSettings::updateLabels()
{
	uint8_t minBlockDegree = memorySettings->getMinBlockDegree();
	uint8_t totalMemoryDegree = memorySettings->getTotalMemoryDegree();

	ui->minBlockSizeLabel->setText(MemorySettings::degreeToString(minBlockDegree));
	ui->totalMemoryLabel->setText(MemorySettings::degreeToString(totalMemoryDegree));
}

void DialogSettings::updateCmdsTable()
{
	ui->cmds->clear();
	uint16_t cmdsCount = processor->getCmdsCount();

	QStringList colHeaders, rowHeaders;
	ui->cmds->setColumnCount(3);
	ui->cmds->setRowCount(cmdsCount);

	colHeaders << "Action" << "Process Name" << "Block size";
	for (uint16_t cmdIndex = 0; cmdIndex < cmdsCount; ++cmdIndex)
	{
		ui->cmds->setRowHeight(cmdIndex, 25);
		rowHeaders << QString("%1").arg(cmdIndex + 1);
	}
	ui->cmds->setHorizontalHeaderLabels(colHeaders);
	ui->cmds->setVerticalHeaderLabels(rowHeaders);

	for (uint16_t cmdIndex = 0; cmdIndex < cmdsCount; ++cmdIndex)
	{
		Command* cmd = processor->getCmd(cmdIndex);
		switch (cmd->action)
		{
			case CommandAction::Allocate:
				ui->cmds->setItem(cmdIndex, 0, new QTableWidgetItem("Allocate"));
				break;
			case CommandAction::Free:
				ui->cmds->setItem(cmdIndex, 0, new QTableWidgetItem("Free"));
				break;
			case CommandAction::Query:
				ui->cmds->setItem(cmdIndex, 0, new QTableWidgetItem("Query"));
				break;
		}
		ui->cmds->setItem(cmdIndex, 1, new QTableWidgetItem(cmd->blockName));
		if (cmd->blockSize != 0)
		{
			ui->cmds->setItem(cmdIndex, 2, new QTableWidgetItem(MemorySettings::bytesToString(cmd->blockSize)));
		}
		else
		{
			ui->cmds->setItem(cmdIndex, 2, new QTableWidgetItem(""));
		}
	}

	highlightNextCommand();

	// manage autosave
	if (memorySettings->getAutoSaveCmds())
	{
		saveCmdsToFile();
	}
}

void DialogSettings::highlightNextCommand()
{
	ui->cmds->clearSelection();
	int16_t index = processor->getNextCmdIndex();
	if (index >= 0)
	{
		ui->cmds->selectRow(index);
	}
}

void DialogSettings::saveCmdsToFile()
{
	if (cmdFilePath == "") return;
	// trying to open file
	QFile file(cmdFilePath);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		printMessage("Cannot open file for writing", MessageStatus::Error);
		return;
	}
	// writing cmds to file
	QTextStream wfstream(&file);
	QVector<Command*>* cmds = processor->getAllCmds();
	foreach (Command* cmd, *cmds)
	{
		wfstream << cmd->cmdToStr() << '\n';
	}
	delete cmds;
	wfstream.flush();
	// closing file
	file.flush();
	file.close();
	printMessage(QString("Commands saved to %1.").arg(cmdFilePath), MessageStatus::Info);
}

void DialogSettings::resetProcessor()
{
	execTimer->stop();
	ui->autoExec->setText("Automatic execution");

	QVector<Command*>* cmds = processor->getAllCmds();
	delete processor;
	processor = new CommandProcessor(memorySettings);
	foreach (Command* cmd, *cmds)
	{
		processor->addCmd(cmd);
	}
}

void DialogSettings::printMessage(const QString& string, const MessageStatus& status)
{
	QString msg(string);
	switch (status)
	{
		case MessageStatus::Error:
			msg = QString("[ERROR] %1").arg(msg);
			break;
		case MessageStatus::Info:
			msg = QString("[INFO] %1").arg(msg);
			break;
		default:
			break;
	}

	emit eventMessage(msg);
	emit writeLog(msg);
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

void DialogSettings::queryChart()
{
	emit sendChart(processor->toChart());
}

void DialogSettings::querySvg(const QString& filePath)
{
	if (processor->toSvg(filePath) == QResult_Success)
	{
		emit sendSvg(filePath);
	}
	else
	{
		emit sendSvg("");
	}
}

void DialogSettings::queryInfo()
{
	processor->queryInfo();
}

void DialogSettings::on_minBlockDegreeSlider_sliderMoved(int position)
{
	if (updateInProgress) return;

	if (memorySettings->setMinBlockDegree(position) != QResult_Success)
	{
		qDebug() << "Cannot update min block degree value.";
	}

	updateSettingsFromObject();
}

void DialogSettings::on_minBlockDegreeSpinBox_valueChanged(int value)
{
	if (updateInProgress) return;

	if (memorySettings->setMinBlockDegree(value) != QResult_Success)
	{
		qDebug() << "Cannot update total memory degree value.";
	}

	updateSettingsFromObject();
}

void DialogSettings::on_totalMemorySlider_sliderMoved(int position)
{
	if (updateInProgress) return;

	if (memorySettings->setTotalMemoryDegree(position) != QResult_Success)
	{
		qDebug() << "Cannot update total memory degree value.";
	}

	updateSettingsFromObject();
}

void DialogSettings::on_totalMemorySpinBox_valueChanged(int value)
{
	if (updateInProgress) return;

	if (memorySettings->setTotalMemoryDegree(value) != QResult_Success)
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
	QDateTime dateTime = QDateTime::currentDateTime();
	QString timestamp = dateTime.toString("yyyy-MM-dd HH:mm:ss");
	QString fileName = QFileDialog::getSaveFileName(this,
								 "Choose file for writing log",
								 QString("Log %1.txt").arg(timestamp),
								 "Log files (*.log) ;; Text files (*.txt) ;; All files (*.*)");
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

void DialogSettings::on_deleteCmd()
{
	QItemSelectionModel* select = ui->cmds->selectionModel();
	if (!select->hasSelection()) return;
	QModelIndexList selectedIndexes = select->selectedIndexes();

	QSet<uint16_t> indexesToDelete;

	foreach (QModelIndex index, selectedIndexes)
	{
		indexesToDelete.insert(index.row());
	}
	QSet<uint16_t>::reverse_iterator iter;
	for (iter = indexesToDelete.rbegin(); iter != indexesToDelete.rend(); ++iter)
	{
		processor->removeCmd(*iter);
	}
	updateCmdsTable();
}

void DialogSettings::on_autoExec_clicked()
{
	if (execTimer->isActive())
	{
		// stop is clicked
		execTimer->stop();
		ui->autoExec->setText("Automatic execution");
	}
	else
	{
		// start is clicked
		uint32_t interval = (uint32_t)1000 * memorySettings->getStepsExecutionSpeed();
		execTimer->setInterval(interval);
		execTimer->start();
		ui->autoExec->setText("Stop execution");
	}
}

void DialogSettings::on_execNextCmd_clicked()
{
	QString result = "";

	if (processor->execNextCmd(&result) == QResult_Success)
	{
		if (execTimer->isActive())
		{
			emit redraw();
		}

		lastCmdError = false;
		printMessage(result, MessageStatus::Info);
	}
	else
	{
		lastCmdError = true;
		printMessage(result, MessageStatus::Error);
		if (execTimer->isActive())
		{
			// stop is clicked
			execTimer->stop();
			ui->autoExec->setText("Automatic execution");
		}
	}
	highlightNextCommand();
}

void DialogSettings::on_resetExec_clicked()
{
	processor->resetExec();

	updateCmdsTable();
}

void DialogSettings::on_cmdOperation_currentIndexChanged(const QString &string)
{
	if (string != "Allocate")
	{
		ui->blockSize->setEnabled(false);
		ui->blockSizeUnits->setEnabled(false);
	}
	else
	{
		ui->blockSize->setEnabled(true);
		ui->blockSizeUnits->setEnabled(true);
	}
}

void DialogSettings::on_addCmd_clicked()
{
	Command* cmd = new Command();
	if (ui->cmdOperation->currentText() == "Allocate")
	{
		cmd->action	= CommandAction::Allocate;
	}
	else if (ui->cmdOperation->currentText() == "Free")
	{
		cmd->action	= CommandAction::Free;
	}
	else if (ui->cmdOperation->currentText() == "Query")
	{
		cmd->action	= CommandAction::Query;
	}
	// settings name
	if (ui->cmdBlockName->text().length() == 0)
	{
		cmd->blockName = processor->getRandomName();
	}
	else
	{
		cmd->blockName = ui->cmdBlockName->text();
	}

	if (cmd->action == CommandAction::Allocate)
	{
		cmd->blockSize = (uint64_t)ui->blockSize->value();
	}

	uint64_t multiplier = 1;
	if (ui->blockSizeUnits->currentText() != "B")
	{
		multiplier <<= 10;
		if (ui->blockSizeUnits->currentText() != "KB")
		{
			multiplier <<= 10;
			if (ui->blockSizeUnits->currentText() != "MB")
			{
				multiplier <<= 10;
			}
		}
	}
	cmd->blockSize *= multiplier;

	processor->addCmd(cmd);

	updateCmdsTable();
}

void DialogSettings::on_autoSave_clicked()
{
	if (updateInProgress) return;

	memorySettings->setAutoSaveCmds(ui->autoSave->isChecked());
	updateSettingsFromObject();
}

void DialogSettings::on_stepsExecSpeedSpinBox_valueChanged(double value)
{
	if (updateInProgress) return;

	memorySettings->setStepsExecutionSpeed(value);
	updateSettingsFromObject();
}

void DialogSettings::on_drawingTool_currentIndexChanged(const QString &str)
{
	if (str == "dot") memorySettings->setDrawUtility(DrawUtility::dot);
	else if (str == "neato") memorySettings->setDrawUtility(DrawUtility::neato);
	else if (str == "fdp") memorySettings->setDrawUtility(DrawUtility::fdp);
	else if (str == "sfdp") memorySettings->setDrawUtility(DrawUtility::sfdp);
	else if (str == "twopi") memorySettings->setDrawUtility(DrawUtility::twopi);
	else if (str == "circo") memorySettings->setDrawUtility(DrawUtility::circo);
}

void DialogSettings::on_stepsExecSpeedSlider_sliderMoved(int position)
{
	if (updateInProgress) return;

	if (position > memorySettings->MAX_STEPS_EXECUTION_SPEED)
	{
		position = memorySettings->MAX_STEPS_EXECUTION_SPEED;
	}
	memorySettings->setStepsExecutionSpeed(position);

	updateSettingsFromObject();
}

void DialogSettings::on_saveCmds_clicked()
{
	// choosing file to export
	QDateTime dateTime = QDateTime::currentDateTime();
	QString timestamp = dateTime.toString("yyyy-MM-dd HH:mm:ss");
	QString fileName = QFileDialog::getSaveFileName(this,
								 "Choose file for writing log",
								 QString("Commands %1.cmds").arg(timestamp),
								 "Command files (*.cmds) ;; Text files (*.txt) ;; All files (*.*)");
	if (fileName != "")
	{
		cmdFilePath = fileName;
		saveCmdsToFile();
	}
}

void DialogSettings::on_loadCmds_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
				this,
				"Choose file with commands",
				QDir::currentPath(),
				QString("Command file (*.cmds);;Text files (*.txt);;All files (*.*)"));
	this->cmdFilePath = fileName;
	// trying to open file
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		printMessage("Cannot open file with commands.", MessageStatus::Error);
		return;
	}
	// read and validate table
	CommandProcessor* newProcessor = new CommandProcessor(memorySettings);

	QTextStream stream(&file);
	while (!stream.atEnd())
	{
		QString line = stream.readLine();
		Command* cmd = new Command();

		if (cmd->strToCmd(line) != QResult_Success)
		{
			printMessage(QString("Error in line: '%1'.").arg(line), MessageStatus::Error);
			delete cmd;
			continue;
		}
		newProcessor->addCmd(cmd);
	}
	// check success
	if (newProcessor->getCmdsCount() == 0)
	{
		printMessage(QString("No valid commands found in file."), MessageStatus::Error);
		delete newProcessor;
	}
	else
	{
		delete processor;
		processor = newProcessor;
		printMessage(QString("Found %1 valid commands.").arg(processor->getCmdsCount()), MessageStatus::Info);
	}

	updateCmdsTable();
}

void DialogSettings::on_execAll_clicked()
{
	int16_t curCmdIndex = processor->getNextCmdIndex();
	if (curCmdIndex >= 0)
	{
		curCmdIndex = curCmdIndex == 0 ? curCmdIndex : curCmdIndex -1;
		execTimer->stop();
		ui->autoExec->setText("Automatic execution");
		lastCmdError = false;
		for (int16_t next = curCmdIndex; next < processor->getCmdsCount(); ++next)
		{
			on_execNextCmd_clicked();
			if (lastCmdError) break;
		}
	}
}
