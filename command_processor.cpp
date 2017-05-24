#include "command_processor.h"

CommandProcessor::CommandProcessor(MemorySettings* settings) :
	cmds(new QVector<Command*>()),
	mem(new Memory(settings)),
	nextCmdIndex(0)
{

}

CommandProcessor::~CommandProcessor()
{
	cmds->clear();
	delete cmds;
	delete mem;
}

void CommandProcessor::AddCmd(Command* cmd)
{
	cmds->push_back(cmd);
}

Command* CommandProcessor::GetCmd(const uint16_t index)
{
	if (GetCmdsCount() <= index)
	{
		return nullptr;
	}
	else
	{
		return cmds->at(index);
	}
}

QVector<Command*>* CommandProcessor::GetAllCmds()
{
	return cmds;
}

QResultStatus CommandProcessor::RemoveCmd(const uint16_t index)
{
	QResultStatus resultStatus = QResult_Success;
	if (GetCmdsCount() <= index)
	{
		resultStatus = QResult_IndexOutOfRange;
	}
	else
	{
		cmds->removeAt(index);
	}
	return resultStatus;
}

void CommandProcessor::RemoveAllCmds()
{
	cmds->clear();
}

uint16_t CommandProcessor::GetCmdsCount() const
{
	return cmds->size();
}

QResultStatus CommandProcessor::execNextCmd(QString* result)
{
	QResultStatus resultStatus = QResult_Success;
	uint8_t sizeDegree = 0;

	if (cmds->size() > nextCmdIndex)
	{
		Command* cmd = cmds->at(nextCmdIndex);

		switch (cmd->action)
		{
			case CommandAction::Allocate:
				resultStatus = mem->allocate(cmd->blockSize, cmd->blockName);
				if (result != nullptr)
				{
					if (resultStatus == QResult_Success)
					{
						result->append(QString("Process %1 occupied %2.").arg(cmd->blockName, MemorySettings::DegreeToString(mem->query(cmd->blockName))));
					}
					else
					{
						result->append(QString("No space found for process %1.").arg(cmd->blockName));
					}
				}
				break;
			case CommandAction::Free:
				resultStatus = mem->free(cmd->blockName);
				if (result != nullptr)
				{
					if (resultStatus == QResult_Success)
					{
						result->append(QString("Memory of process %1 freed.").arg(cmd->blockName));
					}
				}
				else
				{
					result->append(QString("Block free failed for process %1.").arg(cmd->blockName));
				}
				throw resultStatus;
				break;
			case CommandAction::Query:
				if (result == nullptr)
				{
					throw QResult_NullPointer;
				}

				sizeDegree = mem->query(cmd->blockName);
				if (sizeDegree > 0)
				{
					result->append(QString("Process %1 occupies %2.").arg(cmd->blockName, MemorySettings::DegreeToString(sizeDegree)));
				}
				else
				{
					result->append(QString("Cannot find process %1.").arg(cmd->blockName));
					resultStatus = QResult_Failure;
				}
				break;
			default:
				resultStatus = QResult_IncorrectData;
		}
	}
	else
	{
		resetExec();
	}

	return resultStatus;
}

void CommandProcessor::resetExec()
{
	nextCmdIndex = 0;
}

QResultStatus CommandProcessor::toSvg(const QString& pathToFile, const DrawUtility& algo)
{
	return mem->toSvg(pathToFile, algo);
}

QChartView* CommandProcessor::toChart()
{
	return mem->toChart();
}


