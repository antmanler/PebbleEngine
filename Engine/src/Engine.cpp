
#include <Engine/Platform.h>

#include <string>
#include <fstream>
#include <iomanip>

#include "egModule.h"

using namespace std;
using namespace Pebble;

ofstream outf;

bool gInitialized = false;
const char *emptyCString = "";

DLLEXP const char *piwGetVersionString()
{
    return "V0.5";
}

DLLEXP bool piwInit(const char* buf, int sz)
{
    if(!gInitialized) {
        EngineConfig *conf = EngineModules::engineConfig();
        const char* defualt_setting = "<Settings>\n"
                                      "    <Setting key=\"maxLogLevel\" value=\"4\" />\n"
                                      "    <Setting key=\"pluginDir\" value=\"./\" />\n"
                                      "    <Setting key=\"WindowSize\" value=\"[320,240]\" />\n"
                                      "</Settings>";
        conf->setEngineConfig(defualt_setting, strlen(defualt_setting));
    }
    EngineModules::engineConfig()->setEngineConfig(buf, sz);
    EngineModules::engineLogger()->writeDebugInfo("PIW initialized!");
    gInitialized = true;
    return gInitialized;
}


DLLEXP void piwRelease()
{
    EngineModules::release();
    gInitialized = false;
    EngineModules::engineLogger()->writeDebugInfo("PIW released!");
}

DLLEXP void piwParseSysArgv(int argc, char** argv)
{
    EngineModules::controller()->parseArgv(argc, argv);
}


DLLEXP bool piwLoadPlugin(const char* filename)
{
#if !defined (PLATFORM_IOS)
#   ifdef PLATFORM_WIN
    std::string prefix = "";
    std::string postfix = ".dll";
#   elif defined PLATFORM_MAC
    std::string prefix = "lib";
    std::string postfix = ".dylib";
#   else
    std::string prefix = "lib";
    std::string postfix = ".so";
#   endif
    std::string plugin_dir(EngineModules::engineConfig()->getConfig("pluginDir")->value());
    std::string plugin = plugin_dir + prefix + std::string(filename) + postfix;
#else
    std::string plugin = std::string(filename);
#endif
    
    return EngineModules::controller()->loadPlugin(plugin.c_str());
}

DLLEXP void piwUpdate()
{
    EngineModules::controller()->update();
}

DLLEXP void piwRender()
{
    EngineModules::controller()->render();
}

DLLEXP const char* piwGetComNames()
{
    return EngineModules::controller()->pluginMan()->componentNames();
}

DLLEXP void piwResize(unsigned int w, unsigned int h)
{
    EngineModules::controller()->resize(w, h);
}

DLLEXP bool piwLoadScene(const char* buf, uint32 size)
{
    if (NULL == buf) {
        // close current scene
        EngineModules::world()->clear();
        return true;
    }
    return EngineModules::controller()->changeScene(buf, size);
}

DLLEXP const char *piwGetMessage( int *level, float *time )
{
    static string msgText;
    static LogMessage msg;

    if( EngineModules::engineLogger()->getMessage( msg ) )
    {
        if( level != 0x0 ) *level = msg.level;
        if( time != 0x0 ) *time = msg.time;
        return msg.text;
    }
    else
        return emptyCString;
}

DLLEXP void piwBroadcastMessage(const char* evt_name, const char* param)
{
    if (gInitialized)
        EngineModules::world()->sendMessage(evt_name, param);
}

DLLEXP bool piwutLoadSceneFromFile(const char* filename)
{
    char *dataBuf = 0x0;
    int bufSize = 0;

    ifstream inf;
    inf.clear();
    inf.open( filename, ios::binary );

    // Scene file found
    if( inf.good() )  {
        // Find size of resource file
        inf.seekg( 0, ios::end );
        int fileSize = inf.tellg();
        if( fileSize == 0 ) return false;

        if( bufSize < fileSize  ) {
            delete[] dataBuf;
            dataBuf = new char[fileSize];
            if( !dataBuf ) {
                bufSize = 0;
                return false;
            }
            bufSize = fileSize;
        }
        // Copy resource file to memory
        inf.seekg( 0 );
        inf.read( dataBuf, fileSize );
        inf.close();
        // Send resource data to engine
        bool ret = piwLoadScene(dataBuf, fileSize );
        delete []dataBuf;
        return ret;
    }
    return false;
}

DLLEXP bool piwutDumpMessages()
{
    if( !outf.is_open() )
    {
        // Reset log file
        outf.setf( ios::fixed );
        outf.precision( 3 );
        outf.open( "Pebble_Log.html", ios::out );
        if( !outf ) return false;

        outf << "<html>\n";
        outf << "<head>\n";
        outf << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
        outf << "<title>Pebble in Water Log</title>\n";
        outf << "<style type=\"text/css\">\n";

        outf << "body, html {\n";
        outf << "background: #000000;\n";
        outf << "width: 1000px;\n";
        outf << "font-family: Arial;\n";
        outf << "font-size: 16px;\n";
        outf << "color: #C0C0C0;\n";
        outf << "}\n";

        outf << "h1 {\n";
        outf << "color : #FFFFFF;\n";
        outf << "border-bottom : 1px dotted #888888;\n";
        outf << "}\n";

        outf << "pre {\n";
        outf << "font-family : arial;\n";
        outf << "margin : 0;\n";
        outf << "}\n";

        outf << ".box {\n";
        outf << "border : 1px dotted #818286;\n";
        outf << "padding : 5px;\n";
        outf << "margin: 5px;\n";
        outf << "width: 950px;\n";
        outf << "background-color : #292929;\n";
        outf << "}\n";

        outf << ".err {\n";
        outf << "color: #EE1100;\n";
        outf << "font-weight: bold\n";
        outf << "}\n";

        outf << ".warn {\n";
        outf << "color: #FFCC00;\n";
        outf << "font-weight: bold\n";
        outf << "}\n";

        outf << ".info {\n";
        outf << "color: #C0C0C0;\n";
        outf << "}\n";

        outf << ".debug {\n";
        outf << "color: #CCA0A0;\n";
        outf << "}\n";

        outf << "</style>\n";
        outf << "</head>\n\n";

        outf << "<body>\n";
        outf << "<h1>Pebble Log</h1>\n";
        outf << "<h3>" << piwGetVersionString() << "</h3>\n";
        outf << "<div class=\"box\">\n";
        outf << "<table>\n";

        outf.flush();
    }

    int level;
    float time;
    string text =  piwGetMessage( &level, &time );

    while( text != "" )
    {
        outf << "<tr>\n";
        outf << "<td width=\"100\">";
        outf << time;
        outf << "</td>\n";
        outf << "<td class=\"";

        switch( level )
        {
        case 1:
            outf << "err";
            break;
        case 2:
            outf << "warn";
            break;
        case 3:
            outf << "info";
            break;
        default:
            outf << "debug";
        }

        outf << "\"><pre>\n";
        outf << text.c_str();
        outf << "\n</pre></td>\n";
        outf << "</tr>\n";

        outf.flush();

        text = piwGetMessage( &level, &time );
    }

    return true;
}
