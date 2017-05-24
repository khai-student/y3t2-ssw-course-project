#ifndef GRAPH_H
#define GRAPH_H

#include <QVector>
#include <QSet>
#include <QDebug>
#include <QObject>
#include <QFile>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "common.h"
#include "graph_node.h"

typedef QVector<QVector<bool>> AdjencyMatrix_t;

namespace GraphNamespace {
    class GraphNode;

	enum GraphVizAlgo
	{
		dot,
		neato,
		fdp,
		sfdp,
		twopi,
		circo
	};

    class Graph
    {
    public:
        Graph();
        Graph(const size_t& nodesQuantity);
        ~Graph();

        Graph &operator=(const Graph* graph);

        size_t GetNodesQuantity() const;
        ///
        /// \brief  Gets chromatic number.
        /// \return Chromatic number.
        ///
        int8_t GetChromaticNumber() const;
        ///
        /// \brief  Gets adjency matrix
        /// \return Matrix
        ///
        AdjencyMatrix_t GetAdjencyMatrix() const;
        ///
        /// \brief Sets adjency matrix
        /// \return QResult_Success if success
        ///
        QResultStatus SetAdjencyMatrix(const AdjencyMatrix_t *matrix);
        ///
        /// \brief  Adds node to list.
        /// \return QResult_Success if success.
        ///
        QResultStatus AddNode();
        ///
        /// \brief  Adds nodes to list.
        /// \param[in]  nodesQuantity   Quantity of nodes to add.
        /// \return QResult_Success if success.
        ///
        QResultStatus AddNodes(const size_t& nodesQuantity);
        ///
        /// \brief  Deletes node to list.
        /// \return QResult_Success if success.
        ///
        QResultStatus DeleteLastNode();
        ///
        /// \brief  Deletes all.
        ///
        void DeleteAllNodes();

        ///
        /// \brief Link first node with second one.
        /// \param first    Index of the first node.
        /// \param second   Index of the second one.
        /// \param oriented If true - will only link first to second without back link.
        /// \return QResultSuccess if succeeded.
        ///
        QResultStatus LinkNodes(size_t first, size_t second, bool oriented = false);

        ///
        /// \brief Link first node with second one.
        /// \param first    Pointer to the first node.
        /// \param second   Pointer to the second one.
        /// \param oriented If true - will only link first to second without back link.
        /// \return QResultSuccess if succeeded.
        ///
        QResultStatus LinkNodes(GraphNode* first, GraphNode* second, bool oriented = false);

        ///
        /// \brief Link first node with second one.
        /// \param first    Pointer to the first node.
        /// \param second   Pointer to the second one.
        /// \param oriented If true - will only link first to second without back link.
        /// \return QResultSuccess if succeeded.
        ///
        QResultStatus LinkNodes(QVector<GraphNode*>::iterator first,
                                QVector<GraphNode*>::iterator second,
                                bool oriented = false);

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        /// \brief Unlink first node and second one.
        /// \param first    Index of the first node.
        /// \param second   Index of the second one.
        /// \return QResultSuccess if succeeded.
        ///
        QResultStatus UnLinkNodes(size_t first, size_t second);

        ///
        /// \brief Unlink first node and second one.
        /// \param first    Pointer to the first node.
        /// \param second   Pointer to the second one.
        /// \return QResultSuccess if succeeded.
        ///
        QResultStatus UnLinkNodes(GraphNode* first, GraphNode* second);

        ///
        /// \brief Unlink first node and second one.
        /// \param first    Pointer to the first node.
        /// \param second   Pointer to the second one.
        /// \return QResultSuccess if succeeded.
        ///
        QResultStatus UnLinkNodes(QVector<GraphNode*>::iterator first,
                                QVector<GraphNode*>::iterator second);

        ///
        /// \brief Calculates its chromatic number
        /// \return QResult_Success if success.
        ///
        QResultStatus CalculateChromaticNumber();

        ///
        /// \brief Generates it's dot code.
        /// \return QResult_Success if success.
        ///
        QResultStatus GenerateDot(QString *result);

		/**
		 * @brief   Converts graph object to image/svg.
		 * @param[in]   graph   Graph object.
		 * @param[in]   pathToFile There must be abs. path to file for making an image.
		 * @return  QResult_Success if success.
		 */
		QResultStatus ConvertGraphToImage(const QString *pathToFile, const GraphVizAlgo& algo);

    private:
        QVector<GraphNode*> *nodes;
        int8_t chromaticNumber;
    };
}
#endif // GRAPH_H
