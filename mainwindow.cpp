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


#include <QtWidgets>
#include <QtSvg/QSvgGenerator>
#include <QPainter>

#include "mainwindow.h"
#include "graphwidget.h"
#include "parameterdialog.h"
#include "sigmaequationdialog.h"
#include "dialogrecordingparameters.h"

MainWindow::MainWindow()
{
	w = new GraphWidget(this);
    setCentralWidget(w);
    setWindowIcon(QIcon(":/images/clique.png"));
	
	parameterDialog = 0;
	sigmaEquationDialog = 0;
	dialogRecordingParameters = 0;
	currentSimulationTime = 0;
	isRunningSimulation = false;
	isRecordingSimulation = false;
	
	
    createActions();
    createMenus();   
	createToolBars();
	
    setWindowTitle(tr("Electric Leaf"));
    setMinimumSize(400, 400);
    resize(1000, 600);
	
	readRecentFilesList();
	
	QSettings settings("Andrea Perna", "Electric Leaf Program");
	w->setChargePerParticle(settings.value("chargePerParticle", QVariant(0.05)).toDouble());	
	totRunningTime = settings.value("totRunningTime", QVariant(2000.0)).toInt();	
	w->setDeltaT(settings.value("deltaT", QVariant(0.001)).toDouble());	
	w->setMinSigma(settings.value("minSigma", QVariant(0.001)).toDouble());
	w->setParticlesAtSource(settings.value("particlesAtSource", QVariant(10)).toInt());
	w->setInitialNParticlesPerNode(settings.value("initialNParticlesPerNode", QVariant(10)).toInt());
	w->setExponentEdgeWidthForSigma(settings.value("exponentEdgeWidthForSigma", QVariant(2)).toDouble());
	w->setMultiplicativeFactorEdgeSigma(settings.value("multiplicativeFactorEdgeSigma", QVariant(10)).toDouble());	
	curFileName = settings.value("curFileName", QVariant(QDir::homePath())).toString();

	myTimerID = 0;
}


void MainWindow::readRecentFilesList()
{

    QSettings settings("Andrea Perna", "Electric Leaf Program");
	
	// recently opened files
    QStringList files = settings.value("recentFileList").toStringList();
	
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
	
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
	
    separatorAct->setVisible(numRecentFiles > 0);
	
}

//void MainWindow::writeSettings()
//{
//	QSettings settings("Andrea Perna", "Electric Leaf Program");
	// add more saved settings here with settings.setValue()
//}



void MainWindow::quit()
{
//	if (true)
//		writeSettings();
	exit(EXIT_SUCCESS);
}



void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), curFileName);
    if (!fileName.isEmpty())
	{
		QSettings settings("Andrea Perna", "Electric Leaf Program");
		settings.setValue("curFileName", fileName);

        w->drawGraph(fileName);
		setCurrentFile(fileName);
		
		// enable all the actions on the open network
		enableActionsAndMenus();
		statusBar()->showMessage(tr("File loaded"), 2000);
		resetSimulationTime();
	}
}





void MainWindow::save()
{
    if (curFileName.isEmpty())
	{
        saveAs();
	}
    else
	{
        w->saveGraph(curFileName);
		statusBar()->showMessage(tr("File saved"), 2000);
	}
}





void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), curFileName);
    if (fileName.isEmpty())
        return;
	
	QSettings settings("Andrea Perna", "Electric Leaf Program");
	settings.setValue("curFileName", fileName);
	
    w->saveGraph(fileName);
	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File saved"), 2000);
}




void MainWindow::exportPicture(QString exportFileName)
{
	QImage *image = new QImage(QSize(1100, 1100), QImage::Format_ARGB32_Premultiplied);
	QPainter painter;
	painter.begin(image);
	painter.setRenderHint(QPainter::Antialiasing, true);
	w->paintPicture(painter);
	painter.end();
	
	// Save it..
	image->save(exportFileName, "PNG");
}

void MainWindow::selectExportingFormat()
{
	int lastDotPosition = curFileName.lastIndexOf(".");
	QString exportFileName = curFileName;
	exportFileName.truncate(lastDotPosition);
	exportFileName.append(".svg");
	exportFileName = QFileDialog::getSaveFileName(this, tr("Export picture"),
													   exportFileName, tr("SVG files (*.svg);; all image files (*.png *.jpg);; txt network files (*.txt *net *eln)"));
	
	// cout << exportFileName.toStdString() << endl;
	
	if (exportFileName.endsWith(".svg", Qt::CaseInsensitive))
	{
		exportSVG(exportFileName);
	}
	else if (exportFileName.endsWith(".png", Qt::CaseInsensitive) || exportFileName.endsWith(".jpg", Qt::CaseInsensitive))
	{
		exportPicture(exportFileName);
	}
	else if (exportFileName.endsWith(".txt", Qt::CaseInsensitive) || exportFileName.endsWith(".net", Qt::CaseInsensitive) || exportFileName.endsWith(".eln", Qt::CaseInsensitive))
	{
		w->saveGraph(exportFileName);
	}
}


void MainWindow::exportSVG(QString exportFileName)
{		
		if (exportFileName.isEmpty())
			return;
		

	
	QSvgGenerator generator;
	generator.setFileName(exportFileName);
	generator.setSize(QSize(1100, 1100));
	generator.setViewBox(QRect(0, 0, 1100, 1100));
	generator.setTitle(tr("Electric Leaf SVG generator"));
	generator.setDescription(tr("An SVG snapshot created by the Electric Leaf program"
								"Andrea Perna 2011."));
	
	QPainter painter;
	painter.begin(&generator);
	painter.setRenderHint(QPainter::Antialiasing, true);
	
	w->paintPicture(painter);
	painter.end();
	
}



void MainWindow::enableActionsAndMenus()
{
	changeSelectionMethodMenu->setEnabled(true);
	setAllNodesPropertyMenu->setEnabled(true);
	setAllEdgesPropertyMenu->setEnabled(true);
	setAllStomataPropertyMenu->setEnabled(true);
	saveAct->setEnabled(true);
	saveAsAct->setEnabled(true);
	exportAct->setEnabled(true);
	setSourceOrSinkForAllNodesAct->setEnabled(true);
	setNParticlesForAllNodesAct->setEnabled(true);
	setSigmaForAllEdgesAct->setEnabled(true);
	setSigmaAsFunctionOfWidthAndLengthAct->setEnabled(true);
	setSigmaAsFunctionOfFlowAct->setEnabled(true);
	setWidthForAllEdgesAct->setEnabled(true);
	setLengthForAllEdgesAct->setEnabled(true);
	setSigmaForAllStomataAct->setEnabled(true);
	setAllStomataSigmaMenu->setEnabled(true);
	setStomaticSigmaAsFunctionOfPotentialAct->setEnabled(true);
	setAllEdgesSigmaMenu->setEnabled(true);
	setStomaticSigmaAsFunctionOfPotentialAct->setEnabled(true);
	selectionMethodCombo->setEnabled(true);
	setSelectNodesAct->setEnabled(true);
	setSelectEdgesAct->setEnabled(true);
	setSelectStomataAct->setEnabled(true);
	toggleNodesVisibleAct->setEnabled(true);
	toggleEdgesVisibleAct->setEnabled(true);
	toggleStomataVisibleAct->setEnabled(true);
	setColouringNodesWithDegreeAct->setEnabled(true);
	setColouringNodesWithNParticlesAct->setEnabled(true);
	setColouringNodesWithSourceOrSinkAct->setEnabled(true);
	setColouringEdgesWithFlowAct->setEnabled(true);
	setColouringEdgesWithFlowDirAct->setEnabled(true);
	setColouringEdgesWithSigmaAct->setEnabled(true);
	setColouringEdgesWithWidthAct->setEnabled(true);
	setColouringStomataWithSigmaAct->setEnabled(true);
	setColouringStomataWithFlowAct->setEnabled(true);
	runAct->setEnabled(true);
	zoomInAct->setEnabled(true);
	zoomOutAct->setEnabled(true);
}



void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
	{
		QString fileName=action->data().toString();
		w->drawGraph(fileName);
		setCurrentFile(fileName);
		enableActionsAndMenus();
		resetSimulationTime();
	}
}



void MainWindow::setSigmaAsFunctionOfWidthAndLength()
{
	// printf("now calling sigma equation dialog\n");
	if (!sigmaEquationDialog)                                  
		sigmaEquationDialog = new SigmaEquationDialog(this);
	// printf("now created sigma equation dialog\n");
	if(sigmaEquationDialog->exec())
	{
		w->setMultiplicativeFactorEdgeSigma(sigmaEquationDialog->multiplicativeFactorEdgeSigma().toDouble());
		w->setExponentEdgeWidthForSigma(sigmaEquationDialog->exponentEdgeWidthForSigma().toDouble());
	
		QSettings settings("Andrea Perna", "Electric Leaf Program");
		settings.setValue("exponentEdgeWidthForSigma", w->getExponentEdgeWidthForSigma());		
		settings.setValue("multiplicativeFactorEdgeSigma", w->getMultiplicativeFactorEdgeSigma());
		w->setSigmaAsFunctionOfWidthAndLength();
	}
}

void MainWindow::setParameters()
{
	if (!parameterDialog)                                  
		parameterDialog = new ParameterDialog(this);                           
	if(parameterDialog->exec())
	{
		w->setChargePerParticle(parameterDialog->chargePerParticle().toDouble());
		totRunningTime = parameterDialog->totRunningTime().toInt();
		w->setDeltaT(parameterDialog->deltaT().toDouble());
		w->setMinSigma(parameterDialog->minSigma().toDouble());
		w->setParticlesAtSource(parameterDialog->particlesAtSource().toInt());
		w->setInitialNParticlesPerNode(parameterDialog->initialNParticlesPerNode().toInt());

		
		QSettings settings("Andrea Perna", "Electric Leaf Program");
		settings.setValue("chargePerParticle", w->getChargePerParticle());		
		settings.setValue("totRunningTime", totRunningTime);		
		settings.setValue("deltaT", w->getDeltaT());			
		settings.setValue("minSigma", w->getMinSigma());		
		settings.setValue("particlesAtSource", w->getParticlesAtSource());
		settings.setValue("initialNParticlesPerNode", w->getInitialNParticlesPerNode());
		
		
	}
		
}

void MainWindow::runSimulation(bool isRunning)
{
	if (isRunning)
	{
		runAct->setIcon(QIcon(":/images/player_pause.svgz"));
		runAct->setToolTip(tr("pause"));
		startRunning();
	}
	else
	{
		runAct->setIcon(QIcon(":/images/player_play.svgz"));
		runAct->setToolTip(tr("start"));
		stopRunning();
	}

}


void MainWindow::record(bool shouldRecord)
{
	isRecordingSimulation = shouldRecord;
	if (shouldRecord)
	{
		if (!dialogRecordingParameters)                                  
			dialogRecordingParameters = new DialogRecordingParameters(this);                           
		if(dialogRecordingParameters->exec())
		{
			recordingTimeInterval = dialogRecordingParameters->recordingTimeInterval().toInt();
			recordFileName = dialogRecordingParameters->recordFileName();
			
			// cout << " " << recordingTimeInterval << endl;
			QSettings settings("Andrea Perna", "Electric Leaf Program");
			settings.setValue("recordingTimeInterval", recordingTimeInterval);		
			settings.setValue("recordFileName", recordFileName);	
		}
		else {
			isRecordingSimulation = false;
			recordAct->setChecked(false);
		}

	}
}



void MainWindow::startRunning()
{
	isRunningSimulation = true;
	if (!myTimerID)
		myTimerID = startTimer(0);
}


void MainWindow::stopRunning()
{
	isRunningSimulation = false;
	killTimer(myTimerID);
	myTimerID = 0;
}

void MainWindow::showUpdate(bool shouldShowUpdate)
{
	if (shouldShowUpdate)
	{
		showUpdateAct->setIcon(QIcon(":/images/open_eye.svgz"));
		showUpdateAct->setText(tr("Show update"));
		showUpdateAct->setStatusTip(tr("update the image during simulation"));
		showUpdateAct->setToolTip(tr("update the image during simulation"));
	}
	else 
	{
		showUpdateAct->setIcon(QIcon(":/images/closed_eye.svgz"));
		showUpdateAct->setText(tr("Hide update"));
		showUpdateAct->setStatusTip(tr("don't update the image during simulation"));
		showUpdateAct->setToolTip(tr("don't update the image during simulation"));		
	}

	w->setShowUpdate(shouldShowUpdate);
}



void MainWindow::increaseSimulationTime(int timeIncrement)
{
	currentSimulationTime += timeIncrement;
}


void MainWindow::resetSimulationTime()
{
	currentSimulationTime = 0;
	lcdNumber->display(currentSimulationTime * w->getDeltaT());
	lcdNumber->update();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == myTimerID)
	{
		// printf("timer event\n");
		while (currentSimulationTime <= (totRunningTime/w->getDeltaT()) && isRunningSimulation)
				{
					lcdNumber->display(currentSimulationTime * w->getDeltaT());
					lcdNumber->update();
					w->performOneSimulationStep();
					// printf("simulation running at %d\n", currentSimulationTime);
					
					if (isRecordingSimulation)
					{
						if (currentSimulationTime % recordingTimeInterval == 0)
						{
							if (recordFileName.endsWith(".svg", Qt::CaseInsensitive))
							{

								QString currRecordFileName = recordFileName;
								int indexOfFileExtension = currRecordFileName.lastIndexOf(".svg");
								
								// cout << currRecordFileName.toStdString() << " " << indexOfFileExtension << endl;
								currRecordFileName.truncate(indexOfFileExtension);
								// cout << currRecordFileName.toStdString() << endl;
								QString number = QString("%1").arg(currentSimulationTime, 8, 10, QChar('0')).toUpper();
								currRecordFileName.append(number).append(".svg");
								// cout << currRecordFileName.toStdString() << endl;
								exportSVG(currRecordFileName);
							}
							else if (recordFileName.endsWith(".png", Qt::CaseInsensitive))
							{
								QString currRecordFileName = recordFileName;
								int indexOfFileExtension = currRecordFileName.lastIndexOf(".png");
								
								// cout << currRecordFileName.toStdString() << " " << indexOfFileExtension << endl;
								currRecordFileName.truncate(indexOfFileExtension);
								// cout << currRecordFileName.toStdString() << endl;
								QString number = QString("%1").arg(currentSimulationTime, 8, 10, QChar('0')).toUpper();
								currRecordFileName.append(number).append(".png");
								// cout << currRecordFileName.toStdString() << endl;
								exportPicture(currRecordFileName);
							}
							else if (recordFileName.endsWith(".txt", Qt::CaseInsensitive))
							{
								QString currRecordFileName = recordFileName;
								int indexOfFileExtension = currRecordFileName.lastIndexOf(".txt");
								
								// cout << currRecordFileName.toStdString() << " " << indexOfFileExtension << endl;
								currRecordFileName.truncate(indexOfFileExtension);
								// cout << currRecordFileName.toStdString() << endl;
								QString number = QString("%1").arg(currentSimulationTime, 8, 10, QChar('0')).toUpper();
								currRecordFileName.append(number).append(".txt");
								// cout << currRecordFileName.toStdString() << endl;
								w->saveGraph(currRecordFileName);
							}
						}
					}
					qApp->processEvents();
//					stopRunning(); // run a step at a time, for debugging
				}

	}
}






void MainWindow::zoomOut()
{
	qreal a = 0.8;
	w->zoom(a);
}

void MainWindow::zoomIn()
{
	qreal a = 1.2;
	w->zoom(a);
}








void MainWindow::toggleEdgesVisible()
{
	if (w->getEdgesVisible()) // if nodes are visible, make them invisible
	{
		w->setEdgesVisible(false);
		toggleEdgesVisibleAct->setIcon(QIcon(":/images/inactive_edge.svgz"));
		// toggleEdgesVisibleAct->setText(tr("Show edges"));
		toggleEdgesVisibleAct->setStatusTip(tr("show edges"));
		toggleEdgesVisibleAct->setToolTip(tr("show edges"));
		
		// as the edges are invisible, it is better to shift to selecting nodes
//		w->setSelectingNodes(false);
//		toggleSelectionMethodAct->setText(tr("Selecting Nodes"));
		
	}
	else
	{
		w->setEdgesVisible(true);
		toggleEdgesVisibleAct->setIcon(QIcon(":/images/active_edge.svgz"));
		// toggleEdgesVisibleAct->setText(tr("edges"));
		toggleEdgesVisibleAct->setStatusTip(tr("hide edges"));
		toggleEdgesVisibleAct->setToolTip(tr("hide edges"));
	}
}







void MainWindow::toggleStomataVisible()
{
	if (w->getStomataVisible()) // if stomata are visible, make them invisible
	{
		w->setStomataVisible(false);
		toggleStomataVisibleAct->setIcon(QIcon(":/images/invisible_stoma.png"));
		//toggleStomataVisibleAct->setText(tr("Show stomata"));
		toggleStomataVisibleAct->setStatusTip(tr("show stomata"));
		toggleStomataVisibleAct->setToolTip(tr("show stomata"));
		// as the stomata are invisible, it is better to shift to selecting nodes
//		w->setSelectingNodes(false);
//		toggleSelectionMethodAct->setText(tr("Selecting Nodes"));
		
	}
	else
	{
		w->setStomataVisible(true);
		toggleStomataVisibleAct->setIcon(QIcon(":/images/visible_stoma.png"));
		//toggleStomataVisibleAct->setText(tr("Hide stomata"));
		toggleStomataVisibleAct->setStatusTip(tr("hide stomata"));
		toggleStomataVisibleAct->setToolTip(tr("hide stomata"));
	}
}







void MainWindow::toggleNodesVisible()
{
	if (w->getNodesVisible()) // if nodes are visible, make them invisible
	{
		w->setNodesVisible(false);
		toggleNodesVisibleAct->setIcon(QIcon(":/images/inactive_node.svgz"));
		// toggleNodesVisibleAct->setText(tr("Show nodes"));
		toggleNodesVisibleAct->setToolTip(tr("show nodes"));
		toggleNodesVisibleAct->setStatusTip(tr("show nodes"));
		
		// as the nodes are invisible, it is better to shift to selecting edges
//		w->setSelectingNodes(false);
//		toggleSelectionMethodAct->setText(tr("Selecting Edges"));
		
		
	}
	else
	{
		w->setNodesVisible(true);
		toggleNodesVisibleAct->setIcon(QIcon(":/images/active_node.svgz"));
		//toggleNodesVisibleAct->setText(tr("Hide nodes"));
		toggleNodesVisibleAct->setToolTip(tr("hide nodes"));
		toggleNodesVisibleAct->setStatusTip(tr("hide nodes"));
	}
}


void MainWindow::setSelectNodes()
{
	selectionMethodCombo->setCurrentIndex(0);
//	setSelectEdgesAct->setChecked(false);
//	setSelectStomataAct->setChecked(false);
}

void MainWindow::setSelectEdges()
{
	selectionMethodCombo->setCurrentIndex(1);
//	setSelectNodesAct->setChecked(false);
//	setSelectStomataAct->setChecked(false);
}

void MainWindow::setSelectStomata()
{
	selectionMethodCombo->setCurrentIndex(2);
//	setSelectNodesAct->setChecked(false);
//	setSelectEdgesAct->setChecked(false);
}


void MainWindow::setColouringNodesWithSourceOrSink()
{
	changeNodeColouringMethod("SourceOrSink");
//	setColouringNodesWithDegreeAct->setChecked(false);
//	setColouringNodesWithNParticlesAct->setChecked(false);
}

void MainWindow::setColouringNodesWithDegree()
{
	changeNodeColouringMethod("Degree");
//	setColouringNodesWithSourceOrSinkAct->setChecked(false);
//	setColouringNodesWithNParticlesAct->setChecked(false);
}

void MainWindow::setColouringNodesWithNParticles()
{
	changeNodeColouringMethod("nParticles");
//	setColouringNodesWithSourceOrSinkAct->setChecked(false);
//	setColouringNodesWithDegreeAct->setChecked(false);
}


void MainWindow::changeNodeColouringMethod(QString nodeColouringMethod)
{
	w->setColouringNodesParameter(nodeColouringMethod);
	if (nodeColouringMethod == tr("SourceOrSink"))
	{
		// cout << "colouring nodes depending if sources or sink"<<endl;
		setColouringNodesWithSourceOrSinkAct->setChecked(true);
		setColouringNodesWithNParticlesAct->setChecked(false);
		setColouringNodesWithDegreeAct->setChecked(false);
	}
	else if (nodeColouringMethod == tr("nParticles"))
	{
		// cout << "colouring nodes with n particles"<<endl;
		setColouringNodesWithSourceOrSinkAct->setChecked(false);
		setColouringNodesWithNParticlesAct->setChecked(true);
		setColouringNodesWithDegreeAct->setChecked(false);
	}
	else if (nodeColouringMethod == tr("Degree"))
	{
		// cout << "colouring nodes with degree"<<endl;
		setColouringNodesWithSourceOrSinkAct->setChecked(false);
		setColouringNodesWithNParticlesAct->setChecked(false);
		setColouringNodesWithDegreeAct->setChecked(true);
	}
}




void MainWindow::setColouringStomataWithFlow()
{
	changeStomataColouringMethod("Flow");
}


void MainWindow::setColouringStomataWithSigma()
{
	changeStomataColouringMethod("Sigma");
}

void MainWindow::setColourScaleNodesLinear()
{
	changeColourScale("Linear", "Nodes");
}
void MainWindow::setColourScaleNodesLog()
{
	changeColourScale("Log", "Nodes");
}

void MainWindow::setColourScaleEdgesLinear()
{
	changeColourScale("Linear", "Edges");
}
void MainWindow::setColourScaleEdgesLog()
{
	changeColourScale("Log", "Edges");
}

void MainWindow::setColourScaleStomataLinear()
{
	changeColourScale("Linear", "Stomata");
}
void MainWindow::setColourScaleStomataLog()
{
	changeColourScale("Log", "Stomata");
}
void MainWindow::changeColourScale(QString selectedColourScale, QString selectedTarget)
{
	if (selectedTarget == "Nodes")
	{
	if (selectedColourScale == "Linear")
	{
		w->setNodesColourScale("Linear");
		setColourScaleNodesLogAct->setChecked(false);
		setColourScaleNodesLinearAct->setChecked(true);
	}
	else if (selectedColourScale == "Log")
	{
		w->setNodesColourScale("Log");
		setColourScaleNodesLogAct->setChecked(true);
		setColourScaleNodesLinearAct->setChecked(false);
	}
	}
	else if (selectedTarget == "Edges")
	{
		if (selectedColourScale == "Linear")
		{
			w->setEdgesColourScale("Linear");
			setColourScaleEdgesLogAct->setChecked(false);
			setColourScaleEdgesLinearAct->setChecked(true);
		}
		else if (selectedColourScale == "Log")
		{
			w->setEdgesColourScale("Log");
			setColourScaleEdgesLogAct->setChecked(true);
			setColourScaleEdgesLinearAct->setChecked(false);
		}
	}
	else 	if (selectedTarget == "Stomata")
	{
		if (selectedColourScale == "Linear")
		{
			w->setStomataColourScale("Linear");
			setColourScaleStomataLogAct->setChecked(false);
			setColourScaleStomataLinearAct->setChecked(true);
		}
		else if (selectedColourScale == "Log")
		{
			w->setStomataColourScale("Log");
			setColourScaleStomataLogAct->setChecked(true);
			setColourScaleStomataLinearAct->setChecked(false);
		}
	}
}


void MainWindow::changeStomataColouringMethod(QString stomataColouringMethod)
{
	w->setColouringStomataParameter(stomataColouringMethod);
	if (stomataColouringMethod == "Flow")
	{
		// cout << "colouring stomata with flow" << endl;
		setColouringStomataWithFlowAct->setChecked(true);
		setColouringStomataWithSigmaAct->setChecked(false);
	}
	else if (stomataColouringMethod == "Sigma")
	{
		// cout << "colouring stomata with sigma"<<endl;
		setColouringStomataWithFlowAct->setChecked(false);
		setColouringStomataWithSigmaAct->setChecked(true);
	}
}



void MainWindow::setColouringEdgesWithWidth()
{
	changeEdgeColouringMethod("Width");
}

void MainWindow::setColouringEdgesWithFlow()
{
	changeEdgeColouringMethod("Flow");
}

void MainWindow::setColouringEdgesWithFlowDir()
{
	changeEdgeColouringMethod("FlowDir");
}

void MainWindow::setColouringEdgesWithSigma()
{
	changeEdgeColouringMethod("Sigma");
//	setColouringEdgesWithFlowDirAct->setChecked(false);
//	setColouringEdgesWithFlowAct->setChecked(false);
}

void MainWindow::changeEdgeColouringMethod(QString edgeColouringMethod)
{
	w->setColouringEdgesParameter(edgeColouringMethod);
	if (edgeColouringMethod == "Flow")
	{
		cout << "colouring edges with flow" << endl;
		setColouringEdgesWithFlowAct->setChecked(true);
		setColouringEdgesWithFlowDirAct->setChecked(false);
		setColouringEdgesWithSigmaAct->setChecked(false);
		setColouringEdgesWithWidthAct->setChecked(false);
	}
	else if (edgeColouringMethod == "FlowDir")
	{
		cout << "colouring edges with flow direction"<<endl;
		setColouringEdgesWithFlowAct->setChecked(false);
		setColouringEdgesWithFlowDirAct->setChecked(true);
		setColouringEdgesWithSigmaAct->setChecked(false);
		setColouringEdgesWithWidthAct->setChecked(false);
	}
	else if (edgeColouringMethod == "Sigma")
	{
		cout << "colouring edges with sigma"<<endl;
		setColouringEdgesWithFlowAct->setChecked(false);
		setColouringEdgesWithFlowDirAct->setChecked(false);
		setColouringEdgesWithSigmaAct->setChecked(true);
		setColouringEdgesWithWidthAct->setChecked(false);
	}
	else if (edgeColouringMethod == "Width")
	{
		cout << "colouring edges with width"<<endl;
		setColouringEdgesWithFlowAct->setChecked(false);
		setColouringEdgesWithFlowDirAct->setChecked(false);
		setColouringEdgesWithSigmaAct->setChecked(false);
		setColouringEdgesWithWidthAct->setChecked(true);
	}
}


void MainWindow::setColourMapRainbow()
{
	setColourMapRainbowAct->setChecked(true);
	setColourMapGrayAct->setChecked(false);
	setColourMapPM3DAct->setChecked(false);
	setColourMapDaltonicFriendlyAct->setChecked(false);
	w->setCurrentColourMap("rainbow");
}

void MainWindow::setColourMapGray()
{
	setColourMapRainbowAct->setChecked(false);
	setColourMapGrayAct->setChecked(true);
	setColourMapPM3DAct->setChecked(false);
	setColourMapDaltonicFriendlyAct->setChecked(false);
	w->setCurrentColourMap("gray");
}

void MainWindow::setColourMapPM3D()
{
	setColourMapRainbowAct->setChecked(false);
	setColourMapGrayAct->setChecked(false);
	setColourMapPM3DAct->setChecked(true);
	setColourMapDaltonicFriendlyAct->setChecked(false);
	w->setCurrentColourMap("pm3d");
}

void MainWindow::setColourMapDaltonicFriendly()
{
	setColourMapRainbowAct->setChecked(false);
	setColourMapGrayAct->setChecked(false);
	setColourMapPM3DAct->setChecked(false);
	setColourMapDaltonicFriendlyAct->setChecked(true);
	w->setCurrentColourMap("daltonic friendly");
}


void MainWindow::changeSelectionMethod(QString selectingThis)
{
	
	w->setSelecting(selectingThis);
	if (selectingThis == tr("Nodes"))
	{
		cout << "selecting now nodes"<<endl;
		setSelectNodesAct->setChecked(true);
		setSelectEdgesAct->setChecked(false);
		setSelectStomataAct->setChecked(false);
	}
	else if (selectingThis == tr("Edges"))
	{
		cout << "selecting edges " << endl;
		setSelectNodesAct->setChecked(false);
		setSelectEdgesAct->setChecked(true);
		setSelectStomataAct->setChecked(false);
	}
	else if (selectingThis == tr("Stomata"))
	{
		cout << "selecting edges " << endl;
		setSelectNodesAct->setChecked(false);
		setSelectEdgesAct->setChecked(false);
		setSelectStomataAct->setChecked(true);
	}
}



void MainWindow::about()
{
    QMessageBox::about(this, tr("Info"),
					   tr("This is a small program implementing the Electric Leaf algorithm"));
}



void MainWindow::programInfo()
{
	QString message = QString(tr("Current network\n%1\n")).arg(curFileName);
	message.append(tr("N. nodes: %1\n").arg(w->getNumberOfNodes()));
	message.append(tr("N. edges: %1\n").arg(w->getNumberOfEdges()));
	message.append(tr("N. stomata: %1\n").arg(w->getNumberOfStomata()));
	message.append(tr("N. source nodes: %1\n").arg(w->getNumberOfSourceNodes()));
	message.append(tr("N. sink nodes: %1\n").arg(w->getNumberOfSinkNodes()));
	message.append(tr("edge flow range: %1 to %2\n").arg(w->getMinFlow()).arg(w->getMaxFlow()));
    QMessageBox::about(this, tr("Info"),
					   message);
}


void MainWindow::createActions()
{
	
    openAct = new QAction(QIcon(":/images/document_open.svgz"),tr("&Open"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
		
    saveAct = new QAction(QIcon(":/images/document_save.svgz"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
	saveAct->setEnabled(false);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
	
	saveAsAct = new QAction(QIcon(":/images/document_save_as.svgz"), tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
	saveAsAct->setEnabled(false);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	
	exportAct = new QAction(QIcon(":/images/document_export.svgz"), tr("Export..."), this);
    exportAct->setStatusTip(tr("Export to a different format"));
	exportAct->setEnabled(false);
    connect(exportAct, SIGNAL(triggered()), this, SLOT(selectExportingFormat()));
	
	
	
	for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
	
    exitAct = new QAction(QIcon(":/images/application_exit.svgz"),tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
//	exitAct->setVisible(true);
	connect(exitAct, SIGNAL(triggered()), this, SLOT(quit()));
	
	
	setParametersAct = new QAction(QIcon(":/images/systemsettings.svgz"), tr("Set Parameters"), this);
    setParametersAct->setShortcut(tr("Ctrl+P"));
    setParametersAct->setStatusTip(tr("set parameters"));
    connect(setParametersAct, SIGNAL(triggered()), this, SLOT(setParameters()));
	
	resetTimerAct = new QAction(QIcon(":/images/player_rew.svgz"), tr("Reset timer"), this);
    resetTimerAct->setStatusTip(tr("reset timer"));
	resetTimerAct->setEnabled(true);
    connect(resetTimerAct, SIGNAL(triggered(bool)), this, SLOT(resetSimulationTime()));
	
	
	runAct = new QAction(QIcon(":/images/player_play.svgz"), tr("Run"), this);
    runAct->setShortcut(tr("Ctrl+R"));
    runAct->setStatusTip(tr("run"));
	runAct->setEnabled(false);
	runAct->setCheckable(true);
    runAct->setChecked(false);
    connect(runAct, SIGNAL(triggered(bool)), this, SLOT(runSimulation(bool)));
	
	showUpdateAct = new QAction(QIcon(":/images/open_eye.svgz"), tr("Show Update"), this);
    showUpdateAct->setStatusTip(tr("show simulation update"));
	showUpdateAct->setToolTip(tr("show simulation update"));
	showUpdateAct->setEnabled(true);
	showUpdateAct->setCheckable(true);
    showUpdateAct->setChecked(true);
    connect(showUpdateAct, SIGNAL(triggered(bool)), this, SLOT(showUpdate(bool)));
	
	recordAct = new QAction(QIcon(":/images/player_record.svgz"), tr("Record simulation steps"), this);
    recordAct->setStatusTip(tr("record snapshots during simulation"));
	recordAct->setEnabled(true);
	recordAct->setCheckable(true);
    recordAct->setChecked(false);
    connect(recordAct, SIGNAL(triggered(bool)), this, SLOT(record(bool)));
	
	
    zoomInAct = new QAction(QIcon(":/images/zoom_in.svgz"), tr("&Zoom in..."), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setStatusTip(tr("zoom in"));
	zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
	
	zoomInAct = new QAction(QIcon(":/images/zoom_in.svgz"), tr("&Zoom in..."), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setStatusTip(tr("zoom in"));
	zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
	
    zoomOutAct = new QAction(QIcon(":/images/zoom_out.svgz"),tr("Zoom out..."), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setStatusTip(tr("Zoom out"));
	zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
	
    setSourceOrSinkForAllNodesAct = new QAction(QIcon(":/images/edit_select_all.svgz"),tr("Source or sink"), this);
    setSourceOrSinkForAllNodesAct->setStatusTip(tr("Source or sink"));
	setSourceOrSinkForAllNodesAct->setEnabled(false);
    connect(setSourceOrSinkForAllNodesAct, SIGNAL(triggered()), w, SLOT(setSourceOrSinkForAllNodes()));
	
	setNParticlesForAllNodesAct = new QAction(tr("N particles"), this);
    setNParticlesForAllNodesAct->setStatusTip(tr("N particles"));
	setNParticlesForAllNodesAct->setEnabled(false);
    connect(setNParticlesForAllNodesAct, SIGNAL(triggered()), w, SLOT(setNParticlesForAllNodes()));
	
	setSigmaForAllEdgesAct = new QAction(tr("same value for all edges"), this);
    setSigmaForAllEdgesAct->setStatusTip(tr("same value for all edges"));
	setSigmaForAllEdgesAct->setEnabled(false);
    connect(setSigmaForAllEdgesAct, SIGNAL(triggered()), w, SLOT(setSigmaForAllEdges()));
	
	setSigmaAsFunctionOfWidthAndLengthAct = new QAction(tr("function of width and length"), this);
    setSigmaAsFunctionOfWidthAndLengthAct->setStatusTip(tr("function of width and length"));
	setSigmaAsFunctionOfWidthAndLengthAct->setEnabled(false);
    connect(setSigmaAsFunctionOfWidthAndLengthAct, SIGNAL(triggered()), this, SLOT(setSigmaAsFunctionOfWidthAndLength()));

	setSigmaAsFunctionOfFlowAct = new QAction(tr("update as function of flow"), this);
    setSigmaAsFunctionOfFlowAct->setStatusTip(tr("update as function of flow"));
	setSigmaAsFunctionOfFlowAct->setEnabled(false);
	setSigmaAsFunctionOfFlowAct->setCheckable(true);
	setSigmaAsFunctionOfFlowAct->setChecked(false);
    connect(setSigmaAsFunctionOfFlowAct, SIGNAL(toggled(bool)), w, SLOT(setSigmaAsFunctionOfFlow(bool)));
	connect(setSigmaAsFunctionOfFlowAct, SIGNAL(toggled(bool)), setSigmaAsFunctionOfFlowAct, SLOT(setChecked(bool)));

	
	
	setWidthForAllEdgesAct = new QAction(tr("Width"), this);
    setWidthForAllEdgesAct->setStatusTip(tr("Width"));
	setWidthForAllEdgesAct->setEnabled(false);
    connect(setWidthForAllEdgesAct, SIGNAL(triggered()), w, SLOT(setWidthForAllEdges()));
	
	setLengthForAllEdgesAct = new QAction(tr("Length"), this);
    setLengthForAllEdgesAct->setStatusTip(tr("Length"));
	setLengthForAllEdgesAct->setEnabled(false);
    connect(setLengthForAllEdgesAct, SIGNAL(triggered()), w, SLOT(setLengthForAllEdges()));
	
	setSigmaForAllStomataAct = new QAction(tr("same value for all stomata"), this);
    setSigmaForAllStomataAct->setStatusTip(tr("same value for all stomata"));
	setSigmaForAllStomataAct->setEnabled(false);
    connect(setSigmaForAllStomataAct, SIGNAL(triggered()), w, SLOT(setSigmaForAllStomata()));
	
	setStomaticSigmaAsFunctionOfPotentialAct = new QAction(tr("function of potential at node"), this);
    setStomaticSigmaAsFunctionOfPotentialAct->setStatusTip(tr("function of potential at node"));
	setStomaticSigmaAsFunctionOfPotentialAct->setEnabled(false);
	setStomaticSigmaAsFunctionOfPotentialAct->setCheckable(true);
	setStomaticSigmaAsFunctionOfPotentialAct->setChecked(false);
    connect(setStomaticSigmaAsFunctionOfPotentialAct, SIGNAL(toggled(bool)), w, SLOT(setStomaticSigmaAsFunctionOfPotential(bool)));
	connect(setStomaticSigmaAsFunctionOfPotentialAct, SIGNAL(toggled(bool)), setStomaticSigmaAsFunctionOfPotentialAct, SLOT(setChecked(bool)));

	
	
	setSelectNodesAct = new QAction(tr("Select nodes"), this);
	setSelectNodesAct->setStatusTip(tr("Select nodes"));
	setSelectNodesAct->setCheckable(true);
	setSelectNodesAct->setChecked(true);
	setSelectNodesAct->setEnabled(false);
	connect(setSelectNodesAct, SIGNAL(triggered()), this, SLOT(setSelectNodes()));
	
	setSelectEdgesAct = new QAction(tr("Select edges"), this);
	setSelectEdgesAct->setStatusTip(tr("Select edges"));
	setSelectEdgesAct->setCheckable(true);
	setSelectEdgesAct->setChecked(false);
	setSelectEdgesAct->setEnabled(false);
	connect(setSelectEdgesAct, SIGNAL(triggered()), this, SLOT(setSelectEdges()));
	
	setSelectStomataAct = new QAction(tr("Select stomata"), this);
	setSelectStomataAct->setStatusTip(tr("Select stomata"));
	setSelectStomataAct->setCheckable(true);
	setSelectStomataAct->setChecked(false);
	setSelectStomataAct->setEnabled(false);
	connect(setSelectStomataAct, SIGNAL(triggered()), this, SLOT(setSelectStomata()));
																		

	
	
	
	selectionMethodCombo = new QComboBox;
    selectionMethodCombo->setEditable(false);
    QStringList selMethods;
    selMethods  << tr("Nodes") << tr("Edges") << tr("Stomata");
    selectionMethodCombo->addItems(selMethods);
    selectionMethodCombo->setCurrentIndex(0);
    connect(selectionMethodCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(changeSelectionMethod(const QString &)));
	
	
	
	
	
	
	
	
	
	toggleNodesVisibleAct = new QAction(QIcon(":/images/active_node.svgz"),tr("nodes"), this);
    toggleNodesVisibleAct->setStatusTip(tr("hide nodes"));
	toggleNodesVisibleAct->setToolTip(tr("hide nodes"));
	toggleNodesVisibleAct->setEnabled(true);
	toggleNodesVisibleAct->setCheckable(true);
    toggleNodesVisibleAct->setChecked(true);
    connect(toggleNodesVisibleAct, SIGNAL(triggered()), this, SLOT(toggleNodesVisible()));
	
	toggleEdgesVisibleAct = new QAction(QIcon(":/images/active_edge.svgz"),tr("edges"), this);
    toggleEdgesVisibleAct->setStatusTip(tr("hide edges"));
	toggleEdgesVisibleAct->setToolTip(tr("hide edges"));
	toggleEdgesVisibleAct->setEnabled(true);
	toggleEdgesVisibleAct->setCheckable(true);
    toggleEdgesVisibleAct->setChecked(true);
    connect(toggleEdgesVisibleAct, SIGNAL(triggered()), this, SLOT(toggleEdgesVisible()));
	
	toggleStomataVisibleAct = new QAction(QIcon(":/images/visible_stoma.png"),tr("stomata"), this);
    toggleStomataVisibleAct->setStatusTip(tr("hide stomata"));
	toggleStomataVisibleAct->setToolTip(tr("hide stomata"));
	toggleStomataVisibleAct->setEnabled(true);
	toggleStomataVisibleAct->setCheckable(true);
    toggleStomataVisibleAct->setChecked(true);
    connect(toggleStomataVisibleAct, SIGNAL(triggered()), this, SLOT(toggleStomataVisible()));

	
	setColouringNodesWithNParticlesAct = new QAction(tr("N particles"), this);
    setColouringNodesWithNParticlesAct->setStatusTip(tr("set node colouring with N particles"));
	setColouringNodesWithNParticlesAct->setEnabled(false);
	setColouringNodesWithNParticlesAct->setCheckable(true);
    setColouringNodesWithNParticlesAct->setChecked(true);
    connect(setColouringNodesWithNParticlesAct, SIGNAL(triggered()), this, SLOT(setColouringNodesWithNParticles()));
	
	setColouringNodesWithSourceOrSinkAct = new QAction(tr("Source or sink"), this);
    setColouringNodesWithSourceOrSinkAct->setStatusTip(tr("set node colouring with source or sink"));
	setColouringNodesWithSourceOrSinkAct->setEnabled(false);
	setColouringNodesWithSourceOrSinkAct->setCheckable(true);
    setColouringNodesWithSourceOrSinkAct->setChecked(false);
    connect(setColouringNodesWithSourceOrSinkAct, SIGNAL(triggered()), this, SLOT(setColouringNodesWithSourceOrSink()));
	
	setColouringNodesWithDegreeAct = new QAction(tr("Degree"), this);
    setColouringNodesWithDegreeAct->setStatusTip(tr("set node colouring with source or sink"));
	setColouringNodesWithDegreeAct->setEnabled(false);
	setColouringNodesWithDegreeAct->setCheckable(true);
    setColouringNodesWithDegreeAct->setChecked(false);
    connect(setColouringNodesWithDegreeAct, SIGNAL(triggered()), this, SLOT(setColouringNodesWithDegree()));
	
	setColouringEdgesWithFlowAct = new QAction(tr("Flow"), this);
    setColouringEdgesWithFlowAct->setStatusTip(tr("set edge colouring with flow"));
	setColouringEdgesWithFlowAct->setEnabled(false);
	setColouringEdgesWithFlowAct->setCheckable(true);
    setColouringEdgesWithFlowAct->setChecked(true);
    connect(setColouringEdgesWithFlowAct, SIGNAL(triggered()), this, SLOT(setColouringEdgesWithFlow()));
	
	setColouringEdgesWithFlowDirAct = new QAction(tr("FlowDir"), this);
    setColouringEdgesWithFlowDirAct->setStatusTip(tr("set edge colouring with flow direction"));
	setColouringEdgesWithFlowDirAct->setEnabled(false);
	setColouringEdgesWithFlowDirAct->setCheckable(true);
    setColouringEdgesWithFlowDirAct->setChecked(false);
    connect(setColouringEdgesWithFlowDirAct, SIGNAL(triggered()), this, SLOT(setColouringEdgesWithFlowDir()));
	
	setColouringEdgesWithSigmaAct = new QAction(tr("Sigma"), this);
    setColouringEdgesWithSigmaAct->setStatusTip(tr("set edge colouring with sigma"));
	setColouringEdgesWithSigmaAct->setEnabled(false);
	setColouringEdgesWithSigmaAct->setCheckable(true);
    setColouringEdgesWithSigmaAct->setChecked(false);
    connect(setColouringEdgesWithSigmaAct, SIGNAL(triggered()), this, SLOT(setColouringEdgesWithSigma()));
	
	setColouringEdgesWithWidthAct = new QAction(tr("Width"), this);
    setColouringEdgesWithWidthAct->setStatusTip(tr("set edge colouring with width"));
	setColouringEdgesWithWidthAct->setEnabled(false);
	setColouringEdgesWithWidthAct->setCheckable(true);
    setColouringEdgesWithWidthAct->setChecked(false);
    connect(setColouringEdgesWithWidthAct, SIGNAL(triggered()), this, SLOT(setColouringEdgesWithWidth()));
	
	setColouringStomataWithFlowAct = new QAction(tr("Flow"), this);
    setColouringStomataWithFlowAct->setStatusTip(tr("set stomata colouring with flow"));
	setColouringStomataWithFlowAct->setEnabled(false);
	setColouringStomataWithFlowAct->setCheckable(true);
    setColouringStomataWithFlowAct->setChecked(false);
    connect(setColouringStomataWithFlowAct, SIGNAL(triggered()), this, SLOT(setColouringStomataWithFlow()));
	
	setColouringStomataWithSigmaAct = new QAction(tr("Sigma"), this);
    setColouringStomataWithSigmaAct->setStatusTip(tr("set edge colouring with sigma"));
	setColouringStomataWithSigmaAct->setEnabled(false);
	setColouringStomataWithSigmaAct->setCheckable(true);
    setColouringStomataWithSigmaAct->setChecked(true);
    connect(setColouringStomataWithSigmaAct, SIGNAL(triggered()), this, SLOT(setColouringStomataWithSigma()));
	
	setColourScaleNodesLinearAct = new QAction(tr("Linear"), this);
    setColourScaleNodesLinearAct->setStatusTip(tr("use linear colour scale"));
	setColourScaleNodesLinearAct->setEnabled(true);
	setColourScaleNodesLinearAct->setCheckable(true);
    setColourScaleNodesLinearAct->setChecked(true);
    connect(setColourScaleNodesLinearAct, SIGNAL(triggered()), this, SLOT(setColourScaleNodesLinear()));
	
	setColourScaleNodesLogAct = new QAction(tr("Logarithmic"), this);
    setColourScaleNodesLogAct->setStatusTip(tr("use logarithmic colour scale"));
	setColourScaleNodesLogAct->setEnabled(true);
	setColourScaleNodesLogAct->setCheckable(true);
    setColourScaleNodesLogAct->setChecked(false);
    connect(setColourScaleNodesLogAct, SIGNAL(triggered()), this, SLOT(setColourScaleNodesLog()));
	
	setColourScaleEdgesLinearAct = new QAction(tr("Linear"), this);
    setColourScaleEdgesLinearAct->setStatusTip(tr("use linear colour scale"));
	setColourScaleEdgesLinearAct->setEnabled(true);
	setColourScaleEdgesLinearAct->setCheckable(true);
    setColourScaleEdgesLinearAct->setChecked(true);
    connect(setColourScaleEdgesLinearAct, SIGNAL(triggered()), this, SLOT(setColourScaleEdgesLinear()));
	
	setColourScaleEdgesLogAct = new QAction(tr("Logarithmic"), this);
    setColourScaleEdgesLogAct->setStatusTip(tr("use logarithmic colour scale"));
	setColourScaleEdgesLogAct->setEnabled(true);
	setColourScaleEdgesLogAct->setCheckable(true);
    setColourScaleEdgesLogAct->setChecked(false);
    connect(setColourScaleEdgesLogAct, SIGNAL(triggered()), this, SLOT(setColourScaleEdgesLog()));
	
	setColourScaleStomataLinearAct = new QAction(tr("Linear"), this);
    setColourScaleStomataLinearAct->setStatusTip(tr("use linear colour scale"));
	setColourScaleStomataLinearAct->setEnabled(true);
	setColourScaleStomataLinearAct->setCheckable(true);
    setColourScaleStomataLinearAct->setChecked(true);
    connect(setColourScaleStomataLinearAct, SIGNAL(triggered()), this, SLOT(setColourScaleStomataLinear()));
	
	setColourScaleStomataLogAct = new QAction(tr("Logarithmic"), this);
    setColourScaleStomataLogAct->setStatusTip(tr("use logarithmic colour scale"));
	setColourScaleStomataLogAct->setEnabled(true);
	setColourScaleStomataLogAct->setCheckable(true);
    setColourScaleStomataLogAct->setChecked(false);
    connect(setColourScaleStomataLogAct, SIGNAL(triggered()), this, SLOT(setColourScaleStomataLog()));
	
	setColourMapRainbowAct = new QAction(tr("rainbow"), this);
    setColourMapRainbowAct->setStatusTip(tr("use rainbow colourmap"));
	setColourMapRainbowAct->setEnabled(true);
	setColourMapRainbowAct->setCheckable(true);
    setColourMapRainbowAct->setChecked(true);
    connect(setColourMapRainbowAct, SIGNAL(triggered()), this, SLOT(setColourMapRainbow()));
	
	setColourMapGrayAct = new QAction(tr("gray"), this);
    setColourMapGrayAct->setStatusTip(tr("use gray colourmap"));
	setColourMapGrayAct->setEnabled(true);
	setColourMapGrayAct->setCheckable(true);
    setColourMapGrayAct->setChecked(false);
    connect(setColourMapGrayAct, SIGNAL(triggered()), this, SLOT(setColourMapGray()));
	
	setColourMapPM3DAct = new QAction(tr("PM3D"), this);
    setColourMapPM3DAct->setStatusTip(tr("use PM3D colourmap (Gnuplot default)"));
	setColourMapPM3DAct->setEnabled(true);
	setColourMapPM3DAct->setCheckable(true);
    setColourMapPM3DAct->setChecked(false);
    connect(setColourMapPM3DAct, SIGNAL(triggered()), this, SLOT(setColourMapPM3D()));
	
	setColourMapDaltonicFriendlyAct = new QAction(tr("daltonic friendly"), this);
    setColourMapDaltonicFriendlyAct->setStatusTip(tr("use daltonic friendly colourmap"));
	setColourMapDaltonicFriendlyAct->setEnabled(true);
	setColourMapDaltonicFriendlyAct->setCheckable(true);
    setColourMapDaltonicFriendlyAct->setChecked(false);
    connect(setColourMapDaltonicFriendlyAct, SIGNAL(triggered()), this, SLOT(setColourMapDaltonicFriendly()));
	
	aboutAct = new QAction(QIcon(":/images/help_about.svgz"),tr("About"), this);
    aboutAct->setStatusTip(tr("About"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	
	infoAct = new QAction(QIcon(":/images/help_about.svgz"),tr("Program info"), this);
    infoAct->setStatusTip(tr("Program info"));
    connect(infoAct, SIGNAL(triggered()), this, SLOT(programInfo()));
	
	lcdNumber = new QLCDNumber(this);
	lcdNumber->setSegmentStyle(QLCDNumber::Filled);
    //lcdNumber->setNumDigits(6);
	lcdNumber->display(currentSimulationTime * w->getDeltaT());
	
}

void MainWindow::createMenus()
{

    fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
	fileMenu->addAction(saveAsAct);
	fileMenu->addAction(exportAct);
	fileMenu->addAction(exitAct);
	separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
	
    algorithmMenu = menuBar()->addMenu(tr("&Algorithm"));
	algorithmMenu->addAction(setParametersAct);
	algorithmMenu->addAction(resetTimerAct);
	algorithmMenu->addAction(runAct);
	algorithmMenu->addAction(showUpdateAct);
	algorithmMenu->addAction(recordAct);
	
	
	visibilityMenu = new QMenu(tr("Set visible"), this);
	visibilityMenu->addAction(toggleNodesVisibleAct);
	visibilityMenu->addAction(toggleEdgesVisibleAct);
	visibilityMenu->addAction(toggleStomataVisibleAct);
	
    viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addMenu(visibilityMenu);

	colouringNodesMenu = new QMenu(tr("Colour nodes"), this);
	colouringNodesMenu->addAction(setColouringNodesWithDegreeAct);
	colouringNodesMenu->addAction(setColouringNodesWithNParticlesAct);
	colouringNodesMenu->addAction(setColouringNodesWithSourceOrSinkAct);
	
	colouringEdgesMenu = new QMenu(tr("Colour edges"), this);
	colouringEdgesMenu->addAction(setColouringEdgesWithFlowAct);
	colouringEdgesMenu->addAction(setColouringEdgesWithFlowDirAct);
	colouringEdgesMenu->addAction(setColouringEdgesWithSigmaAct);
	colouringEdgesMenu->addAction(setColouringEdgesWithWidthAct);
	
	colouringStomataMenu = new QMenu(tr("Colour stomata"), this);
	colouringStomataMenu->addAction(setColouringStomataWithFlowAct);
	colouringStomataMenu->addAction(setColouringStomataWithSigmaAct);
	
	viewMenu->addMenu(colouringNodesMenu);
	viewMenu->addMenu(colouringEdgesMenu);
	viewMenu->addMenu(colouringStomataMenu);
	viewMenu->addSeparator();

	
	colourScaleMenu = new QMenu(tr("Colourscale"), this);
	colourScaleNodesMenu = new QMenu(tr("nodes"), this);
	colourScaleEdgesMenu = new QMenu(tr("edges"), this);
	colourScaleStomataMenu = new QMenu(tr("stomata"), this);
	
	colourScaleEdgesMenu->addAction(setColourScaleEdgesLinearAct);
	colourScaleEdgesMenu->addAction(setColourScaleEdgesLogAct);
	
	colourScaleNodesMenu->addAction(setColourScaleNodesLinearAct);
	colourScaleNodesMenu->addAction(setColourScaleNodesLogAct);
	
	colourScaleStomataMenu->addAction(setColourScaleStomataLinearAct);
	colourScaleStomataMenu->addAction(setColourScaleStomataLogAct);
	
	
	colourScaleMenu->addMenu(colourScaleNodesMenu);
	colourScaleMenu->addMenu(colourScaleEdgesMenu);
	colourScaleMenu->addMenu(colourScaleStomataMenu);
	
	viewMenu->addSeparator();
	viewMenu->addMenu(colourScaleMenu);

	setColourMapMenu = new QMenu(tr("Colourmap"), this);
	setColourMapMenu->addAction(setColourMapRainbowAct);
	setColourMapMenu->addAction(setColourMapGrayAct);
	setColourMapMenu->addAction(setColourMapPM3DAct);
	setColourMapMenu->addAction(setColourMapDaltonicFriendlyAct);
	viewMenu->addMenu(setColourMapMenu);
	
	changeSelectionMethodMenu = new QMenu(tr("Selection method"), this);
	changeSelectionMethodMenu->setEnabled(false);
	changeSelectionMethodMenu->addAction(setSelectNodesAct);
	changeSelectionMethodMenu->addAction(setSelectEdgesAct);
	changeSelectionMethodMenu->addAction(setSelectStomataAct);
	
	
	
	
	
	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addMenu(changeSelectionMethodMenu);
	
	setAllNodesPropertyMenu = new QMenu(tr("Set property to all nodes"), this);
	setAllNodesPropertyMenu->setEnabled(false);
	setAllNodesPropertyMenu->addAction(setNParticlesForAllNodesAct);
	setAllNodesPropertyMenu->addAction(setSourceOrSinkForAllNodesAct);
	
	setAllEdgesSigmaMenu = new QMenu(tr("sigma"), this);
	setAllEdgesSigmaMenu->setEnabled(false);
	setAllEdgesSigmaMenu->addAction(setSigmaForAllEdgesAct);
	setAllEdgesSigmaMenu->addAction(setSigmaAsFunctionOfWidthAndLengthAct);
	setAllEdgesSigmaMenu->addSeparator();
	setAllEdgesSigmaMenu->addAction(setSigmaAsFunctionOfFlowAct);
	
	setAllEdgesPropertyMenu = new QMenu(tr("Set property to all edges"), this);
	setAllEdgesPropertyMenu->setEnabled(false);
	setAllEdgesPropertyMenu->addMenu(setAllEdgesSigmaMenu);
	setAllEdgesPropertyMenu->addAction(setWidthForAllEdgesAct);
	setAllEdgesPropertyMenu->addAction(setLengthForAllEdgesAct);
	
	setAllStomataSigmaMenu = new QMenu(tr("sigma"), this);
	setAllStomataSigmaMenu->setEnabled(false);
	setAllStomataSigmaMenu->addAction(setSigmaForAllStomataAct);
	setAllStomataSigmaMenu->addSeparator();
	setAllStomataSigmaMenu->addAction(setStomaticSigmaAsFunctionOfPotentialAct);
	
	setAllStomataPropertyMenu = new QMenu(tr("Set property to all stomata"), this);
	setAllStomataPropertyMenu->setEnabled(false);
	setAllStomataPropertyMenu->addMenu(setAllStomataSigmaMenu);
	
	editMenu->addMenu(setAllNodesPropertyMenu);
	editMenu->addMenu(setAllEdgesPropertyMenu);
	editMenu->addMenu(setAllStomataPropertyMenu);
	
	
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(infoAct);
	helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
	
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
	fileToolBar->addAction(saveAsAct);
	fileToolBar->addAction(exportAct);
	fileToolBar->addAction(exitAct);
	
    viewToolBar = addToolBar(tr("View"));
	viewToolBar->addAction(zoomInAct);
	viewToolBar->addAction(zoomOutAct);
	viewToolBar->addAction(toggleNodesVisibleAct);
	viewToolBar->addAction(toggleEdgesVisibleAct);
	viewToolBar->addAction(toggleStomataVisibleAct);
	
    editToolBar = addToolBar(tr("Edit"));
	editToolBar->addWidget(selectionMethodCombo);
	editToolBar->addAction(setSourceOrSinkForAllNodesAct);
	
	algorithmToolBar = addToolBar(tr("Algorithm"));
	algorithmToolBar->addAction(setParametersAct);
	algorithmToolBar->addAction(runAct);
	algorithmToolBar->addAction(showUpdateAct);
	algorithmToolBar->addAction(recordAct);
	algorithmToolBar->addWidget(lcdNumber);
	
	helpToolBar = addToolBar(tr("Help"));
	helpToolBar->addAction(infoAct);
}










void MainWindow::setCurrentFile(const QString &fileName)
{
    curFileName = fileName;
    setWindowFilePath(curFileName);
	
	QSettings settings("Andrea Perna", "Electric Leaf Program");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();
	
    settings.setValue("recentFileList", files);
	
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::updateRecentFileActions()
{
	QSettings settings("Andrea Perna", "Electric Leaf Program");
    QStringList files = settings.value("recentFileList").toStringList();
	
    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);
	
    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);
	
    separatorAct->setVisible(numRecentFiles > 0);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

