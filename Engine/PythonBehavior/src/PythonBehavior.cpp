
#include <vector>
#include <string>
#include <fstream>
#include <iterator> 
#include <algorithm>
#include <Engine/utXML.h>
#include <Engine/rapidxml_print.h>
#include "dPython.h"
#include "PythonBehavior.h"
#include "PythonBehavior_static_api.h"

namespace
{
using namespace std;

void print_pyerr()
{
    // fetch error
    PyObject* pytype = 0, *pyvalue = 0, *pytrace = 0;
    PyErr_Fetch (&pytype, &pyvalue, &pytrace);
    Py_XINCREF  (pytype);
    Py_XINCREF  (pyvalue);
    Py_XINCREF  (pytrace);
    // restore...
    PyErr_Restore (pytype, pyvalue, pytrace);
    // and print
    PyErr_Print();
    PyErr_Clear();
}

string cleanPath( string path )
{
        // Remove spaces at the beginning
        int cnt = 0;
        for( int i = 0; i < (int)path.length(); ++i )
        {
                if( path[i] != ' ' ) break;
                else ++cnt;
        }
        if( cnt > 0 ) path.erase( 0, cnt );

        // Remove slashes, backslashes and spaces at the end
        cnt = 0;
        for( int i = (int)path.length() - 1; i >= 0; --i )
        {
                if( path[i] != '/' && path[i] != '\\' && path[i] != ' ' ) break;
                else ++cnt;
        }

        if( cnt > 0 ) path.erase( path.length() - cnt, cnt );

        return path;
}

}

namespace Pebble
{
using namespace std;

class PyBehaviorCreator : public DefaultComManager, public has_slots<single_threaded>
{

public:
    PyBehaviorCreator(const string& com_name, IPluginManager *manager)
        : com_name_(com_name)
        , manager_(manager)
        , eg_modules_(manager->getModules())
        , code_obj_(NULL)
    {
        eg_modules_->sigStartNewScene()->connect(new _connection0<PyBehaviorCreator, single_threaded>(this, &PyBehaviorCreator::on_new_scene));
    }

    ~PyBehaviorCreator()
    {
        eg_modules_->engineLogger()->writeDebugInfo("Releasing PyBehaviorCreator (%s)", com_name_.c_str());
//        eg_modules_->sigStartNewScene()->disconnect(this);
        if (code_obj_) {
            if ( 0 == import_PythonBehavior() ) {
                char tzbuf[128] = {0};
                sprintf(tzbuf, "%s", com_name_.c_str());
                release_pythonbehavior_module(tzbuf);
            }
            Py_DecRef(code_obj_);
        }
    }

    bool setModule(const char* path) {

        string rep_path = eg_modules_->engineConfig()->getConfig("ResourceRepositry")->value();
        //rep_path = rep_path + string("/") + path;

        string dir;
        vector< string > dirs;

        // Split path string
        const char *c = rep_path.c_str();
        do {
            if( *c != '|' && *c != '\0' ) dir += *c;
            else {
                dir = cleanPath( dir );
                if( dir != "" ) dir += '/';
                dirs.push_back( dir );
                dir = "";
            }
        } while( *c++ != '\0' );

        char *dataBuf = 0;
        int bufSize = 0;

        ifstream inf;
        // Loop over search paths and try to open files
        for( unsigned int i = 0; i < dirs.size(); ++i ) {
            string fileName = dirs[i] +  string("/") + path;
            inf.clear();
            inf.open( fileName.c_str(), ios::binary );
            if( inf.good() ) break;
        }

        // Open resource file
        if( inf.good() ) {
            // Find size of resource file
            inf.seekg( 0, ios::end );
            int fileSize = inf.tellg();
            if( fileSize == 0 ) return false;

            dataBuf = new char[fileSize+1];
            bufSize = fileSize;
            // Copy resource file to memory
            inf.seekg( 0 );
            inf.read(dataBuf, fileSize);
            inf.close();
            dataBuf[fileSize] = '\0';

            PyObject *code = Py_CompileString(dataBuf, path, Py_file_input);
            print_pyerr();
            // clear
            delete []dataBuf;
            if ( NULL == code) return false;
            code_obj_ = code;
            // increase refrence
            Py_IncRef(code_obj_);
            return true;
        }

        return false;
    }

    // implements
    EG_IMPLEMENTS
    IExportInterface* EGCALL Create()
    {
        if (code_obj_) {
            char tzbuf[128] = {0};
            sprintf(tzbuf, "%s", com_name_.c_str());
            IBehavior *bhv = NULL;
            if ( 0 == import_PythonBehavior() ) {
                bhv = create_behavior_from_code(tzbuf, code_obj_);
            }
            print_pyerr();
            return bhv;
        }
        return NULL;
    }

    void EGCALL update() const
    {

    }

    const char* EGCALL GetFactoryName() { return com_name_.c_str(); }

    void on_new_scene()
    {
        eg_modules_->sigStartNewScene()->disconnect_unsafe(this);
        // when start a new scene, remove this factory from previouse comsys
        manager_->removeComponent(this);
    }

private:
    string com_name_;
    IPluginManager *manager_;
    IEngineModules *eg_modules_;
    PyObject *code_obj_;
};


PythonBehavior::PythonBehavior(IPluginManager *manager)
    : manager_(manager)
    , eg_modules_(manager->getModules())
{
}

void EGCALL PythonBehavior::Destroy()
{
}

//const char* EGCALL PythonBehavior::saveToXml()
//{
//	return "";
//}

void EGCALL PythonBehavior::loadFromXml(const char* contentbuf, uint32 size)
{
    XMLDoc doc;
    doc.parseBuffer( contentbuf, size );
    if( doc.hasError() ) {
        eg_modules_->engineLogger()->writeError("PythonBehavior::loadFromXml(): scene parsing failed, %d@%s", __LINE__, __FILE__);
        return;
    }

    XMLNode rootNode = doc.getRootNode();
    if ( !rootNode.isEmpty() && (strcmp(rootNode.getName(), "PythonBehavior") == 0) ) {
        string py_src = rootNode.getAttribute("src", "");
        if ( py_src.empty() ) {
            eg_modules_->engineLogger()->writeError("SceneGraph::loadFromXml(): "
                                                    "it's not a valid python script, %d@%s", __LINE__, __FILE__);
            return;
        }
        size_t beg = py_src.find_last_of('/') + 1;
        size_t end = py_src.rfind('.');

        string mod_name = py_src.substr(beg, end-beg);
        if (!mod_name.empty()) {
            IComponent *pybhv = NULL;
            // already know this type of python behavior
            if(manager_->GetHost(COMPONET_HOST)->HasFactoryOf(mod_name.c_str())) {
                pybhv = manager_->createComponent(mod_name.c_str());
            } else {
                // we need a factory first
                PyBehaviorCreator *creator = new PyBehaviorCreator(mod_name, manager_);
                // set src path
                if (creator->setModule(py_src.c_str()) &&
                    (pybhv = static_cast<IComponent*>(creator->Create()))) {
                    // add to component system
                    manager_->registerComponent(creator);
                } else {
                    delete creator;
                    eg_modules_->engineLogger()->writeError("PythonBehavior::loadFromXml(): "
                                                            "can not create type factory for %s", mod_name.c_str());
                }
            }
            // set up bython behavior
            if (pybhv && owner()->addComponent(pybhv)) {
                // load settings from setup file
                XMLNode xmlNode1 = rootNode.getFirstChild("Settings");
                if (!xmlNode1.isEmpty()) {
                    string set_xml = "";
                    rapidxml::print( back_inserter( set_xml ), *xmlNode1.getRapidXMLNode(), 0 );
					char *txt = new char[set_xml.length()];
					memcpy(txt, set_xml.c_str(), set_xml.length());
                    eg_modules_->engineConfig()->setupBehaviorConfig(reinterpret_cast<IBehavior*>(pybhv),
                                                                     txt, set_xml.length());
					delete []txt;
                }
//                XMLNode node = rootNode.getFirstChild();
//                if (!node.isEmpty()) {
//                    string com_xml = "";
//                    rapidxml::print( back_inserter( com_xml ), *node.getRapidXMLNode(), 0 );
//                    // set serialized data
//                    char *txt = new char[com_xml.length()];
//                    memcpy(txt, com_xml.c_str(), com_xml.length());
//                    pybhv->loadFromXml(txt, com_xml.length());
//                    delete []txt;
//                }
            }
        } else {
            eg_modules_->engineLogger()->writeError("PythonBehavior::loadFromXml(): "
                                                    "it's not a valid script name, %d@%s", __LINE__, __FILE__);
        }

    } else {
        eg_modules_->engineLogger()->writeError("PythonBehavior::loadFromXml(): "
                                                "it's not a valid script data, %d@%s", __LINE__, __FILE__);
    }
    // remove self from owner
    owner()->removeComponent(this, false);
}

namespace {
Property invalid_porp = {Property::PT_UNKNOWN, 0x0};
}

#define Ret_VOID
#define check_proxy(meth, Invalid_ret) \
    if(!((proxy_ && proxy_->pyobj && (proxy_->meth)) || behavior_)) return Invalid_ret

PyBehaviorWrapper::PyBehaviorWrapper(PyBehavior *proxy)
    : behavior_(0x0)
    , proxy_(proxy)
{
    if(proxy && proxy->pyobj) {
        Py_XINCREF(proxy_->pyobj);
    }
}

PyBehaviorWrapper::PyBehaviorWrapper(IBehavior *behavior)
    : behavior_(behavior)
    , proxy_(0x0)
{
    // share owner
    setOwner(behavior->owner());
}

PyBehaviorWrapper::~PyBehaviorWrapper()
{
    if(proxy_) {
		pythonObjRelease();
        Py_XDECREF(proxy_->pyobj);
        delete proxy_;
        proxy_ = 0x0;
    }
}

void EGCALL PyBehaviorWrapper::init()
{
    check_proxy(init, Ret_VOID);
    if(proxy_) {
        proxy_->init(proxy_->pyobj);
    } else {
        behavior_->init();
    }
}



void PyBehaviorWrapper::pythonObjRelease()
{
	check_proxy(release, Ret_VOID);
	if (proxy_){
		proxy_->release(proxy_->pyobj);
	}
	
}

void EGCALL PyBehaviorWrapper::start()
{
    check_proxy(start, Ret_VOID);
    if(proxy_) {
        proxy_->start(proxy_->pyobj);
    } else {
        behavior_->start();
    }
}

void EGCALL PyBehaviorWrapper::update()
{
    check_proxy(update, Ret_VOID);
    if(proxy_) {
        proxy_->update(proxy_->pyobj);
    } else {
        behavior_->update();
    }
}

const char* EGCALL PyBehaviorWrapper::typeName() const
{
    check_proxy(typeName, "");
    if(proxy_) {
        return proxy_->typeName(proxy_->pyobj);
    }
    return behavior_->typeName();
}

bool EGCALL PyBehaviorWrapper::processMessage(const char* evt, const char *params)
{
    check_proxy(processMessage, true);
    if(proxy_) {
//        printf("%ul, %c, %ul : %s, %s\n", strlen(evt), evt[5], strlen(params), evt, params);
//        string evt_name(evt);
//        printf("OK->%",evt_name.c_str());
        return proxy_->processMessage(proxy_->pyobj, const_cast<char*>(evt), const_cast<char*>(params));
    }
    return behavior_->processMessage(evt, params);
}

//const char* EGCALL PyBehaviorWrapper::saveToXml()
//{
//    check_proxy(saveToXml, "");
//    if(proxy_) {
//        return proxy_->saveToXml(proxy_->pyobj);
//    }
//    return behavior_->saveToXml();
//}

void EGCALL PyBehaviorWrapper::loadFromXml(const char* contentbuf, uint32 size)
{
    check_proxy(loadFromXml, Ret_VOID);
    if(proxy_) {
        proxy_->loadFromXml(proxy_->pyobj, const_cast<char*>(contentbuf), size);
    } else {
        behavior_->loadFromXml(contentbuf, size);
    }
}

uint32 EGCALL PyBehaviorWrapper::queryPropertyCount() const
{
    check_proxy(queryPropertyCount, 0);
    if(proxy_) {
        return proxy_->queryPropertyCount(proxy_->pyobj);
    }
    return behavior_->queryPropertyCount();
}

const char* EGCALL PyBehaviorWrapper::queryPropertyNameOf(uint32 idx)
{
    check_proxy(queryPropertyNameOf, "");
    if(proxy_) {
        return proxy_->queryPropertyNameOf(proxy_->pyobj, idx);
    }
    return behavior_->queryPropertyNameOf(idx);
}

Property PyBehaviorWrapper::getProperty(const char* name) const
{
    check_proxy(getPropertyByName, invalid_porp);
    if(proxy_) {
        return proxy_->getPropertyByName(proxy_->pyobj,const_cast<char*>(name));
    }
    return behavior_->getProperty(name);
}

Property PyBehaviorWrapper::getProperty(uint32 idx) const
{
    check_proxy(getPropertyByIdx, invalid_porp);
    if(proxy_) {
        return proxy_->getPropertyByIdx(proxy_->pyobj, idx);
    }
    return behavior_->getProperty(idx);
}

void EGCALL PyBehaviorWrapper::setProperty(const char* name,  Property prop)
{
    check_proxy(setPropertyByName, Ret_VOID);
    if(proxy_) {
        proxy_->setPropertyByName(proxy_->pyobj, const_cast<char*>(name), prop);
    } else {
        behavior_->setProperty(name, prop);
    }
}

void EGCALL PyBehaviorWrapper::setProperty(uint32 idx, Property prop)
{
    check_proxy(setPropertyByIdx, Ret_VOID);
    if(proxy_) {
        proxy_->setPropertyByIdx(proxy_->pyobj, idx, prop);
    } else {
        behavior_->setProperty(idx, prop);
    }
}

IBehavior* PyBehaviorWrapper::gocBehavior(char *type_name)
{
    const char* self_name = typeName();
    // can not get self
    if ( strcmp(self_name, type_name ) == 0 ) return NULL;
    IComponent* com = NULL;
    IBehavior* ret = NULL;
    // get from owner
    com = owner()->getComponent(type_name);
    ret = reinterpret_cast<IBehavior*>(com);
    if ( NULL == com && ret == NULL) {
        // try to create one
        com = gPluginMan->createComponent(type_name);
        ret = reinterpret_cast<IBehavior*>(com);
        if (ret && owner()->addComponent(ret)) {
            //ret->start();
            gPluginMan->getModules()->engineLogger()->writeDebugInfo("PythonBehavior: add a new componet %s", type_name);
        } else {
            if(com) com->Destroy();
            ret = NULL;
            gPluginMan->getModules()->engineLogger()->writeError("PythonBehavior: can not get or create componet %s", type_name);
        }
    }
    return ret;
}

//bool PyBehaviorWrapper::addBehavior(PyBehaviorWrapper *behavior){}

bool PyBehaviorWrapper::removeBehavior(PyBehaviorWrapper *behavior)
{
    IBehavior *bhv = behavior;
    // this is a warpper from python
    if ( behavior->behavior_ ) bhv = behavior->behavior_;
    return owner()->removeComponent(bhv);
}

void PyBehaviorWrapper::sendMessage(char* evt_name, char* param)
{
    owner()->sendMessage(evt_name, param);
}


//void PyBehaviorWrapper::broadcastMessage(char *evt_name, char *param)
//{

//}

//********** Utilites
bool isPyBehaviorWrapper(IBehavior *bheavior)
{
    PyBehaviorWrapper *py = reinterpret_cast<PyBehaviorWrapper*>(bheavior);
    return py && py->proxy_;
}

PyObject* castToPythonObject(PyBehaviorWrapper* wrapper)
{
    if (wrapper->proxy_) {
        return wrapper->proxy_->pyobj;
    }
    return NULL;
}

#undef Ret_VOID
#undef check_proxy

}
