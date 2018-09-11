#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include <stdint.h>
#include "icomponent.h"
#include "component_update.h"

#define RENDER_COMPONENT_NAME "render"

struct RenderData
{
	RenderData() :
		spriteIsVisible(true),
		lightIsVisible(false),
		shouldDropShadow(false),
		spriteScale(1.0f)
	{}

	RenderData(
		bool spriteIsVisible,
		bool lightIsVisible,
		bool shouldDropShadow,
		float lightR,
		float lightG,
		float lightB,
		float lightIntensity,
		float spriteScale) :

		spriteIsVisible(spriteIsVisible),
		lightIsVisible(lightIsVisible),
		shouldDropShadow(shouldDropShadow),
		lightR(lightR),
		lightG(lightG),
		lightB(lightB),
		lightIntensity(lightIntensity),
		spriteScale(spriteScale)
	{}

	bool spriteIsVisible;
	bool lightIsVisible;
	bool renderLightOnTop;
	bool shouldDropShadow;
	float lightR;
	float lightG;
	float lightB;
	float lightIntensity;
	float spriteScale;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, RenderData& data)
	{
		s << (uint8_t) data.spriteIsVisible << (uint8_t) data.lightIsVisible << (uint8_t) data.shouldDropShadow
		  << (uint8_t) data.renderLightOnTop << data.lightR << data.lightG << data.lightB << data.lightIntensity
		  << data.spriteScale;
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, RenderData& data)
	{
		s >> (uint8_t&) data.spriteIsVisible >> (uint8_t&) data.lightIsVisible >> (uint8_t&) data.shouldDropShadow
		  >> (uint8_t&) data.renderLightOnTop >> data.lightR >> data.lightG >> data.lightB >> data.lightIntensity
		  >> data.spriteScale;
		return s;
	}

};



struct RenderUpdate : ComponentUpdate
{
	RenderUpdate(RenderData& renderData)
		: ComponentUpdate("render"),
		renderData(renderData)
	{}
	RenderUpdate()
		: ComponentUpdate("render")
	{}
	RenderData renderData;

	template<typename STREAM_T>
	friend STREAM_T& operator<<(STREAM_T& s, RenderUpdate& renderUpdate)
	{
		s << renderUpdate.renderData;
		return s;
	}

	template<typename STREAM_T>
	friend STREAM_T& operator>>(STREAM_T& s, RenderUpdate& renderUpdate)
	{
		s >> renderUpdate.renderData;
		return s;
	}
};

struct RenderComponent : IComponent
{

	void onEvent(const Event& event) {}
	bool hasUpdate(int systemID);
	std::string getName() { return RENDER_COMPONENT_NAME; }
	std::shared_ptr<ComponentUpdate> getUpdate(int syatemID);
	std::shared_ptr<IComponent> loadFromXml(const boost::property_tree::ptree& tree);
private:
	RenderData renderData;

};

#endif