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

#include "dialogrecordingparameters.h"

DialogRecordingParameters::DialogRecordingParameters(QWidget *parent)
: QDialog(parent)
{
	QSettings settings("Andrea Perna", "Electric Leaf Program");

	
	
	// total running time
	QString savedRecordingTimeIntervalStr = settings.value("recordingTimeInterval", QVariant(tr("10"))).toString();
	
	recordingTimeIntervalComboBox = new QComboBox;
	recordingTimeIntervalComboBox->addItem(savedRecordingTimeIntervalStr);
	recordingTimeIntervalComboBox->addItem(tr("100"));
	recordingTimeIntervalComboBox->setEditable(true);
    recordingTimeIntervalComboBox->setCurrentIndex(0);
	
	recordingTimeIntervalLabel = new QLabel(tr("&I (recording time interval):"));
    recordingTimeIntervalLabel->setBuddy(recordingTimeIntervalComboBox);
	
	
	// output file name
	QString savedRecordFileName = settings.value("recordFileName", QVariant(tr("temp.png"))).toString();

	
	recordFileNameLabel = new QLabel;
	recordFileNameLabel->setText(tr("Output file name"));
    recordFileNameButton = new QPushButton(savedRecordFileName);
	connect(recordFileNameButton, SIGNAL(clicked()),
            this, SLOT(setRecordFileName()));
	
	
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
	
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	
	int row = 0;
	QGridLayout *mainLayout = new QGridLayout;

	mainLayout->addWidget(recordingTimeIntervalLabel, row, 0);
    mainLayout->addWidget(recordingTimeIntervalComboBox, row, 1);
	row++;
	mainLayout->addWidget(recordFileNameLabel, row, 0);
    mainLayout->addWidget(recordFileNameButton, row, 1);
	row++;
	mainLayout->addWidget(buttonBox, row, 0, 1, 2);
	
    setLayout(mainLayout);
	
    setWindowTitle(tr("Set recording parameters"));
    resize(650, 400);
	setWindowOpacity(0.90);
}


void DialogRecordingParameters::setRecordFileName()
{
    QFileDialog::Options options;
    QString selectedFilter;
	
	QString fileName = QFileDialog::getSaveFileName(this, tr("Record to file"), recordFileNameButton->text(), tr("All Files (*);;Image Files (*.png *.jpeg);; Vectorial files (*.svg);; Network files (*.txt *.net)"));
    if (!fileName.isEmpty())
        recordFileNameButton->setText(fileName);
}


QString DialogRecordingParameters::recordingTimeInterval() const
{
    return recordingTimeIntervalComboBox->currentText();
}

QString DialogRecordingParameters::recordFileName() const
{
    return recordFileNameButton->text();
}
