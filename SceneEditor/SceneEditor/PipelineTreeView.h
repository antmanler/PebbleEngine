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

#ifndef PIPELINETREEVIEW_H_
#define PIPELINETREEVIEW_H_

#include <QtGui/QTreeView>

class QXmlTreeNode;
class RenderTargetView;

class PipelineTreeView : public QTreeView
{
	Q_OBJECT
public:
	PipelineTreeView(QWidget* parent = 0);
	virtual ~PipelineTreeView();

	bool loadPipeline(const QString& fileName, unsigned int pipelineID);

	/**
	 * Deletes the current model and emits a currentNodeChanged(0) signal
	 */
	void closeTree();

signals:

	/**
	* Signal will be emitted if the currently selected node has changed
	* @param node pointer to the currently active node or a Null pointer if no node has been selected
	*/
	void currentNodeChanged(QXmlTreeNode* node);


protected slots:	
	void currentNodeChanged(const QModelIndex& current, const QModelIndex& previous);

	void showRenderTarget(const QModelIndex& index);

	void removeView(RenderTargetView* view);

private:
	// List of all view widgets for the render targets
	QList<RenderTargetView*> m_renderTargetViews;

	/// The resource id handle of the currently loaded pipeline
	unsigned int			 m_pipelineID;
};
#endif

