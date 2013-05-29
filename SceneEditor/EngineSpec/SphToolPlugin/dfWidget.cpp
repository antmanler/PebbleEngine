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
#include "dfwidget.h"

#ifdef Q_WS_MAC
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <dflib/mesh.h>
#include <dflib/distancefield3d.h>


DFWidget::DFWidget( QWidget* parent, const QGLWidget* name )
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent, name )
    , vbo_color_(0)
    , vbo_vert_(0)
    , pos_count_(0)
{

    setWindowTitle("distance field");
    xRot = yRot = zRot = 0.0;
    zoom = -0.35f;
    xSpeed = ySpeed = 0.0;

    filter = 0;
    light = false;
    removeflag = false;
    bigflag = false;
    timer = new QTimer;
    connect( timer, SIGNAL(timeout()), this, SLOT(eddy()) );

}

DFWidget::~DFWidget()
{
    // makeCurrent();
//     if(vbo_color_) glDeleteBuffers(1, &vbo_color_);
//     if(vbo_vert_) glDeleteBuffers(1, &vbo_vert_);
}

void DFWidget::setDf(QFileInfo dffile)
{
    if (dffile.exists()) {

        DistanceField3D* df = new DistanceField3D() ;
        df->readFromFile(dffile.absoluteFilePath().toStdString());

        // fill buffers
        int nbx = df->nbx, nby = df->nby, nbz = df->nbz;
        vector3f h = df->h;

        pos_count_ = nbx*nby*nbz;

        vector<float> colors; colors.reserve(nbx*nby*nbz*4);
        vector<float> verts; verts.reserve(nbx*nby*nbz*3);

        for(int i = 0; i<= nbx ;i++)
            for(int j = 0 ; j <= nby ; j++)
                for(int k = 0 ; k <= nbz ; k++) {
                    float dist = df->getDistance(i,j,k);
                    dist *= 20;
                    //dist += 0.3f;
                    if(dist >= 0) {
                        colors.push_back(exp(-1.0f*fabs(dist)));
                        colors.push_back(exp(-1.0f*fabs(dist)));
                        colors.push_back(exp(-1.0f*fabs(dist)));
                        colors.push_back(exp(-1.0f*fabs(dist)));

                        verts.push_back((float)i * h.x - nbx*h.x*0.5);
                        verts.push_back((float)j*h.y);
                        verts.push_back((float)k*h.z- nbz*h.z*0.5);

                    }
                }

        pos_count_ = verts.size()/3;

        glInit ();

        makeCurrent();

        if(vbo_color_) glDeleteBuffers(1, &vbo_color_);
        if(vbo_vert_) glDeleteBuffers(1, &vbo_vert_);

        glGenBuffers(1, &vbo_color_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_);
        glBufferData(GL_ARRAY_BUFFER, pos_count_*4*sizeof(float), &colors[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &vbo_vert_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vert_);
        glBufferData(GL_ARRAY_BUFFER, pos_count_*3*sizeof(float), &verts[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        delete df;
        setWindowTitle(dffile.fileName());
    }
}

void DFWidget::initializeGL()
{
    makeCurrent();
    getGLExtensionFunctions().resolve(context());
    format().setVersion(2,1);
    setFormat(format());
    glShadeModel( GL_SMOOTH );
}

void DFWidget::paintGL()
{
    if ( vbo_color_==0 || vbo_color_==0 ) return;

    makeCurrent();

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glClearColor(0.3f ,0.3f ,0.3f ,0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);


    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    glTranslatef( -0.0,  -0.4f, -2.0f);

    glRotatef( xRot,  1.0f,  0.0,  0.0 );
    glRotatef( yRot,  0.0,  1.0f,  0.0 );

    glPointSize(1.5);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_vert_);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_color_);
    glColorPointer(4, GL_FLOAT, 0, 0);
    glEnableClientState(GL_COLOR_ARRAY);


    glDrawArrays(GL_POINTS, 0, pos_count_);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glPopAttrib();

    xRot +=0.1;
    yRot -=0.1;
}

void DFWidget::resizeGL( int x, int y )
{
    glViewport(0,0,x,y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50, x/(y*1.0), 0.05f, 10.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.5f, 0.0, 0, 0.0, 0.0, 0, 0, 1, 0);
}

void DFWidget::keyPressEvent( QKeyEvent *e )
{

    switch ( e->key() )
    {
    case Qt::Key_Up:
        xSpeed -= 0.01;
        updateGL();
        break;
    case Qt::Key_Down:
        xSpeed += 0.01;
        updateGL();
        break;
    case Qt::Key_Escape:
        close();
    default:
        break;
    }

}


void DFWidget::mousePressEvent(QMouseEvent * e)
{
    //if(isTopLevel())
    {
        presspoint = nextpoint = prepoint = e->pos();
    }
}

void DFWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
    //if(isTopLevel())
    {
        if( zoom<-5.0001 || zoom>-4.999 )  {
            removeflag = true;
            if(zoom > -4.999)
                bigflag = true;
            else
                bigflag = false;
            timer->start(10);
        }
    }

}

void DFWidget::mouseMoveEvent(QMouseEvent * e)
{
    if(isTopLevel())  {
        prepoint = nextpoint;
        nextpoint = e->pos();
    }
    xRot += (GLfloat)(nextpoint.y() - prepoint.y())*100/width();
    yRot += (GLfloat)(nextpoint.x() - prepoint.x())*100/height();
    xSpeed =(GLfloat)(nextpoint.y() - prepoint.y())*50/width();
    ySpeed = (GLfloat)(nextpoint.x() - prepoint.x())*50/height();
    updateGL();

}

void DFWidget::mouseReleaseEvent(QMouseEvent * e)
{
    if(isTopLevel()) {
        if( presspoint != e->pos() ) {
            timer->start(10);
        } else if(!removeflag) {
            timer->stop();
        }
    }
}

void DFWidget::eddy()
{
    xRot += xSpeed;
    yRot += ySpeed;

    if(removeflag) remove();

    updateGL();
    timer->start(20);
}

void DFWidget::remove()
{
    if(bigflag) {
        zoom += -0.1;
        if(zoom < -5.0001) {
            removeflag = false;
            zoom = -5.0;
            if( (xSpeed>-0.0001 && xSpeed<0.0001)
                && (ySpeed>-0.0001 && ySpeed<0.0001) ) {
                timer->stop();
                updateGL();
                return;
            }
        }
    }

    if(!bigflag) {
        zoom += 0.1;
        if(zoom > -4.9999) {
            removeflag = false;
            zoom = -5.0;
            if( (xSpeed>-0.0001 && xSpeed<0.0001)
                && (ySpeed>-0.0001 && ySpeed<0.0001) )  {
                timer->stop();
                updateGL();
                return;
            }
        }
    }
}

void DFWidget::closeEvent(QCloseEvent * event)
{
	if(vbo_color_) glDeleteBuffers(1, &vbo_color_);
	if(vbo_vert_) glDeleteBuffers(1, &vbo_vert_);
	QWidget::closeEvent(event);
}
