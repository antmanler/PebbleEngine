cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTXML TRUE)

SET(Horde3DEditor_SOURCES
	AboutDialog.cpp
	CameraToolBar.cpp
	CheckBoxList.cpp
	ColladaImporter.cpp
	ExtraTreeView.cpp
	FindDialog.cpp
	GLWidget.cpp
	HordeModelDialog.cpp
	HordeSceneEditor.cpp
	ImgLabel.cpp
	LuaBindingsHorde.cpp
	LuaController.cpp
	MaterialWidget.cpp
	MiscToolBar.cpp
	main.cpp
	#NodeWidget.cpp
	PipelineTreeModel.cpp
	PipelineTreeView.cpp
	PlugInPage.cpp
	QCameraNode.cpp
	QCameraNodePage.cpp
	QEmitterNode.cpp
	QEmitterNodePage.cpp
	QGroupNode.cpp
	QGroupNodePage.cpp
	QJointNode.cpp
	QLightNode.cpp
	QLightNodePage.cpp
	QMeshNode.cpp
	QModelNode.cpp
	QOneTimeDialog.cpp
	QPipelineCommandQueueNode.cpp
	QPipelineNode.cpp
	QPipelineRenderTargetNode.cpp
	QPipelineClearTargetNode.cpp
	QPipelineDrawGeometryNode.cpp
	QPipelineBindBufferNode.cpp
	QPipelineUnbindBuffersNode.cpp
	QPipelineSwitchTargetNode.cpp
	QPipelineDrawQuadNode.cpp
	QPipelineDrawOverlaysNode.cpp
	QPipelineDoForwardLightLoopNode.cpp
	QPipelineDoDeferredLightLoopNode.cpp
	QPipelineSetUniformNode.cpp
	QPipelineSetupNode.cpp
	QPipelineStageNode.cpp
	QReferenceNode.cpp
	QReferenceNodePage.cpp
	QStdOutListener.cpp
	QTexUnit.cpp
	RenderTargetView.cpp
	SceneFilePage.cpp
	SceneTreeView.cpp
	SceneTreeWidget.cpp
	SceneWizard.cpp
	SettingsDialog.cpp
)

SET(Horde3DEditor_HEADERS
	AboutDialog.h
	CameraToolBar.h
	CheckBoxList.h
	ColladaImporter.h
	ExtraTreeView.h
	FindDialog.h
	GLWidget.h
	HordeModelDialog.h
	HordeSceneEditor.h
	ImgLabel.h
	LuaBindingsHorde.h
	LuaController.h
	MaterialWidget.h
	MiscToolBar.h
	#NodeWidget.h
	PipelineTreeModel.h
	PipelineTreeView.h
	PlugInPage.h
	QCameraNode.h
	QCameraNodePage.h
	QEmitterNode.h
	QEmitterNodePage.h
	QGroupNode.h
	QGroupNodePage.h
	QJointNode.h
	QLightNode.h
	QLightNodePage.h
	QMeshNode.h
	QModelNode.h
	QOneTimeDialog.h
	QPipelineCommandQueueNode.h
    QPipelineNode.h
	QPipelineRenderTargetNode.h
	QPipelineClearTargetNode.h
	QPipelineDrawGeometryNode.h
	QPipelineBindBufferNode.h
	QPipelineUnbindBuffersNode.h
	QPipelineSwitchTargetNode.h
	QPipelineDrawQuadNode.h
	QPipelineDrawOverlaysNode.h
	QPipelineDoForwardLightLoopNode.h
	QPipelineDoDeferredLightLoopNode.h
	QPipelineSetUniformNode.h
	QPipelineSetupNode.h
	QPipelineStageNode.h
	QReferenceNode.h
	QReferenceNodePage.h
	QStdOutListener.h
	QTexUnit.h
	RenderTargetView.h
	SceneFilePage.h
	SceneTreeView.h
	SceneTreeWidget.h
	SceneWizard.h
	SettingsDialog.h	
)

SET(Horde3DEditor_RESOURCES
	resources.qrc
)

SET(Horde3DEditor_FORMS
	AboutDialog.ui
	CameraToolBar.ui
	ColladaImporter.ui
	FindDialog.ui
	HordeSceneEditor.ui
	LuaController.ui
	MaterialWidget.ui
	MiscToolBar.ui
	PlugInPage.ui
	QCameraNodePage.ui
	QEmitterNodePage.ui
	QGroupNodePage.ui
	QLightNodePage.ui
	QOneTimeDialog.ui
	QReferenceNodePage.ui
	RenderTargetView.ui
	SceneFilePage.ui
	SceneTreeWidget.ui
	SettingsDialog.ui
)

source_group("Forms" FILES ${Horde3DEditor_FORMS})

IF(MSVC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

QT4_WRAP_CPP(Horde3DEditor_HEADERS_MOC ${Horde3DEditor_HEADERS})
QT4_WRAP_UI(Horde3DEditor_FORMS_HEADERS ${Horde3DEditor_FORMS})
QT4_ADD_RESOURCES(Horde3DEditor_RESOURCES_RCC ${Horde3DEditor_RESOURCES})

INCLUDE_DIRECTORIES(
    "../"
    "${CMAKE_CURRENT_BINARY_DIR}" 
    ${CMAKE_CURRENT_SOURCE_DIR} )

INCLUDE(${QT_USE_FILE})

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(SceneEditor MACOSX_BUNDLE
    ${Horde3DEditor_SOURCES} 
	${Horde3DEditor_HEADERS}
    ${Horde3DEditor_HEADERS_MOC} 
    ${Horde3DEditor_FORMS_HEADERS} 
    ${Horde3DEditor_RESOURCES_RCC}
    MMaths.h
    MMaths.cpp
    Trackball.h
    Trackball.cpp
    )

TARGET_LINK_LIBRARIES(SceneEditor 
    PebbleEditorCore
    EditorLib
    #QXmlTree
    #QPropertyEditor
    Lua
    #Horde3D
    #Horde3DUtils
    ${QT_LIBRARIES} 
    ${OPENGL_glu_LIBRARY}   )

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(APPLICATION_SERVICES_LIBRARY ApplicationServices)
    FIND_LIBRARY(AGL_LIBRARY AGL)
    target_link_libraries(SceneEditor ${COCOA_LIBRARY} ${APPLICATION_SERVICES_LIBRARY} ${AGL_LIBRARY})
endif()
