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



//#include <QXmlTree/QXmlTreeNode.h>

#include <QMessageBox>
#include <QApplication>
#include <QTime>
#include <QKeyEvent>
#include <QFile>
#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtCore/QDateTime>
#include <QtGui/QLabel>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <Horde3D/Horde3D.h>
#include <Horde3D/Horde3DUtils.h>

#include <PythonQt/PythonQtScriptingConsole.h>

#include <math.h>

#include <limits>

#if defined (Q_WS_MAC)
#include <OpenGL/glu.h>
#endif

#include <Engine/Engine.h>

#include "GLWidget.h"
//#include "QPIWEditorSettings.h"

GLWidget::GLWidget(QWidget *parent, Qt::WFlags flags /*= 0*/)
    : QGLWidget(parent, 0, flags)
    , fps_(0.0)
    , frame_id_(0)
    , timer_id_(0)
    , m_fullScreen(false)
    , is_welcome_view_(true)
    , debug_console_(NULL)
{
    setFocusPolicy(Qt::ClickFocus);
    render_timer_ = new QTimer(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setAcceptDrops(true);

    connect(render_timer_, SIGNAL(timeout()), this, SLOT(updateGL()));
    // init PIW
    const char* defualt_setting = "<Settings>\n"
                                  "    <Setting key=\"ResourceRepositry\" value=\"../../Data/Common\" />\n"
                                  "    <Setting key=\"gravity\" value=\"-9.81\" />\n"
                                  "</Settings>";
    piwInit(defualt_setting, strlen(defualt_setting));
    // load plugins
    piwLoadPlugin("SceneGraph");
    piwLoadPlugin("PythonBehavior");
    piwLoadPlugin("BulletPhysics");

    piwParseSysArgv(QApplication::argc(), QApplication::argv());
    piwutDumpMessages();
}

GLWidget::~GLWidget()
{
	h3dinitialized_ = false;
	render_timer_->stop();
    if (is_welcome_view_) {
        h3dRelease();
    }
    piwRelease();
    piwutDumpMessages();
}

void GLWidget::changeScene(const char *filename)
{
    if (!isVisible()) {
        cur_scn_ = "";
        return;
    }

	h3dinitialized_ = false;
    render_timer_->stop();
    if (is_welcome_view_) {
        h3dRelease();
		is_welcome_view_ = false;
    }

    piwutLoadSceneFromFile(filename);
	cur_scn_ = filename;

    piwutDumpMessages();
    h3dutDumpMessages();
    setWindowTitle(tr("Pebble - %2").arg(filename));
    QApplication::setActiveWindow(this); 
	render_timer_->start(0);
	h3dinitialized_ = true;
}

void GLWidget::reset()
{
    if (!cur_scn_.isEmpty()) {
        // close current scene
        piwLoadScene(NULL, 0);
        cur_scn_ = "";
    }

    if ( h3dInit() == false ) {
        h3dutDumpMessages();
        QMessageBox::warning(this, tr("Error"), tr("Error initializing Horde3D!"));
        return;
    }

    // create welcome view
    h3dinitialized_ = true; m_debugViewMode = false; m_wireframeMode = false;
    h3dClear();

    m_fontMatRes = h3dAddResource( H3DResTypes::Material, "overlays/font.material.xml", 0 );
    m_panelMatRes = h3dAddResource( H3DResTypes::Material, "overlays/panel.material.xml", 0 );
    m_logoMatRes = h3dAddResource( H3DResTypes::Material, "overlays/logo.material.xml", 0 );
    H3DRes simple_pipeline = h3dAddResource( H3DResTypes::Pipeline, "pipelines/simple.pipeline.xml", 0 );
    cur_camera_ = h3dAddCameraNode( H3DRootNode, "__default_camera__", simple_pipeline );
    // load overlay resources
    load_res_frome_qrc() ;
    QSize sz = size() ;
    if (sz.width() > 0 && sz.height() > 0) {
        int width = sz.width(), height = sz.height();
        // Resize viewport
        h3dSetNodeParamI( cur_camera_, H3DCamera::ViewportXI, 0 );
        h3dSetNodeParamI( cur_camera_, H3DCamera::ViewportYI, 0 );
        h3dSetNodeParamI( cur_camera_, H3DCamera::ViewportWidthI, width );
        h3dSetNodeParamI( cur_camera_, H3DCamera::ViewportHeightI, height );

        // Set virtual camera parameters
        h3dSetupCameraView( cur_camera_, 45.0f, (float)width / height, 0.1f, 1000.0f );
        H3DRes pipeline = h3dGetNodeParamI( cur_camera_, H3DCamera::PipeResI ) ;
        h3dResizePipelineBuffers( pipeline, width, height );
    }
    //    timer_id_ = startTimer(0);
    render_timer_->start(0);
    is_welcome_view_ = true;
}

void GLWidget::initializeGL()
{		
    reset();
}

void GLWidget::resizeGL(int width, int height)
{
    if (is_welcome_view_) {
        // Resize viewport
        h3dSetNodeParamI( cur_camera_, H3DCamera::ViewportXI, 0 );
        h3dSetNodeParamI( cur_camera_, H3DCamera::ViewportYI, 0 );
        h3dSetNodeParamI( cur_camera_, H3DCamera::ViewportWidthI, width );
        h3dSetNodeParamI( cur_camera_, H3DCamera::ViewportHeightI, height );

        // Set virtual camera parameters
        h3dSetupCameraView( cur_camera_, 45.0f, (float)width / height, 0.1f, 1000.0f );
        H3DRes pipeline = h3dGetNodeParamI( cur_camera_, H3DCamera::PipeResI ) ;
        h3dResizePipelineBuffers( pipeline, width, height );
    }
    piwResize(width, height);
}

//void GLWidget::timerEvent( QTimerEvent *ev )
//{
//    if (ev->timerId() != timer_id_ ) return;
//    updateGL();
//}

void GLWidget::focusInEvent(QFocusEvent * event)
{
    //    frame_skip_ = false;
    //    timer_id_ = startTimer(0);
    if (!render_timer_->isActive()) {
        render_timer_->start(0);
        event->accept();
        return;
    }
    QGLWidget::focusInEvent(event);
}

void GLWidget::focusOutEvent(QFocusEvent * event)
{
    //    frame_skip_ = true;
    //    killTimer(timer_id_);
    //    timer_id_ = 0;
    if (render_timer_->isActive()) {
        render_timer_->stop();
        event->accept();
        return;
    }
    QGLWidget::focusInEvent(event);
}

void GLWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void GLWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty()) return;

    changeScene(fileName.toStdString().c_str());

}

void GLWidget::paintGL()
{
	if (!h3dinitialized_) return;

    static QTime initTime(QTime::currentTime());
    static int fps = 0;

    ++fps;
    int mseconds = initTime.msecsTo( QTime::currentTime() );
    if( fps%5 == 0 && mseconds >= 2000) {
        // Calculate frame rate
        fps_ =  fps * 1000.0f / mseconds;;
        // Reset counter
        fps = 0;
        initTime = QTime::currentTime();
    }

    frame_id_++;

    h3dSetOption( H3DOptions::DebugViewMode, m_debugViewMode ? 1.0f : 0.0f );
    h3dSetOption( H3DOptions::WireframeMode, m_wireframeMode ? 1.0f : 0.0f );

    makeCurrent();

    if (is_welcome_view_) {
        // Show logo
        const float ww = (float)h3dGetNodeParamI( cur_camera_, H3DCamera::ViewportWidthI ) /
                         (float)h3dGetNodeParamI( cur_camera_, H3DCamera::ViewportHeightI );
        const float ovLogo[] =
        {
            ww - 0.4f,  0.8f,  0,  1,
            ww - 0.4f,  1,     0,  0,
            ww,         1,     1,  0,
            ww,         0.8f,  1,  1
        };
        h3dShowOverlays( ovLogo, 4, 1.f, 1.f, 1.f, 1.f, m_logoMatRes, 0 );

        h3dRender( cur_camera_ );
        h3dFinalizeFrame();
        // Remove all overlays
        h3dClearOverlays();
        // Write all messages to log file
        h3dutDumpMessages();
    } else {
        // update
        piwUpdate();
        piwutDumpMessages();

        // render
        piwRender();
        piwutDumpMessages();
    }
}

void GLWidget::wheelEvent(QWheelEvent* event)
{

}

void GLWidget::keyPressEvent(QKeyEvent* ev)
{
    if( ev->key() == Qt::Key_Escape ) {
        if( m_fullScreen )  {
            showNormal();
            m_fullScreen = false;
        }
    } else if( ev->key() == Qt::Key_F1 && !m_fullScreen ) {
        m_fullScreen = true;
        showFullScreen();
    } else if( ev->key() == Qt::Key_F7 )
        m_debugViewMode = !m_debugViewMode;
    else if( ev->key() == Qt::Key_F8 )  // F8
        m_wireframeMode = !m_wireframeMode;
    else if (ev->key() == Qt::Key_G ) {
        // create debug console
        if (NULL == debug_console_) {
            PythonQtObjectPtr  __main__context = PythonQt::self()->getMainModule();
            debug_console_ = new PythonQtScriptingConsole(0, __main__context);
            debug_console_->resize(400, 200);
            __main__context.addObject("console", debug_console_);
        }
        debug_console_->clear();
        //        debug_console_->appendCommandPrompt();
        debug_console_->show();
    }

}

void GLWidget::keyReleaseEvent(QKeyEvent* event)
{

}

void GLWidget::mousePressEvent ( QMouseEvent * event ) 
{

}

void GLWidget::mouseDoubleClickEvent(QMouseEvent* event)
{

}

void GLWidget::mouseReleaseEvent(QMouseEvent* event)
{

}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{	

}

bool GLWidget::load_res_frome_qrc()
{
    bool result = true;

    // Get the first resource that needs to be loaded
    int res = h3dQueryUnloadedResource( 0 );

    char *dataBuf = 0;
    int bufSize = 0;

    while( res != 0 ) {
        QString filename = ":/" + QString::fromAscii(h3dGetResName( res ))  ;
        QFile  inf(filename);

        // Open resource file
        if( inf.open(QIODevice::ReadOnly) ) {
            // Find size of resource file
            int fileSize = inf.size();

            if( bufSize < fileSize  ) {
                delete[] dataBuf;
                dataBuf = new char[fileSize];
                if( !dataBuf ) {
                    bufSize = 0;
                    continue;
                }
                bufSize = fileSize;
            }
            if( fileSize == 0 )	continue;
            // Copy resource file to memory
            inf.read( dataBuf, fileSize );
            inf.close();
            // Send resource data to engine
            result &= h3dLoadResource( res, dataBuf, fileSize );
        } else  {
            // Tell engine to use the dafault resource by using NULL as data pointer
            h3dLoadResource( res, 0x0, 0 );
            result = false;
        }
        // Get next unloaded resource
        res = h3dQueryUnloadedResource( 0 );
    }
    delete[] dataBuf;

    return result;
}
