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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QStatusBar>
#include <QMenu>
#include <QLCDNumber>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>

#include "graphwidget.h"
#include "parameterdialog.h"
#include "sigmaequationdialog.h"
#include "dialogrecordingparameters.h"

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class GraphWidget;
class QComboBox;
class QPushButton;
class QLCDNumber;
class QPainter;

class MainWindow : public QMainWindow
{
    Q_OBJECT
	
	
public:
    MainWindow();
	void increaseSimulationTime(int timeIncrement);
	
private slots:
    void open();
	void openRecentFile();
    void save();
	void saveAs();
	void selectExportingFormat();
	void quit();
    void programInfo();
	void about();
	void zoomIn();
	void zoomOut();
	void setSelectNodes();
	void setSelectEdges();
	void setSelectStomata();
	void changeSelectionMethod(QString selectingThis);
	void changeNodeColouringMethod(QString nodeColouringMethod);
	void changeEdgeColouringMethod(QString edgeColouringMethod);
	void changeStomataColouringMethod(QString stomataColouringMethod);
	void changeColourScale(QString selectedColourScale, QString selectedTarget);
	void setColourScaleNodesLinear();
	void setColourScaleNodesLog();
	void setColourScaleEdgesLinear();
	void setColourScaleEdgesLog();
	void setColourScaleStomataLinear();
	void setColourScaleStomataLog();
	void setColouringNodesWithSourceOrSink();
	void setColouringNodesWithDegree();
	void setColouringNodesWithNParticles();
	void setColouringEdgesWithSigma();
	void setColouringEdgesWithWidth();
	void setColouringEdgesWithFlow();
	void setColouringEdgesWithFlowDir();
	void setColouringStomataWithSigma();
	void setColouringStomataWithFlow();
	void setColourMapRainbow();
	void setColourMapGray();
	void setColourMapPM3D();
	void setColourMapDaltonicFriendly();
	
	void setSigmaAsFunctionOfWidthAndLength();
	void toggleNodesVisible();
	void toggleEdgesVisible();
	void toggleStomataVisible();
	void setParameters();
	void runSimulation(bool isRunning);
	void showUpdate(bool shouldShowUpdate);
	void record(bool shouldRecord);
	void resetSimulationTime();
	
	
protected:
	void timerEvent(QTimerEvent *event);
	
private:
	void createActions();
    void createMenus();
    void createToolBars();
	void enableActionsAndMenus();
	
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);
	
	void exportSVG(QString exportFileName);
	void exportPicture(QString exportFileName);
	

	void startRunning();
	void stopRunning();
	
//	void writeSettings();
	void readRecentFilesList();
	
	
    QMenu *fileMenu;
	QMenu *recentFilesMenu;
    QMenu *viewMenu;
	QMenu *algorithmMenu;
	QMenu *editMenu;
    QMenu *helpMenu;
	QMenu *visibilityMenu;
	QMenu *colourScaleMenu;
	QMenu *colourScaleNodesMenu;
	QMenu *colourScaleEdgesMenu;
	QMenu *colourScaleStomataMenu;
	QMenu *colouringNodesMenu;
	QMenu *colouringEdgesMenu;
	QMenu *colouringStomataMenu;
	QMenu *changeSelectionMethodMenu;
	QMenu *setAllNodesPropertyMenu;
	QMenu *setAllEdgesPropertyMenu;
	QMenu *setAllStomataPropertyMenu;
	QMenu *setAllEdgesSigmaMenu;
	QMenu *setAllStomataSigmaMenu;
	QMenu *setColourMapMenu;
	
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *editToolBar;
	QToolBar *algorithmToolBar;
	QToolBar *helpToolBar;
	
	
	// file menu
    QActionGroup *alignmentGroup;
    QAction *openAct;
    QAction *saveAct;
	QAction *saveAsAct;
	QAction *exportAct;
    QAction *exitAct;
    QAction *separatorAct;
	
	
	// algorithm menu
	QAction *setParametersAct;
	QAction *resetTimerAct;
	QAction *runAct;
	QAction *showUpdateAct;
	QAction *recordAct;
	
	// view menu
    QAction *zoomInAct;
    QAction *zoomOutAct;
	QAction	*toggleNodesVisibleAct;
	QAction	*toggleEdgesVisibleAct;
	QAction *toggleStomataVisibleAct;

	QAction *setColourScaleNodesLinearAct;
	QAction *setColourScaleNodesLogAct;
	QAction *setColourScaleEdgesLinearAct;
	QAction *setColourScaleEdgesLogAct;
	QAction *setColourScaleStomataLinearAct;
	QAction *setColourScaleStomataLogAct;
	QAction *setColouringNodesWithDegreeAct;
	QAction *setColouringNodesWithNParticlesAct;
	QAction *setColouringNodesWithSourceOrSinkAct;
	QAction *setColouringEdgesWithSigmaAct;
	QAction *setColouringEdgesWithWidthAct;
	QAction *setColouringEdgesWithFlowAct;
	QAction *setColouringEdgesWithFlowDirAct;
	QAction *setColouringStomataWithSigmaAct;
	QAction *setColouringStomataWithFlowAct;
	QAction *setColourMapRainbowAct;
	QAction *setColourMapGrayAct;
	QAction *setColourMapPM3DAct;
	QAction *setColourMapDaltonicFriendlyAct;
	
	// edit menu
	QAction *setSelectNodesAct;
	QAction *setSelectEdgesAct;
	QAction *setSelectStomataAct;
	QComboBox *selectionMethodCombo;
	QAction *setSourceOrSinkForAllNodesAct;
	QAction *setNParticlesForAllNodesAct;
	QAction *setSigmaForAllEdgesAct;
	QAction *setSigmaAsFunctionOfWidthAndLengthAct;
	QAction *setSigmaAsFunctionOfFlowAct;
	QAction *setWidthForAllEdgesAct;
	QAction *setLengthForAllEdgesAct;
	QAction *setSigmaForAllStomataAct;
	QAction *setStomaticSigmaAsFunctionOfPotentialAct;
	
	// help menu
    QAction *infoAct;
	QAction *aboutAct;
	
	enum { MaxRecentFiles = 10 };
    QAction *recentFileActs[MaxRecentFiles];
	
	QLCDNumber *lcdNumber;

	GraphWidget *w;
	ParameterDialog *parameterDialog;
	SigmaEquationDialog *sigmaEquationDialog;
	DialogRecordingParameters *dialogRecordingParameters;
	
	QString curFileName;
	// simulation parameters
	int currentSimulationTime;
	int totRunningTime;
	int recordingTimeInterval;
	QString recordFileName;
	
	
	int myTimerID;
	bool isRunningSimulation;
	bool isRecordingSimulation;
};

#endif
