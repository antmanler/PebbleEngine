
#pragma once

#include <vector>

#include <Engine/EngineSDK.h>
#include <btBulletDynamicsCommon.h>

namespace Pebble {


/// Helper struct for loading collision objects
struct CollisionShape
{
    enum Type {Box, Sphere, Mesh, Unknown};
    Type type;
    float mass;
    bool kinematic;

    union {
        float extents[3];
        float radius;
    };

    CollisionShape() : type(Unknown), mass(0.0f), kinematic(false) {}
};

class BulletPhysics : public DefaultComponent<IBehavior>, public has_slots<>
{
public:
    BulletPhysics(IEngineModules *eg_modules, btDynamicsWorld* physics_world);
    virtual ~BulletPhysics ();
    BulletPhysics(const BulletPhysics& object);

    // implementations
    EG_IMPLEMENTS
    virtual const char* EGCALL typeName() const {return COM_TYPE_NAME(BulletPhysics);}
    virtual void EGCALL init();
    virtual void EGCALL start();
    virtual void EGCALL update();
    // virtual const char* EGCALL saveToXml();
    virtual void EGCALL loadFromXml(const char* buf, uint32 size);
    //virtual bool EGCALL processMessage(const char *evt, void* pparam);

    // property system
    virtual uint32 EGCALL queryPropertyCount() const { return 0u;}
    virtual const char* EGCALL queryPropertyNameOf(uint32 idx);
    virtual Property EGCALL getProperty(const char* name) const;
    virtual Property EGCALL getProperty(uint32 idx) const;
    virtual void EGCALL setProperty(const char* name, Property prop);
    virtual void EGCALL setProperty(uint32 idx, Property prop);

    // response for reset signal
    void reset() ;

private :

    bool createCollisionShape();

protected:
    IEngineModules* eg_modules_;
    CollisionShape shape_;
    // physics world
    btDynamicsWorld*  m_physicsWorld;
    /// Motion state for dynamic objects
    btDefaultMotionState* m_motionState;
    /// The main rigid body physics object
    btRigidBody* m_rigidBody;
    /// The collision shape used for the physics engine
    btCollisionShape* m_collisionShape;
    /// Triangle Collision Mesh, allocated only if the collision shape is of type Mesh
    btTriangleMesh* m_btTriangleMesh;
    /// ID within the Horde3D scenegraph
    int	horde_id_;
};

/**
     * \brief Horde3D Attachement Node for Physics
     */
class PhysicsNode
{
    friend class Physics;

public:
    /**
         * Constructor
         * @param shape information data about the collision shape
         * @param meshNodeID id of the mesh node needed in case the collision shape is of type mesh
         */
    PhysicsNode( CollisionShape shape, int hordeID);
    /// Destructor
    virtual ~PhysicsNode();

    void reset();

    /**
         * Will be called by Physics to update the transformation of the node this node is
         * attached to. The transformation calculated by the physics engine will be transfered to
         * this node.
         */
    void update();

private:

    /// Motion state for dynamic objects
    btDefaultMotionState*			m_motionState;
    /// The main rigid body physics object
    btRigidBody*					m_rigidBody;
    /// The collision shape used for the physics engine
    btCollisionShape*				m_collisionShape;
    /// Triangle Collision Mesh, allocated only if the collision shape is of type Mesh
    btTriangleMesh*					m_btTriangleMesh;
    /// needed to avoid strange effects when physics transformation get updated from the outside
    bool							m_selfUpdate;
    /// ID within the Horde3D scenegraph
    int								m_hordeID;
};

/**
     * \brief Class for managing physics world
     *
     * This class creates the btDynamicsWorld instance that will handle all physic objects
     * It is also responsible for calling the update method of each physics object
     */
class Physics
{

public:
    /**
         * Returns a singleton instance of the Physics Manager
         */
    static Physics* instance();

    /**
         * Deletes the singleton instance of the Physics Manager
         */
    static void release();

    /**
         * Calling this method will update the physic world state and their objects' transformations
         */
    void render();

    /**
         * Adds a node to the world
         * @param node pointer to a phyiscs node
         */
    void addNode(PhysicsNode* node);

    /**
         * Removes the node from the world (does not delete the instance)
         * @param node pointer to the instance that should be removed
         */
    void removeNode( PhysicsNode* node );


    /**
         * Resets the world
         */
    void reset();

    /**
         * Clear the world
         */
    void clear();

    /**
         * Static function that will be called by the Horde3D engine when a physics attachment has been found
         * @param hordeID the id of the Horde3D node the attachment node was attached to
         * @param xmlText code of the attachment node
         */
    static void createPhysicsNode( int hordeID, const char *xmlText);

    /**
         * Static function that has to be called when a node with a physics attachment has been removed from the scene graph
         * @param hordeID the id of the Horde3D node
         */
    static void removePhysicsNode( int hordeID);

private:
    /// Private constructor (Singleton)
    Physics();
    /// Private destructor
    ~Physics();

private:

    btDynamicsWorld*  m_physicsWorld;
    // Default Collision Configuration (TODO: what can be configured using this variable?)
    btDefaultCollisionConfiguration*   m_configuration;
    btCollisionDispatcher*  m_dispatcher;
    btBroadphaseInterface*  m_pairCache;
    btConstraintSolver*  m_constraintSolver;
    btClock*  m_clock;
    std::vector<PhysicsNode*> m_physicsNodes;

    static Physics* m_instance;
};


}
