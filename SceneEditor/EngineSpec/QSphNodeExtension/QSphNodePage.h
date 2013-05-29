// ****************************************************************************************
//
// Pebble Scene Editor 
// --------------------------------------
// Copyright (C) 2011 Antmanler
// 
// This file is part of the Pebble Scene Editor.
//
// The Pebble Scene Editor is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License 
//
// The Pebble Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Credits : Volker Wiendl who created the major work of the original editor and Horde3D
// is a flexible graphics engine. see <http://www.horde3d.org> and <http://http://goo.gl/Me2HT>
//
// ****************************************************************************************
#pragma once
#include <QtGui/QWizard>
#include <dflib/vector3f.h>
#include "./ui_QSphNodePage.h"

class DistanceField3D;

class SphNodeUtils : public QObject
{
    Q_OBJECT
public:
    explicit SphNodeUtils(QObject* parent = 0);
public slots:
    QString createParticlesFromSDF();
    QString createParticlesFromModle();
private:
    unsigned int dfToParticles(DistanceField3D *df3d,
                               const vector3f& bbmin, const vector3f& bbmax,
                               const float sample_dist,
                               const float density,
                               const bool negative,
                               const bool rigid,
                               const QString& filename);
};

class QSphNodePage : public QWizardPage, protected Ui_QSphNodePage
{
    Q_OBJECT

public:
    QSphNodePage(QWidget* parent = 0);
    virtual ~QSphNodePage();
    void initializePage();

private slots:
    void currentChanged(int index);

private :
    QString last_sel_;
};
