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




#ifndef DIALOGRECORDINGPARAMETERS_H
#define DIALOGRECORDINGPARAMETERS_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
class QComboBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QTableWidget;
QT_END_NAMESPACE

class DialogRecordingParameters : public QDialog
{
    Q_OBJECT
public:
    DialogRecordingParameters(QWidget *parent = 0);
	
	QString recordingTimeInterval() const;
	QString recordFileName() const;

private slots:
	void setRecordFileName();
	
private:
	QLabel *recordingTimeIntervalLabel;
	QComboBox *recordingTimeIntervalComboBox;
	QLabel *recordFileNameLabel;
	QPushButton *recordFileNameButton;

	QDialogButtonBox *buttonBox;

};
#endif
