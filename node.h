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

#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>

class Edge;
class Stoma;
class GraphWidget;
class QMouseEvent;

class Node : public QGraphicsItem
{
public:
    Node(GraphWidget *graphWidget);
	
    QRectF boundingRect() const;
    QPainterPath shape() const;
	void addEdge(Edge *edge);
	void removeEdge(Edge *edge);
	void addStoma(Stoma *newStoma);
	void removeStoma();
	size_t degree();
	
    QList<Edge *> edges() const;

	enum { Type = UserType + 2 };
    int type() const { return Type; }
	
	void paintPicture(QPainter &painter);
	void setNumber(int newNumber);
	int getNumber();

	void addParticles(int nParticlesAdded);
	void subtractParticles(int nParticlesSubtracted);
	int getNParticles();
	bool isSourceNode();
	bool isSinkNode();
	void setLabel(QString newLabel);
	QString getLabel();
	void reColour(QString colouringNodesParameter, QString nodesColourScale);
	Stoma *getStoma();
	
	
	void setAsSource();
	void setAsSink();
	void setAsNeitherSourceNorSink();

// public slots:
	void setNParticles(int newNParticles);
	
	
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
 	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
 
//    void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
private:
	
    GraphWidget *pGraph;
	QList<Edge *> edgeList;
	int number;
	QString label;
	
	bool isSource;
	bool isSink;
	int nParticles;
	QColor colourDark;
	QColor colourLight;
	Stoma *pStoma;
	
	
	
};

#endif
