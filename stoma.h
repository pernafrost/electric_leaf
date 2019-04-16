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




#ifndef STOMA_H
#define STOMA_H

#include <QGraphicsItem>
#include <QList>

class Node;
class GraphWidget;
class QMouseEvent;




class Stoma : public QGraphicsItem
{
public:
    Stoma(GraphWidget *graphWidget, Node *sourceNode);
    ~Stoma();
	
	Node *getSourceNode();
	void setSourceNode(Node *node);
	

	void reColour(QString colouringStomataParameter, QString stomataColourScale);
	void paintPicture(QPainter &painter);
//	
//	double getLength();
//	void setLength(double newLength);
//	
//	double getWidth();
//	void setWidth(double newWidth);
//	
//	double getWeight();
//	void setWeight(double newWeight);
	
	double getSigma();
	void setSigma(double newSigma);
	
	int getFlow();
	void setFlow(int newFlow);
	
	void initialize();
		
	QPainterPath shape() const;
	
	

	
    enum { Type = UserType + 1 };
    int type() const { return Type; }
		
	
	
protected:
	QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	
	
	
	
private:
    QPointF sourcePoint;
	QColor lineColour;
	Node *source;
	GraphWidget *pGraph;
	
	double sigma; // edge conductivity
//	double length;
//	double weight;
//	double width;
	int flow;
	
	QColor colourDark;
	QColor colourLight;
	
};

#endif
