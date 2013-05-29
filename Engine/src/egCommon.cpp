
#include <algorithm>
#include <string>
#include <iostream>

#include <Engine/Platform.h>
#include <Engine/utXML.h>
#include <Engine/rapidxml_print.h>

#include "egModule.h"
#include "egCommon.h"


namespace Pebble
{
/*namespace {
inline Any* GConf(void* val) {
    return static_cast<Any*>(val);
}
}*/

using namespace std;

ConfigSet::ConfigSet(const char* buf, int size)
{
    loadXmlConfig(buf, size);
}

void ConfigSet::loadXmlConfig(const char* buf, int size, bool clear)
{
    if (clear) kwmap_.clear();

    XMLDoc doc;
    doc.parseBuffer( buf, size );
    if( doc.hasError() ) {
        return;
    }

    XMLNode rootNode = doc.getRootNode();
    // load configurations
    if ( !rootNode.isEmpty() && (strcmp(rootNode.getName(), "Settings") == 0) ) {
        XMLNode node = rootNode.getFirstChild("Setting");
        while ( !node.isEmpty() ) {
            string key = node.getAttribute("key", "");
            if ( !key.empty() ) {
                const char* val = node.getAttribute("value", "");
                HashMap::iterator itr = kwmap_.find(key);
                if (itr != kwmap_.end()) {
                    // update exits item
                    itr->second = ConfigItem(val);
                } else {
                    kwmap_.insert(make_pair<string, ConfigItem>(key, ConfigItem(val)));
                }
            }
            node = node.getNextSibling("Setting");
        }
    }
}

ConfigItem* ConfigSet::getItem(const std::string& key)
{
    HashMap::iterator itr = kwmap_.find(key);
    if (itr != kwmap_.end()) {
        return &(itr->second);
    }
    return NULL;
}

IConifgItem *EngineConfig::getConfig(const char *config_name)
{
    /*ConfigItems::iterator itr = items_.find(string(config_name));
    if ( itr != items_.end() )
        return &itr->second;
    return NULL;*/
    string key(config_name);
    ConfigItem* item = ex_setting_.getItem(key);
    if ( NULL == item )
        item = eg_setting_.getItem(key);
    return item;
}

void EngineConfig::setupBehaviorConfig(IBehavior *bhv, const char *xml_str, const uint32 size)
{
    if ( NULL == bhv ) return;

    uint32 prop_num = bhv->queryPropertyCount();
    if (0 == prop_num || 0 == size) return;
    ConfigSet settings(xml_str, size);

    for (uint32 i=0; i<prop_num; ++i) {
        // get property name
        string prop_name(bhv->queryPropertyNameOf(i));
        ConfigItem *setting = settings.getItem(prop_name);
        if ( NULL == setting ) continue;
        // it's a valid setting, inspect its type
        Property property =  bhv->getProperty(i);

        // set property
        switch (property.p_type) {
        case Property::PT_BOOL :
            {
                bool ret = false;
                if ( 0 == _stricmp(setting->value(), "true") || 1 == setting->cast<int>() ) ret = true;
                else if ( 0 == _stricmp(setting->value(), "false") || 0 == setting->cast<int>()) ret = false;
                property.val = &ret;
                bhv->setProperty(i, property);
            }
            break;
        case Property::PT_INT32:
            {
                int ret = setting->cast<int>();
                property.val = &ret;
                bhv->setProperty(i, property);
            }
            break;
        case Property::PT_FLOAT:
            {
                float ret = setting->cast<float>();
                property.val = &ret;
                bhv->setProperty(i, property);
            }
            break;
        case Property::PT_CSTRING:
            {
				static char tempstr[512] = {'\0'};
				strcpy(tempstr, setting->value());
                property.val = &tempstr; //setting->value();
                bhv->setProperty(i, property);
            }
            break;
        case Property::PT_VEC2F:
            {
                Vec2f ret = setting->cast<Vec2f>();
                property.val = &ret;
                bhv->setProperty(i, property);
            }
            break;
        case Property::PT_VEC3F:
            {
                Vec3f ret = setting->cast<Vec3f>();
                property.val = &ret;
                bhv->setProperty(i, property);
            }
            break;
        case Property::PT_VEC4F:
            {
                Vec4f ret = setting->cast<Vec4f>();
                property.val = &ret;
                bhv->setProperty(i, property);
            }
            break;
        default : continue;
        }
    }

}

/*void EngineConfig::setConfig(const char *config_name, const void *val)
{
    const Any *value = static_cast<const Any*>(val);
    items_[string(config_name)] = *value;
}*/

LogMessage make_message(const char* msg, int level, float time)
{
    LogMessage ret ;
    memcpy(ret.text, msg, strlen(msg)+1);
    ret.level = level;
    ret.time = time;
    return ret;
}

EngineLog::EngineLog()
{
    _timer.setEnabled( true );
    _maxNumMessages = 512;
}

void EngineLog::pushMessage( int level, const char *msg, va_list args )
{
    float time = _timer.getElapsedTimeMS() / 1000.0f;

#if defined( PLATFORM_WIN )
#pragma warning( push )
#pragma warning( disable:4996 )
    vsnprintf( _textBuf, 2048, msg, args );
#pragma warning( pop )
#else
    vsnprintf( _textBuf, 2048, msg, args );
#endif

    if( _messages.size() < _maxNumMessages - 1 )
    {
        _messages.push( make_message( _textBuf, level, time ) );
    }
    else if( _messages.size() == _maxNumMessages - 1 )
    {
        _messages.push( make_message( "Message queue is full", 1, time ) );
    }

    const char *headers[6] = { "", "  [peeble-err] ", "  [peeble-warn] ", "[peeble] ", "  [peeble-dbg] ", "[peeble- ] "};
#if defined( PLATFORM_WIN )
    OutputDebugString( headers[min( (uint32)level, (uint32)5 )] );
    OutputDebugStringA( _textBuf );
    OutputDebugString( TEXT("\r\n") );
#elif defined( PLATFORM_ANDROID )
	std::string msgStr = headers[std::min( (uint32)level, (uint32)5 )];
	msgStr += _textBuf;
	LOGI( msgStr.c_str() );
#else
    std::cerr << headers[std::min( (uint32)level, (uint32)5 )];
    std::cerr << _textBuf;
    std::cerr << std::endl;
#endif
}
    
void EGCALL EngineLog::writeMessage( const char *msg, ... )
{
    va_list args;
    va_start( args, msg );
    pushMessage( 0, msg, args );
    va_end( args );
}

void EGCALL EngineLog::writeError( const char *msg, ... )
{
    if( EngineModules::engineConfig()->getConfig("maxLogLevel")->cast<int>() < 1) return;

    va_list args;
    va_start( args, msg );
    pushMessage( 1, msg, args );
    va_end( args );
}


void EGCALL EngineLog::writeWarning( const char *msg, ... )
{
    if( EngineModules::engineConfig()->getConfig("maxLogLevel")->cast<int>() < 2 ) return;

    va_list args;
    va_start( args, msg );
    pushMessage( 2, msg, args );
    va_end( args );
}


void EGCALL EngineLog::writeInfo( const char *msg, ... )
{
    if( EngineModules::engineConfig()->getConfig("maxLogLevel")->cast<int>() < 3 ) return;

    va_list args;
    va_start( args, msg );
    pushMessage( 3, msg, args );
    va_end( args );
}


void EGCALL EngineLog::writeDebugInfo( const char *msg, ... )
{
    if( EngineModules::engineConfig()->getConfig("maxLogLevel")->cast<int>() < 4 ) return;

    va_list args;
    va_start( args, msg );
    pushMessage( 4, msg, args );
    va_end( args );
}


bool EGCALL EngineLog::getMessage( LogMessage &msg )
{
    if( !_messages.empty() )
    {
        msg = _messages.front();
        _messages.pop();
        return true;
    }
    return false;
}

}
