
#pragma once

#include <QtGui/QWidget>
#include <Horde3D.h>
#include "Config.h"

class SceneFile;

class DLLEXP IEditorService
{
public:
    virtual SceneFile* currentScene() = 0;
    virtual QString getModelFile(const QString& targetPath, QWidget* parent, const QString& caption) = 0;
    //virtual QString getResourceFile( H3DResTypes::List resourceType, const QString& targetPath, QWidget* parent, const QString& caption) = 0;
};


class DLLEXP EditorService
{
public:
    static void registerProvider(IEditorService *provider);
    static SceneFile* currentScene() ;
    static QString getModelFile(const QString& targetPath, QWidget* parent, const QString& caption);
    static QString getResourceFile( H3DResTypes::List resourceType, const QString& targetPath, QWidget* parent, const QString& caption);

private:
    static IEditorService *provider_;
};
