#pragma once

#include <string>
#include <cstdarg>
#include <queue>
#include <map>
#include <Engine/EngineCommon.h>

#if defined PLATFORM_MAC || defined __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

#include "utils.h"

namespace Pebble
{

class EngineModules;

//The following class defines a hash function for strings
//( This class is taken from Thingol's forum post on http://www.codeguru.com/forum/showthread.php?t=315286 )
//( This class may have originated from the "FreeStyle Dash" project on google code ( http://code.google.com/p/freestyledash/ ), though I'm not certain )
#if defined PLATFORM_MAC || defined __GNUC__
class stringhasher
#else
class stringhasher : public stdext::hash_compare <std::string>
#endif
{
public:
    /**
   * Required by
   * Inspired by the java.lang.String.hashCode() algorithm
   * (it's easy to understand, and somewhat processor cache-friendly)
   * @param The string to be hashed
   * @return The hash value of s
   */
    size_t operator() (const std::string& s) const
    {
        size_t h = 0;
        std::string::const_iterator p, p_end;
        for(p = s.begin(), p_end = s.end(); p != p_end; ++p)
        {
            h = 31 * h + (*p);
        }
        return h;
    }

    /**
   *
   * @param s1 The first string
   * @param s2 The second string
   * @return true if the first string comes before the second in lexicographical order
   */
    bool operator() (const std::string& s1, const std::string& s2) const
    {
#if defined PLATFORM_MAC || defined __GNUC__
        return s1 == s2;
#else
        return s1 < s2;
#endif

    }
};


class ConfigItem : public IConifgItem
{
public:
    ConfigItem() : value_("") {}
    ConfigItem(const char* value) : value_(value) {}

    virtual const char* EGCALL value() const
    {
        return value_.c_str();
    }
private:
    std::string value_;
};

class ConfigSet
{
#if defined PLATFORM_MAC || defined __GNUC__
    typedef __gnu_cxx::hash_map<std::string, ConfigItem, stringhasher, stringhasher> HashMap;
#else
    typedef stdext::hash_map<std::string, ConfigItem, stringhasher> HashMap;
#endif

public :
    ConfigSet() {}
    ConfigSet(const char* buf, int size);
    void loadXmlConfig(const char* buf, int size, bool clear = true);
    ConfigItem* getItem(const std::string& key);

private:
    //a key-value pair mapping for storing settings
    HashMap kwmap_;
};

class EngineConfig : public IEngineConfig
{
    friend class EngineModules;

public:
    EngineConfig() {}

    virtual IConifgItem* EGCALL getConfig(const char *config_name);
    virtual void EGCALL setupBehaviorConfig(IBehavior *bhv, const char *xml_str, const uint32 size);

    void setEngineConfig(const char* buf, int size)
    {
        eg_setting_.loadXmlConfig(buf, size, false);
    }

    void setExtraConfig(const char* buf, int size)
    {
        ex_setting_.loadXmlConfig(buf, size);
    }

private:
    EngineConfig(const EngineConfig&) {}
    ~EngineConfig() {}

    ConfigSet eg_setting_;
    ConfigSet ex_setting_;

};

// =================================================================================================
// Engine Log
// =================================================================================================

class EngineLog : public IEngineLogger
{
    friend class EngineModules;
public:
    EngineLog();

    virtual void EGCALL writeMessage( const char *msg, ... );
    virtual void EGCALL writeError( const char *msg, ... );
    virtual void EGCALL writeWarning( const char *msg, ... );
    virtual void EGCALL writeInfo( const char *msg, ... );
    virtual void EGCALL writeDebugInfo( const char *msg, ... );

    virtual bool EGCALL getMessage( LogMessage &msg );

    virtual uint32 EGCALL getMaxNumMessages() { return _maxNumMessages; }
    virtual void EGCALL setMaxNumMessages( const uint32 maxNumMessages ) { _maxNumMessages = maxNumMessages; }

private:
    EngineLog(const EngineLog&) {}
    ~EngineLog() {}
    void pushMessage( const std::string &text, uint32 level );
    void pushMessage( int level, const char *msg, va_list ap );

protected:
    Timer                     _timer;
    char                      _textBuf[2048];
    uint32                    _maxNumMessages;
    std::queue< LogMessage >  _messages;
};
}
