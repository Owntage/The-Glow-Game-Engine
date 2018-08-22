#include "light_manager.h"
#include "math.h"

struct SimpleLightManagerImpl
{
	SimpleLightManagerImpl(float screenWidth, float screenHeight, float tileSize);
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity);
	int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
	void onWindowResize(float screenWidth, float screenHeight);
private:
	const unsigned int BUBBLE_TEXTURE_SIZE = 100;
	sf::Texture bubbleTexture;
};

SimpleLightManagerImpl::SimpleLightManagerImpl(float screenWidth, float screenHeight, float tileSize)
{
	sf::Image bubbleImage;
	bubbleImage.create(BUBBLE_TEXTURE_SIZE, BUBBLE_TEXTURE_SIZE, sf::Color::Black);
	float fSize = (float) BUBBLE_TEXTURE_SIZE;
	float centerX = fSize / 2.0f;
	float centerY = fSize / 2.0f;
	float radius = BUBBLE_TEXTURE_SIZE / 2.0f;
	for (unsigned int x = 0; x < BUBBLE_TEXTURE_SIZE; x++)
	{
		for (unsigned int y = 0; y < BUBBLE_TEXTURE_SIZE; y++)
		{
			float fX = (float) x;
			float fY = (float) y;
			float dist = sqrtf(powf(fX - centerX, 2) + powf(fY - centerY, 2));
			unsigned char intensity = (unsigned char) (255.0f * (radius - dist) / radius);
			if (dist >= radius)
			{
				intensity = 0;
			}
			bubbleImage.setPixel(x, y, sf::Color(intensity, intensity, intensity));
		}
	}
	bubbleImage.saveToFile("bubble.png");
	//todo: write bubble to texture
}

int SimpleLightManagerImpl::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	//todo: implement
	return 0;
}

int SimpleLightManagerImpl::addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size)
{
	//todo: implement
	return 0;
}

void SimpleLightManagerImpl::draw(sf::RenderTarget& renderTarget)
{
	//todo: implement
}

void SimpleLightManagerImpl::setPosition(int lightSourceIndex, sf::Vector2f pos)
{
	//todo: implement
}

void SimpleLightManagerImpl::removeLightSource(int lightSourceIndex)
{
	//todo: implement
}

void SimpleLightManagerImpl::onWindowResize(float screenWidth, float screenHeight)
{
	//todo: implement
}

SimpleLightManager::SimpleLightManager(float screenWidth, float screenHeight, float tileSize)
{
	impl = new SimpleLightManagerImpl(screenWidth, screenHeight, tileSize);
}

SimpleLightManager::~SimpleLightManager()
{
	delete impl;
}

int SimpleLightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	return impl->addLightSource(pos, color, intensity);
}

int SimpleLightManager::addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size)
{
	return impl->addRectangleObstacle(pos, size);
}

void SimpleLightManager::draw(sf::RenderTarget& renderTarget)
{
	impl->draw(renderTarget);
}

void SimpleLightManager::setPosition(int lightSourceIndex, sf::Vector2f pos)
{
	impl->setPosition(lightSourceIndex, pos);
}

void SimpleLightManager::removeLightSource(int lightSourceIndex)
{
	impl->removeLightSource(lightSourceIndex);
}
void SimpleLightManager::onWindowResize(float screenWidth, float screenHeight)
{
	impl->onWindowResize(screenWidth, screenHeight);
}

