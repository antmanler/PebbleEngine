// ****************************************************************************************
//
// Pebble Scene Editor 
// Copyright (C) 2011 Antmanler
// This file is part of the Pebble Scene Editor.
// The Pebble Scene Editor is free software; you can redistribute it and/or modify
// This file is part of the Horde3D Scene Editor.
//
// The Pebble Scene Editor is distributed in the hope that it will be useful,
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 3 of the License 
//
// The Horde3D Scene Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Credits : Volker Wiendl who created the major work of the original editor and Horde3D
// is a flexible graphics engine. see <http://www.horde3d.org> and <http://http://goo.gl/Me2HT>
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include <SceneEditorCore/AttachmentPlugIn.h>
#include <SceneEditorCore/SceneFile.h>
#include <SceneEditorCore/PlugInManager.h>

#include "ExtraTreeView.h"
#include "SceneWizard.h"
#include "HordeSceneEditor.h"

#include <QXmlTree/QXmlTreeModel.h>
#include <QXmlTree/QXmlTreeNode.h>
#include <QXmlTree/QXmlTreeUndoCommands.h>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QAction>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>


ExtraTreeView::ExtraTreeView(QWidget* parent /*= 0*/) : QXmlTreeView(parent)
{
}


ExtraTreeView::~ExtraTreeView()
{
	closeTree();
}

void ExtraTreeView::contextMenuEvent(QContextMenuEvent *event)
{	
	setCurrentIndex(indexAt(event->pos()));
	QModelIndex index(currentIndex());
	if (!index.isValid()) clearSelection();
	QList<QAction*> actions = this->actions();
	for (int i = 0; i<actions.size();)
		if (!actions[i]->isEnabled())
			actions.takeAt(i);
		else 
			++i;
	if( !actions.isEmpty() )
	{
		m_contextCall = true;
		QMenu::exec(actions, mapToGlobal(event->pos()));
		m_contextCall = false;
	}
	event->accept();
}

void ExtraTreeView::closeTree()
{
	emit QXmlTreeView::currentNodeChanged(0);
	delete model();
	setModel(0);	
}

QUndoStack* ExtraTreeView::currentUndoStack()
{
	QXmlTreeModel* treeModel = static_cast<QXmlTreeModel*>(model());
	if( treeModel )	return treeModel->undoStack();	
	else return 0;
}

bool ExtraTreeView::loadExtras(SceneFile* sceneFile)
{
        if( model() )
        {
                emit QXmlTreeView::currentNodeChanged(0);
                delete model();
        }
        if( sceneFile && sceneFile->pluginManager()->attachmentPlugIn() )
        {
                AttachmentPlugIn* plugin = sceneFile->pluginManager()->attachmentPlugIn();
                QXmlTreeModel* model = plugin->initExtras(sceneFile->sceneFileDom().documentElement().firstChildElement("Extras"), this);
                connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(nodeDataChanged(const QModelIndex&)));
                setModel(model);
        }
        return true;
}


void ExtraTreeView::addNode()
{
    QAction* source = qobject_cast<QAction*>(sender());
    if( !source ) return;

    QXmlTreeModel* treeModel = static_cast<QXmlTreeModel*>(model());
    if( treeModel == 0)	return;

    QXmlTreeNode* H3DRootNode = treeModel->rootNode();
    // select the currently selected node as root
    if( currentIndex().isValid() ) H3DRootNode = static_cast<QXmlTreeNode*>(currentIndex().internalPointer());

    QDomElement newNode = HordeSceneEditor::instance()->pluginManager()->createNode( source->data().toString(), this );
    if( !newNode.isNull() )
        treeModel->undoStack()->push(createAddUndoCommand(newNode, H3DRootNode->xmlNode(), treeModel, source->text()));
}

void ExtraTreeView::currentNodeChanged(const QModelIndex& current, const QModelIndex& previous)
{
    QXmlTreeNode* prevNode = static_cast<QXmlTreeNode*>(previous.internalPointer());
    if( prevNode ) {
        prevNode->removeEventFilter(this);
    }
    if ( current.internalPointer() != previous.internalPointer() )
        emit nodeAboutToChange();
    QXmlTreeView::currentNodeChanged(current, previous);
    QXmlTreeNode* node = static_cast<QXmlTreeNode*>(current.internalPointer());
    if( node ) {
        node->installEventFilter(this);
    }
}

bool ExtraTreeView::eventFilter(QObject *obj, QEvent *event)
{
    if( event->type() == QEvent::DynamicPropertyChange )
    {
        nodeDataChanged(currentIndex());
    }
    return QXmlTreeView::eventFilter(obj, event);
}

bool ExtraTreeView::selectNode( int H3DNode )
{
    if( model() == 0 ) return false;
    else if (H3DNode == 0)
    {
        setCurrentIndex(QModelIndex());
        return false;
    }

    QModelIndexList items(model()->match(model()->index(0,2), Qt::DisplayRole, H3DNode, 1, Qt::MatchExactly | Qt::MatchCaseSensitive | Qt::MatchRecursive));
    if (!items.isEmpty())
    {
        setCurrentIndex(items.first());
        scrollTo(items.first(), QAbstractItemView::PositionAtCenter);
        return true;
    }
    else
        return false;
}

void ExtraTreeView::nodeDataChanged(const QModelIndex & index)
{
    if (index.isValid() && index.row() == currentIndex().row())
    {
        QXmlTreeNode* object = static_cast<QXmlTreeNode*>(currentIndex().internalPointer());
        emit nodePropertyChanged(object);
    }
}

