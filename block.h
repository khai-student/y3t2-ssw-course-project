#ifndef BLOCK_H
#define BLOCK_H

#include <QtCore/qglobal.h>
#include <QString>
#include <QPair>
#include <QVector>

#include "common.h"

class Block
{
	public:
		Block();
		Block(const uint8_t degree, const uint8_t minDegree);
		~Block();
		Block(Block* parent);

		QPair<Block*, Block*>* split();
		QResultStatus merge(Block* block);
		bool isChildOf(const Block* parent) const;
		bool isFree() const;
		QResultStatus free();

		QResultStatus setProcName(const QString& name);
		QString getProcName() const;
		uint8_t getDegree() const;
		uint8_t getMinDegree() const;
		bool hasChilds() const;
		Block* getParent() const;
		Block* getFirstChild() const;
		Block* getSecondChild() const;

	protected:
		void mergeChilds();

	private:
		uint8_t sizeDegree;
		uint8_t minDegree;
		QString procName;
		Block* parent;
		Block* childFirst;
		Block* childSecond;
};


#endif // BLOCK_H
