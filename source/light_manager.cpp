#include "light_manager.h"


LightManager::LightManager(float screenWidth, float screenHeight, float tileWidth) :
		screenWidth(screenWidth),
		screenHeight(screenHeight),
		tileWidth(tileWidth),
		counter(0),
		shaderArraySize(0),
		shadowsArraySize(0),
		textureDivider(1)
{

	sf::FileInputStream vertexStream;
	vertexStream.open(LIGHT_VERTEX_SHADER);
	sf::FileInputStream fragmentStream;
	fragmentStream.open(LIGHT_FRAGMENT_SHADER);
	shader.loadFromStream(vertexStream, fragmentStream);

	sf::FileInputStream multiplyFragmentStream;
	multiplyFragmentStream.open(MULTIPLY_FRAGMENT_SHADER);
	multiplyShader.loadFromStream(multiplyFragmentStream, sf::Shader::Fragment);
	//vertices.setPrimitiveType(sf::Quads);
	renderTexture.create(screenWidth / textureDivider, screenHeight / textureDivider);
	renderTexture.setSmooth(true);

	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

int LightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	idToShaderIndex[counter] = shaderArraySize;
	shaderIndexToId[shaderArraySize] = counter;
	shader.setUniform("light_pos[" + std::to_string(shaderArraySize) + "]", pos);
	sf::Vector3f colorVec((int) color.r, (int) color.g, (int) color.b);
	colorVec.x /= 255.0f;
	colorVec.y /= 255.0f;
	colorVec.z /= 255.0f;
	shader.setUniform("light_color[" + std::to_string(shaderArraySize) + "]", colorVec);
	shader.setUniform("light_intensity[" + std::to_string(shaderArraySize) + "]", intensity);
	shader.setUniform("sources_size", ++shaderArraySize);

	idToData[counter] = LightData(pos, colorVec, intensity);
	return counter++;
}

int LightManager::addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size)
{
	shader.setUniform("shadow_pos[" + std::to_string(shadowsArraySize) + "]", pos);
	shader.setUniform("shadow_size[" + std::to_string(shadowsArraySize) + "]", size);
	shadowsArraySize++;
	shader.setUniform("shadows_size", shadowsArraySize);
	return counter++;
}

void LightManager::draw(sf::RenderTarget& renderTarget)
{
	sf::RenderStates renderStates;
	renderStates.shader = &shader;
	renderStates.blendMode = sf::BlendAdd;
	sf::Vector2f center = renderTarget.getView().getCenter();


	double multiplier = tileWidth / textureDivider;
	double offsetX = center.x * multiplier;
	double offsetY = center.y * multiplier;
	//offsetX = offsetX - floor(offsetX);
	//offsetY = offsetY - floor(offsetY);
	offsetX /= multiplier;
	offsetY /= multiplier;

	center.x -= offsetX;
	center.y -= offsetY;

	sf::Vector2f centerInBlocks = center;

	center.x *= tileWidth;
	center.y *= tileWidth;

	shader.setUniform("offset", center);

	std::vector<sf::Vertex> vertices;
	vertices.push_back(sf::Vertex(sf::Vector2f(-screenWidth / 2, -screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(screenWidth / 2, -screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(screenWidth / 2, screenHeight / 2)));
	vertices.push_back(sf::Vertex(sf::Vector2f(-screenWidth / 2, screenHeight / 2)));

	sf::View lightView;
	lightView.setCenter(sf::Vector2f(0, 0));
	lightView.setSize(screenWidth, screenHeight);
	renderTexture.setView(lightView);
	renderTexture.clear();
	renderTexture.draw(&vertices[0], vertices.size(), sf::Quads, renderStates);
	renderTexture.display();
	shape.setTexture(&renderTexture.getTexture());
	shape.setPosition(centerInBlocks);

	sf::RenderStates multiplyRenderStates;

	multiplyRenderStates.blendMode = sf::BlendAdd;
	multiplyShader.setUniform("multiplier", 1.0f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);

	multiplyRenderStates.blendMode = sf::BlendMultiply;
	multiplyShader.setUniform("multiplier", 3.0f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);
}

void LightManager::setPosition(int lightSourceID, sf::Vector2f pos)
{
	shader.setUniform("light_pos[" + std::to_string(idToShaderIndex[lightSourceID]) + "]", pos);
}

void LightManager::removeLightSource(int lightSourceID)
{
	//should swap with the last element in shader array
	int lastId = shaderIndexToId[shaderArraySize - 1];
	LightData lastData = idToData[lastId];
	int currentIndex = idToShaderIndex[lightSourceID];
	shaderIndexToId[currentIndex] = lastId;
	idToShaderIndex[lastId] = currentIndex;
	shader.setUniform("light_pos[" + std::to_string(currentIndex) + "]", lastData.position);
	shader.setUniform("light_color[" + std::to_string(currentIndex) + "]", lastData.color);
	shader.setUniform("light_intensity[" + std::to_string(currentIndex) + "]", lastData.intensity);
	shaderArraySize--;
	shader.setUniform("sources_size", shaderArraySize);

	//todo: remove lightSourceID data from maps

}

void LightManager::onWindowResize(float screenWidth, float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}