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
// 
#ifndef HORDEMODELDIALOG_H_
#define HORDEMODELDIALOG_H_

#include <SceneEditorCore/HordeFileDialog.h>

class GLWidget;

class HordeModelDialog : public HordeFileDialog
{
    Q_OBJECT
public:
    HordeModelDialog(const QString& targetPath, QWidget* parent = 0, Qt::WFlags flags = 0);
    virtual ~HordeModelDialog();

    /**
         * Returns the selected model filename relative to the currently set Horde3D scenegraph path
     * @param parent widget the HordeFileDialog will be child of
         * @param caption string to be displayed as window caption
         * @return QString filename of the selected model
         */
    static QString getModelFile(const QString& targetPath, QWidget* parent, const QString& caption);

protected slots:
    virtual void itemChanged(QListWidgetItem* current, QListWidgetItem* previous);

private slots:
    void importCollada();

private:
    void initModelViewer();

    void loadModel(const QString& fileName, bool repoFile);

    GLWidget*			m_glWidget;
    QPushButton*			m_importButton;

    unsigned int			m_oldScene;
    unsigned int			m_newScene;
    unsigned int			m_currentModel;
    unsigned int			m_cameraID;

};
#endif
