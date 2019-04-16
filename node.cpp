
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



#include <QGraphicsScene>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QMenu>
#include <QtGui>
#include <QMutableListIterator>
#include <QInputDialog>
#include <QObject>



#include "edge.h"
#include "node.h"
#include "stoma.h"
#include "graphwidget.h"

#include <iostream>
#include <cmath>

Node::Node(GraphWidget *graphWidget)
: pGraph(graphWidget)
{
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);    
	setZValue(1);
	setVisible(pGraph->getNodesVisible());
	pStoma = NULL;
}





void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

size_t Node::degree()
{
	return edgeList.size();
}


void Node::removeEdge(Edge *edge)
{
//	printf("before edgeList.size() = %d", edgeList.size());
	edgeList.removeOne(edge);
	this->reColour(pGraph->getColouringNodesParameter(), pGraph->getNodesColourScale());
//	printf("now edgeList.size() = %d", edgeList.size());
}


void Node::addStoma(Stoma *newStoma)
{
	pStoma = newStoma;
}

void Node::removeStoma()
{
	pStoma = NULL;
}

Stoma *Node::getStoma()
{
	return pStoma;
}



void Node::setNumber(int newNumber)
{
	number = newNumber;
}

int Node::getNumber()
{
	return number;
}

void Node::setLabel(QString newLabel)
{
	label = newLabel;
}

QString Node::getLabel()
{
	return label;
}

void Node::setNParticles(int newNParticles)
{
	nParticles = newNParticles;
}


void Node::addParticles(int nParticlesAdded)
{
	nParticles += nParticlesAdded;
	if (nParticles < 0) // this is to allow adding negative numbers of particles
		nParticles = 0;
}

void Node::subtractParticles(int nParticlesSubtracted)
{
	nParticles -= nParticlesSubtracted;
	if (nParticles < 0)
		nParticles = 0;
}


int Node::getNParticles()
{
	return nParticles;
}

void Node::setAsSource()
{
	if (pStoma != NULL)
	{
	delete(pStoma);
	removeStoma();
	}
	isSink = false;
	isSource = true;
	reColour(pGraph->getColouringNodesParameter(), pGraph->getNodesColourScale());
	update();
}



void Node::setAsNeitherSourceNorSink()
{
	if (pStoma != NULL)
	{
	delete(pStoma);
	removeStoma();
	}
	isSink = false;
	isSource = false;
	reColour(pGraph->getColouringNodesParameter(), pGraph->getNodesColourScale());
	update();
}


bool Node::isSourceNode()
{
	return isSource;
}

void Node::setAsSink()
{
	Stoma *pMyStoma = pGraph->createNewStoma(this);
	addStoma(pMyStoma);
	isSink = true;
	isSource = false;
	reColour(pGraph->getColouringNodesParameter(), pGraph->getNodesColourScale());
	update();
}

bool Node::isSinkNode()
{
	return isSink;
}





QList<Edge *> Node::edges() const
{
    return edgeList;
}













QRectF Node::boundingRect() const
{
    qreal adjust = 1;
    return QRectF(-1.5 - adjust, -1.5 - adjust,
                  3.5 + adjust, 3.5 + adjust);
}










QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-1.5, -1.5, 3, 3);
    return path;
}




void Node::paintPicture(QPainter &painter)
{
	if (pGraph->getNodesVisible())
	{
		painter.setPen(Qt::NoPen);
		painter.setBrush(Qt::darkGray);
		painter.drawEllipse(this->x() -1,this->y()-1, 3, 3);
		
		QRadialGradient gradient(this->pos(), 1.5);
		gradient.setColorAt(0, colourLight);
		gradient.setColorAt(1, colourDark);
		
		painter.setBrush(gradient);
		painter.setPen(QPen(Qt::black, 0));
		painter.drawEllipse(this->x() -1.5,this->y() -1.5, 3, 3);
	}
}





void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	bool drawNodes = true; // do we really want to draw the nodes or not?
	if (drawNodes)
	{
		painter->setPen(Qt::NoPen);
		painter->setBrush(Qt::darkGray);
		painter->drawEllipse(-1, -1, 3, 3);
		
		QRadialGradient gradient(-0.5, -0.5, 1.5);
		gradient.setColorAt(0, colourLight);
		gradient.setColorAt(1, colourDark);

		painter->setBrush(gradient);
		painter->setPen(QPen(Qt::black, 0));
		painter->drawEllipse(-1.5, -1.5, 3, 3);
	}
}








void Node::reColour(QString colouringNodesParameter, QString nodesColourScale)
{
//	printf("nParticles %d; maxNParticles %d; minNParticles %d\n", nParticles, pGraph->getMaxNParticles(), pGraph->getMinNParticles());
	if (colouringNodesParameter == "nParticles")
	{
		if (nodesColourScale == "Linear")
		{
		colourLight = pGraph->colourMap(
											   double(nParticles - pGraph->getMinNParticles())/
											   (pGraph->getMaxNParticles() - pGraph->getMinNParticles())
											   );
		
		
		colourDark = colourLight.darker(130);
		}
		else if (nodesColourScale == "Log")
		{
//			cout << double(log(nParticles+1) - log(pGraph->getMinNParticles()+1))/
//			(log(pGraph->getMaxNParticles()+1) - log(pGraph->getMinNParticles()+1)) << endl;

			colourLight = pGraph->colourMap(
												   double(log(nParticles+1) - log(pGraph->getMinNParticles()+1))/
												   (log(pGraph->getMaxNParticles()+1) - log(pGraph->getMinNParticles()+1))
												   );
			
			
			colourDark = colourLight.darker(130);
		}
	}
//	if (colouringNodesParameter == "StomaticFlow")
//	{
//		lineColour = pGraph->colourMap(
//											  double(flow - pGraph->getMinFlow())/
//											  (pGraph->getMaxFlow() - pGraph->getMinFlow())
//											  );
//		
//	}
//	else if (colouringNodesParameter == "StomaticSigma")
//	{
//		lineColour = pGraph->colourMap(
//											  double(sigma - pGraph->getMinSigma())/
//											  (pGraph->getMaxSigma() - pGraph->getMinSigma())
//											  );
//		
//	}
	
	else if (colouringNodesParameter == "SourceOrSink")
	{
		if (isSource)
		{
			colourLight = Qt::yellow;
			colourDark = Qt::darkYellow;
		}
		else if (isSink)
		{
			colourLight = Qt::cyan;
			colourDark = Qt::darkCyan;
		}
		else
		{
			colourLight = Qt::red;
			colourDark = Qt::darkRed;
		}
	}
	else if (colouringNodesParameter =="Degree")
	{
//		printf("edgeList.size() = %d", edgeList.size());
		switch (edgeList.size()){
			case 0:
				colourLight = Qt::gray;
				colourDark = Qt::darkGray;
				break;
			case 1:
				colourLight = Qt::cyan;
				colourDark = Qt::darkCyan;
				break;
			case 2:
				colourLight = Qt::green;
				colourDark = Qt::darkGreen;
				break;
			case 3:
				colourLight = Qt::yellow;
				colourDark = Qt::darkYellow;
				break;
			case 4:
				colourLight = Qt::red;
				colourDark = Qt::darkRed;
				break;
			default:
				colourLight = QColor(255, 200, 200);
				colourDark = QColor(180, 64, 64);
				break;
		}
	}
	else
	{
		colourLight = Qt::cyan;
		colourDark = Qt::darkCyan;
	}
	update();
}








QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
		case ItemPositionChange:
			foreach (Edge *edge, edgeList)
            edge->adjust();
			break;
		default:
			break;
    };
	
    return QGraphicsItem::itemChange(change, value);
}







//void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    QGraphicsItem::mousePressEvent(event);
//	select();
//	update();
//}







//
//void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//	
//    QGraphicsItem::mouseReleaseEvent(event);
//	
//}




//
//
//void Node::nodePropertiesMenu(QMouseEvent *event)
//{
//	
//    QPoint globalPos = event->pos();// this->mapToGlobal(pos);
//    QMenu myMenu;
//	QMenu *setAsMenu;
//	QAction *setNodeAsSourceAct = new QAction("source", pGraph);
//	QAction *setNodeAsSinkAct = new QAction("sink", pGraph);
//	QAction *setNodeAsNeitherSourceNorSinkAct = new QAction("neither", pGraph);
//	connect(setNodeAsSourceAct, SIGNAL(triggered()), this, SLOT(setAsSource()));
//	connect(setNodeAsSinkAct, SIGNAL(triggered()), this, SLOT(setAsSink()));
//	connect(setNodeAsNeitherSourceNorSinkAct, SIGNAL(triggered()), this, SLOT(setAsNeitherSourceNorSink()));
//	
//	switch (event->button())
//	{
//		case (Qt::LeftButton):
//			myMenu.addAction("Node number");
//			setAsMenu = myMenu.addMenu("&Set as");
//			setAsMenu->addAction(setNodeAsSourceAct);
//			setAsMenu->addAction(setNodeAsSinkAct);
//			setAsMenu->addAction(setNodeAsNeitherSourceNorSinkAct);
//			break;
//		case (Qt::RightButton):
//			myMenu.addAction("Menu button right");
//			break;
//		default:
//			myMenu.addAction("Menu Other button");
//	}
//    // ...
//	
//    QAction* selectedItem = myMenu.exec(globalPos);
//    if (selectedItem)
//    {
//        // something was chosen, do stuff
//    }
//    else
//    {
//        // nothing was chosen
//    }
//}
//
//
//



