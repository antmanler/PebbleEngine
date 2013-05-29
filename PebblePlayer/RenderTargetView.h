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

#ifndef RENDERTARGETVIEW_H_
#define RENDERTARGETVIEW_H_

#include <QtXml/qdom.h>
#include <QtGui/QLabel>

#include "ui_RenderTargetView.h"

class QImage;
class ImgLabel;
class QScrollArea;
class QScrollBar;

class RenderTargetView : public QWidget, protected Ui_RenderTargetView
{
	Q_OBJECT
public:
	RenderTargetView(QWidget* parent = 0, Qt::WFlags flags = 0);
	virtual ~RenderTargetView();
    
signals:
	void closed(RenderTargetView*);
    
    
public slots:
    void setCamera(int cam) ;
    
private slots:
    void changeTarget(int index) ;
	void showPixel(int x, int y);
	void saveBuffer();
	void zoomIn();
	void zoomOut();
	void normalSize();	
	void showBuffer(int index = -2);
    
protected:
	void closeEvent(QCloseEvent* event);
	void mousePressEvent(QMouseEvent* event);
    
private:
	QImage convertImageData(float* data, unsigned int width, unsigned int height, unsigned int depth, bool ignoreAlpha);
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	
	unsigned int	m_pipelineID;
    unsigned int    m_camera;
    
    ImgLabel*	    m_imageLabel;
    QScrollArea*    m_scrollArea;
    double		    m_scaleFactor;
    QString         curr_id_;
    
    float*		    m_imgData;
    int		        m_imgDepth;
    float           depth_min_;
    float           depth_max_;
};

class ImgLabel : public QLabel
{
	Q_OBJECT
public:
	ImgLabel(QWidget* parent = 0);
	virtual ~ImgLabel();
    
signals:
	void currentPixel(int x, int y);
	void zoomOut();
	void zoomIn();
    
protected:
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
    
};

class RenderTargetViewWarpper : public QObject
{
    Q_OBJECT
    public slots:
    RenderTargetView *new_RenderTargetView(QWidget* parent = 0, Qt::WFlags flags = 0) { return new RenderTargetView(parent, flags); }
    void delete_RenderTargetView(RenderTargetView *widget) { widget->setParent(NULL); delete widget; }
} ;

#endif
