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




#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
class QComboBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QTableWidget;
QT_END_NAMESPACE

class ParameterDialog : public QDialog
{
    Q_OBJECT
public:
    ParameterDialog(QWidget *parent = 0);
	
	QString chargePerParticle() const;
	QString totRunningTime() const;
	QString deltaT() const;
	QString minSigma() const;
	QString particlesAtSource() const;
	QString initialNParticlesPerNode() const;
	
private:
	QLabel *chargeLabel;
	QComboBox *chargeComboBox;
	QLabel *totRunningTimeLabel;
	QComboBox *totRunningTimeComboBox;
	QLabel *deltaTLabel;
	QComboBox *deltaTComboBox;
	QLabel *minSigmaLabel;
	QComboBox *minSigmaComboBox;
	QLabel *particlesAtSourceLabel;
	QComboBox *particlesAtSourceComboBox;
	QLabel *nParticlesLabel;
	QComboBox *nParticlesComboBox;

	QDialogButtonBox *buttonBox;

};
#endif
