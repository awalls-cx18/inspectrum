/*
 *  Copyright (C) 2015, Mike Walters <mike@flomp.net>
 *  Copyright (C) 2015, Jared Boone <jared@sharebrained.com>
 *
 *  This file is part of inspectrum.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "spectrogramcontrols.h"
#include <QIntValidator>
#include <QFileDialog>
#include <QLabel>
#include <cmath>

SpectrogramControls::SpectrogramControls(const QString & title, QWidget * parent)
    : QDockWidget::QDockWidget(title, parent)
{
    widget = new QWidget(this);
    layout = new QFormLayout(widget);

    fileOpenButton = new QPushButton("Open file...", widget);
    layout->addRow(fileOpenButton);

    sampleRate = new QLineEdit();
    sampleRate->setValidator(new QIntValidator(this));
    layout->addRow(new QLabel(tr("Sample rate:")), sampleRate);

    // Spectrogram settings
    layout->addRow(new QLabel()); // TODO: find a better way to add an empty row?
    layout->addRow(new QLabel(tr("<b>Spectrogram</b>")));

    fftSizeSlider = new QSlider(Qt::Horizontal, widget);
    fftSizeSlider->setRange(7, 13);
    layout->addRow(new QLabel(tr("FFT size:")), fftSizeSlider);

    zoomLevelSlider = new QSlider(Qt::Horizontal, widget);
    zoomLevelSlider->setRange(0, 10);
    layout->addRow(new QLabel(tr("Zoom:")), zoomLevelSlider);

    powerMaxSlider = new QSlider(Qt::Horizontal, widget);
    powerMaxSlider->setRange(-100, 20);
    layout->addRow(new QLabel(tr("Power max:")), powerMaxSlider);

    powerMinSlider = new QSlider(Qt::Horizontal, widget);
    powerMinSlider->setRange(-100, 20);
    layout->addRow(new QLabel(tr("Power min:")), powerMinSlider);

    // Time selection settings
    layout->addRow(new QLabel()); // TODO: find a better way to add an empty row?
    layout->addRow(new QLabel(tr("<b>Time selection</b>")));

    cursorsCheckBox = new QCheckBox(widget);
    layout->addRow(new QLabel(tr("Enable cursors:")), cursorsCheckBox);

    cursorBitsSpinBox = new QSpinBox();
    cursorBitsSpinBox->setMinimum(1);
    cursorBitsSpinBox->setMaximum(9999);
    layout->addRow(new QLabel(tr("Bits:")), cursorBitsSpinBox);

    timeSelectionFreqLabel = new QLabel();
    layout->addRow(new QLabel(tr("Frequency:")), timeSelectionFreqLabel);

    timeSelectionTimeLabel = new QLabel();
    layout->addRow(new QLabel(tr("Time:")), timeSelectionTimeLabel);

    bitSelectionFreqLabel = new QLabel();
    layout->addRow(new QLabel(tr("Bit frequency:")), bitSelectionFreqLabel);

    bitSelectionTimeLabel = new QLabel();
    layout->addRow(new QLabel(tr("Bit time:")), bitSelectionTimeLabel);

    widget->setLayout(layout);
    setWidget(widget);

    connect(fftSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(fftOrZoomChanged(int)));
    connect(zoomLevelSlider, SIGNAL(valueChanged(int)), this, SLOT(fftOrZoomChanged(int)));
    connect(fileOpenButton, SIGNAL(clicked()), this, SLOT(fileOpenButtonClicked()));
    connect(cursorsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(cursorsStateChanged(int)));
}

void SpectrogramControls::clearCursorLabels()
{
    timeSelectionTimeLabel->setText("");
    timeSelectionFreqLabel->setText("");
    bitSelectionTimeLabel->setText("");
    bitSelectionFreqLabel->setText("");
}

void SpectrogramControls::cursorsStateChanged(int state)
{
    if (state == Qt::Unchecked) {
        clearCursorLabels();
    }
}

void SpectrogramControls::setDefaults()
{
    sampleRate->setText("8000000");
    fftSizeSlider->setValue(9);
    zoomLevelSlider->setValue(0);
    powerMaxSlider->setValue(0);
    powerMinSlider->setValue(-50);
    cursorsCheckBox->setCheckState(Qt::Unchecked);
    cursorBitsSpinBox->setValue(1);
}

void SpectrogramControls::fftOrZoomChanged(int value)
{
    int fftSize = pow(2, fftSizeSlider->value());
    int zoomLevel = std::min(fftSize, (int)pow(2, zoomLevelSlider->value()));
    emit fftOrZoomChanged(fftSize, zoomLevel);
}

void SpectrogramControls::fileOpenButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                           this, tr("Open File"), "", tr("complex<float> file (*.cfile *.cf32);;complex<int8> HackRF file (*.cs8);;complex<uint8> RTL-SDR file (*.cu8);;All files (*)")
                       );
    if (!fileName.isEmpty())
        emit openFile(fileName);
}

void SpectrogramControls::timeSelectionChanged(float time)
{
    if (cursorsCheckBox->checkState() == Qt::Checked) {
        timeSelectionTimeLabel->setText(QString::number(time) + "s");
        timeSelectionFreqLabel->setText(QString::number(1 / time) + "Hz");

        int bits = cursorBitsSpinBox->value();
        bitSelectionTimeLabel->setText(QString::number(time / bits) + "s");
        bitSelectionFreqLabel->setText(QString::number(bits / time) + "Hz");
    }
}

void SpectrogramControls::zoomIn()
{
    zoomLevelSlider->setValue(zoomLevelSlider->value() + 1);
}

void SpectrogramControls::zoomOut()
{
    zoomLevelSlider->setValue(zoomLevelSlider->value() - 1);
}
