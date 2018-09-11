//
// Created by Матвей on 15.12.2017.
//
using namespace std;

#include <cmath>
#include "sound_manager.h"

bool SoundManager::loadSound(std::string filename, bool low)
{
    std::string lowname = filename.substr(0, filename.find(".")) + "_low" + filename.substr(filename.find("."));

    if (buffers.count(filename) == 0)
    {
        sf::SoundBuffer buffer;

        if (!buffer.loadFromFile(filename))
            return false;

        buffers.insert(std::pair<std::string, sf::SoundBuffer>(filename, buffer));

        if (!buffer.loadFromFile(lowname))
            return false;

        buffers.insert(std::pair<std::string, sf::SoundBuffer>(lowname, buffer));
    }

    sound.setBuffer(buffers.find(low ? lowname : filename) -> second);

    return true;
}

bool SoundManager::playSound(float x, float y, std::string filename)
{
    int obstacle_count = 0;

    for (int i = 0; i < obstacles.size(); i++)
    {
        if (checkLine(obstacles[i], x, y))
            obstacle_count++;
    }

    if(!loadSound(filename, obstacle_count > 0 || distanceToListener(x, y) > lowpass_distance))
        return false;

	float volume = volume_const * (float) (1.0f / pow(distanceToListener(x, y), 2));
    float obstacle_multiplier = 1.0f / (float) pow(obstacle_count + 1, 1);

    sound.setVolume(obstacle_multiplier * 0.5f * (volume < 2.5 ? 0 : volume));

	sound.play();

	return true;
}

void SoundManager::setListenerPosition(float x, float y)
{
	listenerX = x;
	listenerY = y;
}

void SoundManager::setObstaclePosition(float centerX, float centerY, float sizeX, float sizeY)
{
	obstacles.push_back(ObstacleData(centerX, centerY, sizeX, sizeY));
}

float SoundManager::distanceToListener(float x, float y)
{
	return (float) sqrt(pow(listenerX - x, 2) + pow(listenerY - y, 2));
}

bool SoundManager::checkLine(ObstacleData obstacle, float x, float y)
{
	float half_x = obstacle.sizeX / 2, half_y = obstacle.sizeY / 2;

	return lineIntersect(obstacle.centerX - half_x, obstacle.centerY - half_y, obstacle.centerX + half_x,
						 obstacle.centerY - half_y, x, y, listenerX, listenerY) ||
		   lineIntersect(obstacle.centerX - half_x, obstacle.centerY - half_y, obstacle.centerX - half_x,
						 obstacle.centerY + half_y, x, y, listenerX, listenerY) ||
		   lineIntersect(obstacle.centerX + half_x, obstacle.centerY - half_y, obstacle.centerX + half_x,
						 obstacle.centerY + half_y, x, y, listenerX, listenerY) ||
		   lineIntersect(obstacle.centerX - half_x, obstacle.centerY + half_y, obstacle.centerX + half_x,
						 obstacle.centerY + half_y, x, y, listenerX, listenerY);
}

bool SoundManager::lineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	return intersect(x1, x2, x3, x4) &&
		   intersect(y1, y2, y3, y4) &&
		   (areaSign(x1, y1, x2, y2, x3, y3) ^ areaSign(x1, y1, x2, y2, x4, y4)) &&
		   (areaSign(x3, y3, x4, y4, x1, y1) ^ areaSign(x3, y3, x4, y4, x2, y2));
}

bool SoundManager::areaSign(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return std::signbit((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1)) == 0;
}

bool SoundManager::intersect(float a, float b, float c, float d)
{
	if (a > b)
		std::swap(a, b);

	if (c > d)
		std::swap(c, d);

	return std::max(a, c) <= std::min(b, d);
}