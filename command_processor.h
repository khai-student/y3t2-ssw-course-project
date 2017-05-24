#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <QtCore/qglobal.h>
#include <QVector>
#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

#include "common.h"
#include "memory.h"
#include "memory_settings.h"

enum CommandAction
{
	Allocate,
	Free,
	Query
};

class Command
{
	public:
		Command() { }

		Command(const CommandAction& action, const QString& blockName, const uint64_t blockSize)
		{
			this->action = action;
			this->blockName = blockName;
			this-> blockSize = blockSize;
		}

		CommandAction action;
		QString blockName;
		uint64_t blockSize;
};

class CommandProcessor
{
	public:
		CommandProcessor(MemorySettings* settings);
		~CommandProcessor();

		void AddCmd(Command* cmd);
		Command* GetCmd(const uint16_t index);
		QVector<Command*>* GetAllCmds();
		QResultStatus RemoveCmd(const uint16_t index);
		void RemoveAllCmds();
		uint16_t GetCmdsCount() const;

		QResultStatus execNextCmd(QString* result = nullptr);
		void resetExec();
		uint16_t getNextCmdIndex();

		QResultStatus toSvg(const QString& pathToFile, const DrawUtility& algo);
		QChartView* toChart();

	private:
		QVector<Command*>* cmds;
		Memory *mem;
		uint16_t nextCmdIndex;
};

#endif // COMMAND_PROCESSOR_H
