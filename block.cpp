#include "block.h"

Block::Block(const uint8_t sizeDegree, const uint8_t minDegree) : Block()
{
	this->sizeDegree = sizeDegree;
	this->minDegree = minDegree;
}

Block::~Block()
{
	mergeChilds();
}

Block::Block(Block* parent) : Block()
{
	this->parent = parent;
	this->sizeDegree = parent->getDegree() - 1;
	this->minDegree = parent->getMinDegree();
}

QPair<Block*, Block*>* Block::split()
{
	if (childFirst != nullptr || childSecond != nullptr || sizeDegree == minDegree)
	{
		return nullptr;
	}

	QPair<Block*, Block*>* pair = new QPair<Block*, Block*>();
	childFirst = new Block(this);
	childSecond = new Block(this);

	childFirst->beginAddress = this->beginAddress;
	childSecond->beginAddress = this->beginAddress + MemorySettings::degreeToBytes(sizeDegree) / 2;

	pair->first = childFirst;
	pair->second = childSecond;
	return pair;
}

QResultStatus Block::merge(Block* block)
{
	QResultStatus resultStatus = QResult_Success;
	if (parent != nullptr && block != nullptr && block->isChildOf(parent) && block->isFree() && this->isFree())
	{
		this->parent->mergeChilds();
	}
	else
	{
		resultStatus = QResult_ActionUnavailable;
	}
	return resultStatus;
}

bool Block::isChildOf(const Block* parent) const
{
	return this->parent == parent;
}

bool Block::isFree() const
{
	return !(procName.length()) && childFirst == nullptr && childSecond == nullptr;
}

QResultStatus Block::free()
{
	QResultStatus resultStatus = QResult_Success;
	if (childFirst == nullptr && childSecond == nullptr)
	{
		procName = "";
	}
	else
	{
		resultStatus = QResult_ActionUnavailable;
	}
	return resultStatus;
}

QResultStatus Block::setProcName(const QString& name)
{
	QResultStatus resultStatus = QResult_Success;
	if (isFree())
	{
		this->procName = name;
	}
	else
	{
		resultStatus = QResult_ActionUnavailable;
	}
	return resultStatus;
}

QString Block::getProcName() const
{
	return procName;
}

uint8_t Block::getDegree() const
{
	return sizeDegree;
}

uint8_t Block::getMinDegree() const
{
	return minDegree;
}

bool Block::hasChilds() const
{
	return childFirst != nullptr || childSecond != nullptr;
}

Block*Block::getParent() const
{
	return parent;
}

Block*Block::getFirstChild() const
{
	return childFirst;
}

Block*Block::getSecondChild() const
{
	return childSecond;
}

QColor Block::getColor() const
{
	return color;
}

uint64_t Block::getBeginAddress() const
{
	return beginAddress;
}

void Block::mergeChilds()
{

	if (childFirst != nullptr && childFirst->isFree() &&
		childSecond != nullptr && childSecond->isFree())
	{
		delete childFirst;
		delete childSecond;

		childFirst = nullptr;
		childSecond = nullptr;
	}
}

Block::Block()
{
	this->parent = nullptr;
	this->childFirst = nullptr;
	this->childSecond = nullptr;
	color = QColor(rand()%256, rand()%256, rand()%256);
	beginAddress = 0;
}
