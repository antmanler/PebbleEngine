
#include "egPhysics.h"


namespace Pebble
{

class BulletPhysicsManager : public DefaultComManager
{
public:
    BulletPhysicsManager(IPluginManager *manager)
        : manager_(manager)
        , eg_modules_(manager->getModules())
    {
        m_clock = new btClock();
        m_configuration = new btDefaultCollisionConfiguration();
        m_dispatcher = new btCollisionDispatcher(m_configuration);
        btVector3 worldMin(-1000,-1000,-1000);
        btVector3 worldMax(1000,1000,1000);
        m_pairCache = new btAxisSweep3(worldMin,worldMax);
        m_constraintSolver = new btSequentialImpulseConstraintSolver();
        m_physicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_pairCache, m_constraintSolver, m_configuration);
        // get gravity setting
        float gravity = eg_modules_->engineConfig()->getConfig("gravity")->cast<float>();
        m_physicsWorld->setGravity(btVector3(0, gravity, 0));
    }

    //implements
    virtual void EGCALL Destroy()
    {
        delete m_physicsWorld;
        delete m_pairCache;
        delete m_constraintSolver;
        delete m_dispatcher;
        delete m_configuration;
        delete m_clock;
    }

    virtual const char* EGCALL GetFactoryName()  {return COM_TYPE_NAME(BulletPhysics);}

    // Create PythonBehavior
    virtual IExportInterface* EGCALL Create()
    {
        // create a new bullet instance
        return new BulletPhysics(eg_modules_, m_physicsWorld);
    }

    virtual void EGCALL update() const
    {
        float dt = m_clock->getTimeMicroseconds() * 0.00001f;
        m_clock->reset();
        m_physicsWorld->stepSimulation(dt, 5);
    }

private:
    IPluginManager* manager_;
    IEngineModules *eg_modules_;
    btDynamicsWorld*  m_physicsWorld;
    // Default Collision Configuration (TODO: what can be configured using this variable?)
    btDefaultCollisionConfiguration*   m_configuration;
    btCollisionDispatcher*  m_dispatcher;
    btBroadphaseInterface*  m_pairCache;
    btConstraintSolver*  m_constraintSolver;
    btClock*  m_clock;
};

// register to component system
#if defined (PLATFORM_IOS)
    void YAPSCALL BulletPhysics_register_plugin(IPluginManager *man) {
#else
    DLLEXP void YAPSCALL register_plugin(IPluginManager *man) {
#endif
    man->registerComponent(new BulletPhysicsManager(man));
}

}
