#include "light_manager.h"

ILightManager& LightManagerFactory::getLightManager(float screenWidth, float screenHeight, float tileSize)
{
#ifdef DESKTOP
	static ShadowLightManager result;
#else
	static SimpleLightManager result;
#endif
	static bool initialized = false;
	if (!initialized)
	{
		result.init(screenWidth, screenHeight, tileSize);
	}
	return result;
}

