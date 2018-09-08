#include "light_manager.h"

#include "math.h"
#include <unordered_map>

using namespace std;

struct LightData
{
	float radius; //in blocks
	sf::Color color;
	sf::Vector2f pos;
	float additivePart; // [0..1]

	LightData() {}

	LightData(float radius, sf::Color color, sf::Vector2f pos, float additivePart) :
			radius(radius),
			color(color),
			additivePart(additivePart),
			pos(pos)
	{}
};

struct SimpleLightManagerImpl
{
	SimpleLightManagerImpl(float screenWidth, float screenHeight, float tileSize);
	SimpleLightManagerImpl();
	void init(float screenWidth, float screenHeight, float tileSize);
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity, float additiveFactor);
	int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
	void onWindowResize(float screenWidth, float screenHeight);
private:
	const unsigned int BUBBLE_TEXTURE_SIZE = 100;
	sf::Texture bubbleTexture;
	sf::RenderTexture renderTexture;
	sf::RenderTexture additiveRenderTexture;
	unordered_map<int, LightData> lights;
	int lightCounter;
	float tileSize;
};

SimpleLightManagerImpl::SimpleLightManagerImpl(float screenWidth, float screenHeight, float tileSize) :
	lightCounter(0)
{
	init(screenWidth, screenHeight, tileSize);
}

SimpleLightManagerImpl::SimpleLightManagerImpl() :
	lightCounter(0)
{}

void SimpleLightManagerImpl::init(float screenWidth, float screenHeight, float tileSize)
{
	this->tileSize = tileSize;
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
	bubbleTexture.loadFromImage(bubbleImage);
	bubbleTexture.setSmooth(true);
	bubbleTexture.generateMipmap();

	renderTexture.create(screenWidth, screenHeight);
	additiveRenderTexture.create(screenWidth, screenHeight);
}

int SimpleLightManagerImpl::addLightSource(sf::Vector2f pos, sf::Color color, float intensity, float additiveFactor)
{
	LightData lightData(intensity, color, pos, additiveFactor);
	lights[lightCounter] = lightData; //todo: make additive part configurable
	return lightCounter++;
}

int SimpleLightManagerImpl::addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size)
{
	//obstacles can't be implemented without shaders
	return 0;
}

void SimpleLightManagerImpl::draw(sf::RenderTarget& renderTarget)
{
	sf::RectangleShape rect;
	rect.setTexture(&bubbleTexture);

	renderTexture.setView(renderTarget.getView());
	additiveRenderTexture.setView(renderTarget.getView());
	renderTexture.clear(sf::Color::Black);
	additiveRenderTexture.clear(sf::Color::Black);

	sf::RenderStates renderStates;
	renderStates.blendMode = sf::BlendAdd;

	for (auto iter = lights.begin(); iter != lights.end(); iter++)
	{
		LightData data = iter->second;
		rect.setSize(sf::Vector2f(data.radius, data.radius));
		rect.setOrigin(data.radius / 2, data.radius / 2);
		rect.setPosition(data.pos / tileSize);
		rect.setFillColor(data.color);
		renderTexture.draw(rect, renderStates);
		//now dealing with additive part
		float r = data.color.r;
		float g = data.color.g;
		float b = data.color.b;
		r *= data.additivePart;
		g *= data.additivePart;
		b *= data.additivePart;
		rect.setFillColor(sf::Color(r, g, b));
		additiveRenderTexture.draw(rect, renderStates);

	}
	renderTexture.display();
	additiveRenderTexture.display();
	sf::RectangleShape resultRect;
	resultRect.setOrigin(renderTarget.getView().getSize() / 2.0f);
	resultRect.setSize(renderTarget.getView().getSize());
	resultRect.setPosition(renderTarget.getView().getCenter());

	renderStates.blendMode = sf::BlendAdd;
	resultRect.setTexture(&additiveRenderTexture.getTexture());
	renderTarget.draw(resultRect, renderStates);

	renderStates.blendMode = sf::BlendMultiply;
	resultRect.setTexture(&renderTexture.getTexture());
	renderTarget.draw(resultRect, renderStates);

}

void SimpleLightManagerImpl::setPosition(int lightSourceIndex, sf::Vector2f pos)
{
	lights[lightSourceIndex].pos = pos;
}

void SimpleLightManagerImpl::removeLightSource(int lightSourceIndex)
{
	lights.erase(lightSourceIndex);
}

void SimpleLightManagerImpl::onWindowResize(float screenWidth, float screenHeight)
{
	//todo: store lightmap texture and re-create it, only when window is resized
}

SimpleLightManager::SimpleLightManager(float screenWidth, float screenHeight, float tileSize)
{
	impl = new SimpleLightManagerImpl(screenWidth, screenHeight, tileSize);
}

SimpleLightManager::SimpleLightManager()
{
	impl = new SimpleLightManagerImpl();
}

SimpleLightManager::~SimpleLightManager()
{
	delete impl;
}

void SimpleLightManager::init(float screenWidth, float screenHeight, float tileSize)
{
	impl->init(screenWidth, screenHeight, tileSize);
}

int SimpleLightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity, float additiveFactor)
{
	return impl->addLightSource(pos, color, intensity, additiveFactor);
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

