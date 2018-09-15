#include "render_system.h"
#include "components/animation_update.h"
#include "delete_update.h"
#include "components/variant_update.h"

bool isFileExists(std::string filename)
{
	/*
	std::ifstream file(filename.c_str());
	return file.good();
	 */
	return true; //in the singleplayer each file should exist
}

DrawableActor::DrawableActor(Console& console, RenderSystem& renderSystem, SoundManager& soundManager, std::shared_ptr<ILightManager> lightManager) :
	isMain(false),
	rect(sf::Vector2f(1.0f, 1.0f)), 
	console(console), 
	lastMessagePrinted(-1), 
	renderSystem(renderSystem),
	deltaTime(0.0f),
	positionX(0.0f),
	positionY(0.0f),
	speedX(0.0f),
	speedY(0.0f),
	sizeX(0.0f),
	sizeY(0.0f),
	lightManager(lightManager),
	hasLightSource(false),
	hasShadow(false),
	soundManager(soundManager)
{
	rect.setOrigin(0.5f, 0.5f);
	isDrawing = false;
	animationStateChanged = false;
	serverTime = -1.0f;
	
}

DrawableActor::~DrawableActor()
{
	if(hasLightSource)
	{
		lightManager->removeLightSource(lightSourceID);
	}
}

void DrawableActor::setMain(bool isMain)
{
	this->isMain = isMain;
}

int counter = 0;

void DrawableActor::onUpdate(ActorUpdate& update)
{
	//std::cout << "onUpdate started" << std::endl;
	for(auto it = update.updates.begin(); it != update.updates.end(); it++)
	{
		if((*it)->name == "render")
		{
			RenderUpdate& renderUpdate = static_cast<RenderUpdate&> (*(*it));
			renderData = renderUpdate.renderData;
		}
		if((*it)->name == "weapon")
		{
			
			WeaponUpdate& weaponUpdate = static_cast<WeaponUpdate&> (*(*it));
			if(isMain)
			{
				renderSystem.gameGuiManager.setWeaponUpdate(weaponUpdate);
			}
		}

		if((*it)->name == "variant")
		{
			VariantUpdate &variantUpdate = static_cast<VariantUpdate &> (*(*it));
			std::string name = variantUpdate.get<std::string>("name");
			if (name == "nickname")
			{
				std::cout << "variant update received: " << std::endl;
				std::cout << "name: " << variantUpdate.get<std::string>("name") << std::endl;
				std::cout << "nickname: " << variantUpdate.get<std::string>("nickname") << std::endl;
			}

			if (name == "deathmatch")
			{
				renderSystem.gameGuiManager.setDeathmatchUpdate(variantUpdate);
			}

			if (name == "hp")
			{
				if(isMain)
				{
					renderSystem.gameGuiManager.setHpUpdate(variantUpdate);
				}
			}

			if (name == "sound")
			{
				if (variantUpdate.get<std::string>("type") == "sound")
				{
					soundManager.playSound(positionX, positionY, variantUpdate.get<std::string>("sound"));
				}
			}

			if (name == "chat")
			{
				for (int i = 0; i < variantUpdate.getVector<std::string>("messages").size(); i++)
				{
					if (i + variantUpdate.get<int>("rangeBegin") > lastMessagePrinted)
					{
						console.println(variantUpdate.getVector<std::string>("messages")[i]);
					}
				}
				lastMessagePrinted = variantUpdate.get<int>("rangeEnd");
			}

			if (name == "move")
			{
				positionX = variantUpdate.get<float>("x");
				positionY = variantUpdate.get<float>("y");
				speedX = variantUpdate.get<float>("speedX");
				speedY = variantUpdate.get<float>("speedY");
				sizeX = variantUpdate.get<float>("sizeX");
				sizeY = variantUpdate.get<float>("sizeY");
				serverTime = variantUpdate.get<float>("time");
				deltaTime = 0;

				if (isMain)
				{
					soundManager.setListenerPosition(positionX, positionY);
				}
			}
			if (name == "tile")
			{
				counter++;
				std::cout << "received tile update " << counter << std::endl;
				std::string imageName = variantUpdate.get<std::string>("image");
				if(renderSystem.imagesInTileset.find(imageName) == renderSystem.imagesInTileset.end())
				{
					sf::Image image;
					if(!isFileExists(imageName))
					{
						//std::cout << "loading image in tile update" << std::endl;
						image.loadFromFile(DEFAULT_TEXTURE);
						if(renderSystem.imageLoadRequests.find(imageName) == renderSystem.imageLoadRequests.end())
						{
							renderSystem.imageLoadRequests.insert(imageName);
							renderSystem.imagesToLoad.push_back(imageName);
						}
					}
					else
					{
						//std::cout << "loading default image in tile update" << std::endl;
						image.loadFromFile(imageName);
					}

					int pos = renderSystem.imagesInTileset.size();
					renderSystem.imagesInTileset[imageName] = pos;
					int tileset_x = pos % TILESET_WIDTH;
					int tileset_y = pos / TILESET_WIDTH;
					renderSystem.tileset.update(image, tileset_x * TILE_SIZE, tileset_y * TILE_SIZE);
				}

				float tileset_x = renderSystem.imagesInTileset[imageName] % TILESET_WIDTH;
				float tileset_y = renderSystem.imagesInTileset[imageName] / TILESET_WIDTH;

				tileset_x *= TILE_SIZE;
				tileset_y *= TILE_SIZE;

				float x = variantUpdate.get<float>("x");
				float y = variantUpdate.get<float>("y");

				vertices[0].position = sf::Vector2f(x - 0.5f, y - 0.5f);
				vertices[1].position = sf::Vector2f(x + 0.5f, y - 0.5f);
				vertices[2].position = sf::Vector2f(x + 0.5f, y + 0.5f);

				vertices[3].position = sf::Vector2f(x - 0.5f, y - 0.5f);
				vertices[4].position = sf::Vector2f(x + 0.5f, y + 0.5f);
				vertices[5].position = sf::Vector2f(x - 0.5f, y + 0.5f);

				vertices[0].texCoords = sf::Vector2f(tileset_x, tileset_y);
				vertices[1].texCoords = sf::Vector2f(tileset_x + TILE_SIZE, tileset_y);
				vertices[2].texCoords = sf::Vector2f(tileset_x + TILE_SIZE, tileset_y + TILE_SIZE);

				vertices[3].texCoords = sf::Vector2f(tileset_x, tileset_y);
				vertices[4].texCoords = sf::Vector2f(tileset_x + TILE_SIZE, tileset_y + TILE_SIZE);
				vertices[5].texCoords = sf::Vector2f(tileset_x, tileset_y + TILE_SIZE);

				for(int i = 0; i < 6; i++)
				{
					renderSystem.tileVertices.append(vertices[i]);
				}
			}
		}
		
		if((*it)->name == "animation")
		{
			AnimationUpdate& animationUpdate = static_cast<AnimationUpdate&>(*(*it));
			//std::cout << "state: " << animationUpdate.animationState << std::endl;
			for(int i = 0; i < animationUpdate.states.size(); i++)
			{
				if(animationStates.find(animationUpdate.states[i].stateName) == animationStates.end())
				{
					animationStates[animationUpdate.states[i].stateName] = animationUpdate.states[i];
				}
			}
			
			if(animationLayerStates.size() != animationUpdate.currentLayerStates.size())
			{
				animationStateChanged = true;
			}
			else
			{
				animationStateChanged = false;
				for(int i = 0; i < animationLayerStates.size(); i++)
				{
					if(animationLayerStates[i] != animationUpdate.currentLayerStates[i])
					{
						animationStateChanged = true;
						break;
					}
				}
			}
			
			animationLayerStates = animationUpdate.currentLayerStates;
			layerTime.resize(animationLayerStates.size(), 99999.0f);
			layerImageIndex.resize(animationLayerStates.size(), 0);
			isDrawing = true;
		}
	}
	//std::cout << "onUpdate finished" << std::endl;
}



void DrawableActor::draw(sf::RenderTarget& renderTarget)
{
	
	rect.setPosition(positionX + speedX * deltaTime, positionY + speedY * deltaTime);
	deltaTime += 1.0f / 60.0f;

	

	if(isDrawing)
	{
		for(int i = 0; i < layerTime.size(); i++)
		{
			if(!animationLayerStates[i].isDrawing)
			{
				continue;
			}
			layerTime[i] += 1.0f / 60.0f;
			
			
			if(layerTime[i] > animationStates[animationLayerStates[i].state].delay) //no check if animation state is changed
			{
				animationStateChanged = false;
				layerTime[i] = 0;
				
				layerImageIndex[i] = (layerImageIndex[i] + 1) % animationStates[animationLayerStates[i].state].images.size();
				
			}
			
			layerImageIndex[i] = layerImageIndex[i] % animationStates[animationLayerStates[i].state].images.size();
			std::string nextImage = animationStates[animationLayerStates[i].state].images[layerImageIndex[i]];
			if(renderSystem.textures.find(nextImage) == renderSystem.textures.end())
			{
				if(isFileExists(nextImage) && nextImage != "")
				{
					sf::Texture newTexture;
					newTexture.loadFromFile(nextImage);
					renderSystem.textures[nextImage] = newTexture;
				}
				else
				{
					if(renderSystem.textures.find(DEFAULT_TEXTURE) == renderSystem.textures.end())
					{
						sf::Texture defaultTexture;
						defaultTexture.loadFromFile(DEFAULT_TEXTURE);
						renderSystem.textures[DEFAULT_TEXTURE] = defaultTexture;
					}
					renderSystem.textures[nextImage] = renderSystem.textures[DEFAULT_TEXTURE];
					if(renderSystem.imageLoadRequests.find(nextImage) == renderSystem.imageLoadRequests.end())
					{
						renderSystem.imagesToLoad.push_back(nextImage);
					}
				}
			}
			rect.setTexture(&renderSystem.textures[nextImage]);
			rect.setRotation(animationLayerStates[i].angle);



			if(isMain)
			{
				float view_x = renderSystem.gameView.getCenter().x;
				float view_y = renderSystem.gameView.getCenter().y;
				renderSystem.newGameViewPos = sf::Vector2f(rect.getPosition().x * 0.05 + view_x * 0.95,
														   rect.getPosition().y * 0.05 + view_y * 0.95);
				renderSystem.cameraX = renderSystem.gameView.getCenter().x;
				renderSystem.cameraY = renderSystem.gameView.getCenter().y;
				renderSystem.playerX = rect.getPosition().x;
				renderSystem.playerY = rect.getPosition().y;
				
			}
			renderTarget.setView(renderSystem.gameView);
			if(renderData.spriteIsVisible)
			{
				rect.setScale(sf::Vector2f(renderData.spriteScale, renderData.spriteScale));
				renderTarget.draw(rect);

			}
			

		}
	}
	
	if(renderData.lightIsVisible)
	{
		if(!hasLightSource)
		{
			lightSourceID = lightManager->addLightSource(sf::Vector2f(0, 0),
				sf::Color(renderData.lightR, renderData.lightG, renderData.lightB),
				renderData.lightIntensity, 0.1);
			hasLightSource = true;
		}
		lightManager->setPosition(lightSourceID, sf::Vector2f((positionX + speedX * deltaTime) * TILE_SIZE, (positionY + speedY * deltaTime) * TILE_SIZE));
	}

	if (renderData.shouldDropShadow && ! hasShadow && sizeX > 0.0f && sizeY > 0.0f)
	{
		lightManager->addRectangleObstacle(sf::Vector2f(positionX * TILE_SIZE, positionY * TILE_SIZE),
										   sf::Vector2f(sizeX * TILE_SIZE, sizeY * TILE_SIZE));
		soundManager.setObstaclePosition(positionX, positionY, sizeX, sizeY);
		hasShadow = true;
	}
	
}


RenderSystem::RenderSystem(Console& console, GuiManager& guiManager, float screenWidth, float screenHeight) : 
	gameView(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(screenWidth / TILE_SIZE / 2, screenHeight / TILE_SIZE / 2)),
	mainActor(-1), console(console), 
	gameGuiManager(screenWidth, screenHeight, guiManager)
{

	tileset.create(TILESET_WIDTH * TILE_SIZE, TILESET_HEIGHT * TILE_SIZE);
	tileVertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	//lightManager = std::make_shared<LightManager>(screenWidth, screenHeight, TILE_SIZE);
	lightManager = std::shared_ptr<ILightManager>(LightManagerFactory::createLightManager(screenWidth, screenHeight, TILE_SIZE));

	WeaponData weaponData;
	weaponData.timeSinceReload = 0;
	weaponData.timeSinceShot = 0;
	weaponData.shotsMade = 0;

	WeaponDef weaponDef;
	weaponDef.weaponTexture = NO_WEAPON_TEXTURE;
	weaponDef.bulletsPerHolder = 10;
	weaponDef.reloadTime = 4.0f;
	weaponDef.period = 1.0f;

	WeaponUpdate weaponUpdate(weaponDef, weaponData, WeaponUpdate::WeaponState::CHANGE);
	gameGuiManager.setWeaponUpdate(weaponUpdate);
}

void RenderSystem::onUpdate(std::vector<std::shared_ptr<ActorUpdate> > updates)
{
	for(auto it = updates.begin(); it != updates.end(); it++)
	{
		if((*it)->actorID == -1)
		{
			
			for(auto update_it = (*it)->updates.begin(); update_it != (*it)->updates.end(); update_it++)
			{
				if((*update_it)->name == "delete")
				{
					
					DeleteUpdate& deleteUpdate = (DeleteUpdate&) *(*update_it);
					for(int i = 0; i < deleteUpdate.deletedActors.size(); i++)
					{
						int deletedActor = deleteUpdate.deletedActors[i];
						//std::cout << "deleted actor: " << deletedActor << std::endl;
						deletedActors.insert(deletedActor);
						if(actors.find(deletedActor) != actors.end())
						{
							
							actors.erase(deletedActor);
						}
					}
				}
			}
			continue;
		}

		if(actors.find((*it)->actorID) == actors.end())
		{
			if(deletedActors.find((*it)->actorID) != deletedActors.end())
			{
				continue;
			}
			//std::cout << "render system created actor with id: " << (*it)->actorID << std::endl;
			actors[(*it)->actorID] = std::make_shared<DrawableActor>(console, *this, soundManager, lightManager);
			if((*it)->actorID == mainActor)
			{
				actors[(*it)->actorID]->setMain(true);
			}
		}

		actors[(*it)->actorID]->onUpdate(*(*it));
		
	}
}

void RenderSystem::draw(sf::RenderTarget& renderTarget)
{
	float windowSizeX = (float) renderTarget.getSize().x / TILE_SIZE / 2;
	float windowSizeY = (float) renderTarget.getSize().y / TILE_SIZE / 2;

	gameView.setSize(windowSizeX, windowSizeY);

	renderTarget.setView(gameView);

	//sf::RenderStates renderStates(&tileset);
	renderTarget.draw(tileVertices, &tileset);

	for(auto it = actors.begin(); it != actors.end(); it++)
	{
		it->second->draw(renderTarget);
	}

	lightManager->draw(renderTarget);
	gameView.setCenter(newGameViewPos);

	gameGuiManager.onTimer();
	gameGuiManager.draw(renderTarget);

}

void RenderSystem::setMainActor(int mainActor)
{
	this->mainActor = mainActor;
}

void RenderSystem::onKey(int key, bool isReleased)
{
	gameGuiManager.onKey(key, isReleased);
}

void RenderSystem::onImageLoaded(std::string image)
{
	//check later
	if(textures.find(image) != textures.end())
	{
		sf::Texture texture;
		//std::cout << "loading image in onImageLoaded" << std::endl;
		texture.loadFromFile(image);
		textures[image] = texture;
	}
	if(imagesInTileset.find(image) != imagesInTileset.end())
	{
		sf::Image sfImage;
		//std::cout << "loading image for tileset in onImageLoaded" << std::endl;
		sfImage.loadFromFile(image);
		int pos = imagesInTileset[image];
		int tileset_x = pos % TILESET_WIDTH;
		int tileset_y = pos / TILESET_WIDTH;
		tileset.update(sfImage, tileset_x * TILE_SIZE, tileset_y * TILE_SIZE);
	}
}

float RenderSystem::getCameraX()
{
	return cameraX;
}

float RenderSystem::getCameraY()
{
	return cameraY;
}

float RenderSystem::getPlayerX()
{
	return playerX;
}

float RenderSystem::getPlayerY()
{
	return playerY;
}