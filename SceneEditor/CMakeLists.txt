cmake_minimum_required(VERSION 2.8) 

PROJECT(Horde3DEditor)

include_directories(
    ${PUBLICHEADER_DIR}
    ${PUBLICHEADER_DIR}/Horde3D
    ${PUBLICHEADER_DIR}/Lua
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_subdirectory(EditorLib)
#add_subdirectory(QPropertyEditor)
add_subdirectory(QXmlTree)
add_subdirectory(SceneEditorCore)
add_subdirectory(SceneEditor)

# Plugins
add_subdirectory(tgaPlugIn)
add_subdirectory(psdPlugIn)
add_subdirectory(QTerrainNodeExtension)
add_subdirectory(TerrainToolPlugIn)
add_subdirectory(EngineSpec)

