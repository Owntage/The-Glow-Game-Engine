#include "tile_component.h"
#include "coord_event.h"
#include "string_event.h"
#include "variant_update.h"

void TileComponent::onEvent(const Event& event)
{
	if(event.name == "set_coords")
	{
		const CoordEvent& coordEvent = (const CoordEvent&) event;
		x = coordEvent.x;
		y = coordEvent.y;
	}
	if(event.name == "set_image")
	{
		const StringEvent& stringEvent = (const StringEvent&) event;
		image = stringEvent.str;
	}
}

bool TileComponent::hasUpdate(int systemID)
{
	return lastSystemApproved.find(systemID) == lastSystemApproved.end();
}

std::string TileComponent::getName()
{
	return "tile";
}

std::shared_ptr<ComponentUpdate> TileComponent::getUpdate(int syatemID)
{
	//auto result = std::make_shared<TileUpdate>(image, x, y);
	auto result = std::make_shared<VariantUpdate>("tile");
	result->set("image", image);
	result->set("x", x);
	result->set("y", y);
	result->number = 1;
	return result;
}

std::shared_ptr<IComponent> TileComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	return std::make_shared<TileComponent>();
}

