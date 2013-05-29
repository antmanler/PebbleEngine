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
#ifndef SPHGENERATOR_H
#define SPHGENERATOR_H

#include <QtGui/QWizard>
#include "ui_SPHGenerator.h"

class SPHGenerator : public QWizardPage, private Ui::SPHGenerator
{
    Q_OBJECT
    
public:
    explicit SPHGenerator(QWidget *parent = 0, int pageid = 0);
    void initializePage();
    bool validatePage();

private slots:
    void selectModel();
    void selectDf();

private:
    int page_id_;
};

#endif // SPHGENERATOR_H
