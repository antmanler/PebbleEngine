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

#ifndef RENDERTARGETVIEW_H_
#define RENDERTARGETVIEW_H_

#include <QtXml/qdom.h>

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

	void setRenderTarget(const QDomElement& renderTarget, unsigned int pipelineID);

signals:
	void closed(RenderTargetView*);

protected:
	void closeEvent(QCloseEvent* event);
	void mousePressEvent(QMouseEvent* event);

private slots:
	void showBuffer(int index = -2);
	void showPixel(int x, int y);
	void saveBuffer();
	void zoomIn();
	void zoomOut();
	void normalSize();	

private:
	QImage convertImageData(float* data, unsigned int width, unsigned int height, unsigned int depth, bool ignoreAlpha);
	void scaleImage(double factor);
	void adjustScrollBar(QScrollBar *scrollBar, double factor);
	
	unsigned int	m_pipelineID;

        ImgLabel*	m_imageLabel;
        QScrollArea*    m_scrollArea;
        double		m_scaleFactor;

        float*		m_imgData;
        int		m_imgDepth;
};
#endif
