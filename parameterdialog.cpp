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



#include <QtGui>

#include "parameterdialog.h"

ParameterDialog::ParameterDialog(QWidget *parent)
: QDialog(parent)
{
	QSettings settings("Andrea Perna", "Electric Leaf Program");

	// charge per particle
	QString savedChargePerParticleStr = settings.value("chargePerParticle", QVariant(tr("0.01"))).toString();

	chargeComboBox = new QComboBox;
	chargeComboBox->addItem(savedChargePerParticleStr);
	chargeComboBox->addItem(tr("0.05"));
	chargeComboBox->addItem(tr("0.1"));
	chargeComboBox->setEditable(true);
    chargeComboBox->setCurrentIndex(0);
	
	chargeLabel = new QLabel(tr("&Q (charge of one particle):"));
    chargeLabel->setBuddy(chargeComboBox);
	
	
	// total running time
	QString savedTotRunningTimeStr = settings.value("totRunningTime", QVariant(tr("200"))).toString();
	
	totRunningTimeComboBox = new QComboBox;
	totRunningTimeComboBox->addItem(savedTotRunningTimeStr);
	totRunningTimeComboBox->addItem(tr("300"));
	totRunningTimeComboBox->addItem(tr("500"));
	totRunningTimeComboBox->setEditable(true);
    totRunningTimeComboBox->setCurrentIndex(0);
	
	totRunningTimeLabel = new QLabel(tr("&T (total running time):"));
    totRunningTimeLabel->setBuddy(totRunningTimeComboBox);
	
	

	// delta running time
	QString savedDeltaTStr = settings.value("deltaT", QVariant(tr("0.01"))).toString();
	
	deltaTComboBox = new QComboBox;
	deltaTComboBox->addItem(savedDeltaTStr);
	deltaTComboBox->addItem(tr("0.02"));
	deltaTComboBox->setEditable(true);
    deltaTComboBox->setCurrentIndex(0);
	
	deltaTLabel = new QLabel(tr("&Dt (delta T):"));
    deltaTLabel->setBuddy(deltaTComboBox);
	
	// min sigma
	QString savedMinSigmaStr = settings.value("minSigma", QVariant(tr("0.001"))).toString();
	
	minSigmaComboBox = new QComboBox;
	minSigmaComboBox->addItem(savedMinSigmaStr);
	minSigmaComboBox->addItem(tr("0.002"));
	minSigmaComboBox->setEditable(true);
    minSigmaComboBox->setCurrentIndex(0);
	
	minSigmaLabel = new QLabel(tr("&min(sigma) minimum edge conductivity:"));
    minSigmaLabel->setBuddy(minSigmaComboBox);
	
	
	
	// particles at source
	QString savedParticlesAtSourceStr = settings.value("particlesAtSource", QVariant(tr("10000"))).toString();
	
	particlesAtSourceComboBox = new QComboBox;
	particlesAtSourceComboBox->addItem(savedParticlesAtSourceStr);
	particlesAtSourceComboBox->addItem(tr("5000"));
	particlesAtSourceComboBox->setEditable(true);
    particlesAtSourceComboBox->setCurrentIndex(0);
	
	particlesAtSourceLabel = new QLabel(tr("N particles at &source:"));
    particlesAtSourceLabel->setBuddy(particlesAtSourceComboBox);
	
	
	// n particles
	QString initialNParticlesStr = settings.value("initialNParticlesPerNode", QVariant(tr("0"))).toString();
	
	nParticlesComboBox = new QComboBox;
	nParticlesComboBox->addItem(initialNParticlesStr);
	nParticlesComboBox->addItem(tr("1000"));
	nParticlesComboBox->setEditable(true);
    nParticlesComboBox->setCurrentIndex(0);
	
	nParticlesLabel = new QLabel(tr("initial N particles per node"));
    nParticlesLabel->setBuddy(nParticlesComboBox);
	
	
	
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
	
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	
	int row = 0;
	QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(chargeLabel, row, 0);
    mainLayout->addWidget(chargeComboBox, row, 1);
	row++;
	mainLayout->addWidget(totRunningTimeLabel, row, 0);
    mainLayout->addWidget(totRunningTimeComboBox, row, 1);
	row++;
	mainLayout->addWidget(deltaTLabel, row, 0);
    mainLayout->addWidget(deltaTComboBox, row, 1);
	row++;
	mainLayout->addWidget(minSigmaLabel, row, 0);
    mainLayout->addWidget(minSigmaComboBox, row, 1);
	row++;
	mainLayout->addWidget(particlesAtSourceLabel, row, 0);
    mainLayout->addWidget(particlesAtSourceComboBox, row, 1);
	row++;
	mainLayout->addWidget(nParticlesLabel, row, 0);
    mainLayout->addWidget(nParticlesComboBox, row, 1);
	row++;
	mainLayout->addWidget(buttonBox, row, 0, 1, 2);
	
    setLayout(mainLayout);
	
    setWindowTitle(tr("Set parameter values"));
    resize(650, 400);
	setWindowOpacity(0.90);
}





QString ParameterDialog::chargePerParticle() const
{
    return chargeComboBox->currentText();
}

QString ParameterDialog::totRunningTime() const
{
    return totRunningTimeComboBox->currentText();
}

QString ParameterDialog::deltaT() const
{
    return deltaTComboBox->currentText();
}


QString ParameterDialog::minSigma() const
{
    return minSigmaComboBox->currentText();
}


QString ParameterDialog::particlesAtSource() const
{
    return particlesAtSourceComboBox->currentText();
}

QString ParameterDialog::initialNParticlesPerNode() const
{
    return nParticlesComboBox->currentText();
}
