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
// #include <cstdio>

#include "sigmaequationdialog.h"

SigmaEquationDialog::SigmaEquationDialog(QWidget *parent)
: QDialog(parent)
{
	// printf("in sigma equation dialog\n");
	QSettings settings("Andrea Perna", "Electric Leaf Program");

	// multiplicative factor for edge sigma
	QString savedMultiplicativeFactorEdgeSigmaStr = settings.value("multiplicativeFactorEdgeSigma", QVariant(10)).toString();
	multiplicativeFactorEdgeSigmaComboBox = new QComboBox;
	multiplicativeFactorEdgeSigmaComboBox->addItem(savedMultiplicativeFactorEdgeSigmaStr);
	multiplicativeFactorEdgeSigmaComboBox->addItem(tr("1"));
	multiplicativeFactorEdgeSigmaComboBox->addItem(tr("100"));
	multiplicativeFactorEdgeSigmaComboBox->setEditable(true);
    multiplicativeFactorEdgeSigmaComboBox->setCurrentIndex(0);
	
	multiplicativeFactorEdgeSigmaLabel = new QLabel(tr("A (a):"));
    multiplicativeFactorEdgeSigmaLabel->setBuddy(multiplicativeFactorEdgeSigmaComboBox);

	
	// exponent to edge width for computing sigma
	QString savedExponentEdgeWidthForSigmaStr = settings.value("exponentEdgeWidthForSigma", QVariant(2)).toString();
	
	exponentEdgeWidthForSigmaComboBox = new QComboBox;
	exponentEdgeWidthForSigmaComboBox->addItem(savedExponentEdgeWidthForSigmaStr);
	exponentEdgeWidthForSigmaComboBox->addItem(tr("1"));
	exponentEdgeWidthForSigmaComboBox->addItem(tr("2.5"));
	exponentEdgeWidthForSigmaComboBox->setEditable(true);
    exponentEdgeWidthForSigmaComboBox->setCurrentIndex(0);
	
	exponentEdgeWidthForSigmaLabel = new QLabel(tr("B (b):"));
    exponentEdgeWidthForSigmaLabel->setBuddy(exponentEdgeWidthForSigmaComboBox);
	
	
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
	
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	
	int row = 0;
	QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(multiplicativeFactorEdgeSigmaLabel, row, 0);
    mainLayout->addWidget(multiplicativeFactorEdgeSigmaComboBox, row, 1);
	row++;
	mainLayout->addWidget(exponentEdgeWidthForSigmaLabel, row, 0);
    mainLayout->addWidget(exponentEdgeWidthForSigmaComboBox, row, 1);
	row++;
	mainLayout->addWidget(buttonBox, row, 0, 1, 2);
    setLayout(mainLayout);
	
    setWindowTitle(tr("Set equation for sigma"));
    resize(650, 400);
	setWindowOpacity(0.90);
}





QString SigmaEquationDialog::multiplicativeFactorEdgeSigma() const
{
    return multiplicativeFactorEdgeSigmaComboBox->currentText();
}

QString SigmaEquationDialog::exponentEdgeWidthForSigma() const
{
    return exponentEdgeWidthForSigmaComboBox->currentText();
}
