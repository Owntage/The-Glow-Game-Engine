#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include <thread>
#include "singleton.h"

#define FONT_NAME "res/gui/UbuntuMono-R.ttf"

#define BLINK_TIME 0.2

struct GuiElement
{
	
	GuiElement(float x, float y, float scaleX, float scaleY): 
		x(x), 
		y(y), 
		scaleX(scaleX), 
		scaleY(scaleY),
		isRelativeScale(false), 
		relativeX(0.0), 
		relativeY(0.0), 
		parent(nullptr), 
		usesMouse(false),
		visible(true),
		blinkLevel(0.0f)
	{}
	GuiElement(): 
		x(0.0f), 
		y(0.0f), 
		scaleX(0.0f), 
		scaleY(0.0f), 
		isRelativeScale(false), 
		relativeX(0.0f), 
		relativeY(0.0f), 
		parent(nullptr),
		visible(true),
		usesMouse(false),
		blinkLevel(0.0f)
	{}
	virtual void draw(sf::RenderTarget& renderTarget) = 0;
	virtual void onMouseMove(float x, float y) {}
	virtual void onMouseClick(float x, float y, bool isReleased) {}
	virtual void onTextEntered(unsigned int code) {}
	virtual void onSpecialKey(unsigned int key) {}
	void makeChildOf(GuiElement& other);
	void setX(float x);
	void setY(float y);
	void setRelativeX(float relativeX);
	void setRelativeY(float relativeY);
	void set(float x, float y);
	void setRelative(float relativeX, float relativeY);
	void setScaleX(float scaleX);
	void setScaleY(float scaleY);
	void setScale(float scaleX, float scaleY);
	void setScaleRelativeToParent(bool isRelativeScale);
	float getX();
	float getY();
	float getScaleX();
	float getScaleY();
	void setVisible(bool visible);
	GuiElement* getParent();
	bool usesMouse;
protected:
	virtual ~GuiElement() {}
	float getRelativeToParentX() { return x; }
	float getRelativeToParentY() { return y; }
	
private:
	float blinkLevel;
	float x;
	float y;
	float relativeX;
	float relativeY;
	float scaleX;
	float scaleY;
	bool isRelativeScale;
	bool visible;
	GuiElement* parent;
	friend class GuiManager;
};

struct GuiManager
{
	GuiManager(float screenWidth, float screenHeight) : guiView(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(screenWidth, screenHeight))
	{
		renderTexture.create(screenWidth, screenHeight);
		renderTexture.setView(guiView);
	}
	void addElement(GuiElement& element);
	void draw(sf::RenderTarget& renderTarget);
	void onMouseMove(float x, float y);
	void onMouseClick(float x, float y, bool isReleased);
	void onTextEntered(unsigned int code);
	void onSpecialKey(unsigned int key);
	void onEvent(sf::Event& event);
private:
	bool isSpecialKey(unsigned int keyCode);
	void drawGuiElement(GuiElement* element, sf::RenderTarget& renderTarget);
	std::map<GuiElement*, bool> isDrawn;
	std::map<GuiElement*, int> renderOrder;
	int renderCounter;
	sf::View guiView;
	sf::RenderTexture renderTexture;
	std::vector<GuiElement*> elements;
};

struct NinePatchSprite
{
	NinePatchSprite(sf::Image& image, bool isNinePatch);
	NinePatchSprite() : isNinePatch(false) {}
	NinePatchSprite(std::string imageName, bool isNinePatch);
	void setTexture(sf::Image& image, bool isNinePatch);
	void setX(float x);
	void setY(float y);
	void set(float x, float y);
	void setScaleX(float scaleX);
	void setScaleY(float scaleY);
	void setScale(float scaleX, float scaleY);
	void draw(sf::RenderTarget& renderTarget);
	
private:
	sf::Texture texture;
	sf::RectangleShape shapes[3][3];
	float x;
	float y;
	float scaleX;
	float scaleY;
	int firstFixedSizeX;
	int secondFixedSizeX;
	int firstFixedSizeY;
	int secondFixedSizeY;
	bool isNinePatch;
};

struct WindowPanel : GuiElement
{
	WindowPanel(float x, float y, float scaleX, float scaleY, NinePatchSprite& background) : GuiElement(x, y, scaleX, scaleY),
		activeSprite(&background) {}
	void draw(sf::RenderTarget& renderTarget);
	virtual NinePatchSprite& getActiveSprite();
protected:
	WindowPanel(float x, float y, float scaleX, float scaleY) : GuiElement(x, y, scaleX, scaleY), activeSprite(nullptr) {}
private:
	NinePatchSprite* activeSprite;
};

struct MovableWindowPanel : WindowPanel
{
	MovableWindowPanel(float x, float y, float scaleX, float scaleY, NinePatchSprite& background) : 
		WindowPanel(x, y, scaleX, scaleY, background),	isCaptured(false) { usesMouse = true; }
	void onMouseMove(float x, float y);
	void onMouseClick(float x, float y, bool isReleased);
private:
	bool isCaptured;
	float lastMouseX;
	float lastMouseY;
};

struct Button : WindowPanel
{
	Button(float x, float y, float scaleX, float scaleY) : WindowPanel(x, y, scaleX, scaleY),
		normal(nullptr), hovered(nullptr), pressed(nullptr), state(ButtonState::normal),
		onPressCallback([](){}), onReleaseCallback([](){})	{ usesMouse = true; }
	NinePatchSprite& getActiveSprite();
	void setNormalSprite(NinePatchSprite& normalSprite);
	void setHoveredSprite(NinePatchSprite& hoveredSprite);
	void setPressedSprite(NinePatchSprite& pressedSprite);
	void setOnPressCallback(std::function<void(void)>&& onPressCallback);
	void setOnReleaseCallback(std::function<void(void)>&& onReleaseCallback);
	void onMouseMove(float x, float y);
	void onMouseClick(float x, float y, bool isReleased);
private:
	NinePatchSprite* normal;
	NinePatchSprite* hovered;
	NinePatchSprite* pressed;
	std::function<void(void)> onPressCallback;
	std::function<void(void)> onReleaseCallback;
	enum class ButtonState
	{
		normal,
		hovered,
		pressed
	};
	ButtonState state;
	
};

struct Font : Singleton<Font>
{
	sf::Font font;
	float characterHeight;
	Font()
	{
		font.loadFromFile(FONT_NAME);
	}
};

struct AbstractTextView : GuiElement
{
public:
	virtual void draw(sf::RenderTarget& renderTarget) = 0;
	void setText(std::string text);
	std::string getText();
	void setCharacterSize(int size);
	void setColor(sf::Color color);
protected:
	AbstractTextView(float x, float y, float scaleX, float scaleY) : GuiElement(x, y, scaleX, scaleY), color(sf::Color::Cyan), characterSize(16) {}
	void formatText();
	std::string text;
	int characterSize;
	sf::Color color;
	
};

struct TextView : AbstractTextView
{
	TextView(float x, float y, float scaleX, float scaleY) : AbstractTextView(x, y, scaleX, scaleY) {}
	void draw(sf::RenderTarget& renderTarget);
};


struct Slider : GuiElement
{
	Slider(float x, float y, float scaleX, float scaleY, bool isHorizontal, std::string backgroundNinePatch, std::string sliderNinePatch, float sliderScale);
	void draw(sf::RenderTarget& renderTarget);
	void onMouseMove(float x, float y);
	void onMouseClick(float x, float y, bool isReleased);
	void setMoveCallback(std::function<void(float)> moveCallback);
private:
	enum class SliderStates
	{
		PRESSED,
		RELEASED
	};
	SliderStates state;
	float sliderScale;
	float normalizedSliderPosition;
	bool isHorizontal;
	std::function<void(float)> moveCallback;
	std::shared_ptr<NinePatchSprite> backgroundSprite;
	std::shared_ptr<NinePatchSprite> sliderSprite;
	
};

struct ScrollingTextView : AbstractTextView
{
	ScrollingTextView(float x, float y, float scaleX, float scaleY,
		std::string sliderBackgroundNinePatch, std::string sliderNinePatch);
	void draw(sf::RenderTarget& renderTarget);
	void onMouseMove(float x, float y);
	void onMouseClick(float x, float y, bool isReleased);
private:
	Slider slider;
	float sliderNormalizedPosition;
};

struct InputField : GuiElement 
{
	InputField(float x, float y, float scaleX, float scaleY, std::string normalNinePatch, std::string hoveredNinePatch, std::string activeNinePatch) : 
		GuiElement(x, y, scaleX, scaleY), normalSprite(normalNinePatch, true), hoveredSprite(hoveredNinePatch, true), activeSprite(activeNinePatch, true),
		inputCallback([](std::string s){}), state(InputFieldStates::NORMAL), textColor(sf::Color::White), characterSize(16), isVisibleSymbol(false),
		time(std::chrono::system_clock::now()), cursorPos(0) { usesMouse = true; }
	void setInputCallback(std::function<void(std::string)> inputCallback);
	void draw(sf::RenderTarget& renderTarget);
	void onMouseClick(float x, float y, bool isReleased);
	void onMouseMove(float x, float y);
	void onTextEntered(unsigned int code);
	void onSpecialKey(unsigned int key);
	void setCharacterSize(int characterSize);
	void setTextColor(sf::Color textColor);

private:
	std::string input;
	NinePatchSprite normalSprite;
	NinePatchSprite hoveredSprite;
	NinePatchSprite activeSprite;
	enum class InputFieldStates
	{
		NORMAL,
		HOVERED,
		ACTIVE
	};
	InputFieldStates state;
	std::function<void(std::string)> inputCallback;
	int characterSize;
	sf::Color textColor;
	std::chrono::time_point<std::chrono::system_clock> time;
	bool isVisibleSymbol;
	int cursorPos;
};



struct OutputConsole
{
	OutputConsole(float x, float y, float scaleX, float scaleY, GuiManager& guiManager);
	void println(std::string text);
	void clear();
	void setVisible(bool visible);
protected:
	std::shared_ptr<WindowPanel> background;
private:
	std::shared_ptr<NinePatchSprite> backgroundSprite;
	std::shared_ptr<ScrollingTextView> scrollingTextView;
	
};

struct Console : OutputConsole
{
	Console(float x, float y, float scaleX, float scaleY, GuiManager& guiManager);
	void setInputCallback(std::function<void(std::string)> inputCallback);
	void setVisible(bool visible);
private:
	std::shared_ptr<InputField> inputField;
};

#endif