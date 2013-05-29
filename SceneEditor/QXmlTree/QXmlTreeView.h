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

#ifndef QXMLTREEVIEW_H_
#define QXMLTREEVIEW_H_

#include <QtGui/QTreeView>

class QAddXmlNodeUndoCommand;
class QRemoveXmlNodeUndoCommand;
class QXmlTreeNode;
class QXmlTreeModel;
class QDomElement;


class QXmlTreeView : public QTreeView
{
    Q_OBJECT

public:
    QXmlTreeView(QWidget* parent = 0);
    virtual ~QXmlTreeView();

    void setModel(QAbstractItemModel *model);

    QXmlTreeNode* currentNode();

public slots:
    /**
         * Copies the currently selected node to the clipboard
         */
    virtual void copyCurrentNode();

    /**
         * Pastes a node from the clipboard to the tree
         */
    virtual void pasteNode();

    /**
         * Removes the currently selected node
         * @param confirm if set to true (default) a dialog will popup to confirm the operation
         */
    virtual void removeCurrentNode(bool confirm = true);

signals:

    /**
         * Signal will be emitted if the currently selected node has changed
         * @param node pointer to the currently active node or a Null pointer if no node has been selected
         */
    void currentNodeChanged(QXmlTreeNode* node);


protected slots:	
    virtual void currentNodeChanged(const QModelIndex& current, const QModelIndex& previous);

protected:
    virtual QAddXmlNodeUndoCommand* createAddUndoCommand(const QDomElement& node, const QDomElement& parent, QXmlTreeModel* model, const QString& text) const;

    virtual QRemoveXmlNodeUndoCommand* createRemoveUndoCommand(QXmlTreeNode* node, const QString& text) const;

};
#endif
