#include "light_manager.h"


ILightManager* LightManagerFactory::createLightManager(float screenWidth, float screenHeight, float tileSize)
{
#ifdef DESKTOP
	return new ShadowLightManager(screenWidth, screenHeight, tileSize);
#else
	return new SimpleLightManager(screenWidth, screenHeight, tileSize);
#endif
}

