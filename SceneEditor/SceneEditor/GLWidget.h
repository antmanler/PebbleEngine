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

#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include <QtOpenGL/QtOpenGL>

class QLabel;
class QTimer;
class QListWidgetItem;
class QXmlTreeNode;
struct QVec3f;
class AttachmentPlugIn;
class TrackballManipulator;

/**
 * Rendering widget creating an OpenGL context for the Horde3D engine. <br>
 */
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    enum TransformationModes {None = 0, MoveObject = 100, RotateObject = 200, ScaleObject = 300};
    enum Axis {X = 1, Y = 2, Z = 4, Local_X = 8, Local_Y = 16, Local_Z = 32};
    enum DebugInformation { DRAW_NO_DEBUG = 0, DRAW_GRID = 1, DRAW_BOUNDING_BOX = 2 };

    GLWidget(QLabel* fpsLabel /*= 0*/, QWidget* parent /*= 0*/, Qt::WFlags flags /*= 0*/);
    GLWidget(GLWidget* shared, QWidget* parent = 0, Qt::WFlags flags = 0);
    virtual ~GLWidget();

    void updateLog();

    /**
         * Returns the node id of the camera used for rendering
         */
    int activeCam() {return m_activeCameraID;}

    /**
         * Sets the node id of the camera used for rendering
         */
    void setActiveCam(int cam);

    /**
         * Set the currently loaded attachment plugin
         */
    void setAttachmentPlugIn(AttachmentPlugIn* plugin) { m_attachmentPlugIn = plugin; }

    bool isInitialized() {return m_initialized;}

	bool hasFullScreenWidget() const {return m_fullScreen != NULL;}

    /**
         * Reloads the mouse button settings
         */
    void loadButtonConfig();

public slots:

    /// Fullscreen Hack to be able to use QGLWidget::showFullScreen()
    void setFullScreen(bool fullsreen, GLWidget* widget = 0);

    /**
         * Increasing the the navigation speed results in a
         * less precise but faster change of the camera transformation
         * @param speed a value between 1 and 100
         */
    void setNavigationSpeed(double speed);

    /**
         * Toggles the collision check for the camera
         * @param check if enabled the camera tries to avoid passing through objects when moving
         */
    void setCollisionCheck(bool check) {m_collisionCheck = check;}

    /**
         * Toggles transformation mode when moving the mouse
         * @param mode mouse mode
         */
    void setTransformationMode(int mode);

    /**
         * Resets the transformation change mode
         * @param accept if set to true the transformation changes done will be applied to the object
         *               if set to false the previous transformation will be restored
         */
    void resetMode(bool accept = false);

    /**
         * Toggles between debug wireframe rendering and normal rendering
         * @param debug if set to true the scene will be rendered in wireframe mode
         */
    void enableDebugView(bool debug);


    /**
         * Sets the current node to allow scaling dependent on the distance between the camera and the selected node
         * @param node the currently select node (or 0 if no node is selected)
         */
    void setCurrentNode(QXmlTreeNode* node);

    /**
         * Toggles Base Grid Rendering
         */
    void setBaseGridEnabled(bool enabled) {if (enabled) m_debugInfo |= DRAW_GRID; else m_debugInfo &= 0xFFFF ^ DRAW_GRID;}

    /**
         * Toggles Bounding Box Rendering
         */
    void setAABBEnabled(bool enabled) {if (enabled) m_debugInfo |= DRAW_BOUNDING_BOX; else m_debugInfo &= 0xFFFF ^ DRAW_BOUNDING_BOX;}

signals:
    /**
         * sends a status bar event
         */
    void statusMessage(const QString& message, int timeout);

    //void mouseMoved(int x, int y);
    /**
         * Notifies connected widgets that the mouse has been used to translate an object
         * @param x - Translation in X-direction in World Coordinate System
         * @param y - Translation in Y-direction in World Coordinate System
         */
    void moveObject(const float x, const float y, const float z);

    /**
         * Notifies connected widgets that the mouse has been used to rotate an object
         */
    void rotateObject(const float x, const float y, const float z);

    /**
         * Notifies connected widgets that the mouse has been used to scale an object
         */
    void scaleObject(const float x, const float y, const float z);

    /**
         * Indicates the current transformation mode
         */
    void transformationMode(int);

    /**
         * A new log message from the Horde Engine is available
         */
    void logMessages(const QList<QListWidgetItem*>&);

    /**
         * A node has been selected with the mouse
         */
    void nodeSelected(int H3DNode);

    /**
         * A signal when resizing the context widget
         */
    void resized(int width, int height);

    /**
         * Signal to indicate that the fullscreen mode has been left
         */
    void fullscreenActive(bool);

protected:

    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

    /// Handles Camera Navigation in Z-Direction
    virtual void wheelEvent( QWheelEvent * event );

    /// Handles Camera Navigation in X,Y,Z Direction
    virtual void keyPressEvent(QKeyEvent* event);

    /// Handles Camera Navigation in X,Y,Z Direction and Fixed Positions Top, Left, Front
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);


private slots:
    /**
         * Slot triggered when shared fullscreen widget has been closed
         */
    void fullScreenClosed();

    /**
         * Slot triggered after mouse wheel has been used to stop the movement
         */
    void stopWheelMove() {m_forward = false; m_backward = false;}

private:

    /**
      * Copy camera paramters from active camera to editor camera
      */
    void copyFromActiveCamera();

    /**
      * Select of unselect the lightcone to represent the light node
      */
    void highlightLightcone(bool select);


    /**
         * Remembers the transformation of the currently set camera
         */
    void cameraNavigationStart();

    /**
         * Changes the transformation of the currently set camera relative to the
         * transformation effective at the begining of the camera move
         * @param x translation in x
         * @param y translation in y
         * @param z translation in z
         * @param rx rotation about x
         * @param ry rotation about y
         */
    void cameraNavigationUpdate(float x, float y, float z, float rx, float ry);

    /**
      * handle mouse button state for trackball controller
      */
    void handleMouseButton(QMouseEvent *event, bool down) ;

    /**
         * Creates a translation for the currently selected object based on the axis limitations set
         * @param x translation in x
         * @param y translation in y
         */
    void translateObject(int x, int y);
    // Creates a rotation for the currently selected object based on the axis limitations set
    void rotateObject(int x, int y);
    // Creates a scale for the currently selected object based on the axis limitations set
    void scaleObject(int x, int y);

    void applyChanges(bool save);

    void renderEditorInfo();
    inline void drawAxis(const QVec3f& start, const QVec3f& dir, const QVec3f& color, const float* modelView = 0);
    inline void drawBoundingBox(unsigned int hordeID);
    inline void drawViewportLine(const QPoint& start, const QPoint& end);
    inline void drawViewportCircle(float x, float y, float radius);

    inline void glGizmo();
    inline void drawGizmo(const float* nodeTransform, const float* cam);
    inline void drawBaseGrid(const float camX, const float camY, const float camZ);

    inline bool inSync();

    void getViewportProjection(const double px, const double py, const double pz, double& vx, double& vy, double& vz, const float* mat = 0);

    /*void drawGizmo(bool selection);*/

    bool                m_glClear;

    QLabel*             m_fpsLabel;
    int                 m_transformationMode;

    bool	            m_collisionCheck;

    float               m_navSpeed;
    QPoint              m_navOrigin;
    float               m_navRx, m_navRy;

    float               m_fps;
    GLWidget*           m_fullScreen;
    const bool          m_shared;

    QTimer*             m_wheelTimer;

    bool                m_forward, m_backward, m_left, m_right, m_up, m_down;

    /// Limiting object movement by mouse to one axis
    int                 m_limitToAxis;
    float               m_axisVpX, m_axisVpY;

    int                 m_gizmoSelection;

    //float			m_transScale;
    /// Delta Transformation for object manipulation
    int                 m_x, m_y;

    int                 m_debugInfo;
    int                 m_gridScale;

    QXmlTreeNode*       m_currentNode;

    AttachmentPlugIn*   m_attachmentPlugIn;

    int                 m_activeCameraID;
    // editor camera to draw some helper stuffs, such as lights
    int                 editor_cam_;

    bool                m_initialized;

    int				    m_cameraMoveButton, m_selectButton, m_resetSelectButton;

    TrackballManipulator* camera_ctrl_;
    bool                no_draw_;
};
#endif
