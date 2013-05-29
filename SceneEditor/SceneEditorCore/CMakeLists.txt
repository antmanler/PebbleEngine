cmake_minimum_required(VERSION 2.8.6) 

FIND_PACKAGE(Qt4 REQUIRED)


IF(MSVC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

SET(QPropertyEditor_SOURCES
	QPropertyEditor/ColorCombo.cpp
	QPropertyEditor/EnumProperty.cpp
	QPropertyEditor/Property.cpp
	QPropertyEditor/QPropertyEditorWidget.cpp
	QPropertyEditor/QPropertyModel.cpp
	QPropertyEditor/QVariantDelegate.cpp
)
SET(QPropertyEditor_HEADERS
	QPropertyEditor/ColorCombo.h
	QPropertyEditor/EnumProperty.h
	QPropertyEditor/Property.h
	QPropertyEditor/QPropertyEditorWidget.h
	QPropertyEditor/QPropertyModel.h
	QPropertyEditor/QVariantDelegate.h
)

SET(SceneEditorCore_SOURCES
	CustomTypes.cpp
	EffectComboBox.cpp
	EffectProperty.cpp
	FrustumProperty.cpp
	HordeFileDialog.cpp
	Importer.cpp
	MaterialComboBox.cpp
	MaterialProperty.cpp
	OverwriteFilesDialog.cpp
	PipelineComboBox.cpp
	PipelineProperty.cpp
	PlugInManager.cpp
	QHordeSceneEditorSettings.cpp
	QSceneNode.cpp
	QUniform.cpp
	SceneFile.cpp
	SceneTreeModel.cpp
	ShaderComboBox.cpp
	ShaderData.cpp
	TextureComboBox.cpp
	TextureProperty.cpp
	Vec3fProperty.cpp
	EditorService.cpp
	NodeWidget.cpp
	${QPropertyEditor_SOURCES}
)
SET(SceneEditorCore_HEADERS
    Config.h
	ToolPlugIn.h
	QHordeSceneEditorSettings.h
	ShaderData.h
	CustomTypes.h
	ExtSceneNodePlugIn.h
	Importer.h
	SceneFile.h
	EditorService.h
)

SET(SceneEditorCore_QOBJECT_HEADERS
	AttachmentPlugIn.h
	EffectComboBox.h
	EffectProperty.h
	FrustumProperty.h
	HordeFileDialog.h
	MaterialComboBox.h
	MaterialProperty.h
	OverwriteFilesDialog.h
	PipelineComboBox.h
	PipelineProperty.h
	PlugInManager.h
	QSceneNode.h
	QUniform.h
	SceneTreeModel.h
	ShaderComboBox.h
	TextureComboBox.h
	TextureProperty.h
	Vec3fProperty.h
	NodeWidget.h
	${QPropertyEditor_HEADERS}
)

SET(SceneEditorCore_FORMS
	HordeFileDialog.ui
	OverwriteFilesDialog.ui
)

QT4_WRAP_CPP(SceneEditorCore_HEADERS_MOC ${SceneEditorCore_QOBJECT_HEADERS})
QT4_WRAP_UI(SceneEditorCore_FORMS_HEADERS ${SceneEditorCore_FORMS})
SET(QT_USE_QTXML TRUE)

source_group("Forms" FILES ${SceneEditorCore_FORMS})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
	endif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY( PebbleEditorCore SHARED
    ${SceneEditorCore_SOURCES} 
	${SceneEditorCore_QOBJECT_HEADERS}
	${SceneEditorCore_HEADERS}
	${SceneEditorCore_FORMS_HEADERS}
    ${SceneEditorCore_HEADERS_MOC}  
    EditorUtils.h
    EditorUtils.cpp   
)

TARGET_LINK_LIBRARIES( PebbleEditorCore 
    #QPropertyEditor
    QXmlTree
    Horde3D
    Horde3DUtils
    ${QT_LIBRARIES} )