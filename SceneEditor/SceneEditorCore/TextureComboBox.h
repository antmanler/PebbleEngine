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

#ifndef TEXTURECOMBOBOX_H_
#define TEXTURECOMBOBOX_H_

#include <QtGui/QComboBox>

#include "CustomTypes.h"
#include "Config.h"

class QDir;

class DLLEXP TextureComboBox : public QComboBox
{
    Q_OBJECT

    Q_PROPERTY(QString textureFile READ textureFile)

public:
    TextureComboBox(QWidget* parent = 0);
    virtual ~TextureComboBox();

    void init(const QString& texturePath);

    Texture texture() const;
    void setTexture(Texture texture);

    QString textureFile() {return currentText();}

signals:
    void textureChanged();
    void editFinished();

private slots:
    void currentChanged(int index);

private:
    void addTextures(const QDir& base, const QString dir);

    QString m_init;

    QString m_resourcePath;
};
#endif

