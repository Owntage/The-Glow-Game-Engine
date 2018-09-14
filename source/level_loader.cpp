#include "level_loader.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <map>
#include "components/coord_event.h"
#include "components/string_event.h"
#include <SFML/System.hpp>

using boost::property_tree::ptree;

LevelLoader::LevelLoader(GameLogic& gameLogic):
	gameLogic(gameLogic) {}

bool LevelLoader::loadLevel(std::string path, std::string levelName)
{
	ptree level;
	std::string fullName;
	if (path.size() > 0)
	{
		fullName = path + "/" + levelName;
	}
	else
	{
		fullName = levelName;
	}

	sf::FileInputStream fileInputStream;
	fileInputStream.open(fullName);
	std::stringstream stringstream;
	for (int i = 0; i < fileInputStream.getSize(); i++)
	{
		char c;
		fileInputStream.read(&c, 1);
		stringstream << c;
	}
	read_xml(stringstream, level);

	int width;
	int height;
	level = level.get_child("map");
	std::map<int, std::string> tiles;
	width = level.get<int>("<xmlattr>.width");
	height = level.get<int>("<xmlattr>.height");
	auto tileset = level.get_child("tileset");
	int offset = tileset.get<int>("<xmlattr>.firstgid");
	BOOST_FOREACH(auto& tilesetChild, tileset)
	{
		if(tilesetChild.first == "tile")
		{
			int id = tilesetChild.second.get<int>("<xmlattr>.id") + offset;
			tiles[id] = tilesetChild.second.get<std::string>("image.<xmlattr>.source");
		}
	}

	//parsing tiles
	boost::property_tree::ptree layer = level.get_child("layer");
	int counter = 0;
	BOOST_FOREACH(auto& tile, layer.get_child("data"))
	{
		if(tile.second.get<int>("<xmlattr>.gid") != 0)
		{
			//backgrounds.push_back(new Background((double)(counter % width), (double)(counter / height), tiles[tile.second.get<int>("<xmlattr>.gid")], 0.0));
			int tileActor = gameLogic.createActor("tileActor");
			gameLogic.onEvent(CoordEvent("set_coords", tileActor,(counter % width) + 0.5f, (counter / width) + 0.5f));
			gameLogic.onEvent(StringEvent("set_image", tileActor, path + "/" + tiles[tile.second.get<int>("<xmlattr>.gid")]));
		}
		counter++;
	}

	//parsing objects
	auto objectGroup = level.get_child("objectgroup");
	BOOST_FOREACH(auto& object, objectGroup)
	{
		if(object.first == "object")
		{
			double x = (double)object.second.get<double>("<xmlattr>.x") / SPRITE_SIZE;
			double y = (double) object.second.get<double>("<xmlattr>.y") / SPRITE_SIZE;
			double width;
			double height;
			try
			{
				width = (double)object.second.get<double>("<xmlattr>.width") / SPRITE_SIZE;
				height = (double)object.second.get<double>("<xmlattr>.height") / SPRITE_SIZE;
			} catch (...)
			{
				width = 0.3;
				height = 0.3;
			}
			std::string type = object.second.get<std::string>("<xmlattr>.type");
			//std::string name = object.second.get<std::string>("<xmlattr>.name");
			int objectActor = gameLogic.createActor(type);
			gameLogic.onEvent(CoordEvent("set_coords", objectActor, x + width / 2, y + height / 2));
			gameLogic.onEvent(CoordEvent("set_scale", objectActor, width, height));
			std::cout << "created object: " << type << std::endl;
		}
	}

	return true;


	
}

void LevelLoader::deleteAllLevels()
{
	for(int i = 0; i < objects.size(); i++)
	{
		gameLogic.destroyActor(objects[i]);
	}
}