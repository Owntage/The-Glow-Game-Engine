#include "gui.h"
#include <iostream>
#include <algorithm>
#include <utility>
using namespace std;

void GuiElement::makeChildOf(GuiElement& other)
{
	parent = &other;
}

void GuiElement::setX(float x)
{
	this->x = x;
}

void GuiElement::setVisible(bool visible)
{
	this->visible = visible;
}

void GuiElement::setY(float y)
{
	this->y = y;
}

void GuiElement::setRelativeX(float relativeX)
{
	this->relativeX = relativeX;
}

void GuiElement::setRelativeY(float relativeY)
{
	this->relativeY = relativeY;
}

void GuiElement::set(float x, float y)
{
	this->x = x;
	this->y = y;
}

void GuiElement::setRelative(float relativeX, float relativeY)
{
	this->relativeX = relativeX;
	this->relativeY = relativeY;
}

void GuiElement::setScaleX(float scaleX)
{
	this->scaleX = scaleX;
}

void GuiElement::setScaleY(float scaleY)
{
	this->scaleY = scaleY;
}

void GuiElement::setScale(float scaleX, float scaleY)
{
	this->scaleX = scaleX;
	this->scaleY = scaleY;
}

void GuiElement::setScaleRelativeToParent(bool isRelativeScale)
{
	this->isRelativeScale = isRelativeScale;
}

float GuiElement::getX()
{
	if(parent != nullptr)
	{
		return parent->getX() + x + parent->getScaleX() * relativeX;
		
	}
	return x;
}

float GuiElement::getY()
{
	if(parent != nullptr)
	{
		return parent->getY() + y + parent->getScaleY() * relativeY;
	}
	return y;
}

float GuiElement::getScaleX()
{
	float parentScaleX = 1.0f;
	if(parent != nullptr && isRelativeScale)
	{
		parentScaleX = parent->getScaleX();
	}
	return parentScaleX * scaleX;
}

float GuiElement::getScaleY()
{
	float parentScaleY = 1.0f;
	if(parent != nullptr && isRelativeScale)
	{
		parentScaleY = parent->getScaleY();
	}
	return parentScaleY * scaleY;
}

GuiElement* GuiElement::getParent()
{
	return parent;
}

void GuiManager::addElement(GuiElement& element)
{
	elements.push_back(&element);
	isDrawn[&element] = false;
	renderOrder[&element] = -1;
}

void GuiManager::drawGuiElement(GuiElement* guiElement, sf::RenderTarget& renderTarget)
{
	if(guiElement->getParent() != nullptr)
	{
		drawGuiElement(guiElement->getParent(), renderTarget);
	}
	if(!isDrawn[guiElement])
	{
		
		renderTexture.clear(sf::Color::Transparent);
		guiElement->draw(renderTexture);
		renderTexture.display();
		
		sf::Sprite renderSprite(renderTexture.getTexture());
		
		sf::IntRect textureRect;
		textureRect.width = guiElement->getScaleX();
		textureRect.height = guiElement->getScaleY();
		textureRect.left = guiElement->getX() - guiElement->getScaleX() / 2 + guiView.getSize().x / 2;
		textureRect.top = guiElement->getY() - guiElement->getScaleY() / 2 + guiView.getSize().y / 2;

		if(guiElement->visible)
		{
			guiElement->blinkLevel += 1.0f / 60.0f / BLINK_TIME;
		}
		else
		{
			guiElement->blinkLevel -= 1.0f / 60.0f / BLINK_TIME;
		}
		guiElement->blinkLevel = std::min(guiElement->blinkLevel, 1.0f);
		guiElement->blinkLevel = std::max(guiElement->blinkLevel, 0.0f);

		renderSprite.setTextureRect(textureRect);

		renderSprite.setOrigin(guiElement->getScaleX() / 2, guiElement->getScaleY() / 2);
		renderSprite.setPosition(guiElement->getX(), guiElement->getY());
		renderSprite.setColor(sf::Color(255,255,255, 255.0f * guiElement->blinkLevel));
		
		renderTarget.draw(renderSprite);
		
		isDrawn[guiElement] = true;
		renderOrder[guiElement] = renderCounter;
		renderCounter++;
	}
}

void GuiManager::draw(sf::RenderTarget& renderTarget)
{
	guiView.setSize(renderTarget.getSize().x, renderTarget.getSize().y);

	renderCounter = 1;
	sf::View renderView = renderTarget.getView();
	renderTarget.setView(guiView);
	for(auto it = isDrawn.begin(); it != isDrawn.end(); it++)
	{
		it->second = false;
	}
	for(auto it = elements.begin(); it != elements.end(); it++)
	{
		drawGuiElement(*it, renderTarget);
	}
	renderTarget.setView(renderView);
}

void GuiManager::onMouseMove(float x, float y)
{
	for(auto it = elements.begin(); it != elements.end(); it++)
	{
		(*it)->onMouseMove(x, y);
	}
}

bool isInRectangle(float x, float y, float rectangleCenterX, float rectangleCenterY, float scaleX, float scaleY)
{
	if(x > rectangleCenterX + scaleX / 2) return false;
	if(x < rectangleCenterX - scaleX / 2) return false;
	if(y > rectangleCenterY + scaleY / 2) return false;
	if(y < rectangleCenterY - scaleY / 2) return false;
	return true;
}

void GuiManager::onMouseClick(float x, float y, bool isReleased)
{
	if(isReleased)
	{
		for(auto it = elements.begin(); it != elements.end(); it++)
		{
			(*it)->onMouseClick(x, y, isReleased);
		}
	}
	else
	{
		vector<GuiElement*> clickedElements;
		for(auto it = elements.begin(); it != elements.end(); it++)
		{
			if(isInRectangle(x, y, (*it)->getX(), (*it)->getY(), (*it)->getScaleX(), (*it)->getScaleY()) && (*it)->usesMouse)
			{
				clickedElements.push_back(*it);
			}
		}
		std::sort(clickedElements.begin(), clickedElements.end(), [this](GuiElement* first, GuiElement* second) -> bool
		{
			return this->renderOrder[first] > this->renderOrder[second];
		});
		if(clickedElements.size() > 0)
		{
			clickedElements[0]->onMouseClick(x, y, isReleased);
		}
	}
}

bool GuiManager::isSpecialKey(unsigned int keyCode)
{
	return keyCode == sf::Keyboard::BackSpace || keyCode == sf::Keyboard::Return ||
		keyCode == sf::Keyboard::Left || keyCode == sf::Keyboard::Right || keyCode == sf::Keyboard::Up ||
		keyCode == sf::Keyboard::Down || keyCode == sf::Keyboard::Escape || keyCode == sf::Keyboard::Delete;
}

void GuiManager::onTextEntered(unsigned int key)
{
	for(auto it = elements.begin(); it != elements.end(); it++)
	{
		(*it)->onTextEntered(key);
	}
}

void GuiManager::onSpecialKey(unsigned int key)
{
	for(auto it = elements.begin(); it != elements.end(); it++)
	{
		(*it)->onSpecialKey(key);
	}
}

void GuiManager::onEvent(sf::Event& event)
{
	switch(event.type)
	{
		//todo: re-implement without using RenderWindow
		/*
		case sf::Event::MouseButtonPressed:
			onMouseClick(event.mouseButton.x - RenderWindow::getInstance()->getWidth() / 2, 
				event.mouseButton.y - RenderWindow::getInstance()->getHeight() / 2, false);
			break;
		case sf::Event::MouseButtonReleased:
			onMouseClick(event.mouseButton.x - RenderWindow::getInstance()->getWidth() / 2, 
				event.mouseButton.y - RenderWindow::getInstance()->getHeight() / 2, true);
			break;
		case sf::Event::MouseMoved:
			onMouseMove(event.mouseMove.x - RenderWindow::getInstance()->getWidth() / 2, 
				event.mouseMove.y - RenderWindow::getInstance()->getHeight() / 2);
			break;
		 */
	}
	if(event.type == sf::Event::KeyPressed && isSpecialKey(event.key.code))
	{
		onSpecialKey(event.key.code);
	}
	else
	{
		if(event.type == sf::Event::TextEntered && event.text.unicode >= 32 && event.text.unicode <= 126)
		{
			onTextEntered(event.text.unicode);
		}
	}
}


NinePatchSprite::NinePatchSprite(sf::Image& image, bool isNinePatch)
{
	setTexture(image, isNinePatch);
}

NinePatchSprite::NinePatchSprite(std::string imageName, bool isNinePatch)
{
	std::cout << "creating an image" << std::endl;
	sf::Image image;
	std::cout << "loading it from file" << std::endl;
	image.loadFromFile(imageName);
	std::cout << "image is loaded from file" << std::endl;
	setTexture(image, isNinePatch);
}

void NinePatchSprite::setTexture(sf::Image& image, bool isNinePatch)
{
	texture.loadFromImage(image);
	this->isNinePatch = isNinePatch;
	if(isNinePatch)
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				shapes[i][j].setTexture(&texture);
				
			}
		}
		int imageWidth = image.getSize().x;
		int imageHeight = image.getSize().y;
		
		//reading for x projection
		bool readingFirst = true;
		for(int i = 1; i < imageWidth - 1; i++)
		{
			sf::Color color = image.getPixel(i, 0);
			if(readingFirst == true && color != sf::Color::Black)
			{
				readingFirst = false;
				firstFixedSizeX = i - 1;
			}
			if(readingFirst == false && color == sf::Color::Black)
			{
				secondFixedSizeX = imageWidth - i - 1;
				break;
			}
		}
		
		
		
		//reading for y projection
		readingFirst = true;
		for(int i = 1; i < imageHeight - 1; i++)
		{
			sf::Color color = image.getPixel(0, i);
			if(readingFirst == true && color != sf::Color::Black)
			{
				readingFirst = false;
				firstFixedSizeY = i - 1;
			}
			if(readingFirst == false && color == sf::Color::Black)
			{
				secondFixedSizeY = imageHeight - i - 1;
				break;
			}
		}
		
		
		//setting texture rects
		int middleSizeX = imageWidth - firstFixedSizeX - secondFixedSizeX - 2;
		int middleSizeY = imageHeight - firstFixedSizeY - secondFixedSizeY - 2;
		
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				int sizeX;
				int sizeY;
				int offsetX;
				int offsetY;
				switch(i)
				{
					case 0: sizeX = firstFixedSizeX; offsetX = 1; break;
					case 1: sizeX = middleSizeX; offsetX = 1 + firstFixedSizeX; break;
					case 2: sizeX = secondFixedSizeX; offsetX = 1 + firstFixedSizeX + middleSizeX; break;
				}
				switch(j)
				{
					case 0: sizeY = firstFixedSizeY; offsetY = 1; break; 
					case 1: sizeY = middleSizeY; offsetY = 1 + firstFixedSizeY; break;
					case 2: sizeY = secondFixedSizeY; offsetY = 1 + firstFixedSizeY + middleSizeY; break;
				}
				shapes[i][j].setTextureRect(sf::IntRect(offsetX, offsetY, sizeX, sizeY));
			}
		}
		
	}
	else
	{
		shapes[1][1].setTexture(&texture);
	}
}

void NinePatchSprite::setX(float x)
{
	this->x = x;
}

void NinePatchSprite::setY(float y)
{
	this->y = y;
}

void NinePatchSprite::set(float x, float y)
{
	this->x = x;
	this->y = y;
}

void NinePatchSprite::setScaleX(float scaleX)
{
	this->scaleX = scaleX;
}

void NinePatchSprite::setScaleY(float scaleY)
{
	this->scaleY = scaleY;
}

void NinePatchSprite::setScale(float scaleX, float scaleY)
{
	this->scaleX = scaleX;
	this->scaleY = scaleY;
}

void NinePatchSprite::draw(sf::RenderTarget& renderTarget)
{
	if(isNinePatch)
	{
		float firstSizeX = (float) firstFixedSizeX;
		float firstSizeY = (float) firstFixedSizeY;
		float secondSizeX = (float) secondFixedSizeX;
		float secondSizeY = (float) secondFixedSizeY;
		float middleSizeX = scaleX - firstSizeX - secondSizeX;
		float middleSizeY = scaleY - firstSizeY - secondSizeY;
	
		
		//setting sizes, positions and origins. rendering
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				float sizeX;
				float sizeY;
				float positionX;
				float positionY;
				switch(i)
				{
					case 0:
						sizeX = firstSizeX;
						positionX = -middleSizeX / 2 - firstSizeX;
						break;
					case 1:
						sizeX = middleSizeX;
						positionX = -middleSizeX / 2;
						break;
					case 2:
						sizeX = secondSizeX;
						positionX = middleSizeX / 2.0f;
						break;
				}
				switch(j)
				{
				case 0:
					sizeY = firstSizeY;
					positionY = -middleSizeY / 2 - firstSizeY;
					break;
				case 1:
					sizeY = middleSizeY;
					positionY = -middleSizeY / 2;
					break;
				case 2:
					sizeY = secondSizeY;
					positionY = middleSizeY / 2.0f;
					break;
				}
				//shapes[i][j].setOrigin(sizeX / 2.0f, sizeY / 2.0f);
				shapes[i][j].setSize(sf::Vector2f(sizeX, sizeY));
				shapes[i][j].setPosition(x + positionX, y + positionY);
				
				//RenderWindow::getInstance()->window.draw(shapes[i][j]);
				
				renderTarget.draw(shapes[i][j]);

			}
		}
		
		
	}
	else
	{
		shapes[1][1].setSize(sf::Vector2f(scaleX, scaleY));
		shapes[1][1].setPosition(x, y);
		shapes[1][1].setOrigin(scaleX / 2, scaleY / 2);
		//RenderWindow::getInstance()->window.draw(shapes[1][1]);
		renderTarget.draw(shapes[1][1]);
	}
}

NinePatchSprite& WindowPanel::getActiveSprite()
{
	return *activeSprite;
}

void WindowPanel::draw(sf::RenderTarget& renderTarget)
{
	NinePatchSprite& sprite = getActiveSprite();
	sprite.set(getX(), getY());
	sprite.setScale(getScaleX(), getScaleY());
	sprite.draw(renderTarget);
}

void MovableWindowPanel::onMouseClick(float x, float y, bool isReleased)
{
	if(isReleased)
	{
		isCaptured = false;
	}
	else
	{
		isCaptured = true;
		lastMouseX = x;
		lastMouseY = y;
	}
}

void MovableWindowPanel::onMouseMove(float x, float y)
{
	if(isCaptured)
	{
		setX(getRelativeToParentX() + x - lastMouseX);
		setY(getRelativeToParentY() + y - lastMouseY);
		lastMouseX = x;
		lastMouseY = y;
	}
}

NinePatchSprite& Button::getActiveSprite()
{
	switch(state)
	{
		case ButtonState::normal: return *normal; break;
		case ButtonState::hovered: return *hovered; break;
		case ButtonState::pressed: return *pressed; break;
	}
}

void Button::setNormalSprite(NinePatchSprite& normal)
{
	this->normal = &normal;
}

void Button::setHoveredSprite(NinePatchSprite& hovered)
{
	this->hovered = &hovered;
}

void Button::setPressedSprite(NinePatchSprite& pressed)
{
	this->pressed = &pressed;
}

void Button::setOnPressCallback(std::function<void(void)>&& onPressCallback)
{
	this->onPressCallback = onPressCallback;
}

void Button::setOnReleaseCallback(std::function<void(void)>&& onReleaseCallback)
{
	this->onReleaseCallback = onReleaseCallback;
}

void Button::onMouseMove(float x, float y)
{
	switch(state)
	{
		case ButtonState::normal:
			if(isInRectangle(x, y, getX(), getY(), getScaleX(), getScaleY()))
			{
				state = ButtonState::hovered;
			}
			break;
		case ButtonState::hovered:
			if(!isInRectangle(x, y, getX(), getY(), getScaleX(), getScaleY()))
			{
				state = ButtonState::normal;
			}
			break;
		case ButtonState::pressed:
			if(!isInRectangle(x, y, getX(), getY(), getScaleX(), getScaleY()))
			{
				state = ButtonState::normal;
			}
			break;
	}
}

void Button::onMouseClick(float x, float y, bool isReleased)
{
	switch(state)
	{
		case ButtonState::normal:
			if(!isReleased)
			{
				state = ButtonState::pressed;
				onPressCallback();
			}
			break;
		case ButtonState::hovered:
			if(!isReleased)
			{
				state = ButtonState::pressed;
				onPressCallback();
			}
			break;
		case ButtonState::pressed:
			if(isReleased)
			{
				state = ButtonState::hovered;
				onReleaseCallback();
			}
			break;
	}
}

void AbstractTextView::setText(std::string text)
{
	this->text = text;
}

std::string AbstractTextView::getText()
{
	return text;
}

void AbstractTextView::setCharacterSize(int characterSize)
{
	this->characterSize = characterSize;
}

void AbstractTextView::setColor(sf::Color color)
{
	this->color = color;
}

void AbstractTextView::formatText()
{
	float characterWidth;
	sf::Text oneCharacter;
	oneCharacter.setCharacterSize(characterSize);
	oneCharacter.setFont(Font::getInstance()->font);
	oneCharacter.setString("R");
	characterWidth = oneCharacter.getLocalBounds().width;
	int charactersPerLine = getScaleX() / characterWidth;

	int counter = 0;
	for(int i = 0; i < this->text.size(); i++)
	{
		if(this->text[i] == '\n')
		{
			counter = 0;
		}
		else
		{
			counter++;
		}
		if(counter > charactersPerLine)
		{
			this->text.insert(this->text.begin() + i, '\n');
			counter = 0;
		}
	}
}



void TextView::draw(sf::RenderTarget& renderTarget)
{
	formatText();
	sf::Text text;
	text.setFont(Font::getInstance()->font);
	text.setString(this->text);
	text.setCharacterSize(characterSize);
	text.setColor(color);
	text.setPosition(getX(), getY());
	text.setOrigin(getScaleX() / 2, getScaleY() / 2);
	renderTarget.draw(text);
}



Slider::Slider(float x, float y, float scaleX, float scaleY, bool isHorizontal, std::string backgroundNinePatch,
			   std::string sliderNinePatch, float sliderScale) : GuiElement(x, y, scaleX, scaleY), sliderScale(sliderScale),
			   normalizedSliderPosition(0.95f), isHorizontal(isHorizontal)
{
	usesMouse = true;
	moveCallback = [](float x){};
	state = SliderStates::RELEASED;
	sf::Image backgroundImage;
	backgroundImage.loadFromFile(backgroundNinePatch);
	backgroundSprite = std::make_shared<NinePatchSprite>(backgroundImage, true);

	sf::Image sliderImage;
	sliderImage.loadFromFile(sliderNinePatch);
	sliderSprite = std::make_shared<NinePatchSprite>(sliderImage, true);
}

void Slider::setMoveCallback(std::function<void(float)> moveCallback)
{
	this->moveCallback = moveCallback;
}

void Slider::draw(sf::RenderTarget& renderTarget)
{
	backgroundSprite->set(getX(), getY());
	backgroundSprite->setScale(getScaleX(), getScaleY());
	backgroundSprite->draw(renderTarget);

	if(isHorizontal)
	{
		sliderSprite->set(getX() + (normalizedSliderPosition - 0.5) * getScaleX(), getY());
		sliderSprite->setScale(sliderScale, getScaleY());
	}
	else
	{
		sliderSprite->set(getX(), getY() + (normalizedSliderPosition - 0.5) * getScaleY());
		sliderSprite->setScale(getScaleX(), sliderScale);	
	}
	sliderSprite->draw(renderTarget);
}

void Slider::onMouseMove(float x, float y)
{
	if(state == SliderStates::PRESSED)
	{
		if(isHorizontal)
		{
			normalizedSliderPosition = max(0.05, min(0.95, (x - getX()) / getScaleX() + 0.5));
		}
		else
		{
			normalizedSliderPosition = max(0.05, min(0.95, (y - getY()) / getScaleY() + 0.5));
		}
		moveCallback(normalizedSliderPosition);
	}
}

void Slider::onMouseClick(float x, float y, bool isReleased)
{
	if(isReleased)
	{
		state = SliderStates::RELEASED;
	}
	else
	{
		if(isHorizontal)
		{
			if(x > getX() + (normalizedSliderPosition - 0.5) * getScaleX() - sliderScale * 0.5 && x < getX() + (normalizedSliderPosition - 0.5) * getScaleX() + sliderScale * 0.5)
			{
				state = SliderStates::PRESSED;
			}
		}
		else
		{
			if(y > getY() + (normalizedSliderPosition - 0.5) * getScaleY() - sliderScale * 0.5 && y < getY() + (normalizedSliderPosition - 0.5) * getScaleY() + sliderScale * 0.5)
			{
				state = SliderStates::PRESSED;
			}
		}
	}
}

ScrollingTextView::ScrollingTextView(float x, float y, float scaleX, float scaleY, std::string sliderBackgroundNinePatch, std::string sliderNinePatch) :
	AbstractTextView(x, y, scaleX, scaleY), slider(0, 0, 0, 0, false, sliderBackgroundNinePatch, sliderNinePatch, 20)
{
	usesMouse = true;
		
	sliderNormalizedPosition = 1.0;
	slider.setMoveCallback([this](float pos)
	{
		this->sliderNormalizedPosition = (pos - 0.05) / 0.9;
	});
}


void ScrollingTextView::draw(sf::RenderTarget& renderTarget)
{
	formatText();
	sf::Text text;
	text.setFont(Font::getInstance()->font);
	text.setString(this->text);
	text.setCharacterSize(characterSize);
	text.setColor(color);
	
	text.setOrigin(getScaleX() / 2, getScaleY() / 2);
	
	float deltaY = 0;
	if(text.getGlobalBounds().height > getScaleY())
	{
		deltaY = (text.getGlobalBounds().height - getScaleY()) * sliderNormalizedPosition + characterSize * sliderNormalizedPosition;
	}

	text.setPosition(getX(), getY() - deltaY);

	slider.setScale(10, getScaleY());
	slider.set(getX() + getScaleX() / 2 - slider.getScaleX() / 2, getY());
	
	slider.draw(renderTarget);

	renderTarget.draw(text);
}

void ScrollingTextView::onMouseMove(float x, float y)
{
	slider.onMouseMove(x, y);
}

void ScrollingTextView::onMouseClick(float x, float y, bool isReleased)
{
	if(!isReleased)
	{
		if(x > getX() + getScaleX() / 2 - slider.getScaleX() && x < getX() + getScaleX() / 2)
		{
			slider.onMouseClick(x, y, isReleased);
		}
	}
	else
	{
		slider.onMouseClick(x, y, isReleased);
	}
}

void InputField::setInputCallback(std::function<void(std::string)> inputCallback)
{
	this->inputCallback = inputCallback;
}

void InputField::setCharacterSize(int characterSize)
{
	this->characterSize = characterSize;
}

void InputField::setTextColor(sf::Color textColor)
{
	this->textColor = textColor;
}

void InputField::draw(sf::RenderTarget& renderTarget)
{

	if(std::chrono::system_clock::now() > time)
	{
		time = time + std::chrono::milliseconds(300);
		isVisibleSymbol = !isVisibleSymbol;
	}

	NinePatchSprite* sprite;
	switch(state)
	{
		case InputFieldStates::NORMAL:
			sprite = &normalSprite;
			break;
		case InputFieldStates::HOVERED:
			sprite = &hoveredSprite;
			break;
		case InputFieldStates::ACTIVE:
			sprite = &activeSprite;
			break;
	}
	sprite->set(getX(), getY());
	sprite->setScale(getScaleX(), getScaleY());
	sprite->draw(renderTarget);

	sf::Text text;
	std::string renderInput = input;
	if(isVisibleSymbol && state == InputFieldStates::ACTIVE)
	{
		//renderInput += '|';
		renderInput.insert(renderInput.end() - cursorPos, '|');
	}
	else
	{
		//renderInput += ' ';
		renderInput.insert(renderInput.end() - cursorPos, ' ');
	}
	text.setFont(Font::getInstance()->font);
	text.setCharacterSize(characterSize);
	text.setColor(textColor);
	text.setOrigin(getScaleX() / 2, getScaleY() / 2);
	text.setString(renderInput);

	float deltaX = 0.0f;
	if(text.getGlobalBounds().width > getScaleX())
	{
		deltaX = text.getGlobalBounds().width - getScaleX();
	}
	text.setPosition(getX() - deltaX, getY());
	renderTarget.draw(text);
}

void InputField::onMouseClick(float x, float y, bool isReleased)
{
	if(!isReleased)
	{
		state = InputFieldStates::ACTIVE;
	}
}

void InputField::onMouseMove(float x, float y)
{
	switch(state)
	{
		case InputFieldStates::NORMAL:
			if(isInRectangle(x, y, getX(), getY(), getScaleX(), getScaleY()))
			{
				state = InputFieldStates::HOVERED;	
			}
			break;
		case InputFieldStates::HOVERED:
			if(!isInRectangle(x, y, getX(), getY(), getScaleX(), getScaleY()))
			{
				state = InputFieldStates::NORMAL;
			}
			break;
	}
}

void InputField::onTextEntered(unsigned int code)
{
	if(state == InputFieldStates::ACTIVE)
	{
		//input += (char) code;
		input.insert(input.end() - cursorPos, (char) code);
	}
}

void InputField::onSpecialKey(unsigned int key)
{
	if(state == InputFieldStates::ACTIVE)
	{
		switch(key)
		{
			case sf::Keyboard::Escape:
				state = InputFieldStates::NORMAL;
				break;
			case sf::Keyboard::BackSpace:
				if(cursorPos != input.size())
				{
					input.erase(input.end() - 1 - cursorPos);
				}
				break;
			case sf::Keyboard::Return:
				inputCallback(input);
				input.clear();
				state = InputFieldStates::NORMAL;
				break;
			case sf::Keyboard::Left:
				if(cursorPos < input.size())
				{
					cursorPos++;
				}
				break;
			case sf::Keyboard::Right:
				if(cursorPos > 0)
				{
					cursorPos--;
				}
				break;
			case sf::Keyboard::Delete:
				if(cursorPos != 0)
				{
					input.erase(input.end() - cursorPos);
					cursorPos--;
				}
				break;
		}
	}
	else
	{
		switch(key)
		{
			case sf::Keyboard::Return:
				state = InputFieldStates::ACTIVE;
				break;
		}
	}
}


OutputConsole::OutputConsole(float x, float y, float scaleX, float scaleY, GuiManager& guiManager)
{
	backgroundSprite = std::make_shared<NinePatchSprite>("res/gui/console/background.png", true);
	background = std::make_shared<WindowPanel>(x, y - 9, scaleX, scaleY - 18, *backgroundSprite);
	guiManager.addElement(*background);
	std::shared_ptr<ScrollingTextView> tempScrollPtr(new ScrollingTextView(0, 0, scaleX, scaleY - 18, "res/gui/console/input_background.png", "res/gui/console/slider.png"));
	scrollingTextView = tempScrollPtr;
	scrollingTextView->makeChildOf(*background);
	scrollingTextView->setCharacterSize(24);
	guiManager.addElement(*scrollingTextView);
	
}

void OutputConsole::println(std::string text)
{
	text.append("\n");
	scrollingTextView->setText(scrollingTextView->getText() + text);
}

void OutputConsole::clear()
{
	scrollingTextView->setText("");
}

void OutputConsole::setVisible(bool visible)
{
	background->setVisible(visible);
	scrollingTextView->setVisible(visible);
}

Console::Console(float x, float y, float scaleX, float scaleY, GuiManager& guiManager) :
	OutputConsole(x, y, scaleX, scaleY, guiManager)
{
	std::shared_ptr<InputField> tempInputPtr(new InputField(0,scaleY / 2, scaleX, 20, "res/gui/console/input_background.png", "res/gui/console/input_hovered_background.png", "res/gui/console/input_active_background.png"));
	inputField = tempInputPtr;
	inputField->makeChildOf(*background);
	guiManager.addElement(*inputField);	
}

void Console::setInputCallback(std::function<void(string)> inputCallback)
{
	inputField->setInputCallback(inputCallback);
}

void Console::setVisible(bool visible)
{
	OutputConsole::setVisible(visible);
	inputField->setVisible(visible);
}