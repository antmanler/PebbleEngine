
#pragma once

#include "Platform.h"

namespace Pebble
{

class IBehavior;

class IConifgItem
{
public:
    virtual const char* EGCALL value() const = 0;
    template< typename TVAL >
    TVAL EGCALL cast() const;
};

template< typename TVAL >
inline TVAL EGCALL IConifgItem::cast() const
{
    return (TVAL)this->value();
}

template<>
inline int EGCALL IConifgItem::cast() const
{
    return atoi(this->value());
}

template<>
inline float EGCALL IConifgItem::cast() const
{
    return static_cast<float>(atof(this->value()));
}

template<>
inline Vec2i EGCALL IConifgItem::cast() const
{
    Vec2i ret(0, 0);
    sscanf(this->value(), "[%d,%d]", &ret.x, &ret.y);
    return ret;
}

template<>
inline Vec2f EGCALL IConifgItem::cast() const
{
    Vec2f ret(0, 0);
    sscanf(this->value(), "[%f,%f]", &ret.x, &ret.y);
    return ret;
}

template<>
inline Vec3f EGCALL IConifgItem::cast() const
{
    Vec3f ret(0.0, 0.0, 0.0);
    sscanf(this->value(), "[%f,%f,%f]", &ret.x, &ret.y, &ret.z);
    return ret;
}

template<>
inline Vec4f EGCALL IConifgItem::cast() const
{
    Vec4f ret(0.0, 0.0, 0.0, 0.0);
    sscanf(this->value(), "[%f,%f,%f,%f]", &ret.x, &ret.y, &ret.z, &ret.w);
    return ret;
}

class IEngineConfig
{
public:
    virtual IConifgItem* EGCALL getConfig(const char *config_name) = 0;
    virtual void EGCALL setupBehaviorConfig(IBehavior *bhv, const char *xml_str, const uint32 size) = 0;
};

// =================================================================================================
// Engine Log
// =================================================================================================

struct ENGINE_EXPORT LogMessage
{
    char   text[512];
    int    level;
    float  time;
};

// =================================================================================================

class IEngineLogger
{
public:
    virtual void EGCALL writeMessage( const char *msg, ... ) = 0;
    virtual void EGCALL writeError( const char *msg, ... ) = 0;
    virtual void EGCALL writeWarning( const char *msg, ... ) = 0;
    virtual void EGCALL writeInfo( const char *msg, ... ) = 0;
    virtual void EGCALL writeDebugInfo( const char *msg, ... ) = 0;

    virtual bool EGCALL getMessage( LogMessage &msg ) = 0;

    virtual uint32 EGCALL getMaxNumMessages() = 0;
    virtual void EGCALL setMaxNumMessages( const uint32 maxNumMessages ) = 0;
};
}
