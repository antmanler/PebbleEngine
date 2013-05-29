

#pragma once

#include "egPrerequisites.h"
#include "egExtensions.h"


namespace Horde3DSph {

using namespace Horde3D;


class ExtSph: public IExtension
{
public:
	virtual const char *getName();
	virtual bool init();
	virtual void release();
};

}

