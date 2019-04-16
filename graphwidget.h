/***************************************************************************
 copyright            : (C) 2011 by Andrea Perna
 email                : perna@math.uu.se
 ***************************************************************************/

/*******************************************************************************
 *     This program is free software: you can redistribute it and/or modify     *
 *     it under the terms of the GNU General Public License as published by     *
 *     the Free Software Foundation, either version 3 of the License, or        *
 *     (at your option) any later version.                                      *
 *                                                                              *
 *     This program is distributed in the hope that it will be useful,          *
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *     GNU General Public License for more details.                             *
 *                                                                              *
 *     You can find a copy of the GNU General Public License at the             *
 *     following address: <http://www.gnu.org/licenses/>.                       *
 ********************************************************************************/

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QRubberBand>


#include <iostream>
#include <fstream>


#include <cstdio>
#include <cstdlib>

#include "mainwindow.h"

using std::string;
using namespace std;


class Node;
class Edge;
class Stoma;
class MainWindow;
class QInputDialog;
class GraphWidget : public QGraphicsView
{
    Q_OBJECT
	
public:
    GraphWidget(MainWindow *pMainWindow);
	
	
	void drawGraph(QString fileName);
	void saveGraph(QString fileName);
	void zoom(qreal scaleFactor);

	void setSelecting(QString whatIsGoingToBeSelecting);
	QString getSelecting();
	
	void setCurrentColourMap(QString chosenColourMap);
	QString getCurrentColourMap();
	
	void setNodesVisible(bool nodesBecomeVisible);
	bool getNodesVisible();
	
	void setStomataVisible(bool stomataBecomeVisible);
	bool getStomataVisible();
	
	void setEdgesVisible(bool edgesBecomeVisible);
	bool getEdgesVisible();
	
	double getExponentEdgeWidthForSigma();
	void setExponentEdgeWidthForSigma(double newExponentEdgeWidthForSigma);
	double getMultiplicativeFactorEdgeSigma();
	void setMultiplicativeFactorEdgeSigma(double newMultiplicativeFactorEdgeSigma);
	
	void paintPicture(QPainter &painter);
	void performOneSimulationStep();
	void setInitialNParticlesPerNode(int newNParticlesPerNode);
	int getInitialNParticlesPerNode();
	
	void setColouringEdgesParameter(QString newColouringEdgesParameter);
	void setColouringNodesParameter(QString newColouringNodesParameter);
	void setColouringStomataParameter(QString newColouringStomataParameter);
	
	void setNodesColourScale(QString newNodesColourScale);
	void setEdgesColourScale(QString newEdgesColourScale);
	void setStomataColourScale(QString newStomataColourScale);
	
	void setSigmaAsFunctionOfWidthAndLength();
	
	QString getNodesColourScale();
	
	Stoma *createNewStoma(Node *sourceNode);
	
	int getNumberOfNodes();
	int getNumberOfEdges();
	int getNumberOfStomata();
	
	int getNumberOfSourceNodes();
	int getNumberOfSinkNodes();
	
	QString getColouringNodesParameter();
//	QString getColouringEdgesParameter();

	double getChargePerParticle();
	double getDeltaT();
	double getMinSigma();
	double getMaxSigma();
	double getMaxEdgeWidth();
	int getParticlesAtSource();
	int getMinNParticles();
	int getMaxNParticles();
	int getMinFlow();
	int getMaxFlow();
	
	
	void setChargePerParticle(double newChargePerParticle);
	void setDeltaT(double newDeltaT);
	void setMinSigma(double newMinSigma);
	void setParticlesAtSource(int newParticlesAtSource);
	QRgb colourMap(double value);
	void setShowUpdate(bool shouldShowUpdate);
	
public slots:
	void setSourceOrSinkForAllNodes();
	void setNParticlesForAllNodes();
	void setSigmaForAllEdges(double newEdgeSigmaValue = -1.0);
	void setWidthForAllEdges();
	void setLengthForAllEdges();
	void setSigmaForAllStomata();
	void setSigmaAsFunctionOfFlow(bool willUpdateEdgeSigma);
	void setStomaticSigmaAsFunctionOfPotential(bool willUpdateStomaticSigma);
	
protected:
    void wheelEvent(QWheelEvent *event);
    void scaleView(qreal scaleFactor);
	void mousePressEvent ( QMouseEvent *event );
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	
private:
	void savePajek(QString fileName);
	void saveEleni(QString fileName);
//	void selectItems(QRect *region);
	void getSelectedGraphicItems();
	
	QRgb rainbowColourMap(double value);
	QRgb grayColourMap(double value);
	QRgb pm3dColourMap(double value);
	QRgb daltonicFriendlyColourMap(double value);
	
	void nodePropertiesMenu(QMouseEvent *event, Node *pNode);
	void edgePropertiesMenu(QMouseEvent *event, Edge *pEdge);
	void stomaPropertiesMenu(QMouseEvent *event, Stoma* pStoma);
	
	void setPropertyToSelectedElements(QMouseEvent *event);
//	void setMultipleEdgesPropertyMenu(QMouseEvent *event);
//	void setMultipleStomataPropertyMenu();
	
	Edge *createNewEdge(Node *sourceNode, Node *destNode, double edgeLength, double edgeWidth, double edgeSigma);
//	void deleteSelectedEdges();
//	bool smallerNodeNumber(const Node &n1, const Node &n2);
	
	//	void setCurrentNodeAsSource();
	//	void setCurrentNodeAsSink();
	//	void setCurrentNodeAsNeitherSourceNorSink();
	
	
	QGraphicsScene *sc;
	Node *currentNode;
	int scaleFactor;
	int numberOfNodes;
	
	QString whatIsSelecting; // is selecting nodes or edges or stomata
	bool areNodesVisible;
	bool areEdgesVisible;
	bool areStomataVisible;
	

	
	QString colouringEdgesParameter;
	QString colouringNodesParameter;
	QString colouringStomataParameter;
	
	QString nodesColourScale;
	QString edgesColourScale;
	QString stomataColourScale;
	
	double chargePerParticle;
	double deltaT;
	double minSigma;
	double exponentEdgeWidthForSigma;
	double multiplicativeFactorEdgeSigma;
	int particlesAtSource;
	bool isUpdatingEdgeSigma;
	bool isUpdatingStomaticSigma;
	
	// recording the maxima and minima is useful when colouring the edges and the nodes.
	// though a better option would be to sort them according to these numbers and colour according
	// to the obtained rank
	int maxFlow;
	int minFlow;
	double maxSigma;
	double maxEdgeWidth;
	int minNParticles;
	int maxNParticles;
	int initialNParticlesPerNode;
	bool isShowingUpdate;
	QString currentColourMap;
	MainWindow *pMainWindow;
	
	QPoint rubberBandOrigin;
	QRubberBand *rubberBand;
	QRectF *currentSelection;
	QList<QGraphicsItem *>selectedGraphicItems;
	bool isRegionSelected;
};

#endif
