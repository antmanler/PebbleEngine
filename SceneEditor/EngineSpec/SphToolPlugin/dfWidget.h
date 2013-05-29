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
#ifndef DFWIDGET_H
#define DFWIDGET_H

#include "glextensions.h"
#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLWidget>
#include <QKeyEvent>
#include <QPoint>
#include <QTimer>
#include <QtCore/QFileInfo>

class DistanceField3D;

class DFWidget : public QGLWidget
{
    Q_OBJECT

public:
    DFWidget(QWidget *parent = 0 , const QGLWidget* name = 0);
    ~DFWidget();

    void setDf(QFileInfo dffile);

    GLfloat xRot, yRot, zRot;
    GLfloat zoom;
    GLfloat xSpeed, ySpeed;
    GLuint  texture[3];
    GLuint  filter;

    QPoint prepoint, nextpoint, presspoint;
    QTimer *timer;
    bool light;
    bool bigflag,removeflag;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL( int width, int height );
    void keyPressEvent( QKeyEvent *e );
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseDoubleClickEvent ( QMouseEvent * e );
    void remove();
	void closeEvent ( QCloseEvent * event );

protected slots:
    void eddy();

private:
    GLuint vbo_color_;
    GLuint vbo_vert_;
    GLuint pos_count_;
};

#endif // DFWIDGET_H
