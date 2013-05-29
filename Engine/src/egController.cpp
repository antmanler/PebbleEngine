
#include <iterator> 
#include <algorithm>
#include <Engine/utXML.h>
#include <Engine/rapidxml_print.h>
#include "egModule.h"
#include "egController.h"

namespace
{
using namespace Pebble;
struct PerformRender
{
    void operator () (IRenderer* rend)
    {
        if(rend->isEnabled())
            rend->render();
    }
};

}

namespace Pebble
{

using namespace std;

Controller::Controller() : manager_(0x0)
{
    manager_ = new PluginManager();
    renderer_set_ = new RendererSet();
}

Controller::~Controller()
{
    if ( manager_ ) {
        manager_->Destroy();
        delete manager_;
        manager_ = 0x0;
    }
    if(renderer_set_) {
        renderer_set_->clear();
        delete renderer_set_;
        renderer_set_ = 0x0;
    }
    EngineModules::engineLogger()->writeDebugInfo("Controller released.");
}

bool Controller::loadPlugin(const char *filename)
{
    if ( manager_->LoadPlugin(filename) ) {
        EngineModules::engineLogger()->writeDebugInfo("Controller::loadPlugin(): plugin %s loaded.", filename);
        return true;
    }
    EngineModules::engineLogger()->writeWarning("Controller::loadPlugin(): plugin %s load failed!", filename);
    return false;
}

bool  Controller::changeScene(const char* buf, uint32 size)
{
    EngineModules::world()->clear();

    XMLDoc doc;
    doc.parseBuffer( buf, size );
    if( doc.hasError() ) {
        EngineModules::engineLogger()->writeError("Controller::changeScene(): scene parsing failed, %d@%s", __LINE__, __FILE__);
        return false;
    }

    sig_start_new_scn_.Emit();

    XMLNode rootNode = doc.getRootNode();
    if ( !rootNode.isEmpty() && (strcmp(rootNode.getName(), "Setup") == 0) ) {
        // load settings from setup file
        XMLNode xmlNode1 = rootNode.getFirstChild("Settings");
        if (!xmlNode1.isEmpty()) {
            string set_xml = "";
            rapidxml::print( back_inserter( set_xml ), *xmlNode1.getRapidXMLNode(), 0 );
            EngineModules::engineLogger()->writeDebugInfo("Controller::changeScene(): "
                                                          "loading scene specified settings");
            EngineModules::engineConfig()->setExtraConfig(set_xml.c_str(), set_xml.length());
        }

        // Parse children, and create the known component
        // add the created components to the root entity in the world
        IEntity *root_ent = EngineModules::world()->getEntity(0u);
        xmlNode1 = rootNode.getFirstChild();
        while( !xmlNode1.isEmpty() ) {
            // skip node
            if (xmlNode1.getName() == 0x0 ||
                _stricmp(xmlNode1.getName(), "Settings") == 0 ||
                //_stricmp(xmlNode1.getName(), "EnginePath") == 0 ||
                _stricmp(xmlNode1.getName(), "Editor") == 0 )  {
                xmlNode1 = xmlNode1.getNextSibling();
                continue;
            }

            IComponent *com = manager_->createComponent(xmlNode1.getName());
            if (com && root_ent->addComponent(com)) {
                string com_xml = "";
                rapidxml::print( back_inserter( com_xml ), *xmlNode1.getRapidXMLNode(), 0 );
                // set serialized data
                char *txt = new char[com_xml.length()];
                memcpy(txt, com_xml.c_str(), com_xml.length());
                com->loadFromXml(txt, com_xml.length());
                delete []txt;
            } else {
                EngineModules::engineLogger()->writeWarning("Controller::changeScene():"
                                                            " can not handle the node of '%s'", xmlNode1.getName());
                if(com) com->Destroy();
            }

            xmlNode1 = xmlNode1.getNextSibling();
        }

        // Emit load scene signal for those unhandled or who need to know the gloabl information
        sig_load_scene_.Emit(buf, size);

    } else {
        EngineModules::engineLogger()->writeError("Controller::changeScene(): "
                                                  "it's not a valid scene setup data, %d@%s", __LINE__, __FILE__);
        return false;
    }
    return true;
}

void  Controller::update()
{
    // Emit update signal
    //    sig_update_.Emit();
    sig_update_.Emit();
    // update each component manager
    manager_->update();
    // update world
    EngineModules::world()->update();
    // update renderers
    RendererSet::iterator itr = renderer_set_->begin();
    for (; itr!=renderer_set_->end(); ++itr) {
        (*itr)->update();
    }
}

void Controller::parseArgv(int argc, char **argv)
{
    // currently just Emit the signal
    //    sig_parse_argv_.Emit(argc, argv);
    sig_parse_argv_.Emit(argc, argv);
}

void Controller::render()
{
    // render each renderer accroding to the priority of renderer
    for_each(renderer_set_->begin(), renderer_set_->end(), PerformRender());
    // post Emit render signal
    //    sig_render_.Emit();
    sig_render_.Emit();
}

void Controller::resize(uint32 w, uint32 h)
{
    //Vec3f *param = new Vec3f(w, h, 0.0);
    //EngineModules::world()->sendEvent("Resize", param);

    //for_each(renderer_set_->begin(), renderer_set_->end(), Resizer(w, h));
    //delete param;
    //    sig_resize_.Emit(w, h);
    sig_resize_.Emit(w, h);
    //    sig_t2_.Emit(w, h);
    //Any value = Vec3f(w, h, 0.0);
    static const char* tpl = "<Settings>\n"
                             "    <Setting key=\"WindowSize\" value=\"[%d,%d]\" />\n"
                             "</Settings>";
    static char buf[128] = {'\0'};
    sprintf(buf, tpl, w, h);
    EngineModules::engineConfig()->setEngineConfig(buf, strlen(buf));
    //EngineModules::engineConfig()->setConfig("WindowSize", &value);
}

IComponent*  Controller::createComponent(const char* com_type, const IEntity *owner)
{
    return NULL;
}

bool Controller::addRenderer( IRenderer* renderer )
{
    pair<RendererSet::iterator, bool> ret = renderer_set_->insert(renderer);
    if (ret.second) {
        EngineModules::engineLogger()->writeDebugInfo("Controller::removeRenderer(): Renderer %s added.", renderer->typeName());
        renderer->start();
    }
    return ret.second;
}

void Controller::removeRenderer( IRenderer* renderer )
{
    RendererSet::iterator itr = find( renderer_set_->begin(), renderer_set_->end(), renderer);
    if ( itr != renderer_set_->end() ) {
        renderer_set_->erase(renderer);
        EngineModules::engineLogger()->writeDebugInfo("Controller::removeRenderer(): Renderer %s is removed.", renderer->typeName());
    }
}

}
