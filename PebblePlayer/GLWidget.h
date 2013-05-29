// ****************************************************************************************
//
// Horde3D Scene Editor 
// --------------------------------------
// Copyright (C) 2007 Volker Wiendl
// 
// This file is part of the Horde3D Scene Editor.
//
// The Horde3D Scene Editor is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License 
//
// The Horde3D Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ****************************************************************************************

#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include <QGLWidget>
#include <Horde3D/Horde3D.h>

class PythonQtScriptingConsole;

/**
 * Rendering widget creating an OpenGL context for the Horde3D engine. <br>
 */
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = 0, Qt::WFlags flags = 0);
    virtual ~GLWidget();

public slots:
    void changeScene(const char *filename);
    void reset();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
//    void timerEvent( QTimerEvent *ev );
    virtual void wheelEvent( QWheelEvent * event );
    virtual void keyPressEvent(QKeyEvent* ev);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    bool load_res_frome_qrc();

private:
    float fps_;
    unsigned long frame_id_;
    bool  frame_skip_;
    int   timer_id_;
    QTimer*  render_timer_;
    bool  m_fullScreen;

    bool  is_welcome_view_;
    bool  h3dinitialized_, m_debugViewMode, m_wireframeMode;

    PythonQtScriptingConsole *debug_console_;

    QString cur_scn_;

    // default view
    H3DRes cur_camera_;
    H3DRes m_fontMatRes, m_panelMatRes;
    H3DRes m_logoMatRes;
};


class GLWidgetWarpper : public QObject
{
    Q_OBJECT
public slots:
    GLWidget *new_GLWidget(QWidget *parent = 0) { return new GLWidget(parent); }
    void delete_GLWidget(GLWidget *widget) { widget->setParent(NULL); delete widget; }
} ;

#endif
