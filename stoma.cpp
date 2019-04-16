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



#include "stoma.h"
#include "node.h"
#include "graphwidget.h"



#include <iostream>
#include <cmath>



Stoma::Stoma(GraphWidget *graphWidget, Node *sourceNode)
: pGraph(graphWidget)
{
//	setFlag(ItemIsMovable);
//	setFlag(ItemSendsGeometryChanges);
//    setCacheMode(DeviceCoordinateCache);    
	setZValue(2);
	
	source = sourceNode;
    source->addStoma(this);
	setPos(source->pos());
	setVisible(pGraph->getStomataVisible());
}

Stoma::~Stoma()
{
}


Node *Stoma::getSourceNode()
{
    return source;
}





void Stoma::setSourceNode(Node *node)
{
    source = node;
}



//double Stoma::getLength()
//{
//    return length;
//}
//
//
//
//
//
//void Stoma::setLength(double newLength)
//{
//    length = newLength;
//}


//double Stoma::getWeight()
//{
//	return weight;
//}
//
//void Stoma::setWeight(double newWeight)
//{
//	weight = newWeight;
//}

double Stoma::getSigma()
{
	return sigma;
}

void Stoma::setSigma(double newSigma)
{
	sigma = newSigma;
}

int Stoma::getFlow()
{
	return flow;
}

void Stoma::setFlow(int newFlow)
{
	flow = newFlow;
}




void Stoma::initialize()
{
	// weight = 0.0;
	flow = 0;
	sigma = 0.0005;
}




//double Stoma::getWidth()
//{
//    return width;
//}
//
//
//
//
//
//void Stoma::setWidth(double newWidth)
//{
//    width = newWidth;
//}













void Stoma::reColour(QString colouringStomataParameter, QString stomataColourScale)
{
	//	printf("nParticles %d; maxNParticles %d; minNParticles %d\n", nParticles, pGraph->getMaxNParticles(), pGraph->getMinNParticles());
	if (colouringStomataParameter == "Sigma")
	{
		colourLight = pGraph->colourMap(
											   double(sigma - pGraph->getMinSigma())/
											   (pGraph->getMaxSigma() - pGraph->getMinSigma())
											   );
		
		
		colourDark = colourLight.darker(130);
	}
	else if (colouringStomataParameter == "Flow")
	{
		// cout << flow << " " << min(1.0, double(abs(flow))/100)<< endl;
		colourLight = pGraph->colourMap(
											   min(1.0, double(abs(flow))/10)
											   );
		
		
		colourDark = colourLight.darker(130);
	}
	else
	{
		colourLight = Qt::cyan;
		colourDark = Qt::darkCyan;
	}
	update();
}





QRectF Stoma::boundingRect() const
{
    qreal adjust = 1;
    return QRectF(-1.5 - adjust, -1.5 - adjust,
                  3.5 + adjust, 3.5 + adjust);
}







QPainterPath Stoma::shape() const
{
    QPainterPath path;
    path.addEllipse(-1.5, -1.5, 3, 3);
    return path;
}










void Stoma::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	bool drawStomata = true; // do we really want to draw the stomata or not?
	if (drawStomata)
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





void Stoma::paintPicture(QPainter &painter)
{
	if (pGraph->getStomataVisible())
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





