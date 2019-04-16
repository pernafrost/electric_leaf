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




#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsItem>
#include <QList>

class Node;
class GraphWidget;
class QMouseEvent;




class Edge : public QGraphicsItem
{
public:
    Edge(GraphWidget *graphWidget,Node *sourceNode, Node *destNode);
    ~Edge();
	
	Node *getSourceNode();
	Node *getDestNode();
    
	void setSourceNode(Node *node);
	void setDestNode(Node *node);
	
    void adjust();
	void reColour(QString colouringEdgesParameter, QString edgesColourScale);
	void paintPicture(QPainter &painter);
	
	double getLength();
	void setLength(double newLength);
	
	double getWidth();
	void setWidth(double newWidth);
	
//	double getWeight();
//	void setWeight(double newWeight);
	
	double getSigma();
	void setSigma(double newSigma);
	
	int getFlow();
	void setFlow(int newFlow);
	
	double getOrientation();
	void setOrientation(double newOrientation);
	
	void initialize(double initSigma = -1);
	void deleteEdge();
		
	QPainterPath shape() const;
	
	

	
    enum { Type = UserType + 3 };
    int type() const { return Type; }
		
	
	
protected:
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
//	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	
	
	
	
private:
    QPointF sourcePoint;
    QPointF destPoint;
	QColor lineColour;
	Node *source;
	Node *dest;
	GraphWidget *pGraph;
	
	double sigma; // edge conductivity
	double length;
//	double weight;
	double width;
	double orientation;
	int flow;
	
	
};

#endif
