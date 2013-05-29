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

#ifndef TERRAINTOOLPLUGIN_H_
#define TERRAINTOOLPLUGIN_H_

#include <QtCore/QObject>
#include <QtCore/QFileInfo>
#include <QtCore/QVector>

#include <dflib/sigslot.h>
#include <dflib/vector3f.h>
#include <SceneEditorCore/ToolPlugIn.h>

class DistanceField3D;
struct Particle;
class QProgressDialog;

class SphToolPlugIn : public QObject, public ToolPlugIn, public sigslot::has_slots<>
{
    Q_OBJECT
    Q_INTERFACES(ToolPlugIn)

public:
    SphToolPlugIn(QObject* parent = 0);
    virtual ~SphToolPlugIn();

    QString plugInName() const;

    QList<QAction*> actions() const;

    void setSceneFile(SceneFile* sceneFile);

private slots:
    void generateSdfForScene();
    void generateSdfForModel();
    void viewSdf();
    void generateSphScene();

private:
    bool generateSdf(QString df_file, const QVector<int>& nodelist, int nbx = 70, int nby = 70, int nbz = 70, int nfr = 2);
    bool generateSphScene(QString sphconf, QString dffilename,
                          const QMap<QString, QVector<int> > &fluidmap, const QMap<QString, QVector<int> > &rigidmap,
                          const float setp_size, const float smooth_length, const vector3f &llower, const float scale);
    QString generateParticleConf(QFileInfo conffile, QString tag, const DistanceField3D *sdf,
                                 const QVector<int> &sphlist, const float smooth_length, const vector3f &llower,
                                 const float scale, const bool is_rigid = false);
    bool constrainParticle(const DistanceField3D *sdf, Particle* particle, const float sample_dist);
    // utils
    void recvNotify(const char* stepname, int curr, int steps);

private:
    QList<QAction*>      m_actions;
    SceneFile*           m_scene;
    QProgressDialog*     curr_procdlg_;
    DistanceField3D*     curr_dfgen_;
};
#endif
