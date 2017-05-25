#include "command_processor.h"

CommandProcessor::CommandProcessor(MemorySettings* settings) :
	cmds(new QVector<Command*>()),
	mem(new Memory(settings)),
	nextCmd(nullptr)
{

}

CommandProcessor::~CommandProcessor()
{
	cmds->clear();
	delete cmds;
	delete mem;
}

void CommandProcessor::addCmd(Command* cmd)
{
	cmds->push_back(cmd);
	if (nextCmd == nullptr)
	{
		nextCmd = cmd;
	}
}

Command* CommandProcessor::getCmd(const uint16_t index)
{
	if (getCmdsCount() <= index)
	{
		return nullptr;
	}
	else
	{
		return cmds->at(index);
	}
}

QVector<Command*>* CommandProcessor::getAllCmds()
{
	QVector<Command*>* clone = new QVector<Command*>();
	foreach (Command* cmd, *cmds)
	{
		clone->push_back(new Command(cmd));
	}
	return clone;
}

QResultStatus CommandProcessor::removeCmd(const uint16_t index)
{
	QResultStatus resultStatus = QResult_Success;
	if (getCmdsCount() <= index)
	{
		resultStatus = QResult_IndexOutOfRange;
	}
	else
	{
		if (index == getNextCmdIndex())
		{
			cmds->removeAt(index);
			resetExec();
		}
		else
		{
			cmds->removeAt(index);
		}

	}
	return resultStatus;
}

void CommandProcessor::removeAllCmds()
{
	cmds->clear();
}

uint16_t CommandProcessor::getCmdsCount() const
{
	return cmds->size();
}

QString CommandProcessor::getRandomName() const
{
	QString name = "";
	uint64_t counter = 0;

	bool isNameUnique = false;
	while (!isNameUnique)
	{
		name = QString("P%1").arg(QString::number(counter++));
		try
		{
			foreach (Command* cmd, *cmds)
			{
				if (cmd->blockName == name) throw true;
			}
			throw false;
		}
		catch (bool isFound)
		{
			isNameUnique = !isFound;
		}
	}

	return name;
}

QResultStatus CommandProcessor::execNextCmd(QString* result)
{
	QResultStatus resultStatus = QResult_Success;

	if (getNextCmdIndex() >= 0)
	{
		Command* cmd = nextCmd;

		switch (cmd->action)
		{
			case CommandAction::Allocate:
				resultStatus = mem->allocate(cmd->blockSize, cmd->blockName);
				if (resultStatus != QResult_Success)
				{
					result->append(QString("Command cannot be done."));
				}
				else if (result != nullptr)
				{
					if (resultStatus == QResult_Success)
					{
						result->append(QString("Successfully allocated %1.").arg(cmd->blockName));
					}
					else
					{
						result->append(QString("No space found for process %1.").arg(cmd->blockName));
					}
				}
				break;
			case CommandAction::Free:
				resultStatus = mem->free(cmd->blockName);
				if (resultStatus == QResult_Success)
				{
					if (result != nullptr)
					{
						result->append(QString("Memory of process %1 freed.").arg(cmd->blockName));
					}
				}
				else
				{
					if (result != nullptr)
					{
						result->append(QString("Block free failed for process %1.").arg(cmd->blockName));
					}
				}
				break;
			case CommandAction::Query:
				if (result == nullptr)
				{
					throw QResult_NullPointer;
				}

				result->append(mem->query(cmd->blockName));
				break;
			default:
				resultStatus = QResult_IncorrectData;
		}
	}
	else
	{
		*result = "Command not found.";
		resetExec();
		resultStatus = QResult_NotFound;
	}

	if (resultStatus == QResult_Success)
	{
		for (uint16_t index = 0; index < cmds->size(); ++index)
		{
			if (cmds->at(index) == nextCmd)
			{
				if (index == cmds->size() - 1)
				{
					nextCmd = cmds->at(0);
				}
				else
				{
					nextCmd = cmds->at(index+1);
				}
				break;
			}
		}
	}

	return resultStatus;
}

void CommandProcessor::resetExec()
{
	if (cmds->size() > 0)
	{
		nextCmd = cmds->at(0);
	}
	if (mem != nullptr)
	{
		mem->clear();
	}
}

int16_t CommandProcessor::getNextCmdIndex()
{
	if (nextCmd == nullptr)
	{
		if (cmds->size())
		{
			nextCmd = cmds->at(0);
		}
		else
		{
			return -1;
		}
	}
	for (uint16_t index = 0; index < cmds->size(); ++index)
	{
		if (cmds->at(index) == nextCmd)
		{
			return index;
		}
	}
	return -1;
}

QResultStatus CommandProcessor::toSvg(const QString& pathToFile)
{
	return mem->toSvg(pathToFile);
}

QChartView* CommandProcessor::toChart()
{
	return mem->toChart();
}

void CommandProcessor::queryInfo()
{
	mem->recalculateInfo();
}

QString Command::cmdToStr()
{
	QString result = "";

	switch (action)
	{
		case CommandAction::Allocate:
			result.append("Allocate");
			break;
		case CommandAction::Free:
			result.append("Free");
			break;
		case CommandAction::Query:
			result.append("Query");
			break;
		default:
			return "";
	}
	result = QString("%1 %2 %3").arg(result, blockName, QString::number(blockSize));

	return result;
}

QResultStatus Command::strToCmd(QString& str)
{
	try
	{
		QStringList subStrings = str.split(QRegularExpression("\\s? \\s?"), QString::SkipEmptyParts);
		if (subStrings.size() != 3) throw QResult_IncorrectData;

		if (subStrings.at(0).toLower() == "allocate")
		{
			action = CommandAction::Allocate;
		}
		else if (subStrings.at(0).toLower() == "free")
		{
			action = CommandAction::Free;
		}
		else if (subStrings.at(0).toLower() == "query")
		{
			action = CommandAction::Query;
		}
		else throw QResult_IncorrectData;

		blockName = subStrings.at(1);
		bool isConvertionSuccessful = false;
		blockSize = ((QString)subStrings[2]).toULongLong(&isConvertionSuccessful);
		if (!isConvertionSuccessful) throw QResult_IncorrectData;

		throw QResult_Success;
	}
	catch (QResultStatus resultStatus)
	{
		return resultStatus;
	}
}
