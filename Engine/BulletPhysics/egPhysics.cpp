// *************************************************************************************************
//
// Bullet Physics Integration into Horde3D 
// --------------------------------------
// Copyright (C) 2007 Volker Wiendl
//
// Updated to Horde3D v1.0 beta4 by Afanasyev Alexei and Giatsintov Alexander
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// *************************************************************************************************

#include <algorithm>
#include <Horde3D/Horde3D.h>
//#include <Horde3D/Horde3DTerrain.h>
#include <Engine/utMath.h>
#include <Engine/utXML.h>

#include "egPhysics.h"

using namespace std;

namespace
{
using namespace Pebble;
Property  make_property(Property::List type, const void *val)
{
    Property prop = {type, val};
    return prop;
}

Property gDefaultProperty = {Property::PT_UNKNOWN, (0x0)};

}

namespace Pebble {

BulletPhysics::BulletPhysics(IEngineModules *eg_modules, btDynamicsWorld *physics_world)
    : eg_modules_(eg_modules)
    , m_physicsWorld(physics_world)
    , m_rigidBody(0x0)
    , m_motionState(0x0)
    , m_collisionShape(0x0)
    , m_btTriangleMesh(0x0)
    , horde_id_(0)
{
}

BulletPhysics::~BulletPhysics ()
{
    // remove from physics world
    if (m_rigidBody) {
        m_physicsWorld->removeRigidBody(m_rigidBody);
        delete m_rigidBody;
    }
    if (m_motionState) delete m_motionState;
    if (m_collisionShape) delete m_collisionShape;
    if (m_btTriangleMesh) delete m_btTriangleMesh;

    //    eg_modules_->sigReset()->Disconnect(this, &BulletPhysics::reset);
    eg_modules_->sigReset()->disconnect(this);
}

BulletPhysics::BulletPhysics(const BulletPhysics& object)
{
    // perform shadow copy
    *this = object;
}

// implementations
void EGCALL BulletPhysics::init()
{
    //    eg_modules_->sigReset()->Connect(this, &BulletPhysics::reset);
    eg_modules_->sigReset()->connect(make_slot(this, &BulletPhysics::reset));
}

void EGCALL BulletPhysics::start()
{
    if (!owner()) return;
    // get horde id
    IComponent * com = owner()->getComponent("Object3D");
    if (com) {
        IBehavior *obj = static_cast<IBehavior*>(com);
        Property prop = obj->getProperty(0u);
        horde_id_ = *(static_cast<const int*>(prop.val));
    }
    if (horde_id_ != 0 && shape_.type != CollisionShape::Unknown && createCollisionShape()) {
        m_physicsWorld->addRigidBody(m_rigidBody);
        reset();
    }
}

void EGCALL BulletPhysics::update()
{
    if ( 0 == horde_id_ || shape_.type == CollisionShape::Unknown || 0x0 == m_motionState) return;

    btTransform transformation;
    m_motionState->getWorldTransform(transformation);

    float x[16];
    transformation.getBasis().scaled(m_collisionShape->getLocalScaling()).getOpenGLSubMatrix(x);
    x[12] = transformation.getOrigin().x();
    x[13] = transformation.getOrigin().y();
    x[14] = transformation.getOrigin().z();
    x[15] = 1.0f;

    const float* parentMat = 0;
    // since the physics transformation is absolute we have to create a relative transformation matrix for Horde3D
    h3dGetNodeTransMats(h3dGetNodeParent(horde_id_), 0, &parentMat);
    if (parentMat) h3dSetNodeTransMat(horde_id_, (Matrix4f(parentMat).inverted() * Matrix4f(x)).x);

    h3dCheckNodeTransFlag( horde_id_, true );
}

//const char* EGCALL BulletPhysics::saveToXml()
//{
//    // TODO : fix this
//    return "<BulletPhysics />";
//}

void EGCALL BulletPhysics::loadFromXml(const char* buf, uint32 size)
{
    XMLDoc doc;
    doc.parseBuffer( buf, size );
    if( doc.hasError() ) {
        eg_modules_->engineLogger()->writeError("BulletPhysics::loadFromXml(): parsing failed, %d@%s", __LINE__, __FILE__);
        return;
    }

    XMLNode rootNode = doc.getRootNode();
    if ( !rootNode.isEmpty() && (strcmp(rootNode.getName(), "BulletPhysics") == 0) ) {
        CollisionShape collisionShape;
        const char* shape = rootNode.getAttribute("shape", "mesh");
        if ( shape && strcmp(shape, "box") == 0 ) {
            collisionShape.type = CollisionShape::Box;
            collisionShape.extents[0] =  static_cast<float>(atof(rootNode.getAttribute("x")));
            collisionShape.extents[1] =  static_cast<float>(atof(rootNode.getAttribute("y")));
            collisionShape.extents[2] =  static_cast<float>(atof(rootNode.getAttribute("z")));

        } else if ( shape && strcmp(shape, "sphere") == 0 ) {
            collisionShape.type = CollisionShape::Sphere;
            collisionShape.radius =  static_cast<float>(atof(rootNode.getAttribute("radius")));

        } else {
            collisionShape.type = CollisionShape::Mesh;
        }

        const char* mass = rootNode.getAttribute("mass", "0.0");
        collisionShape.mass = static_cast<float>(atof(mass));
        const char* kinematic = rootNode.getAttribute("kinematic", "false");
        collisionShape.kinematic = strcmp( kinematic, "true" ) == 0 || strcmp( kinematic, "1" ) == 0;

        shape_ = collisionShape;
    } else {
        eg_modules_->engineLogger()->writeError("BulletPhysics::loadFromXml(): "
                                                "it's not a valid physics data, %d@%s", __LINE__, __FILE__);
    }
}

// property system
const char* EGCALL BulletPhysics::queryPropertyNameOf(uint32 idx)
{
    return "";
}

Property EGCALL BulletPhysics::getProperty(const char* name) const
{
    return gDefaultProperty;
}

Property BulletPhysics::getProperty(uint32 idx) const
{
    return gDefaultProperty;
}

void EGCALL BulletPhysics::setProperty(const char* name, Property prop)
{
}

void EGCALL BulletPhysics::setProperty(uint32 idx, Property prop)
{
}

void BulletPhysics::reset()
{
    if (m_rigidBody && m_motionState) {
        m_motionState->setWorldTransform(m_motionState->m_startWorldTrans);
        m_rigidBody->setWorldTransform( m_motionState->m_startWorldTrans );
        m_rigidBody->setInterpolationWorldTransform( m_motionState->m_startWorldTrans );

        if( !m_rigidBody->isStaticObject() ) {
            m_rigidBody->setLinearVelocity(btVector3(0,0,0));
            m_rigidBody->setAngularVelocity(btVector3(0,0,0));
            m_rigidBody->activate();
        }
    }
}

bool BulletPhysics::createCollisionShape()
{

    switch (shape_.type) {
    case CollisionShape::Box: // Bounding Box Shape
        m_collisionShape = new btBoxShape(btVector3(shape_.extents[0], shape_.extents[1], shape_.extents[2]));
        break;

    case CollisionShape::Sphere: // Sphere Shape
        m_collisionShape = new btSphereShape(shape_.radius);
        break;

    case CollisionShape::Mesh: // Mesh Shape
        {
            H3DRes geoResource = 0;
            int vertexOffset = 0, indexOffset = 0;
            unsigned int numVertices = 0, numTriangleIndices = 0, vertRStart = 0;

            switch(h3dGetNodeType(horde_id_)) {
            case H3DNodeTypes::Mesh:
                geoResource = h3dGetNodeParamI(h3dGetNodeParent(horde_id_), H3DModel::GeoResI);
                numVertices = h3dGetNodeParamI(horde_id_, H3DMesh::VertREndI) - h3dGetNodeParamI(horde_id_, H3DMesh::VertRStartI) + 1;
                numTriangleIndices = h3dGetNodeParamI(horde_id_, H3DMesh::BatchCountI);
                vertRStart = h3dGetNodeParamI(horde_id_, H3DMesh::VertRStartI);
                vertexOffset = vertRStart * 3;
                indexOffset = h3dGetNodeParamI(horde_id_, H3DMesh::BatchStartI);
                break;

            case H3DNodeTypes::Model:
                geoResource = h3dGetNodeParamI(horde_id_, H3DModel::GeoResI);
                numVertices = h3dGetResParamI(geoResource,
                                              H3DGeoRes::GeometryElem,
                                              0, // not useable
                                              H3DGeoRes::GeoVertexCountI);
                numTriangleIndices = h3dGetResParamI(geoResource,
                                                     H3DGeoRes::GeometryElem,
                                                     0, // not useable
                                                     H3DGeoRes::GeoIndexCountI);
                break;
#if 0
            case H3DEXT_NodeType_Terrain:
                geoResource = h3dextCreateTerrainGeoRes(horde_id_,
                                                        h3dGetNodeParamStr( horde_id_, H3DNodeParams::NameStr ),
                                                        h3dGetNodeParamF( horde_id_, H3DEXTTerrain::MeshQualityF, 0) );
                if (0 == geoResource) {
                    geoResource = h3dFindResource(H3DResTypes::Geometry, h3dGetNodeParamStr( horde_id_, H3DNodeParams::NameStr ));
                    if(0 == geoResource) {
                        // failed to construct mesh from a terrain node
                        return false;
                    }
                }
                numVertices = h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoVertexCountI);
                numTriangleIndices = h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexCountI);
                break;
#endif
            }

            float* vertexBase = (float*) h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoVertPosStream, true, false);
            h3dUnmapResStream(geoResource);

            if( vertexBase ) vertexBase += vertexOffset;

            unsigned int* TriangleBase32 = NULL;
            unsigned short* TriangleBase16 = NULL;

            //Triangle indices, must cope with 16 bit and 32 bit
            if (h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndices16I)) {
                unsigned short* tb = (unsigned short*)h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexStream, true, false);
                TriangleBase16 = new unsigned short[ numTriangleIndices ];
                memcpy(TriangleBase16, tb+indexOffset, sizeof(unsigned short)*numTriangleIndices);

                h3dUnmapResStream(geoResource);


            } else {
                unsigned int* tb = (unsigned int*)h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexStream, true, false);
                TriangleBase32 = new unsigned int[numTriangleIndices];
                memcpy(TriangleBase32, tb+indexOffset, sizeof(unsigned int) * numTriangleIndices);
                h3dUnmapResStream(geoResource);

            }

            if(	vertexBase && (TriangleBase32 || TriangleBase16)) {
                // Create new mesh in physics engine
                m_btTriangleMesh = new btTriangleMesh();
                int offset = 3;

                // copy mesh from graphics to physics
                bool index16 = false;
                if (TriangleBase16) index16 = true;

                for (unsigned int i = 0; i < numTriangleIndices - 2; i+=offset) {
                    unsigned int index1 = index16 ? (TriangleBase16[i]   - vertRStart) * 3 : (TriangleBase32[i]   - vertRStart) * 3;
                    unsigned int index2 = index16 ? (TriangleBase16[i+1] - vertRStart) * 3 : (TriangleBase32[i+1] - vertRStart) * 3;
                    unsigned int index3 = index16 ? (TriangleBase16[i+2] - vertRStart) * 3 : (TriangleBase32[i+2] - vertRStart) * 3;

                    m_btTriangleMesh->addTriangle(
                                btVector3(vertexBase[index1], vertexBase[index1+1], vertexBase[index1+2] ),
                                btVector3(vertexBase[index2], vertexBase[index2+1], vertexBase[index2+2] ),
                                btVector3(vertexBase[index3], vertexBase[index3+1], vertexBase[index3+2] )
                                );
                }

                bool useQuantizedAabbCompression = true;
                if (shape_.mass > 0)
                    // You can use GImpact or convex decomposition of bullet to handle more complex meshes
                    m_collisionShape = new btConvexTriangleMeshShape(m_btTriangleMesh);
                else // BvhTriangleMesh can be used only for static objects
                    m_collisionShape = new btBvhTriangleMeshShape(m_btTriangleMesh,useQuantizedAabbCompression);
            } else {
                if ( TriangleBase16 ) delete TriangleBase16;
                if ( TriangleBase32 ) delete TriangleBase32;
                eg_modules_->engineLogger()->writeError("BulletPhysics: The mesh data for the physics representation couldn't be retrieved\n");
                shape_.type = CollisionShape::Unknown;
                return false;
            }

            if ( TriangleBase16 ) delete TriangleBase16;
            if ( TriangleBase32 ) delete TriangleBase32;
        }
    }

    if( m_collisionShape ) {
        // Create initial transformation without scale
        const float* x = 0;
        h3dGetNodeTransMats(horde_id_, 0, &x);
        Matrix4f objTrans( x );
        Vec3f t, r, s;
        objTrans.decompose(t, r, s);
        objTrans.scale( 1.0f / s.x, 1.0f / s.y, 1.0f / s.z );

        btTransform tr;
        tr.setFromOpenGLMatrix( objTrans.x );
        // Set local scaling in collision shape because Bullet does not support scaling in the world transformation matrices
        m_collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));
        btVector3 localInertia(0, 0, 0);

        // Rigidbody is dynamic if and only if mass is non zero otherwise static
        if (shape_.mass != 0)
            m_collisionShape->calculateLocalInertia(shape_.mass, localInertia);
        if (shape_.mass != 0 || shape_.kinematic)
            //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
            m_motionState = new btDefaultMotionState(tr);

        btRigidBody::btRigidBodyConstructionInfo rbInfo(shape_.mass, m_motionState, m_collisionShape, localInertia);
        rbInfo.m_startWorldTransform = tr;

        m_rigidBody = new btRigidBody(rbInfo);
        // TODO: is this needed?
        m_rigidBody->setUserPointer(this);
        m_rigidBody->setDeactivationTime(2.0f);

        // Add support for collision detection if mass is zero but kinematic is explicitly enabled
        if( shape_.kinematic && shape_.mass == 0 ) {
            m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
        }
        return true;
    }
    return false;
}

PhysicsNode::PhysicsNode(CollisionShape shape, int hordeID) :
    m_motionState(0), m_rigidBody(0), m_collisionShape(0), m_btTriangleMesh(0), m_selfUpdate(false), m_hordeID(hordeID)
{
    switch (shape.type) {
    case CollisionShape::Box: // Bounding Box Shape
        m_collisionShape = new btBoxShape(btVector3(shape.extents[0],
                                                    shape.extents[1],
                                                    shape.extents[2]));
        break;

    case CollisionShape::Sphere: // Sphere Shape
        m_collisionShape = new btSphereShape(shape.radius);
        break;

    case CollisionShape::Mesh: // Mesh Shape
        {
            H3DRes geoResource = 0;
            int vertexOffset = 0, indexOffset = 0;
            unsigned int numVertices = 0, numTriangleIndices = 0, vertRStart = 0;

            switch(h3dGetNodeType(m_hordeID)) {
            case H3DNodeTypes::Mesh:
                geoResource = h3dGetNodeParamI(h3dGetNodeParent(m_hordeID), H3DModel::GeoResI);
                numVertices = h3dGetNodeParamI(m_hordeID, H3DMesh::VertREndI) - h3dGetNodeParamI(m_hordeID, H3DMesh::VertRStartI) + 1;
                numTriangleIndices = h3dGetNodeParamI(m_hordeID, H3DMesh::BatchCountI);
                vertRStart = h3dGetNodeParamI(m_hordeID, H3DMesh::VertRStartI);
                vertexOffset = vertRStart * 3;
                indexOffset = h3dGetNodeParamI(m_hordeID, H3DMesh::BatchStartI);
                break;

            case H3DNodeTypes::Model:
                geoResource = h3dGetNodeParamI(m_hordeID, H3DModel::GeoResI);
                numVertices = h3dGetResParamI(geoResource,
                                              H3DGeoRes::GeometryElem,
                                              0, // not useable
                                              H3DGeoRes::GeoVertexCountI);
                numTriangleIndices = h3dGetResParamI(geoResource,
                                                     H3DGeoRes::GeometryElem,
                                                     0, // not useable
                                                     H3DGeoRes::GeoIndexCountI);
                break;
#if 0
            case H3DEXT_NodeType_Terrain:
                geoResource = h3dextCreateTerrainGeoRes(m_hordeID,
                                                        h3dGetNodeParamStr( m_hordeID, H3DNodeParams::NameStr ),
                                                        h3dGetNodeParamF( m_hordeID, H3DEXTTerrain::MeshQualityF, 0) );
                numVertices = h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoVertexCountI);
                numTriangleIndices = h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexCountI);
                break;
#endif
            }

            float* vertexBase = (float*) h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoVertPosStream, true, false);
            h3dUnmapResStream(geoResource);

            if( vertexBase ) vertexBase += vertexOffset;

            unsigned int* TriangleBase32 = NULL;
            unsigned short* TriangleBase16 = NULL;

            //Triangle indices, must cope with 16 bit and 32 bit
            if (h3dGetResParamI(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndices16I)) {
                unsigned short* tb = (unsigned short*)h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexStream, true, false);
                TriangleBase16 = new unsigned short[ numTriangleIndices ];
                memcpy(TriangleBase16, tb+indexOffset, sizeof(unsigned short)*numTriangleIndices);

                h3dUnmapResStream(geoResource);

                //				delete TriangleBase32;

            } else {
                unsigned int* tb = (unsigned int*)h3dMapResStream(geoResource, H3DGeoRes::GeometryElem, 0, H3DGeoRes::GeoIndexStream, true, false);
                TriangleBase32 = new unsigned int[numTriangleIndices];
                memcpy(TriangleBase32, tb+indexOffset, sizeof(unsigned int) * numTriangleIndices);

                h3dUnmapResStream(geoResource);

                //				delete TriangleBase16;
            }


            if(	vertexBase && (TriangleBase32 || TriangleBase16)) {
                // Create new mesh in physics engine
                m_btTriangleMesh = new btTriangleMesh();
                int offset = 3;
                //if (triangleMode == 5) // Triangle Strip
                //	offset = 1;

                // copy mesh from graphics to physics
                bool index16 = false;
                if (TriangleBase16) index16 = true;

                for (unsigned int i = 0; i < numTriangleIndices - 2; i+=offset) {
                    unsigned int index1 = index16 ? (TriangleBase16[i]   - vertRStart) * 3 : (TriangleBase32[i]   - vertRStart) * 3;
                    unsigned int index2 = index16 ? (TriangleBase16[i+1] - vertRStart) * 3 : (TriangleBase32[i+1] - vertRStart) * 3;
                    unsigned int index3 = index16 ? (TriangleBase16[i+2] - vertRStart) * 3 : (TriangleBase32[i+2] - vertRStart) * 3;

                    m_btTriangleMesh->addTriangle(
                                btVector3(vertexBase[index1], vertexBase[index1+1], vertexBase[index1+2] ),
                                btVector3(vertexBase[index2], vertexBase[index2+1], vertexBase[index2+2] ),
                                btVector3(vertexBase[index3], vertexBase[index3+1], vertexBase[index3+2] )
                                );
                }

                bool useQuantizedAabbCompression = true;
                if (shape.mass > 0)
                    // You can use GImpact or convex decomposition of bullet to handle more complex meshes
                    m_collisionShape = new btConvexTriangleMeshShape(m_btTriangleMesh);
                else // BvhTriangleMesh can be used only for static objects
                    m_collisionShape = new btBvhTriangleMeshShape(m_btTriangleMesh,useQuantizedAabbCompression);
            } else {
                printf("The mesh data for the physics representation couldn't be retrieved\n");
                return;
            }

            if ( TriangleBase16 ) delete TriangleBase16;
            if ( TriangleBase32 ) delete TriangleBase32;
        }
    }

    if( m_collisionShape ) {
        // Create initial transformation without scale
        const float* x = 0;
        h3dGetNodeTransMats(m_hordeID, 0, &x);
        Matrix4f objTrans( x );
        Vec3f t, r, s;
        objTrans.decompose(t, r, s);

        objTrans.scale( 1.0f / s.x, 1.0f / s.y, 1.0f / s.z );

        btTransform tr;
        tr.setFromOpenGLMatrix( objTrans.x );
        // Set local scaling in collision shape because Bullet does not support scaling in the world transformation matrices
        m_collisionShape->setLocalScaling(btVector3(s.x,s.y,s.z));
        btVector3 localInertia(0,0,0);

        // Rigidbody is dynamic if and only if mass is non zero otherwise static
        if ( shape.mass != 0)
            m_collisionShape->calculateLocalInertia( shape.mass,localInertia );
        if (shape.mass != 0 || shape.kinematic)
            //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
            m_motionState = new btDefaultMotionState(tr);

        btRigidBody::btRigidBodyConstructionInfo rbInfo( shape.mass,m_motionState,m_collisionShape,localInertia);
        rbInfo.m_startWorldTransform = tr;

        m_rigidBody = new btRigidBody(rbInfo);
        m_rigidBody->setUserPointer(this);
        m_rigidBody->setDeactivationTime(2.0f);

        // Add support for collision detection if mass is zero but kinematic is explicitly enabled
        if( shape.kinematic && shape.mass == 0 ) {
            m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
        }
    }
}

PhysicsNode::~PhysicsNode()
{
    Physics::instance()->removeNode(this);
    delete m_rigidBody;
    delete m_motionState;
    delete m_collisionShape;
    delete m_btTriangleMesh;
}

void PhysicsNode::reset()
{
    if (m_rigidBody && m_motionState) {
        m_motionState->setWorldTransform(m_motionState->m_startWorldTrans);
        m_rigidBody->setWorldTransform( m_motionState->m_startWorldTrans );
        m_rigidBody->setInterpolationWorldTransform( m_motionState->m_startWorldTrans );

        if( !m_rigidBody->isStaticObject() ) {
            m_rigidBody->setLinearVelocity(btVector3(0,0,0));
            m_rigidBody->setAngularVelocity(btVector3(0,0,0));
            m_rigidBody->activate();
        }
    }
}

void PhysicsNode::update()
{
    btTransform transformation;
    m_motionState->getWorldTransform(transformation);

    float x[16];
    transformation.getBasis().scaled(m_collisionShape->getLocalScaling()).getOpenGLSubMatrix(x);
    x[12] = transformation.getOrigin().x();
    x[13] = transformation.getOrigin().y();
    x[14] = transformation.getOrigin().z();
    x[15] = 1.0f;

    const float* parentMat = 0;
    // since the physics transformation is absolute we have to create a relative transformation matrix for Horde3D
    h3dGetNodeTransMats(h3dGetNodeParent(m_hordeID), 0, &parentMat);
    if (parentMat)
        h3dSetNodeTransMat(m_hordeID,
                           (Matrix4f(parentMat).inverted() * Matrix4f(x)).x);

    h3dCheckNodeTransFlag( m_hordeID, true );

}


Physics* Physics::m_instance = 0;

Physics* Physics::instance()
{
    if (!m_instance)
        m_instance = new Physics();
    return m_instance;
}

void Physics::release()
{
    delete m_instance;
    m_instance = 0;
}

Physics::Physics()
{
    m_clock = new btClock();
    m_configuration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_configuration);
    btVector3 worldMin(-1000, -1000, -1000);
    btVector3 worldMax(1000, 1000, 1000);
    m_pairCache = new btAxisSweep3(worldMin,worldMax);
    m_constraintSolver = new btSequentialImpulseConstraintSolver();
    m_physicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_pairCache,m_constraintSolver, m_configuration);
    m_physicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

Physics::~Physics()
{
    delete m_physicsWorld;
    delete m_pairCache;
    delete m_constraintSolver;
    delete m_dispatcher;
    delete m_configuration;
    delete m_clock;
    m_instance = 0;
}

void Physics::reset()
{
    m_clock->reset();
    int numObjects = m_physicsWorld->getNumCollisionObjects();
    for (int i=0;i<numObjects;i++) {
        btCollisionObject* colObj = m_physicsWorld->getCollisionObjectArray()[i];
        ((PhysicsNode*)((btCollisionObject*) colObj)->getUserPointer())->reset();
    }
}

void Physics::clear()
{
    reset() ;
    int numObjects = instance()->m_physicsWorld->getNumCollisionObjects();
    while (numObjects > 0) {
        btCollisionObject* colObj = instance()->m_physicsWorld->getCollisionObjectArray()[0];
        PhysicsNode* nodePtr = ((PhysicsNode*)((btCollisionObject*) colObj)->getUserPointer());
        delete nodePtr;
        numObjects = instance()->m_physicsWorld->getNumCollisionObjects();
    }
}

void Physics::render()
{
    float dt = m_clock->getTimeMicroseconds() * 0.00001f;
    m_clock->reset();

    m_physicsWorld->stepSimulation(dt);

    vector<PhysicsNode*>::iterator iter = m_physicsNodes.begin();
    while ( iter!= m_physicsNodes.end() ) {
        (*iter)->update();
        ++iter;
    }
}

void Physics::addNode(PhysicsNode* node)
{
    vector<PhysicsNode*>::iterator iter = find(m_physicsNodes.begin(), m_physicsNodes.end(), node);
    if (iter == m_physicsNodes.end()) {
        m_physicsWorld->addRigidBody(node->m_rigidBody);
        // add it to the object vector only if it is dynamic
        if (node->m_motionState) m_physicsNodes.push_back(node);
    }
}

void Physics::removeNode(PhysicsNode* node)
{
    // Remove from dynamics physics world
    if (node) {
        m_physicsWorld->removeRigidBody(node->m_rigidBody);
        // remove from Physics
        if (node->m_motionState) {
            vector<PhysicsNode*>::iterator iter = find(m_physicsNodes.begin(), m_physicsNodes.end(), node);
            if( iter != m_physicsNodes.end() ) m_physicsNodes.erase(iter);
        }
    }
}

void Physics::createPhysicsNode( int hordeID, const char *xmlText)
{
    XMLDoc doc;
    doc.parseString(const_cast<char*>(xmlText));
    if( doc.hasError() ) {

    }
    XMLNode xmlNode = doc.getRootNode() ;
    if( !xmlNode.isEmpty() && strcmp(xmlNode.getName(), "Attachment") == 0 ) {
        XMLNode physicsNode;
        const char* type = xmlNode.getAttribute("type", "");
        if ( strcmp(type, "GameEngine")==0 &&
             !( physicsNode = xmlNode.getFirstChild("BulletPhysics") ).isEmpty() ) {

            CollisionShape collisionShape;
            const char* shape = physicsNode.getAttribute("shape");
            if (shape && strcmp(shape, "box")==0) {
                collisionShape.type = CollisionShape::Box;
                collisionShape.extents[0] =  static_cast<float>(atof(physicsNode.getAttribute("x")));
                collisionShape.extents[1] =  static_cast<float>(atof(physicsNode.getAttribute("y")));
                collisionShape.extents[2] =  static_cast<float>(atof(physicsNode.getAttribute("z")));

            } else if (shape && strcmp(shape, "sphere")==0) {
                collisionShape.type = CollisionShape::Sphere;
                collisionShape.radius =  static_cast<float>(atof(physicsNode.getAttribute("radius")));

            } else {
                collisionShape.type = CollisionShape::Mesh;

            }

            const char* mass = physicsNode.getAttribute("mass", "0.0");
            collisionShape.mass = static_cast<float>(atof(mass));
            const char* kinematic = physicsNode.getAttribute("kinematic", "false");
            collisionShape.kinematic = strcmp( kinematic, "true" ) == 0 || strcmp( kinematic, "1" ) == 0;

            // create new physicsnode: livetime of the node instance will be controlled by the Physics instance
            PhysicsNode* physicsNode = new PhysicsNode(collisionShape, hordeID);
            if (physicsNode->m_rigidBody == 0) delete physicsNode;
            else instance()->addNode(physicsNode);
        }
    }
#ifdef PEBBLE_DEBUG
    // 	else if( !xmlNode.isEmpty() )
    // 		printf("Error reading XmlNode: %s", XMLNode::getError(results.error));
#endif
}

void Physics::removePhysicsNode( int node )
{
    int numObjects = instance()->m_physicsWorld->getNumCollisionObjects();
    for (int i = 0; i < numObjects; ++i) {

        btCollisionObject* colObj = instance()->m_physicsWorld->getCollisionObjectArray()[i];
        PhysicsNode* nodePtr = ((PhysicsNode*)((btCollisionObject*) colObj)->getUserPointer());
        if( nodePtr && nodePtr->m_hordeID == node ) {
            delete nodePtr;
            break;
        }
    }
}

#ifndef DLLEXP
#	if defined( WIN32 ) || defined( _WINDOWS )
#		define DLLEXP extern "C" __declspec( dllexport )
#	else
#		if defined( __GNUC__ ) && __GNUC__ >= 4
#		  define DLLEXP extern "C" __attribute__ ((visibility("default")))
#   	else
#		  define DLLEXP extern "C"
#   	endif
#	endif
#endif



DLLEXP void h3dextInitPhysics()
{
    Physics::instance();
}

DLLEXP void h3dextReleasePhysics()
{
    Physics::release();
}

DLLEXP void h3dextUpdatePhysics()
{
    Physics::instance()->render();
}

DLLEXP void h3dextResetPhysics()
{
    Physics::instance()->reset();
}

DLLEXP void h3dextClearPhysics()
{
    Physics::instance()->clear();
}

DLLEXP void h3dextCreatePhysicsNode( const char* xmlData, int hordeID )
{
    Physics::createPhysicsNode( hordeID, xmlData );
}

DLLEXP void h3dextRemovePhysicsNode( int hordeID )
{
    Physics::removePhysicsNode( hordeID );
}


}
