#include "memory.h"

Memory::~Memory()
{
	blocks->clear();
	delete blocks;
	if (rootPair != nullptr)
	{
		delete rootPair;
	}
}

Memory::Memory(MemorySettings* settings)
{
	this->settings = settings;
	blocks = new tree_t();

	size_t levelsCount = settings->getTotalMemoryDegree() + 1 - settings->getMinBlockDegree();
	for (uint8_t levelIndex = 0; levelIndex < levelsCount; ++levelIndex)
	{
		blocks->push_back(new level_t());
	}

	rootPair = new pair_t(new Block(settings->getTotalMemoryDegree(), settings->getMinBlockDegree()), nullptr);
	blocks->at(0)->push_back(rootPair);

	recalculateInfo();
}

QResultStatus Memory::allocate(const uint64_t bytes, const QString& procName)
{
	// looking for block with the same name
	foreach (level_t* level, *blocks)
	{
		foreach (pair_t* pair, *level)
		{
			if ((pair->first != nullptr && pair->first->getProcName() == procName) ||
					(pair->second != nullptr && pair->second->getProcName() == procName))
			{
				return QResult_ActionUnavailable;
			}
		}
	}

	QResultStatus resultStatus = QResult_Success;
	uint8_t searchedDegree = 0;
	// looking for degree
	for (uint64_t size = 1;
		 searchedDegree <= settings->getTotalMemoryDegree() && size < bytes;
		 ++searchedDegree, size <<= 1);

	if (searchedDegree > settings->getTotalMemoryDegree())
	{
		resultStatus = QResult_ActionUnavailable;
	}
	else
	{
		if (searchedDegree < settings->getMinBlockDegree())
		{
			searchedDegree = settings->getMinBlockDegree();
		}

		uint8_t rowIndex = settings->getTotalMemoryDegree() - searchedDegree;
		bool isBlockFound = false;
		foreach (pair_t* pair, *(blocks->at(rowIndex)))
		{
			if ((pair->first != nullptr && pair->first->isFree()) || (pair->second != nullptr && pair->second->isFree()))
			{
				Block* freeBlock = (pair->first != nullptr && pair->first->isFree() ? pair->first : pair->second);
				freeBlock->setProcName(procName);
				isBlockFound = true;
				break;
			}
		}

		if (!isBlockFound)
		{
			// then we must spit blocks until the needed level
			Block* freeBlock = splitUntilDegree(settings->getTotalMemoryDegree() - searchedDegree);
			if (freeBlock == nullptr)
			{
				resultStatus = QResult_ActionUnavailable;
			}
			else
			{
				freeBlock->setProcName(procName);
			}
		}
	}
	return resultStatus;
}

QResultStatus Memory::free(const QString& procName)
{
	QResultStatus resultStatus = QResult_Failure;
	bool isBlockFound = false;
	foreach (level_t* level, *blocks)
	{
		foreach (pair_t* pair, *level)
		{
			if ((pair->first != nullptr && pair->first->getProcName() == procName) || (pair->second != nullptr && pair->second->getProcName() == procName))
			{
				Block* blockToFree = ((pair->first != nullptr && pair->first->getProcName() == procName) ? pair->first : pair->second);
				resultStatus = this->free(blockToFree);
				isBlockFound = true;
				break;
			}
		}
		if (isBlockFound)
		{
			break;
		}
	}
	return resultStatus;
}

QString Memory::query(const QString& procName)
{
	foreach (level_t* level, *blocks)
	{
		foreach (pair_t* pair, *level)
		{
			if ((pair->first != nullptr && pair->first->getProcName() == procName)
					|| (pair->second != nullptr && pair->second->getProcName() == procName))
			{
				Block* block = pair->first->getProcName() == procName ? pair->first : pair->second;

				uint16_t size = MemorySettings::degreeToBytes(block->getDegree());
				uint16_t beginAddress = block->getBeginAddress();

				return QString("Block %1 > Size = %2 -> [%3; %4]").arg(
							block->getProcName(),
							QString::number(size),
							QString::number(beginAddress),
							QString::number(beginAddress + size));
			}
		}
	}
	return QString("Block %1 not found.").arg(procName);
}

QResultStatus Memory::toSvg(const QString& pathToFile)
{
	QResultStatus resultStatus = QResult_Success;
	resultStatus = dotToSvg(pathToFile);
	return resultStatus;
}

QChartView*Memory::toChart()
{
	// обход дерева в глубину с созданием сетов
	/// Quantity of pairs where at least one free block.
	QVector<QBarSet*>* sets = new QVector<QBarSet*>();
	QSet<Block*>* viewedBlocks = new QSet<Block*>();

	Block* curBlock = rootPair->first;

	while (curBlock != nullptr)
	{
		// go to the left
		if (curBlock->getFirstChild() != nullptr && !viewedBlocks->contains(curBlock->getFirstChild()))
		{
			curBlock = curBlock->getFirstChild();
			continue;
		}
		// go to the right
		if (curBlock->getSecondChild() != nullptr && !viewedBlocks->contains(curBlock->getSecondChild()))
		{
			curBlock = curBlock->getSecondChild();
			continue;
		}

		if (!curBlock->hasChilds())
		{
			uint64_t value = MemorySettings::degreeToBytes(curBlock->getDegree());
			QBarSet* set = new QBarSet("");
			*set << value;
			// add to chart sets
			if (curBlock->isFree())
			{
				set->setLabel(MemorySettings::degreeToString(curBlock->getDegree()));
				set->setColor(QColor(0xe0, 0xe0, 0xe0));
			}
			else
			{
				QString label = MemorySettings::degreeToString(curBlock->getDegree());
				label = QString("%1 = %2").arg(curBlock->getProcName(), label);
				set->setLabel(label);
				set->setColor(curBlock->getColor());
			}
			sets->push_back(set);
			// move back to the parent
			viewedBlocks->insert(curBlock);
			curBlock = curBlock->getParent();
		}
		else
		{
			viewedBlocks->insert(curBlock);
			curBlock = curBlock->getParent();
		}
	}
	// creating chart
	QHorizontalPercentBarSeries *series = new QHorizontalPercentBarSeries();
	foreach (QBarSet* set, *sets)
	{
		series->append(set);
	}
	QChart *chart = new QChart();
	chart->addSeries(series);
	chart->setTitle("Buddy memory algorithm");
	chart->setAnimationOptions(QChart::NoAnimation);

	QStringList categories;
	uint8_t totalMemoryDegree = settings->getTotalMemoryDegree();
	categories << MemorySettings::degreeToString(totalMemoryDegree);
	QBarCategoryAxis *axis = new QBarCategoryAxis();
	axis->append(categories);
	chart->createDefaultAxes();
	chart->setAxisY(axis, series);
	chart->legend()->setVisible(true);
	chart->legend()->setAlignment(Qt::AlignBottom);
	QChartView *chartView = new QChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	delete sets;
	delete viewedBlocks;

	return chartView;
}

void Memory::clear()
{
	blocks->clear();
	delete blocks;
	if (rootPair != nullptr)
	{
		delete rootPair;
	}
	blocks = new tree_t();

	size_t levelsCount = settings->getTotalMemoryDegree() + 1 - settings->getMinBlockDegree();
	for (uint8_t levelIndex = 0; levelIndex < levelsCount; ++levelIndex)
	{
		blocks->push_back(new level_t());
	}

	rootPair = new pair_t(new Block(settings->getTotalMemoryDegree(), settings->getMinBlockDegree()), nullptr);
	blocks->at(0)->push_back(rootPair);
}

void Memory::recalculateInfo()
{
	MemoryInfo::usedMemory = "";
	MemoryInfo::usedPercent = 0;
	MemoryInfo::smallestBlockSize = "";
	MemoryInfo::blocksQuantity = 0;

	uint64_t usedMemory = 0;
	uint64_t blockSize = 0;
	uint64_t smallestBlockSize = UINT64_MAX;
	foreach (level_t* level, *blocks)
	{
		foreach (pair_t* pair, *level)
		{
			Block* block = nullptr;
			for (uint8_t blockIndex = 0; blockIndex < 2; ++blockIndex)
			{
				if (blockIndex == 0) block = pair->first;
				else block = pair->second;

				if (block != nullptr && !block->hasChilds())
				{
					MemoryInfo::blocksQuantity++;
					blockSize = MemorySettings::degreeToBytes(block->getDegree());
					if (smallestBlockSize > blockSize)
					{
						smallestBlockSize = blockSize;
					}
					if (block->getProcName().length())
					{
						usedMemory += blockSize;
					}
				}
			}
		}
	}

	MemoryInfo::smallestBlockSize = MemorySettings::bytesToString(smallestBlockSize);
	MemoryInfo::usedPercent = (double)usedMemory / (double)MemorySettings::degreeToBytes(settings->getTotalMemoryDegree());
	MemoryInfo::usedMemory = MemorySettings::bytesToString(usedMemory);
}

Block* Memory::splitUntilDegree(const uint8_t degree)
{
	// looking for the last level with free blocks
	int16_t splitLevel = degree - 1;
	Block* freeBlock = nullptr;
	for (; splitLevel >= 0; --splitLevel)
	{
		if (blocks->at(splitLevel)->size() != 0)
		{
			foreach (pair_t* pair, *(blocks->at(splitLevel)))
			{
				if ((pair->first != nullptr && pair->first->isFree()) ||
						(pair->second != nullptr && pair->second->isFree()))
				{
					freeBlock = ((pair->first != nullptr && pair->first->isFree()) ? pair->first : pair->second);
					break;
				}
			}
			if (freeBlock != nullptr) break;
		}
	}
	// begins splitting
	if (freeBlock != nullptr)
	{
		for (; splitLevel != degree; ++splitLevel)
		{
			pair_t* pair = freeBlock->split();
			if (pair == nullptr)
			{
				freeBlock = nullptr;
				break;
			}
			freeBlock = pair->first;
			blocks->at(splitLevel+1)->push_back(pair);
		}
	}
	return freeBlock;
}

QResultStatus Memory::free(Block* block)
{
	QResultStatus resultStatus = QResult_Failure;
	if (block == nullptr)
	{
		return resultStatus;
	}

	bool isBlockFound = false;
	foreach (level_t* level, *blocks)
	{
		foreach (pair_t* pair, *level)
		{
			if (pair->first == block || pair->second == block)
			{
				Block* parent = block->getParent();
				resultStatus = block->free();
				// trying to merge
				if (resultStatus == QResult_Success && block->merge(pair->second) == QResult_Success)
				{
					level->removeOne(pair);
					if (pair != nullptr) delete pair;
					this->free(parent);
				}
				isBlockFound = true;
				break;
			}
		}
		if (isBlockFound)
		{
			break;
		}
	}
	return resultStatus;
}

QResultStatus Memory::memToDot(QString* result)
{
	try {
		// if no nodes
		if (blocks->size() == 0) {
			throw QResult_ActionUnavailable;
		}
		*result = "digraph Memory{\n";
		foreach (level_t* level, *blocks)
		{
			foreach (pair_t* pair, *level)
			{
				for (uint8_t index = 0; index < 2; ++index)
				{
					Block* block = (index == 0 ? pair->first : pair->second);
					if (block != nullptr)
					{
						uint8_t degree = block->getDegree();
						QString label = MemorySettings::degreeToString(degree);
						result->append(QString("%1 ").arg(QString::number((uint64_t(block)))));
						if (!block->isFree())
						{
							QString code = "";
							if (block->getProcName().length() != 0)
							{
								QString color = block->getColor().name();
								code = QString(" [label=\"%1 = %2\", color=\"%3\", fontcolor=\"#000000\", style=filled];\n").arg(block->getProcName(), label, color);
							}
							else
							{
								code = QString(" [label=\"%1\", color=\"#e0e0e0\", fontcolor=\"#000000\", style=filled];\n").arg(label);
							}
							result->append(code);
						}
						else
						{
							QString code = QString(" [label=\"%1\"];").arg(label);
							result->append(code);
						}
						// adding link from parent
						if (block->getParent() != nullptr)
						{
							QString code = QString("%1 -> %2;\n").arg(QString::number(uint64_t(block->getParent())), QString::number(uint64_t(block)));
							result->append(code);
						}
					}
				}
			}
		}
		result->append("}");

		return QResult_Success;
	} catch (QResultStatus resultStatus) {
		return resultStatus;
	}
}

QResultStatus Memory::dotToSvg(const QString& pathToFile)
{
	try {
		// making dot file
		QString dot = "";
		QResultStatus generationResult = memToDot(&dot);
		if (generationResult != QResult_Success) {
			throw generationResult;
		}
		// writing file
		// trying to create file
		QFile file(QObject::tr("%1.dot").arg(pathToFile));
		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			throw QResult_UnexpectedError;
		}
		// writing table to file
		QTextStream wfstream(&file);
		wfstream << dot;
		wfstream.flush();
		// closing file
		file.flush();
		file.close();
		// calling utility
		QString utility = "";
		switch (settings->getDrawUtility()) {
			case DrawUtility::dot:
				utility = "dot";
				break;
			case DrawUtility::neato:
				utility = "neato";
				break;
			case DrawUtility::fdp:
				utility = "fdp";
				break;
			case DrawUtility::sfdp:
				utility = "sfdp";
				break;
			case DrawUtility::twopi:
				utility = "twopi";
				break;
			case DrawUtility::circo:
				utility = "circo";
				break;
			default:
				utility = "dot";
				break;
		}
		system((QObject::tr("%2 -Tsvg '%1.dot' -o '%1'").arg(pathToFile, utility)).toStdString().data());

		throw QResult_Success;
	} catch (QResultStatus resultStatus) {
		return resultStatus;
	}
}
