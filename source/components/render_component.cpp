#include "render_component.h"


bool RenderComponent::hasUpdate(int systemID)
{
	return lastSystemApproved.find(systemID) == lastSystemApproved.end();
}

std::shared_ptr<ComponentUpdate> RenderComponent::getUpdate(int syatemID)
{
	return std::make_shared<RenderUpdate>(renderData);
}

std::shared_ptr<IComponent> RenderComponent::loadFromXml(const boost::property_tree::ptree& tree)
{
	auto result = std::make_shared<RenderComponent>();
	result->renderData.spriteIsVisible = tree.get("sprite_is_visible", "false") == "true";
	result->renderData.lightIsVisible = tree.get("light_is_visible", "false") == "true";
	result->renderData.renderLightOnTop = tree.get("light_on_top", "false") == "true";
	result->renderData.shouldDropShadow = tree.get("drop_shadow", "false") == "true";
	result->renderData.lightR = tree.get("light_r", 255.0f);
	result->renderData.lightG = tree.get("light_g", 255.0f);
	result->renderData.lightB = tree.get("light_b", 255.0f);
	result->renderData.lightIntensity = tree.get("light_intensity", 1.0f);
	result->renderData.spriteScale = tree.get("sprite_scale", 1.0f);
	return result;
}
