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

///////////////////////////////////////////////////////////
//  CheckBoxList.h
//  Implementation of the Class CheckBoxList
//  Created on:      02-Jun-2008 6:53:56 PM
//  Original author: Nasser M. Al-Ansari (Da-Crystal)
///////////////////////////////////////////////////////////
//	Modification History:
//
//	Who(SNo)			Date			Description
//  Volker Wiendl	    3.5.2009        Added stateChanged signal
//	
///////////////////////////////////////////////////////////

#ifndef CHECKBOXLIST_H
#define CHECKBOXLIST_H

#include <QtGui/QComboBox>

class CheckBoxList: public QComboBox
{
    Q_OBJECT
    Q_PROPERTY( QString Display_Text READ getDisplayText WRITE setDisplayText DESIGNABLE true )

public:
    CheckBoxList(QWidget *widget = 0);
    virtual ~CheckBoxList();
    bool eventFilter(QObject *object, QEvent *event);
    virtual void paintEvent(QPaintEvent *);
    void setDisplayText( const QString& text);
    QString getDisplayText() const;

signals:
    void stateChanged( int index, bool checked );

private slots:
    void dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight );

private:
    QString m_DisplayText;
};

#endif // CHECKBOXLIST_H
