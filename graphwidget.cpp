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




#include "graphwidget.h"
#include "parameterdialog.h"
#include "edge.h"
#include "node.h"
#include "stoma.h"
#include "randomnumbers.h"




#include <QFile>
#include <QList>
#include <Qmenu>
#include <QTextStream>
#include <QStringList>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <QtAlgorithms> // for qsort
#include <QRubberBand>

#include <cmath>
#include <climits>



static const double PI = 3.14159265358979323846264338327950288419717;



GraphWidget::GraphWidget(MainWindow *mainWindow)
: pMainWindow(mainWindow)
{
	
//	setContextMenuPolicy(Qt::CustomContextMenu);

	scaleFactor = 1000;
	numberOfNodes = 0;
	whatIsSelecting = tr("Nodes");
	colouringEdgesParameter = "Flow";
	colouringNodesParameter = "nParticles";
	colouringStomataParameter = "Sigma";
	nodesColourScale = "Linear";
	edgesColourScale = "Linear";
	stomataColourScale = "Linear";
	currentColourMap = "rainbow";
	areNodesVisible = true;
	areEdgesVisible = true;
	areStomataVisible = true;
	maxFlow=0;
	minFlow=0;
	maxSigma=minSigma;
	maxEdgeWidth = 0;
	minNParticles=0;
	maxNParticles=0;
	isShowingUpdate = true;
	isUpdatingEdgeSigma = false;
	isUpdatingStomaticSigma = false;
	rubberBand = NULL;
	isRegionSelected = false;
	currentSelection = NULL;
		
	sc = new QGraphicsScene(this);
	sc->setItemIndexMethod(QGraphicsScene::NoIndex);
	sc->setSceneRect(0, 0, 1100, 1100);
	setScene(sc);
	


	
	
	setDragMode(QGraphicsView::RubberBandDrag);
	setInteractive(true);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	//setBackgroundBrush(QPixmap(":/images/plines.png"));
    setCacheMode(CacheBackground);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);
	
    scale(1, 1);
    setMinimumSize(400, 400);
    setWindowTitle(tr("Graph Viewer"));
	
	
}


void GraphWidget::setCurrentColourMap(QString chosenColourMap)
{
	currentColourMap = chosenColourMap;
	
	foreach (QGraphicsItem *item, sc->items()) 
	{
		if (Node *pNode = qgraphicsitem_cast<Node *>(item))
		{
			pNode->reColour(colouringNodesParameter, nodesColourScale);
		}
		else if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item))
		{
			pEdge->reColour(colouringEdgesParameter, edgesColourScale);
		}
		else if (Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item))
		{
			pStoma->reColour(colouringStomataParameter, stomataColourScale);
		}
	}
	sc->update();
	
	
}


QString GraphWidget::getCurrentColourMap()
{
	return currentColourMap;
}



void GraphWidget::setSelecting(QString whatIsGoingToBeSelecting)
{
	whatIsSelecting = whatIsGoingToBeSelecting;
}


QString GraphWidget::getSelecting()
{
	return whatIsSelecting;
}

void GraphWidget::setShowUpdate(bool shouldShowUpdate)
{
	isShowingUpdate=shouldShowUpdate;
	
	if (isShowingUpdate)
	{
		
		minFlow = INT_MAX;
		maxFlow = 0;
		maxSigma = 0;
		minNParticles = INT_MAX;
		maxNParticles = 0;
		
		
		
		foreach (QGraphicsItem *item, sc->items())
		{
			Node *pNode = qgraphicsitem_cast<Node *>(item);
			if (!pNode)
			{
				Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
				if (!pEdge)
					continue;
				if (pEdge->getSigma() > maxSigma)
				{
					maxSigma = pEdge->getSigma();
				}
				if (pEdge->getFlow() > maxFlow)
				{
					maxFlow = pEdge->getFlow();
				}
				if (pEdge->getFlow() < minFlow)
				{
					minFlow = pEdge->getFlow();
				}
			}
			else
			{
				if (pNode->getNParticles() > maxNParticles)
				{
					maxNParticles = pNode->getNParticles();
				}
				if (pNode->getNParticles() < minNParticles)
				{
					minNParticles = pNode->getNParticles();
				}
			}
		}
	
	
		foreach (QGraphicsItem *item, sc->items())
		{
			Node *pNode = qgraphicsitem_cast<Node *>(item);
			if (!pNode)
			{
				Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
				if (!pEdge)
					continue;
				pEdge->reColour(colouringEdgesParameter, edgesColourScale);
			}
			else
			{
				pNode->reColour(colouringNodesParameter, nodesColourScale);
			}
		}
		
		
		foreach (QGraphicsItem *item, sc->items())
		{
			Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item);
			if (!pStoma)
				continue;
			pStoma->reColour(colouringStomataParameter, stomataColourScale);
		}
	}
	
	
}


void GraphWidget::setNodesVisible(bool nodesBecomeVisible)
{
	if (areNodesVisible != nodesBecomeVisible)
	{
		foreach (QGraphicsItem *item, sc->items()) 
		{
			Node *pNode = qgraphicsitem_cast<Node *>(item);
			if (!pNode)
				continue;
			pNode->setVisible(nodesBecomeVisible);
		}
		areNodesVisible = nodesBecomeVisible;
	}
}



void GraphWidget::setStomataVisible(bool stomataBecomeVisible)
{
	// cout << "in graphwidget set stomata visible" << endl;
	if (areStomataVisible != stomataBecomeVisible)
	{
		foreach (QGraphicsItem *item, sc->items()) 
		{
			Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item);
			if (!pStoma)
				continue;
			// cout << pStoma->getSourceNode()->getNumber() << endl;
			pStoma->setVisible(stomataBecomeVisible);
			// cout << "now visible " << i << endl;
		}
		areStomataVisible = stomataBecomeVisible;
	}
}






bool GraphWidget::getEdgesVisible()
{
	return areEdgesVisible;
}



QString GraphWidget::getColouringNodesParameter()
{
	return colouringNodesParameter;
}


//
//QString GraphWidget::getColouringEdgesParameter()
//{
//	return colouringEdgesParameter;
//}




void GraphWidget::setEdgesVisible(bool edgesBecomeVisible)
{
	if (areEdgesVisible != edgesBecomeVisible)
	{
		foreach (QGraphicsItem *item, sc->items()) 
		{
			Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
			if (!pEdge)
				continue;
			pEdge->setVisible(edgesBecomeVisible);
		}
		areEdgesVisible = edgesBecomeVisible;
	}
}

bool GraphWidget::getNodesVisible()
{
	return areNodesVisible;
}

bool GraphWidget::getStomataVisible()
{
	return areStomataVisible;
}



double GraphWidget::getExponentEdgeWidthForSigma()
{
	return exponentEdgeWidthForSigma;
}

void GraphWidget::setExponentEdgeWidthForSigma(double newExponentEdgeWidthForSigma)
{
	exponentEdgeWidthForSigma = newExponentEdgeWidthForSigma;
}

double GraphWidget::getMultiplicativeFactorEdgeSigma()
{
	return multiplicativeFactorEdgeSigma;
}

void GraphWidget::setMultiplicativeFactorEdgeSigma(double newMultiplicativeFactorEdgeSigma)
{
	multiplicativeFactorEdgeSigma = newMultiplicativeFactorEdgeSigma;
}





int GraphWidget::getMinNParticles()
{
	return minNParticles;
}

int GraphWidget::getMaxNParticles()
{
	return maxNParticles;
}

int GraphWidget::getMinFlow()
{
	return minFlow;
}

int GraphWidget::getMaxFlow()
{
	return maxFlow;
}

int GraphWidget::getInitialNParticlesPerNode()
{
	return initialNParticlesPerNode;
}



void GraphWidget::setInitialNParticlesPerNode(int newNParticlesPerNode)
{
	initialNParticlesPerNode = newNParticlesPerNode;
}



//bool smallerNodeNumber(const Node &n1, const Node &n2)
//{
//	return n1.getNumber() < n2.getNumber();
//}



void GraphWidget::saveEleni(QString fileName)
{
	
	QString firstFileName = fileName; // the first file is for the nodes
	QString secondFileName = fileName;
	int indexOfFileExtension = firstFileName.lastIndexOf(".eln");
	firstFileName.truncate(indexOfFileExtension);
	secondFileName.truncate(indexOfFileExtension);
	firstFileName = firstFileName.append("_nodes").append(".eln");
	secondFileName = secondFileName.append("_neigh").append(".eln");
	
	QFile file1(firstFileName);
	QFile file2(secondFileName);
	if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	if (!file2.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out1(&file1);
	QTextStream out2(&file2);
	
	
	// First renumber nodes to start from source nodes
	int counter = 1;
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Node *pNode = qgraphicsitem_cast<Node *>(item);
		if (!pNode)
			continue;
			if (pNode->isSourceNode())
		{
			pNode->setNumber(counter);
			out1 << pNode->pos().x()/scaleFactor << " " << pNode->pos().y()/scaleFactor << endl;
			out2 << (int)pNode->degree() << endl;
			counter+=1;
		}
	}
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Node *pNode = qgraphicsitem_cast<Node *>(item);
		if (!pNode)
			continue;
		if (!pNode->isSourceNode())
		{
			pNode->setNumber(counter);
			out1 << pNode->pos().x()/scaleFactor << " " << pNode->pos().y()/scaleFactor << endl;
			out2 << (int)pNode->degree() << endl;
			counter+=1;
		}
	}
	file1.close();
	file2.close();
	
	
	//	// write all nodes in a list
	//	QList<Node *> nodeList;
	//	foreach (QGraphicsItem *item, sc->items())
	//	{
	//		Node *pNode = qgraphicsitem_cast<Node *>(item);
	//		if (!pNode)
	//			continue;
	//		nodeList.append(pNode);
	//	}
	//	// sort again according to node number
	//	qSort(nodeList.begin(), nodeList.end(), int (*) (const void*, const void*));
	//	
	//	
	//	
	//	foreach (Node *pNode, nodeList)
	//	{	
	//		out <<  pNode->getNumber() << " " << pNode->getLabel() << " "<< pNode->pos().x()/scaleFactor << " " << pNode->pos().y()/scaleFactor << " " << pNode->getNParticles();
	//	}
	
	
	

	
	// Now write the edges
	QString thirdFileName = fileName; // the second file is for the edges
	QString fourthFileName = fileName;
	QString fifthFileName = fileName;
	// int indexOfFileExtension = firstFileName.lastIndexOf(".eln");
	thirdFileName.truncate(indexOfFileExtension);
	fourthFileName.truncate(indexOfFileExtension);
	fifthFileName.truncate(indexOfFileExtension);
	thirdFileName = thirdFileName.append("_edges").append(".eln");
	fourthFileName = fourthFileName.append("_conductivities").append(".eln");
	fifthFileName = fifthFileName.append("_edge_length_and_width").append(".eln");
	
	// file.setFileName(secondFileName);
	QFile file3(thirdFileName);
	QFile file4(fourthFileName);
	QFile file5(fifthFileName);
	if (!file3.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	if (!file4.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	if (!file5.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out3(&file3);
	QTextStream out4(&file4);
	QTextStream out5(&file5);
	
	
	
//	out << "*Edges" << endl;
	
	
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
		if (!pEdge)
			continue;
		if ((pEdge->getSourceNode()->getNumber() < pEdge->getDestNode()->getNumber()))
		{
			out3 << pEdge->getSourceNode()->getNumber() << " " << pEdge->getDestNode()->getNumber() << endl;
			out4 << pEdge->getSigma() << endl;
			out5 << pEdge->getLength() << " " << pEdge->getWidth() << endl;
		}
		else
		{
			out3 << pEdge->getDestNode()->getNumber() << " " << pEdge->getSourceNode()->getNumber() << endl;
			out4 << pEdge->getSigma() << endl;
			out5 << pEdge->getLength() << " " << pEdge->getWidth() << endl;		}
	}
	file3.close();
	file4.close();
	file5.close();
}








void GraphWidget::savePajek(QString fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&file);
	
	
	out << "*Vertices " << numberOfNodes << endl;
	
	
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Node *pNode = qgraphicsitem_cast<Node *>(item);
		if (!pNode)
			continue;
		out <<  pNode->getNumber() << " " << pNode->getLabel() << " "<< pNode->pos().x()/scaleFactor << " " << pNode->pos().y()/scaleFactor << " " << pNode->getNParticles();
		if (pNode->isSourceNode())
		{
			out << " Source";
		}
		else if (pNode->isSinkNode())
		{
			out << " Sink";
		}
		else {
			out << " Neither";
		}
		if (pNode->getStoma() != NULL)
		{
			out << " " << pNode->getStoma()->getSigma();
		}
		out << endl;
	}
	
	
	out << "*Edges" << endl;
	
	
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
		if (!pEdge)
			continue;
		
		out << pEdge->getSourceNode()->getNumber() << " " << pEdge->getDestNode()->getNumber() << " " << pEdge->getLength() << " " << pEdge->getWidth() << " " << pEdge->getSigma() << endl;
	}
	
	
	file.close();

}






void    GraphWidget::saveGraph(QString fileName)
{
	if (fileName.endsWith(".txt", Qt::CaseInsensitive) || fileName.endsWith(".net", Qt::CaseInsensitive))
	{
		savePajek(fileName);
	}
	else if (fileName.endsWith(".eln", Qt::CaseInsensitive))
	{
		saveEleni(fileName);
	}
}



QRgb GraphWidget::colourMap(double value)
{
	if (currentColourMap == "rainbow")
	{
		// printf("current colourmap is rainbow");
		return (rainbowColourMap(value));
	}
	else if (currentColourMap == "gray")
	{
		return(grayColourMap(value));
	}
	else if (currentColourMap == "pm3d")
	{
		return(pm3dColourMap(value));
	}
	else if (currentColourMap == "daltonic friendly")
	{
		return(daltonicFriendlyColourMap(value));
	}
	return(rainbowColourMap(value));
}



void GraphWidget::setColouringEdgesParameter(QString newColouringEdgesParameter)
{	
	colouringEdgesParameter = newColouringEdgesParameter;
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
		if (!pEdge)
			continue;
		pEdge->reColour(colouringEdgesParameter, edgesColourScale);
	}
	
		sc->update();
}

void GraphWidget::setEdgesColourScale(QString newEdgesColourScale)
{	
	edgesColourScale = newEdgesColourScale;
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
		if (!pEdge)
			continue;
		pEdge->reColour(colouringEdgesParameter, edgesColourScale);
	}
	
	sc->update();
}








void GraphWidget::setColouringStomataParameter(QString newColouringStomataParameter)
{	

		colouringStomataParameter = newColouringStomataParameter;
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item);
		if (!pStoma)
			continue;
		pStoma->reColour(colouringStomataParameter, stomataColourScale);
	}
		sc->update();
}

void GraphWidget::setStomataColourScale(QString newStomataColourScale)
{	
	
	stomataColourScale = newStomataColourScale;
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item);
		if (!pStoma)
			continue;
		pStoma->reColour(colouringStomataParameter, stomataColourScale);
	}
	sc->update();
}












void GraphWidget::setColouringNodesParameter(QString newColouringNodesParameter)
{	
	colouringNodesParameter = newColouringNodesParameter;
		foreach (QGraphicsItem *item, sc->items()) 
		{
			Node *pNode = qgraphicsitem_cast<Node *>(item);
			if (!pNode)
				continue;
			pNode->reColour(colouringNodesParameter, nodesColourScale);
		}
		sc->update();
}

void GraphWidget::setNodesColourScale(QString newNodesColourScale)
{	
	nodesColourScale = newNodesColourScale;
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Node *pNode = qgraphicsitem_cast<Node *>(item);
		if (!pNode)
			continue;
		pNode->reColour(colouringNodesParameter, nodesColourScale);
	}
	sc->update();
}

QString GraphWidget::getNodesColourScale()
{
	return nodesColourScale;
}



void GraphWidget::setNParticlesForAllNodes()
{
	getSelectedGraphicItems();
	if (selectedGraphicItems.size() == 0)
		return;
	
	bool ok;
	int i = QInputDialog::getInt(this, tr("Set N particles in each node"),
									   tr("N:"), 10000, 0, 2147483647, 1, &ok);
	if (ok)
	{
		foreach (QGraphicsItem *item, selectedGraphicItems) 
		{
			if (Node *pNode = qgraphicsitem_cast<Node *>(item))
			{
				// cout << pNode->getNumber() << endl;
				pNode->setNParticles(i);
				// cout << "done in graphwidget" << endl;
			}
		}
		
	}
}






void GraphWidget::getSelectedGraphicItems()
{
	if (currentSelection)
	{
//		cout << currentSelection->normalized().top() << " " << currentSelection->normalized().left() << " " << currentSelection->normalized().width() << " " << currentSelection->normalized().height() << endl;

		selectedGraphicItems=this->items(currentSelection->normalized().left(), currentSelection->normalized().top(), currentSelection->normalized().width(),currentSelection->normalized().height(), Qt::IntersectsItemShape);
	}
	else
	{
		selectedGraphicItems = sc->items();
	}
	return;
}







void GraphWidget::setSourceOrSinkForAllNodes()
{	
	getSelectedGraphicItems();
	if (selectedGraphicItems.size() == 0)
		return;
	
	QStringList possibleChoices;
    possibleChoices << tr("All nodes as source") << tr("All nodes as sink") << tr("Deselect all nodes");
	
	bool ok;
    QString item = QInputDialog::getItem(this, tr("Node selection method"),
                                         tr("Selection method:"), possibleChoices, 2, false, &ok);
	
	
	
	if (ok && !item.isEmpty())
	{
		if (item == possibleChoices[0])
		{
			// printf("select all nodes as Source\n");
			
				foreach (QGraphicsItem *item, selectedGraphicItems) 
			{
				if (Node *pNode = qgraphicsitem_cast<Node *>(item))
				{
					// cout << pNode->getNumber() << endl;
					pNode->setAsSource();
					// cout << "done in graphwidget" << endl;
				}
			}
			
			
			
			
			
		}
		else if (item == possibleChoices[1])
		{
			// printf("select all nodes as Sink\n");
		
			
			foreach (QGraphicsItem *item, selectedGraphicItems) 
			{
				if (Node *pNode = qgraphicsitem_cast<Node *>(item))
				{
					pNode->setAsSink();
					// cout << "done in graphwidget" << endl;
				}
			}
			
			
			
			
			
			
		}
		else if (item == possibleChoices[2])
		{
			// printf("Deselect all nodes\n");
			foreach (QGraphicsItem *item, selectedGraphicItems) 
			{
				if (Node *pNode = qgraphicsitem_cast<Node *>(item))
				{
					// cout << pNode->getNumber() << endl;
					pNode->setAsNeitherSourceNorSink();
					// cout << "done in graphwidget" << endl;
				}
			}
			
			
			
		}
		sc->update();
	}
}



void GraphWidget::setSigmaAsFunctionOfWidthAndLength()
{
		foreach (QGraphicsItem *item, sc->items()) 
		{
			if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item))
			{
				double newSigma = multiplicativeFactorEdgeSigma * pow(pEdge->getWidth(), exponentEdgeWidthForSigma)/pEdge->getLength();
				pEdge->setSigma(newSigma);
			}
		}
}



void GraphWidget::setSigmaForAllEdges(double newEdgeSigmaValue)
{
	if (selectedGraphicItems.size() == 0)
		return;
	
	
	bool ok;
	if (newEdgeSigmaValue < 0)
	{
	newEdgeSigmaValue = QInputDialog::getDouble(this, tr("Set sigma in each edge"),
									   tr("conductivity:"), 0.05, 0, 100000, 8, &ok);
	}
	else {
		ok = true;
	}

	if (ok)
	{
		foreach (QGraphicsItem *item, selectedGraphicItems) 
		{
			if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item))
			{
				pEdge->setSigma(newEdgeSigmaValue);
			}
		}
	}
}

void GraphWidget::setWidthForAllEdges()
{
	if (selectedGraphicItems.size() == 0)
		return;
	
	bool ok;
	double d = QInputDialog::getDouble(this, tr("Set width for each edge"),
									   tr("width:"), 5.0, 0, 1000, 8, &ok);
	if (ok)
	{
		foreach (QGraphicsItem *item, selectedGraphicItems) 
		{
			if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item))
			{
				pEdge->setWidth(d);
			}
		}
	}
}

void GraphWidget::setLengthForAllEdges()
{
	if (selectedGraphicItems.size() == 0)
		return;
	
	bool ok;
	double d = QInputDialog::getDouble(this, tr("Set length for each edge"),
									   tr("length:"), 5.0, 0, 1000, 8, &ok);
	if (ok)
	{
		foreach (QGraphicsItem *item, selectedGraphicItems) 
		{
			if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item))
			{
				pEdge->setLength(d);
			}
		}
	}
}


void GraphWidget::setStomaticSigmaAsFunctionOfPotential(bool willUpdateStomaticSigma)
{
	isUpdatingStomaticSigma = willUpdateStomaticSigma;
}


void GraphWidget::setSigmaAsFunctionOfFlow(bool willUpdateEdgeSigma)
{
	isUpdatingEdgeSigma = willUpdateEdgeSigma;
}


void GraphWidget::setSigmaForAllStomata()
{
	if (selectedGraphicItems.size() == 0)
		return;
	
	bool ok;
	double d = QInputDialog::getDouble(this, tr("Set sigma for each stoma"),
									   tr("conductivity:"), 0.005, 0, 1000, 8, &ok);
	if (ok)
	{
		foreach (QGraphicsItem *item, selectedGraphicItems) 
		{
			if (Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item))
			{
				pStoma->setSigma(d);
			}
		}
	}
}





/* drawGraph reads the graph from file, 
 displays the edges in the scene */

void    GraphWidget::drawGraph(QString fileName)
{
	int isReadingEdges=0; // First it will be reading the list of nodes, then the list of edges
	int nodeCounter=0;
	QString label;
	float x, y;
	int numberSourceNode, numberDestNode;
	double edgeWidth, edgeLength;
	double edgeSigma = -1;
	QFile file(fileName);
	int num_edges=0;
	numberOfNodes = 0;
	
	//areNodesVisible=true;
	//areEdgesVisible=true;
	//areStomataVisible = true;

	
	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	
	if (sc) 
		delete sc;
	sc = new QGraphicsScene(this);
	sc->setItemIndexMethod(QGraphicsScene::NoIndex);
	sc->setSceneRect(0, 0, 1100, 1100);
	// sc->setSceneRect(QRectF ());
	setScene(sc);
	
	QTextStream in(&file);
	QString line = in.readLine();
	QStringList fields = line.split(' ', QString::SkipEmptyParts);
	if (fields.size() == 2)
	{
		QString intestazione = fields.takeFirst();
		numberOfNodes = fields.takeFirst().toInt();
	}
	else{
		return;
	}
	Node *nodo[numberOfNodes];
	
	
	minNParticles = 0;
	maxNParticles = 0;
	
	minFlow = 0;
	maxFlow = 0;
	
	maxSigma = minSigma;
	

	
	
	while (!in.atEnd()) {
		QString line = in.readLine();
		if (line[0] == QChar('*'))
		{
			isReadingEdges =1;
			
			
		}
		else
		{
			if (isReadingEdges==0)
			{
				QStringList fields = line.split(' ', QString::SkipEmptyParts);
				// cout << fields.size() << endl;
				if (fields.size() >= 4) // I accept that the nodes could have more than four fields, 
					// but ignore the fields after the fourth
				{
					nodeCounter = fields.takeFirst().toInt();
					label = fields.takeFirst();
					x = fields.takeFirst().toFloat();
					y = fields.takeFirst().toFloat();

					// cout << fields.size() << endl;
					
					// IMPORTANT! as you can see below the node numbers must be consecutive and start at one
					nodo[nodeCounter-1]=new Node(this);
					nodo[nodeCounter-1]->setPos(QPointF(x*scaleFactor,y*scaleFactor));
					nodo[nodeCounter-1]->setLabel(label);
					nodo[nodeCounter-1]->setNumber(nodeCounter);
					
					if (fields.size() >= 2) // if there are additional fields with nParticles, sourceAndSink
					{
						int nParticles = fields.takeFirst().toInt();
						// cout << nParticles << endl << endl;
						nodo[nodeCounter-1]->setNParticles(nParticles);
						QString sourceOrSink = fields.takeFirst();
						// cout << sourceOrSink.toStdString() << endl;
						if (sourceOrSink == "Source")
						{
							nodo[nodeCounter-1]->setAsSource();
						}
						else if (sourceOrSink == "Sink")
						{
							nodo[nodeCounter-1]->setAsSink();
							if (fields.size()>=1) // if there is additional information about the stomatic sigma
							{
									// get stomatic sigma
								double stomaticSigma = fields.takeFirst().toDouble();
								nodo[nodeCounter-1]->getStoma()->setSigma(stomaticSigma);
							}
						}
						else if (sourceOrSink == "Neither")
						{
							nodo[nodeCounter-1]->setAsNeitherSourceNorSink();
						}
					}
					else
					{
						nodo[nodeCounter-1]->setAsSink();
						nodo[nodeCounter-1]->setNParticles(initialNParticlesPerNode);
					}
					nodo[nodeCounter-1]->reColour(colouringNodesParameter, nodesColourScale);
					sc->addItem(nodo[nodeCounter-1]);

					
//					// Add the stoma to every node (commented, because I only add stomata to sink nodes)
					// createNewStoma(nodo[nodeCounter-1]);
				}
			}
			else // if (isReadingEdges==1)
			{ 
				QStringList fields = line.split(' ', QString::SkipEmptyParts);
				switch (fields.size())
				{
					case 2:
						numberSourceNode = fields.takeFirst().toInt();
						numberDestNode = fields.takeFirst().toInt();
						edgeLength = 1.0;
						edgeWidth = 1.0;
						break;
					case 3:
						numberSourceNode = fields.takeFirst().toInt();
						numberDestNode = fields.takeFirst().toInt();
						edgeLength = fields.takeFirst().toDouble();
						edgeWidth = 1.0;
						break;
					case 14: // this is the number of fields in my files
						double dontCare;
						numberSourceNode = fields.takeFirst().toInt();
						numberDestNode = fields.takeFirst().toInt();
						edgeLength = fields.takeFirst().toDouble();
						dontCare = fields.takeFirst().toDouble();//4
						dontCare = fields.takeFirst().toDouble();//5
						dontCare = fields.takeFirst().toDouble();//6
						dontCare = fields.takeFirst().toDouble();//7
						dontCare = fields.takeFirst().toDouble();//8
						dontCare = fields.takeFirst().toDouble();//9
						dontCare = fields.takeFirst().toDouble();//10
						dontCare = fields.takeFirst().toDouble();//11
						dontCare = fields.takeFirst().toDouble();//12
						edgeWidth = fields.takeFirst().toDouble();
						break;
					case 5:
						numberSourceNode = fields.takeFirst().toInt();
						numberDestNode = fields.takeFirst().toInt();
						edgeLength = fields.takeFirst().toDouble();
						edgeWidth = fields.takeFirst().toDouble();
						edgeSigma = fields.takeFirst().toDouble();
						break;
					case 4:
					default: // I accept that the edges can have more than four fields, 
						// but ignore the fields after the fourth
						numberSourceNode = fields.takeFirst().toInt();
						numberDestNode = fields.takeFirst().toInt();
						edgeLength = fields.takeFirst().toDouble();
						edgeWidth = fields.takeFirst().toDouble();
						break;
				}
				if (edgeWidth > maxEdgeWidth)
				{
					maxEdgeWidth = edgeWidth;
				}
				Edge *pMyEdge = createNewEdge(nodo[numberSourceNode-1], nodo[numberDestNode-1], edgeLength, edgeWidth, edgeSigma);
				if (pMyEdge->getSigma() > maxSigma)
				{
					maxSigma = pMyEdge->getSigma();
				}
				num_edges+=1;
			}
		}
		
	}
	
//	foreach (QGraphicsItem *item, sc->items()) 
//	{
//		if (Node *pMyNode = qgraphicsitem_cast<Node *>(item))
//		{
//
//			pMyNode->reColour(colouringNodesParameter, nodesColourScale);
//		}
//		else if (Stoma *pMyStoma = qgraphicsitem_cast<Stoma *>(item))
//		{
//			pMyStoma->reColour(colouringStomataParameter, stomataColourScale);
//		}
//		else if (Edge *pMyEdge = qgraphicsitem_cast<Edge *>(item))
//		{
//			pMyEdge->reColour(colouringEdgesParameter, edgesColourScale);
//		}
//	}
//	

	
	file.close();
	// sc->update();
}
	



Edge *GraphWidget::createNewEdge(Node *sourceNode, Node *destNode, double edgeLength, double edgeWidth, double edgeSigma)
{
	Edge *pMyEdge = new Edge(this,sourceNode, destNode);
	pMyEdge->setLength(edgeLength);
	pMyEdge->setWidth(edgeWidth);
	pMyEdge->initialize(edgeSigma);
	pMyEdge->reColour(colouringEdgesParameter, edgesColourScale);
	sc->addItem(pMyEdge);
	return pMyEdge;
}


Stoma *GraphWidget::createNewStoma(Node *sourceNode)
{

	Stoma *pMyStoma = new Stoma(this, sourceNode);
	pMyStoma->initialize();
//	pMyStoma->setLength(1);
//	pMyStoma->setWidth(1);
	//pMyStoma->setVisible(false);
	if (pMyStoma->getSigma() > maxSigma)
	{
		maxSigma = pMyStoma->getSigma();
	}
	pMyStoma->reColour(colouringStomataParameter, stomataColourScale);
	sc->addItem(pMyStoma);

	return pMyStoma;
}







void GraphWidget::zoom(qreal scaleFactor)
{
	foreach (QGraphicsItem *item, sc->items()) 
	{
		if (Node *node = qgraphicsitem_cast<Node *>(item))
		{
			node->setPos(node->pos()*scaleFactor);
		}
		else if (Stoma *stoma = qgraphicsitem_cast<Stoma *>(item))
		{
			stoma->setPos(stoma->pos()*scaleFactor);
		}
	}
	
	
	
	foreach (QGraphicsItem *item, sc->items()) {
		if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item)){
			pEdge->adjust();
		}
	}
	
}











void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 500.0));
}









void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;
	
    scale(scaleFactor, scaleFactor);
}







/* I reimplement mousePressEvent so that we can select an edge more precisely. On the other hand, this way 
 we are no longer able to select nodes. */
void GraphWidget::mousePressEvent ( QMouseEvent *event )
{
	currentSelection = NULL;
	if (isRegionSelected)
	{
		isRegionSelected = false;
		return;
	}
	
    int x=event->pos().x();
    int y=event->pos().y();
	
    QList<QGraphicsItem *>items=this->items(x-2.5,y-2.5,5,5,Qt::IntersectsItemShape);
	
	if (whatIsSelecting == "Nodes")
	{
		for(int i=items.size()-1;i>=0;i--)
		{
			QGraphicsItem *item=items.at(i);
			if (Node *pNode = qgraphicsitem_cast<Node *>(item))
			{
				nodePropertiesMenu(event, pNode);
				return; //one node has been selected
			}
		}
    }
	else if (whatIsSelecting == "Edges")
	{
		for(int i=items.size()-1;i>=0;i--)
		{
			QGraphicsItem *item=items.at(i);
			if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item))
			{
				edgePropertiesMenu(event, pEdge);
				return; //one edge has been selected
			}
		}
    }
	else if (whatIsSelecting == "Stomata")
	{
		for(int i=items.size()-1;i>=0;i--)
		{
			QGraphicsItem *item=items.at(i);
			if (Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item))
			{
				stomaPropertiesMenu(event, pStoma);
				return; //one edge has been selected
			}
		}
    }
	rubberBandOrigin = event->pos();
	if (!rubberBand)
		rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	rubberBand->setGeometry(QRect(rubberBandOrigin, QSize()));
	rubberBand->show();
	isRegionSelected = true;
}





void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (rubberBand)
	{
		rubberBand->setGeometry(QRect(rubberBandOrigin, event->pos()).normalized());
	}
}


void GraphWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (rubberBand && isRegionSelected)
	{
		if (!currentSelection)
		{
			currentSelection = new QRectF(rubberBandOrigin, event->pos());
		}
// 		QList<QGraphicsItem *>items=this->items(QRectF(rubberBandOrigin, event->pos()),Qt::IntersectsItemShape);
		// cout << items.size() << endl;
		// setSigmaForAllStomata();


		setPropertyToSelectedElements(event);
		isRegionSelected = false;
		currentSelection = NULL;
		rubberBand->hide();
	}
}



double GraphWidget::getChargePerParticle()
{
	return chargePerParticle;
}


double GraphWidget::getDeltaT()
{
	return deltaT;
}

double GraphWidget::getMinSigma()
{
	return minSigma;
}

double GraphWidget::getMaxSigma()
{
	return maxSigma;
}

double GraphWidget::getMaxEdgeWidth()
{
	return maxEdgeWidth;
}

int GraphWidget::getParticlesAtSource()
{
	return particlesAtSource;
}

void GraphWidget::setChargePerParticle(double newChargePerParticle)
{
	chargePerParticle = newChargePerParticle;
}


void GraphWidget::setDeltaT(double newDeltaT)
{
	deltaT = newDeltaT;
}

void GraphWidget::setMinSigma(double newMinSigma)
{
	minSigma = newMinSigma;
}

void GraphWidget::setParticlesAtSource(int newParticlesAtSource)
{
	particlesAtSource = newParticlesAtSource;
}



//
//
//void GraphWidget::setCurrentNodeAsSource() 
//{
//	if (currentNode == NULL)
//		return;
//	currentNode->setAsSource();
//	currentNode = NULL;
//}
//
//
//void GraphWidget::setCurrentNodeAsSink() 
//{
//	if (currentNode == NULL)
//		return;
//	currentNode->setAsSink();
//	currentNode = NULL;
//}
//
//
//void GraphWidget::setCurrentNodeAsNeitherSourceNorSink() 
//{
//	if (currentNode == NULL)
//		return;
//	currentNode->setAsNeitherSourceNorSink();
//	currentNode = NULL;
//}

QRgb GraphWidget::grayColourMap(double value) // value is between 0 and 1
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
	
	r = 1 - value;
	if (r<0)
		r=0;
	if (r>1)
		r=1;
	
	g = r;
	b = r;
	
	return qRgb(int(r * 255), int(g * 255), int(b * 255));
}

QRgb GraphWidget::pm3dColourMap(double value)
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
	
	r = sqrt(abs(value));
	if (r<0)
		r=0;
	if (r>1)
		r=1;
	
	g = pow(value, 3.0);
	if (g<0)
		g=0;
	if (g>1)
		g=1;
	
	b = sin(value*2*PI);
	if (b<0)
		b=0;
	if (b>1)
		b=1;
	
	return qRgb(int(r * 255), int(g * 255), int(b * 255));
}


QRgb GraphWidget::daltonicFriendlyColourMap(double value) // value is between 0 and 1
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

r = value/0.32 - 0.78125;
	if (r<0)
		r=0;
	if (r>1)
		r=1;

	g = 2*value -0.84;
	if (g<0)
		g=0;
	if (g>1)
		g=1;
	
	if (value <= 0.25)
	{
		b= 4*value;
	}
	else if (value > 0.25 && value <= 0.42)
	{
		b = 1;
	}
	else if (value > 0.42 && value <= 0.92)
	{
		b = -2*value + 1.84;
	}
	else
	{
		b= value/0.08 - 11.5;
	}

	return qRgb(int(r * 255), int(g * 255), int(b * 255));
}



QRgb GraphWidget::rainbowColourMap(double value) // value is between 0 and 1
{
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
	
	double wave = (value * 400.0 + 380.0);
	
    if (wave >= 380.0 && wave <= 440.0) {
        r = -1.0 * (wave - 440.0) / (440.0 - 380.0);
        b = 1.0;
    } else if (wave >= 440.0 && wave <= 490.0) {
        g = (wave - 440.0) / (490.0 - 440.0);
        b = 1.0;
    } else if (wave >= 490.0 && wave <= 510.0) {
        g = 1.0;
        b = -1.0 * (wave - 510.0) / (510.0 - 490.0);
    } else if (wave >= 510.0 && wave <= 580.0) {
        r = (wave - 510.0) / (580.0 - 510.0);
        g = 1.0;
    } else if (wave >= 580.0 && wave <= 645.0) {
        r = 1.0;
        g = -1.0 * (wave - 645.0) / (645.0 - 580.0);
    } else if (wave >= 645.0 && wave <= 780.0) {
        r = 1.0;
    }
	
    double s = 1.0;
    if (wave > 700.0)
        s = 0.3 + 0.7 * (780.0 - wave) / (780.0 - 700.0);
    else if (wave <  420.0)
        s = 0.3 + 0.7 * (wave - 380.0) / (420.0 - 380.0);
	
    r = pow(r * s, 0.8);
    g = pow(g * s, 0.8);
    b = pow(b * s, 0.8);
	
//	printf("Colour: %f %f %f\n", r*255.0, g*255.0, b*255.0);
	
    return qRgb(int(r * 255), int(g * 255), int(b * 255));
}






int GraphWidget::getNumberOfNodes()
{
	int counter = 0;
	foreach (QGraphicsItem *item, sc->items()) 
		if (qgraphicsitem_cast<Node *>(item))
			counter+=1;
	return counter;
}

int GraphWidget::getNumberOfEdges()
{
	int counter = 0;
	foreach (QGraphicsItem *item, sc->items()) 
	if (qgraphicsitem_cast<Edge *>(item))
		counter+=1;
	return counter;
}

int GraphWidget::getNumberOfStomata()
{
	int counter = 0;
	foreach (QGraphicsItem *item, sc->items()) 
	if (qgraphicsitem_cast<Stoma *>(item))
		counter+=1;
	return counter;
}




int GraphWidget::getNumberOfSourceNodes()
{
	int counter = 0;
	foreach (QGraphicsItem *item, sc->items()) 
	if (Node *pNode = qgraphicsitem_cast<Node *>(item))
		if (pNode->isSourceNode())
			counter += 1;
	return counter;
}

int GraphWidget::getNumberOfSinkNodes()
{
	int counter = 0;
	foreach (QGraphicsItem *item, sc->items()) 
	if (Node *pNode = qgraphicsitem_cast<Node *>(item))
		if (pNode->isSinkNode())
			counter += 1;
	return counter;
}


void GraphWidget::setPropertyToSelectedElements(QMouseEvent *event)
{
	getSelectedGraphicItems();
	if (selectedGraphicItems.size() == 0)
		return;
//	cout << selectedGraphicItems.size() << endl;
	isRegionSelected = false;
    QPoint globalPos = event->globalPos();// this->mapToGlobal(pos);
    QMenu myMenu;
	QAction *setAsAction = new QAction(QString(tr("Source or sink")), this);
	QAction *setNParticlesAct = new QAction(QString(tr("N particles")), this);
	QAction *deleteEdgesAct = new QAction(QString(tr("Delete selected edges")), this);
	
	
	switch (event->button())
	{
		case (Qt::LeftButton):
			myMenu.addAction(QString(tr("Set property to selected nodes")));
			myMenu.addSeparator();
			myMenu.addAction(setAsAction);
			myMenu.addAction(setNParticlesAct);
			break;
		case (Qt::RightButton):
			myMenu.addAction(deleteEdgesAct);
			break;
		default:
			myMenu.addAction(tr("Menu Other button"));
	}
	
			
			
	
    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
		if (selectedItem == setAsAction)
		{
			setSourceOrSinkForAllNodes();
		}
		else if (selectedItem == setNParticlesAct)
		{
			setNParticlesForAllNodes();
		}
		else if (selectedItem == deleteEdgesAct)
		{
			foreach (QGraphicsItem *item, selectedGraphicItems) 
			{
				if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item))
					pEdge->deleteEdge();
			}
		}
    }
    else
    {
		selectedGraphicItems.clear();
        return;
    }
	selectedGraphicItems.clear();
}


void GraphWidget::nodePropertiesMenu(QMouseEvent *event, Node *pNode)
{
	
    QPoint globalPos = event->globalPos();// this->mapToGlobal(pos);
    QMenu myMenu;
	QMenu *setAsMenu;
	QString sourceOrSink;
	QAction *setNodeAsSourceAct = new QAction(tr("source"), this);
	setNodeAsSourceAct->setCheckable(true);
	setNodeAsSourceAct->setChecked(pNode->isSourceNode());	
	QAction *setNodeAsSinkAct = new QAction(tr("sink"), this);
	setNodeAsSinkAct->setCheckable(true);
	setNodeAsSinkAct->setChecked(pNode->isSinkNode());	
	QAction *setNodeAsNeitherSourceNorSinkAct = new QAction(tr("neither"), this);
	setNodeAsNeitherSourceNorSinkAct->setCheckable(true);
	setNodeAsNeitherSourceNorSinkAct->setChecked(!pNode->isSourceNode() && !pNode->isSinkNode());	
	QAction *setNParticlesAct = new QAction(QString(tr("N particles %1")).arg((pNode->getNParticles())), this);
	
	switch (event->button())
	{
		case (Qt::LeftButton):
			myMenu.addAction(QString(tr("Node number %1")).arg(pNode->getNumber()));
			myMenu.addSeparator();
			myMenu.addAction(setNParticlesAct);
			if (pNode->isSourceNode())
			{
				sourceOrSink = tr("source node");
				// myMenu.addAction(QString(tr("source node")));
			}
			else if (pNode->isSinkNode())
			{
				sourceOrSink = tr("sink node");
				// myMenu.addAction(QString(tr("sink node")));
			}
			else if (!pNode->isSourceNode() && !pNode->isSinkNode())
			{
				sourceOrSink = tr("transit node");
				// myMenu.addAction(QString(tr("transit node")));
			}
			
			setAsMenu = myMenu.addMenu(sourceOrSink);
			setAsMenu->addAction(setNodeAsSourceAct);
			setAsMenu->addAction(setNodeAsSinkAct);
			setAsMenu->addAction(setNodeAsNeitherSourceNorSinkAct);
			break;
		case (Qt::RightButton):
			myMenu.addAction(tr("Menu button right"));
			break;
		default:
			myMenu.addAction(tr("Menu Other button"));
	}
    // ...
	
    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
		if (selectedItem == setNodeAsSinkAct)
		{
			pNode->setAsSink();
		}
		else if (selectedItem == setNodeAsSourceAct)
		{
			pNode->setAsSource();
		}
		else if (selectedItem == setNodeAsNeitherSourceNorSinkAct)
		{
			pNode->setAsNeitherSourceNorSink();
		}
		else if (selectedItem == setNParticlesAct)
		{
			bool ok;
			int i = QInputDialog::getInt(this, tr("Set N particles in this node"),
										 tr("N:"), pNode->getNParticles(), 0, 2147483647, 1, &ok);
			if (ok)
				pNode->setNParticles(i);
		}
    }
    else
    {
        return;
    }
}











void GraphWidget::edgePropertiesMenu(QMouseEvent *event, Edge* pEdge)
{
	
    QPoint globalPos = event->globalPos();// this->mapToGlobal(pos);
	QAction *deleteEdgeAct = new QAction(tr("remove this edge"), this);
	QAction *setSigmaAct = new QAction(QString(tr("Sigma %1")).arg((pEdge->getSigma())), this);
	QAction *setLengthAct = new QAction(QString(tr("Length %1")).arg((pEdge->getLength())), this);
	QAction *setWidthAct = new QAction(QString(tr("Width %1")).arg((pEdge->getWidth())), this);
	// QAction *setWeightAct = new QAction(QString(tr("Weight %1")).arg((pEdge->getWeight())), this);
	
	
    QMenu myMenu;
	switch (event->button())
	{
		case (Qt::LeftButton):
			myMenu.addAction(QString(tr("Edge %1--%2")).arg(pEdge->getSourceNode()->getNumber()).arg(pEdge->getDestNode()->getNumber()));
			myMenu.addAction(QString(tr("Flow: %1")).arg(pEdge->getFlow()));
			myMenu.addAction(QString(tr("Orientation: %1 pi")).arg(pEdge->getOrientation()/PI));
			myMenu.addSeparator();
			myMenu.addAction(setSigmaAct);
			myMenu.addAction(setLengthAct);
			myMenu.addAction(setWidthAct);
			// myMenu.addAction(setWeightAct);

//			if (pEdge->isStoma() == false)
//			{ myMenu.addAction(QString(tr("Not stoma"))); }
//			else 
//			{ myMenu.addAction(QString(tr("Stoma"))); }
			
			break;
		case (Qt::RightButton):
			myMenu.addAction(deleteEdgeAct);
			break;
		default:
			myMenu.addAction(tr("Menu Other button"));
	}
    // ...
	
    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
		if (selectedItem == setSigmaAct)
		{
			bool ok;
			double d = QInputDialog::getDouble(this, tr("Set sigma for this edge"),
											   tr("conductivity:"), pEdge->getSigma(), 0, 1000, 8, &ok);
			if (ok)
				pEdge->setSigma(d);
		}
		else if (selectedItem == setLengthAct)
		{
			bool ok;
			double d = QInputDialog::getDouble(this, tr("Set length for this edge"),
											   tr("length:"), pEdge->getLength(), 0, 1000, 8, &ok);
			if (ok)
				pEdge->setLength(d);
		}
		else if (selectedItem == setWidthAct)
		{
			bool ok;
			double d = QInputDialog::getDouble(this, tr("Set width for this edge"),
											   tr("width:"), pEdge->getWidth(), 0, 1000, 8, &ok);
			if (ok)
			{
				pEdge->setWidth(d);
				if (d > maxEdgeWidth)
				{
					maxEdgeWidth = d;
				}
			}
		}
//		else if (selectedItem == setWeightAct)
//		{
//			bool ok;
//			double d = QInputDialog::getDouble(this, tr("Set weight for this edge"),
//											   tr("weight:"), pEdge->getWeight(), 0, 1000, 8, &ok);
//			if (ok)
//				pEdge->setWeight(d);
//		}
		else if (selectedItem == deleteEdgeAct)
	   {
		   pEdge->deleteEdge();
	   }
    }
    else
    {
        // nothing was chosen
    }
}







void GraphWidget::stomaPropertiesMenu(QMouseEvent *event, Stoma* pStoma)
{
	
    QPoint globalPos = event->globalPos();// this->mapToGlobal(pos);
	QAction *setSigmaAct = new QAction(QString(tr("Sigma %1")).arg((pStoma->getSigma())), this);
	
    QMenu myMenu;
	switch (event->button())
	{
		case (Qt::LeftButton):
			myMenu.addAction(QString(tr("Stoma %1")).arg(pStoma->getSourceNode()->getNumber()));
			myMenu.addAction(QString(tr("Flow: %1")).arg(pStoma->getFlow()));
			myMenu.addSeparator();
			myMenu.addAction(setSigmaAct);
			
			break;
		case (Qt::RightButton):
			myMenu.addAction(tr("Right mouse button on stoma"));
			break;
		default:
			myMenu.addAction(tr("Menu Other button"));
	}
    // ...
	
    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
		if (selectedItem == setSigmaAct)
		{
			bool ok;
			double d = QInputDialog::getDouble(this, tr("Set sigma for this stoma"),
											   tr("conductivity:"), pStoma->getSigma(), 0, 1000, 8, &ok);
			if (ok)
				pStoma->setSigma(d);
		}
    }
    else
    {
        // nothing was chosen
    }
}









void GraphWidget::performOneSimulationStep()
{
	
	// I normalize the colours of the edges and nodes on the maximum and minimum property (sigma, flow, n particles)
	// These maxima and minima can change from one step to the next, but I don't like the flickering produced by their
	// rapid change. In order to reduce the flickering, I keep trace of the maximum and minimum at the previous step
	// and take some sort of average
	
	int previousMinFlow = minFlow;
	int previousMaxFlow = maxFlow;
	double previousMaxSigma = maxSigma;
	int previousMinNParticles = minNParticles;
	int previousMaxNParticles = maxNParticles;
	
	
	minFlow = INT_MAX;
	maxFlow = 0;
	maxSigma = 0;
	minNParticles = INT_MAX;
	maxNParticles = 0;
	

	
	
	pMainWindow->increaseSimulationTime(1);
//	printf("simulation step\n");
	// update source and sink nodes
	foreach (QGraphicsItem *item, sc->items()) 
	{
		Node *pNode = qgraphicsitem_cast<Node *>(item);
		if (!pNode)
			continue;
		if (pNode->isSourceNode())
		{
			pNode->setNParticles(particlesAtSource);
		}
		else if (pNode->isSinkNode())
		{
			Stoma *pStoma = pNode->getStoma();
			int diffNParticles = pNode->getNParticles(); // atmospheric potential is zero
			pStoma->setFlow(binomDist(diffNParticles, pStoma->getSigma()*deltaT));
			pNode->subtractParticles(pStoma->getFlow());
			
			
			
			// y = 1 / (1+exp(slope*(-x+center)));
			// y=tanh({({x-1})|_cdot_2})|_cdot_0.5+0.5
			// update stomatic conductivity based on node potential
			// remember diffNParticles* chargePerParticle
			
			// double slope = 0.1;
			// double flexPoint = 500;
			// pStoma->setSigma(tanh(slope*(double(diffNParticles) - flexPoint))*0.1 + 0.1);
		}

				
	}
	
	// compute potential difference across edges. Read the edges in random sequence
	// and update the flow and the particles at the two adjacent nodes
	QList <Edge *> edgeList;
	foreach (QGraphicsItem *item, sc->items())
	{
		Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
		if (!pEdge)
			continue;
		edgeList.append(pEdge);
	}
	std::random_shuffle( edgeList.begin(), edgeList.end() ); // shuffle the edgeList
	
	
	foreach (Edge *pEdge, edgeList) 
	{
//		printf("edge %d--%d\n", item->getSourceNode()->getNumber(), item->getDestNode()->getNumber());
//		Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
//		if (!pEdge)
//			continue;
		int diffNParticles = pEdge->getDestNode()->getNParticles() - 
		pEdge->getSourceNode()->getNParticles();
		if (diffNParticles == 0)
		{
			pEdge->setFlow(0);
		}
		else if (diffNParticles > 0)
		{ // if there are more particles in dest than in source, the flow is 
			// from dest to source, i.e. negative
			pEdge->setFlow(- binomDist(diffNParticles, pEdge->getSigma()*deltaT));// * diffNParticles/abs(diffNParticles));
		}
		else if (diffNParticles < 0)
		{
			pEdge->setFlow(binomDist(-diffNParticles, pEdge->getSigma()*deltaT));// * diffNParticles/abs(diffNParticles));

		}
		// move particles
		
		pEdge->getSourceNode()->subtractParticles(pEdge->getFlow());
		pEdge->getDestNode()->addParticles(pEdge->getFlow());
		
		// update conductivity (sigma_ij) for each edge
//		printf("minSigma %f, sigma %f, flow %d\n", minSigma, pEdge->getSigma(), pEdge->getFlow());
		if (isUpdatingEdgeSigma)
		{
			pEdge->setSigma(
						max(
							pEdge->getSigma() * (1.0 - deltaT) + 
							abs(pEdge->getFlow()) * chargePerParticle,
							minSigma
							)
						);
		}
		
		if (pEdge->getSigma() > maxSigma)
		{
			maxSigma = pEdge->getSigma();
		}
		if (pEdge->getFlow() > maxFlow)
		{
			maxFlow = pEdge->getFlow();
		}
		if (pEdge->getFlow() < minFlow)
		{
			minFlow = pEdge->getFlow();
		}
		
	}
	
	
	
	// if the max changed fast I retain most of the previous max and min
	if (maxFlow < previousMaxFlow)
	{
		maxFlow = (maxFlow + previousMaxFlow*9)/10;
	}
	if (minFlow > previousMinFlow)
	{
		minFlow = (minFlow + previousMinFlow*9)/10;
	}
	if (maxSigma < previousMaxSigma)
	{
		maxSigma = (maxSigma + previousMaxSigma*9)/10;
	}

	
	
	
	
	
	// recolour edges and nodes
	if (isShowingUpdate)
	{
	foreach (QGraphicsItem *item, sc->items())
	{
		Node *pNode = qgraphicsitem_cast<Node *>(item);
		if (!pNode)
		{
			if (Edge *pEdge = qgraphicsitem_cast<Edge *>(item))
			{
				pEdge->reColour(colouringEdgesParameter, edgesColourScale);
			}
			else if (Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item)) {
				pStoma->reColour(colouringStomataParameter, stomataColourScale);
			}

		}
		else
		{
			if (pNode->getNParticles() > maxNParticles)
			{
				maxNParticles = pNode->getNParticles();
			}
		
			if (pNode->getNParticles() < minNParticles)
			{
				minNParticles = pNode->getNParticles();
			}
			if (maxNParticles < previousMaxNParticles)
			{
				maxNParticles = (maxNParticles + previousMaxNParticles*9)/10;
			}
			if (minNParticles > previousMinNParticles)
			{
				minNParticles = (minNParticles + previousMinNParticles*9)/10;
			}
		}
	}
	
	
	foreach (QGraphicsItem *item, sc->items())
	{
		Node *pNode = qgraphicsitem_cast<Node *>(item);
		if (!pNode)
			continue;
		pNode->reColour(colouringNodesParameter, nodesColourScale);
	}
}
	
	
	// upade the scene
	//sc->update();
}






void GraphWidget::paintPicture(QPainter &painter)
{
	
	//sc->update();
	foreach (QGraphicsItem *item, sc->items())
	{
		Edge *pEdge = qgraphicsitem_cast<Edge *>(item);
		if (!pEdge)
			continue;
		pEdge->reColour(colouringEdgesParameter, edgesColourScale);
		pEdge->paintPicture(painter);	
		
	}
	
	foreach (QGraphicsItem *item, sc->items())
	{
		Node *pNode = qgraphicsitem_cast<Node *>(item);
		if (!pNode)
			continue;
		pNode->reColour(colouringNodesParameter, nodesColourScale);
		pNode->paintPicture(painter);
	}
	foreach (QGraphicsItem *item, sc->items())
	{
		Stoma *pStoma = qgraphicsitem_cast<Stoma *>(item);
		if (!pStoma)
			continue;
		pStoma->reColour(colouringStomataParameter, stomataColourScale);
		pStoma->paintPicture(painter);
	}
	
	
	
}



