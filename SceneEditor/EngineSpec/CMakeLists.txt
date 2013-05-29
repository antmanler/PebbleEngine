# build distance field library first
add_subdirectory(dflib)

INCLUDE_DIRECTORIES(
    ".."
    "${CMAKE_CURRENT_BINARY_DIR}/.."
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(QSphNodeExtension)
add_subdirectory(SphToolPlugin)

SET_PROPERTY(TARGET dflib                  PROPERTY FOLDER "SceneEditor/PlugIns/EngineSpec")
SET_PROPERTY(TARGET QSphNodeExtension      PROPERTY FOLDER "SceneEditor/PlugIns/EngineSpec")
SET_PROPERTY(TARGET SphToolPlugIn          PROPERTY FOLDER "SceneEditor/PlugIns/EngineSpec")