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
#ifndef CAMERATOOLBAR_H_
#define CAMERATOOLBAR_H_

#include "./ui_CameraToolBar.h"

class QCameraNode;
class QXmlTreeNode;

class CameraToolBar : public QWidget, protected Ui_CameraToolBar
{
    Q_OBJECT

    friend class HordeSceneEditor;

public:
    CameraToolBar(QWidget* parent = 0, Qt::WFlags flags = 0);
    virtual ~CameraToolBar();

    void setActiveCamera(const QCameraNode* camera);
    void setActiveCamera(const QString& name);

    int currentPipelineID() const;

    const QString currentPipelineFile() const;

    QString activeCameraName() const;
    void loadStyle(const QString& stylesheet);

public slots:
    void pipelineChanged(QCameraNode* camera);
    void viewportResized(int width, int height);

    void nodePropertyChanged( QXmlTreeNode* node );

signals:
    void activeCamChanged(const QCameraNode* camera);

private slots:
    void currentCamChanged(int index);
    void activateSpecialCam(bool checked);

private:
    int		m_viewportWidth;
    int		m_viewportHeight;

    float	m_sceneX, m_sceneY, m_sceneZ;
    float	m_sceneRX, m_sceneRY, m_sceneRZ;
    float	m_orthoX, m_orthoY, m_orthoZ;
    float   m_orthoScale, m_sceneScale;
    float	m_topY, m_rightX, m_frontZ;

};
#endif
