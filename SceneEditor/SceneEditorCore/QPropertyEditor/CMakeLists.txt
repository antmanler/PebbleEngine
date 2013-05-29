cmake_minimum_required(VERSION 2.8) 

FIND_PACKAGE(Qt4 REQUIRED)

SET(QPropertyEditor_SOURCES
	ColorCombo.cpp
	EnumProperty.cpp
	Property.cpp
	QPropertyEditorWidget.cpp
	QPropertyModel.cpp
	QVariantDelegate.cpp
)
SET(QPropertyEditor_HEADERS
	ColorCombo.h
	EnumProperty.h
	Property.h
	QPropertyEditorWidget.h
	QPropertyModel.h
	QVariantDelegate.h
)

QT4_WRAP_CPP(QPropertyEditor_HEADERS_MOC ${QPropertyEditor_HEADERS})
SET(QT_USE_QTXML TRUE)

# INCLUDE_DIRECTORIES(../../include)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_LIBRARY( QPropertyEditor STATIC
    ${QPropertyEditor_SOURCES} 
	${QPropertyEditor_HEADERS}
    ${QPropertyEditor_HEADERS_MOC}     
)

TARGET_LINK_LIBRARIES( QPropertyEditor ${QT_LIBRARIES} )