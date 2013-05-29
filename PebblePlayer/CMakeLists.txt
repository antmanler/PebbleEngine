cmake_minimum_required(VERSION 2.8)

project(PebblePlayer)

include_directories(${PUBLICHEADER_DIR}
    ${PebbleInWater_SOURCE_DIR}/Engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )
# include_directories("./Common")
# include_directories("./EditorCore")

find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL QtMain REQUIRED)
include("${QT_USE_FILE}")

# file(GLOB RR_COMMON_SRC "Common/*.cpp")
# file(GLOB RR_COMMON_INC "Common/*.h")

file(GLOB RR_SRC "*.cpp")
file(GLOB RR_INC "*.h*")

# set(RR_SRC ${RR_SRC} ${RR_COMMON_SRC})
# set(RR_INC ${RR_INC} ${RR_COMMON_INC})

file(GLOB RR_PYS "*.py")
# file(GLOB RR_RC_PYS "Resources/*.py")
set(RR_PYS 
    ${RR_PYS}
    # ${PR_RC_PYS}
)
source_group(scprits FILES ${RR_PYS})

set(MOC_SRC
    # GLPlayerWidget.h
    GLWidget.h
    RenderTargetView.h
    # NodePropertyWidget.h
    # SceneNodeTreeView.h
    # MainWindow.h
)

QT4_WRAP_CPP(gen_moc_sources ${MOC_SRC})
source_group(mocs FILES ${gen_moc_sources})

# SET(gen_rc_sources editor.qrc)
QT4_ADD_RESOURCES(gen_rc_sources Resources/editor.qrc)
source_group(rc FILES ${gen_rc_sources} Resources/editor.qrc)

file(GLOB RR_UIS "*.ui")
QT4_WRAP_UI(gen_ui_sources ${RR_UIS})
source_group(ui FILES ${gen_ui_sources} ${RR_UIS})

if(WIN32)
    add_definitions(-DUNICODE)
endif()

# first build dependences
# add_subdirectory(QPropertyEditor)
# add_subdirectory(EditorCore)

# target
add_executable(PebblePlayer WIN32 MACOSX_BUNDLE
    ${RR_SRC} ${RR_INC} 
    ${gen_moc_sources} ${gen_rc_sources} ${gen_ui_sources}
${RR_PYS}
)

target_link_libraries(PebblePlayer 
    # EditorCore
    # QPropertyEditorPIW
    PythonQt
    Engine
    Horde3D
    Horde3DUtils
    # temp:
    # BulletPhysics
    ${QT_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(APPLICATION_SERVICES_LIBRARY ApplicationServices)
    FIND_LIBRARY(AGL_LIBRARY AGL)
    target_link_libraries(PebblePlayer ${COCOA_LIBRARY} ${APPLICATION_SERVICES_LIBRARY} ${AGL_LIBRARY})
endif()

# copy scipts to runtime directory
CopyFilesTo(PebblePlayer 
    "${RR_PYS}" "" 
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../Data/Scripts/"
)


