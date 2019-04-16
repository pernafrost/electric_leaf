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


#include <QPainter>
#include <QGraphicsSceneMouseEvent>



#include "edge.h"
#include "node.h"
#include "graphwidget.h"
#include <cmath>


#include <iostream>
#include <cmath>



Edge::Edge(GraphWidget *graphWidget, Node *sourceNode, Node *destNode)
: pGraph(graphWidget)
{
	source = sourceNode;
    dest = destNode;
	setVisible(pGraph->getEdgesVisible());
	// cout << sourceNode->scenePos().y() << endl;
	orientation = atan2(destNode->pos().y() - sourceNode->pos().y(), destNode->pos().x() - sourceNode->pos().x());
    source->addEdge(this);
    dest->addEdge(this);
    adjust();
}

Edge::~Edge()
{
}

void Edge::deleteEdge()
{
	source->removeEdge(this);
	dest->removeEdge(this);
	delete(this);
}





double Edge::getOrientation()
{
	return orientation;
}




void Edge::setOrientation(double newOrientation)
{
	orientation = newOrientation;
}


Node *Edge::getSourceNode()
{
    return source;
}





void Edge::setSourceNode(Node *node)
{
    source = node;
    adjust();
}






Node *Edge::getDestNode()
{
    return dest;
}





void Edge::setDestNode(Node *node)
{
    dest = node;
    adjust();
}





double Edge::getLength()
{
    return length;
}





void Edge::setLength(double newLength)
{
    length = newLength;
}

//
//double Edge::getWeight()
//{
//	return weight;
//}
//
//void Edge::setWeight(double newWeight)
//{
//	weight = newWeight;
//}

double Edge::getSigma()
{
	return sigma;
}

void Edge::setSigma(double newSigma)
{
	sigma = newSigma;
}

int Edge::getFlow()
{
	return flow;
}

void Edge::setFlow(int newFlow)
{
	flow = newFlow;
}


void Edge::initialize(double initSigma)
{
//	weight = 0.0;
	flow = 0;
	if (initSigma < 0)
	{
		sigma = pGraph->getMultiplicativeFactorEdgeSigma() * pow(width, pGraph->getExponentEdgeWidthForSigma())/length;
	}
	else {
		sigma = initSigma;
	}

}




double Edge::getWidth()
{
    return width;
}





void Edge::setWidth(double newWidth)
{
    width = newWidth;
}








void Edge::adjust()
{
    if (!source || !dest)
        return;
	
    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
	prepareGeometryChange();
	//   removeFromIndex();
    sourcePoint = line.p1();
    destPoint = line.p2();
	//   addToIndex();
}







void Edge::reColour(QString colouringEdgesParameter, QString edgesColourScale)
{
	if (colouringEdgesParameter == "Flow")
	{
//		cout << "Edges" << endl;
//		cout <<	(
//				 min(
//					 1.0, 
//					 log(1.0+abs(double(flow)))/
//					 (max(log(1.0+abs(double(pGraph->getMaxFlow()))), log(1.0+abs(double(pGraph->getMinFlow())))))
//					 )
//				 ) << endl;
//		cout << (
//				 min(
//					 1.0, 
//					 abs(double(flow)*10) /
//					 (max(abs(pGraph->getMaxFlow()), abs(pGraph->getMinFlow())))
//					 )
//				 ) << endl << endl;
//		
		if (edgesColourScale == "Linear")
		{ // this is actually linear with saturation, because I am multiplying by 10
		lineColour = pGraph->colourMap(
											  min(
												  1.0, 
												  abs(double(flow)*10) /
												  (max(abs(pGraph->getMaxFlow()), abs(pGraph->getMinFlow())))
												  )
											   );
		}
		else if (edgesColourScale == "Log")
		{
			lineColour = pGraph->colourMap(
												  min(
													  1.0, 
													  log(1.0+abs(double(flow)))/
														  (max(log(1.0+abs(double(pGraph->getMaxFlow()))), log(1.0 + abs(double(pGraph->getMinFlow())))))
													  )
												  );
			
		}
	}
	else if (colouringEdgesParameter == "FlowDir")
	{
		lineColour = pGraph->colourMap(
											  double(((0 < flow) - (flow < 0)) * sin(orientation))*0.3 + 0.5
											  );
	}
	else if (colouringEdgesParameter == "Sigma")
	{
		lineColour = pGraph->colourMap(
											  double(sigma - pGraph->getMinSigma())/
											  (pGraph->getMaxSigma() - pGraph->getMinSigma())
											  );
	}
	else if (colouringEdgesParameter == "Width")
	{
		lineColour = pGraph->colourMap(
											  double(width)/
											  (pGraph->getMaxEdgeWidth())
											  );
	}
	else
	{
		lineColour = Qt::cyan;
	}
	update();
	
}






/* When an edge is clicked, its rank is updated; if the rank is propagating the ranks, then we call performRankComputation
 and update the ranks of the whole network. In this version of the code, either all the edges are propagating the rank,
 or none of them. */


//void Edge::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//	update();
//    QGraphicsItem::mousePressEvent(event);
//	if ( event->button()==Qt::LeftButton )
//	{
//		select();
//	}
//}








//void Edge::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
//{
//	update();
//    QGraphicsItem::mouseReleaseEvent(event);
//}







QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();
	
	
    qreal extra = 1 / 2.0; // extra must be (more than) half the pen width
	
    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
	.normalized()
	.adjusted(-extra, -extra, extra, extra);
}










QPainterPath Edge::shape() const
{
	QPainterPath path;
	path.moveTo(sourcePoint);
	path.lineTo(destPoint);
    return path;
}









void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;
	
    // Draw the line itself
    QLineF line(sourcePoint, destPoint);
    painter->setPen(QPen(lineColour, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
	
	
}


void Edge::paintPicture(QPainter &painter)
{
    if (!source || !dest)
        return;
	
	if (pGraph->getEdgesVisible())
	{
		// Draw the line itself
		QLineF line(sourcePoint, destPoint);
		painter.setPen(QPen(lineColour, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		painter.drawLine(line);
	}
	
}






