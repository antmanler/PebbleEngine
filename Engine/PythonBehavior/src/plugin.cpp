
#include "dPython.h"
#include "PythonBehavior.h"

#if PY_MAJOR_VERSION < 3
PyMODINIT_FUNC initPythonBehavior(void); /*proto*/
PyMODINIT_FUNC initEngine(void); /*proto*/
#else
PyMODINIT_FUNC PyInit_PythonBehavior(void); /*proto*/
PyMODINIT_FUNC PyInit_Engine(void); /*proto*/
#endif

namespace Pebble
{

char gContentDir[128] = {0} ;
IPluginManager *gPluginMan = NULL;

class PythonBehaviorManager : public DefaultComManager, public has_slots<>
{
public:
    PythonBehaviorManager(IPluginManager *manager)
        : manager_(manager)
        , eg_modules_(manager->getModules())
        , inst_(0x0)
        , inited_(true)
        , py_already_init_(true)
    {

        if (!Py_IsInitialized()) {
            Py_SetProgramName(const_cast<char*>("Pebble::PythonBehavior"));
            Py_Initialize();
            py_already_init_ = false;
            if (!Py_IsInitialized()) {
                eg_modules_->engineLogger()->writeError("PythonBehavior: Could not initialize python interpreter!");
                inited_ = false;
            }
        }

        if (!PyEval_ThreadsInitialized()) {
            PyEval_InitThreads();
            if (!PyEval_ThreadsInitialized()) {
                eg_modules_->engineLogger()->writeError("PythonBehavior: Could not init GIL!");
                inited_ = false;
            }
        }

        if (inited_) {
            const char* res_path = eg_modules_->engineConfig()->getConfig("ResourceRepositry")->value();
            sprintf(gContentDir, "%s", res_path);
            eg_modules_->engineLogger()->writeDebugInfo("PythonBehavior: Python interpreter initialised!");
        }

//        eg_modules_->sigParseSysArgv()->Connect(this, &PythonBehaviorManager::parseSysArgv);
        eg_modules_->sigParseSysArgv()->connect(make_slot(this, &PythonBehaviorManager::parseSysArgv));
    }

    void parseSysArgv(int argc, char** argv)
    {
        eg_modules_->engineLogger()->writeDebugInfo("PythonBehavior: Parsing system arguments!");
        if (inited_) {
            PySys_SetArgv(argc, argv);
            // init python wrapper
            initEngine();
            initPythonBehavior();
            eg_modules_->engineLogger()->writeDebugInfo("PythonBehavior: Python wrapper initialized!");
        }
    }

    //implements
    virtual void EGCALL Destroy()
    {
        if (inited_ && !py_already_init_) {
            Py_Finalize();
            eg_modules_->engineLogger()->writeDebugInfo("PythonBehavior: Python interpreter released!");
        }
        if (inst_) {
            manager_->getModules()->engineLogger()->writeDebugInfo("PythonBehavior: Release PythonBehavior instance");
            delete inst_;
        }
//        eg_modules_->sigParseSysArgv()->Disconnect(this, &PythonBehaviorManager::parseSysArgv);
        eg_modules_->sigParseSysArgv()->disconnect(this);
    }

    virtual const char* EGCALL GetFactoryName()  {return COM_TYPE_NAME(PythonBehavior);}

    // Create PythonBehavior
    virtual IExportInterface* EGCALL Create()
    {
        if ( 0x0 == inst_ ) inst_ = new PythonBehavior(manager_);
        inst_->enable(inited_);
        return inst_;
    }

private:
    IPluginManager* manager_;
    IEngineModules *eg_modules_;
    PythonBehavior* inst_;
    bool inited_;
    // flag if interpreter has beed initilised
    bool py_already_init_;

};

// register to component system
DLLEXP void YAPSCALL register_plugin(IPluginManager *man) {
    gPluginMan = man;
    man->registerComponent(new PythonBehaviorManager(man));
}

}
