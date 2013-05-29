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

#include "PipelineComboBox.h"

#include <SceneEditorCore/EditorUtils.h>
#include <QtCore/QDir>
#include "HordeFileDialog.h"
#include <Horde3D.h>
#include <Horde3DUtils.h>

PipelineComboBox::PipelineComboBox(QWidget* parent /*= 0*/) : QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentChanged(int)));
}


PipelineComboBox::~PipelineComboBox()
{
}

void PipelineComboBox::init( const QString& resourcePath )
{
    clear();
    m_resourcePath = resourcePath;
    blockSignals(true);
    addPipeline(resourcePath);
    addItem(tr("Import from Repository"), QVariant((int) QVariant::UserType));
    if (count() == 1)
        setCurrentIndex(-1);
    blockSignals(false);
}

Pipeline PipelineComboBox::pipeline() const
{
    Pipeline pipeline(currentText());
    pipeline.ResourceID = h3dAddResource(H3DResTypes::Pipeline, qPrintable(currentText()), 0);
    //	h3dutLoadResourcesFromDisk(".");
    SceneEditor::load_h3dres_from_file_qrc(".");
    return pipeline;
}

void PipelineComboBox::setPipeline(Pipeline pipeline)
{
    blockSignals(true);
    setCurrentIndex(findText(pipeline.FileName));
    blockSignals(false);
    m_init = pipeline.FileName;
}

void PipelineComboBox::currentChanged(int index)
{
    if (itemData(index).isValid() && itemData(index) == QVariant((int)QVariant::UserType))
    {
        QString newPipeline = HordeFileDialog::getResourceFile( H3DResTypes::Pipeline, m_resourcePath, this, tr("Select pipeline to import"));
        if (!newPipeline.isEmpty())
        {
            if (findText(newPipeline) == -1)
            {
                removeItem(index);
                addItem(newPipeline);
                addItem(tr("Import from Repository"), QVariant(QVariant::UserType));
            }
            setCurrentIndex(findText(newPipeline));
            return;
        }
        else
            setCurrentIndex(findText(m_init));
        return;
    }
    if (m_init != currentText())
    {
        emit pipelineChanged();
        emit editFinished();
    }
}

void PipelineComboBox::addPipeline(const QString dir)
{
    QDir base(m_resourcePath);
    QList<QFileInfo> pipelines = QDir(dir).entryInfoList(QStringList("*.pipeline.xml"), QDir::Files | QDir::Readable);
    foreach(QFileInfo pipeline, pipelines)
    {
        QString text(base.relativeFilePath(pipeline.absoluteFilePath()));
        addItem(text);
#ifdef Q_WS_WIN
        setItemData(count()-1, text, Qt::ToolTipRole);
#endif
    }
    QList<QFileInfo> pipelineDirs = QDir(dir).entryInfoList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot);
    foreach(QFileInfo directory, pipelineDirs)
    {
        addPipeline(directory.absoluteFilePath());
    }
}
