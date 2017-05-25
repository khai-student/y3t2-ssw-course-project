#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <QtCore/qglobal.h>
#include <QVector>
#include <QtCharts/QChartView>
#include <QRegularExpression>
#include <QStringList>

QT_CHARTS_USE_NAMESPACE

#include "common.h"
#include "memory.h"
#include "memory_settings.h"
#include "memory_info.h"

enum CommandAction
{
	Allocate,
	Free,
	Query
};

class Command
{
	public:
		Command()
		{
			action = Free;
			blockName = "";
			blockSize = 0;
		}

		Command(const Command* cmd) : Command()
		{
			this->action = cmd->action;
			this->blockName = cmd->blockName;
			this->blockSize = cmd->blockSize;
		}
		~Command() { }

		Command(const CommandAction& action, const QString& blockName, const uint64_t blockSize)
		{
			this->action = action;
			this->blockName = blockName;
			this-> blockSize = blockSize;
		}

		CommandAction action;
		QString blockName;
		uint64_t blockSize;

		QString cmdToStr();
		QResultStatus strToCmd(QString& str);
};

class CommandProcessor
{
	public:
		CommandProcessor(MemorySettings* settings);
		~CommandProcessor();

		void addCmd(Command* cmd);
		Command* getCmd(const uint16_t index);
		QVector<Command*>* getAllCmds();
		QResultStatus removeCmd(const uint16_t index);
		void removeAllCmds();
		uint16_t getCmdsCount() const;
		QString getRandomName() const;

		QResultStatus execNextCmd(QString* result = nullptr);
		void resetExec();
		int16_t getNextCmdIndex();

		QResultStatus toSvg(const QString& pathToFile);
		QChartView* toChart();

		void queryInfo();

	private:
		QVector<Command*>* cmds;
		Memory *mem;
		Command* nextCmd;
};

#endif // COMMAND_PROCESSOR_H
