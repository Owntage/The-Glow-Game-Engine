#include "light_manager.h"
#include <iostream>

#define LIGHT_VERTEX_SHADER "res/light_vertex_shader.txt"
#define MULTIPLY_FRAGMENT_SHADER "res/divide_fragment_shader.txt"

#define DEFINE_PLACEHOLDER "$define"
#define LIGHT_SIZE_DEFINE "LIGHT_SIZE"
#define OBSTACLE_SIZE_DEFINE "OBSTACLE_SIZE"
#define ADDITIVE_DEFINE "ADDITIVE_DEFINE"

#define DEFAULT_LIGHTS 200
#define DEFAULT_OBSTACLES 100

static std::string lightFragmentShader =
		"#version 120\n"
		DEFINE_PLACEHOLDER
		"\n"
		"varying vec2 pos_vec;\n"
		"varying vec2 tex_vec;\n"
		"\n"
		"uniform vec2 offset;\n"
		"uniform vec2 light_pos[" LIGHT_SIZE_DEFINE "];\n"
		"uniform vec3 light_color[" LIGHT_SIZE_DEFINE "];\n"
		"uniform float light_intensity[" LIGHT_SIZE_DEFINE "];\n"
		"uniform float light_additive_intensity[" LIGHT_SIZE_DEFINE "];\n"
		"\n"
		"uniform vec2 shadow_pos[" OBSTACLE_SIZE_DEFINE "];\n"
		"uniform vec2 shadow_size[" OBSTACLE_SIZE_DEFINE "];\n"
		"\n"
		"\n"
		"uniform int sources_size;\n"
		"uniform int shadows_size;\n"
		"\n"
		"#define PI 3.141593 //good enough for engineering\n"
		"#define LIGHT_RADIUS 32\n"
		"#define TILE_SIZE 32\n"
		"#define EPS = 0.001\n"
		"\n"
		"float get_angle(vec2 base, vec2 target)\n"
		"{\n"
		"  vec2 delta = target - base;\n"
		"  \n"
		"  if (delta.x < 0)\n"
		"  {\n"
		"    return atan(delta.y / delta.x) + PI;\n"
		"  }\n"
		"  else\n"
		"  {\n"
		"    if (delta.y > 0)\n"
		"    {\n"
		"      return atan(delta.y / delta.x);\n"
		"    }\n"
		"    else\n"
		"    {\n"
		"      return atan(delta.y / delta.x) + PI * 2;\n"
		"    }\n"
		"  }\n"
		"}\n"
		"\n"
		"\n"
		"vec4 get_rectangle_angles(vec2 base, vec2 rect_pos, vec2 rect_size)\n"
		"{\n"
		"  return vec4(\n"
		"    get_angle(base, vec2(rect_pos.x + rect_size.x / 2, rect_pos.y + rect_size.y / 2)),\n"
		"    get_angle(base, vec2(rect_pos.x - rect_size.x / 2, rect_pos.y + rect_size.y / 2)),\n"
		"    get_angle(base, vec2(rect_pos.x - rect_size.x / 2, rect_pos.y - rect_size.y / 2)),\n"
		"    get_angle(base, vec2(rect_pos.x + rect_size.x / 2, rect_pos.y - rect_size.y / 2)));\n"
		"}\n"
		"\n"
		"\n"
		"\n"
		"\n"
		"float vec_min(vec4 v)\n"
		"{\n"
		"  float res = v[0];\n"
		"  for  (int i = 1; i < 4; i++)\n"
		"  {\n"
		"    res = min(res, v[i]);\n"
		"  }\n"
		"  return res;\n"
		"}\n"
		"\n"
		"float vec_max(vec4 v)\n"
		"{\n"
		"  float res = v[0];\n"
		"  for  (int i = 1; i < 4; i++)\n"
		"  {\n"
		"    res = max(res, v[i]);\n"
		"  }\n"
		"  return res;\n"
		"}\n"
		"\n"
		"float get_min_rect_angle(vec2 base, vec2 rect_pos, vec2 rect_size)\n"
		"{\n"
		"  vec2 delta = rect_pos - base;\n"
		"  vec2 a0 = vec2(delta.x - rect_size.x / 2, delta.y - rect_size.y / 2);\n"
		"  vec2 a1 = vec2(delta.x + rect_size.x / 2, delta.y - rect_size.y / 2);\n"
		"  vec2 a2 = vec2(delta.x + rect_size.x / 2, delta.y + rect_size.y / 2);\n"
		"  vec2 a3 = vec2(delta.x - rect_size.x / 2, delta.y + rect_size.y / 2);\n"
		"  \n"
		"  if(a1.x > 0 && a1.y > 0) return get_angle(vec2(0, 0), a1);\n"
		"  if(a2.x < 0 && a2.y > 0) return get_angle(vec2(0, 0), a2);\n"
		"  if(a3.x < 0 && a3.y < 0) return get_angle(vec2(0, 0), a3);\n"
		"  return get_angle(vec2(0, 0), a0);\n"
		"}\n"
		"\n"
		"float get_max_rect_angle(vec2 base, vec2 rect_pos, vec2 rect_size)\n"
		"{\n"
		"  vec2 delta = rect_pos - base;\n"
		"  vec2 a0 = vec2(delta.x - rect_size.x / 2, delta.y - rect_size.y / 2);\n"
		"  vec2 a1 = vec2(delta.x + rect_size.x / 2, delta.y - rect_size.y / 2);\n"
		"  vec2 a2 = vec2(delta.x + rect_size.x / 2, delta.y + rect_size.y / 2);\n"
		"  vec2 a3 = vec2(delta.x - rect_size.x / 2, delta.y + rect_size.y / 2);\n"
		"  \n"
		"  if(a3.x > 0 && a3.y > 0) return get_angle(vec2(0, 0), a3);\n"
		"  if(a0.x < 0 && a0.y > 0) return get_angle(vec2(0, 0), a0);\n"
		"  if(a1.x < 0 && a1.y < 0) return get_angle(vec2(0, 0), a1);\n"
		"  return get_angle(vec2(0, 0), a2);\n"
		"}\n"
		"\n"
		"float correctAngle(float angle)\n"
		"{\n"
		"  \n"
		"  if (angle > 2.0f * PI)\n"
		"  {\n"
		"    angle -= 2.0f * PI;\n"
		"  }\n"
		"  if (angle < 0)\n"
		"  {\n"
		"    angle += 2.0f * PI;\n"
		"  }\n"
		"  return angle;\n"
		"  \n"
		"}\n"
		"\n"
		"//copy-paste, because glsl doesnt allow recursion\n"
		"float getAngularIntersection1(float startAngleA, float endAngleA, float startAngleB, float endAngleB)\n"
		"{\n"
		"  startAngleA = correctAngle(startAngleA);\n"
		"  endAngleA = correctAngle(endAngleA);\n"
		"  startAngleB = correctAngle(startAngleB);\n"
		"  endAngleB = correctAngle(endAngleB);\n"
		"  \n"
		"  float startAngle = max(startAngleA, startAngleB);\n"
		"  float endAngle = min(endAngleA, endAngleB);\n"
		"  return max(endAngle - startAngle, 0.0f);\n"
		"}\n"
		"\n"
		"\n"
		"float getAngularIntersection0(float startAngleA, float endAngleA, float startAngleB, float endAngleB)\n"
		"{\n"
		"  startAngleA = correctAngle(startAngleA);\n"
		"  endAngleA = correctAngle(endAngleA);\n"
		"  startAngleB = correctAngle(startAngleB);\n"
		"  endAngleB = correctAngle(endAngleB);\n"
		"  if (startAngleB > endAngleB)\n"
		"  {\n"
		"    return getAngularIntersection1(startAngleA, endAngleA, startAngleB, 2 * PI) +\n"
		"      getAngularIntersection1(startAngleA, endAngleA, 0, endAngleB);\n"
		"  }\n"
		"  float startAngle = max(startAngleA, startAngleB);\n"
		"  float endAngle = min(endAngleA, endAngleB);\n"
		"  return max(endAngle - startAngle, 0.0f);\n"
		"}\n"
		"\n"
		"float get_angular_intersection(float startAngleA, float endAngleA, float startAngleB, float endAngleB)\n"
		"{\n"
		"  startAngleA = correctAngle(startAngleA);\n"
		"  endAngleA = correctAngle(endAngleA);\n"
		"  startAngleB = correctAngle(startAngleB);\n"
		"  endAngleB = correctAngle(endAngleB);\n"
		"  if (startAngleA > endAngleA)\n"
		"  {\n"
		"    return getAngularIntersection0(startAngleA, 2 * PI, startAngleB, endAngleB) +\n"
		"      getAngularIntersection0(0, endAngleA, startAngleB, endAngleB);\n"
		"  }\n"
		"  if (startAngleB > endAngleB)\n"
		"  {\n"
		"    return getAngularIntersection1(startAngleA, endAngleA, startAngleB, 2 * PI) +\n"
		"      getAngularIntersection1(startAngleA, endAngleA, 0, endAngleB);\n"
		"  }\n"
		"  float startAngle = max(startAngleA, startAngleB);\n"
		"  float endAngle = min(endAngleA, endAngleB);\n"
		"  return max(endAngle - startAngle, 0.0f);\n"
		"}\n"
		"\n"
		"float get_sphere_delta_angle(vec2 base, vec2 sphere, float radius)\n"
		"{  \n"
		"  float angleRadius = radius / length(base - sphere);\n"
		"  return asin(angleRadius);\n"
		"}\n"
		"\n"
		"float dotProduct(vec2 a, vec2 b)\n"
		"{\n"
		"  return a.x * b.x + a.y * b.y;\n"
		"}\n"
		"\n"
		"float crossProduct(vec2 a, vec2 b)\n"
		"{\n"
		"  return a.x * b.y - a.y * b.x;\n"
		"}\n"
		"\n"
		"float point_segment_distance(vec2 s1, vec2 s2, vec2 p)\n"
		"{\n"
		"  vec2 s1p = p - s1;\n"
		"  vec2 s1s2 = s2 - s1;\n"
		"  float firstDotProduct = dotProduct(s1p, s1s2);\n"
		"  if(firstDotProduct < 0)\n"
		"  {\n"
		"    return length(p - s1);\n"
		"  }\n"
		"  vec2 s2p = p - s2;\n"
		"  vec2 s2s1 = -s1s2;\n"
		"  float secondDotProduct = dotProduct(s2p, s2s1);\n"
		"  if(secondDotProduct < 0)\n"
		"  {\n"
		"    return length(p - s2);\n"
		"  }\n"
		"  float parallelogramSquare = abs(crossProduct(s1p, s2p));\n"
		"  return parallelogramSquare / length(s1s2);\n"
		"}\n"
		"\n"
		"float point_rect_distance(vec2 rect_center, vec2 rect_size, vec2 point)\n"
		"{\n"
		"  vec2 rect_p0 = vec2(rect_center.x - rect_size.x / 2, rect_center.y - rect_size.y / 2);\n"
		"  vec2 rect_p1 = vec2(rect_center.x + rect_size.x / 2, rect_center.y - rect_size.y / 2);\n"
		"  vec2 rect_p2 = vec2(rect_center.x + rect_size.x / 2, rect_center.y + rect_size.y / 2);\n"
		"  vec2 rect_p3 = vec2(rect_center.x - rect_size.x / 2, rect_center.y + rect_size.y / 2);\n"
		"  \n"
		"  vec4 res;\n"
		"  res[0] = point_segment_distance(rect_p0, rect_p1, point);\n"
		"  res[1] = point_segment_distance(rect_p1, rect_p2, point);\n"
		"  res[2] = point_segment_distance(rect_p2, rect_p3, point);\n"
		"  res[3] = point_segment_distance(rect_p3, rect_p0, point);\n"
		"  \n"
		"  return vec_min(res);\n"
		"}\n"
		"\n"
		"bool point_in_rectangle(vec2 rect_pos, vec2 rect_size, vec2 point)\n"
		"{\n"
		"  return (point.x >= rect_pos.x - rect_size.x / 2 &&\n"
		"    point.x <= rect_pos.x + rect_size.x / 2 &&\n"
		"    point.y >= rect_pos.y - rect_size.y / 2 &&\n"
		"    point.y <= rect_pos.y + rect_size.y / 2);\n"
		"  \n"
		"}\n"
		"\n"
		"bool left_turn(vec2 a, vec2 b, vec2 c)\n"
		"{\n"
		"  return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x)) > 0;\n"
		"}\n"
		"\n"
		"bool segment_intersect(vec2 a0, vec2 a1, vec2 b0, vec2 b1)\n"
		"{\n"
		"  return left_turn(a0, a1, b0) != left_turn(a0, a1, b1) &&\n"
		"      left_turn(b0, b1, a0) != left_turn(b0, b1, a1);\n"
		"}\n"
		"\n"
		"bool rect_segment_intersect(vec2 rect_center, vec2 rect_size, vec2 a0, vec2 a1)\n"
		"{\n"
		"  vec2 rect_p0 = vec2(rect_center.x - rect_size.x / 2, rect_center.y - rect_size.y / 2);\n"
		"  vec2 rect_p1 = vec2(rect_center.x + rect_size.x / 2, rect_center.y - rect_size.y / 2);\n"
		"  vec2 rect_p2 = vec2(rect_center.x + rect_size.x / 2, rect_center.y + rect_size.y / 2);\n"
		"  vec2 rect_p3 = vec2(rect_center.x - rect_size.x / 2, rect_center.y + rect_size.y / 2);\n"
		"  \n"
		"  return segment_intersect(rect_p0, rect_p1, a0, a1) ||\n"
		"      segment_intersect(rect_p1, rect_p2, a0, a1) ||\n"
		"      segment_intersect(rect_p2, rect_p3, a0, a1) ||\n"
		"      segment_intersect(rect_p3, rect_p0, a0, a1);\n"
		"}\n"
		"\n"
		"float my_abs(float a)\n"
		"{\n"
		"  if (a <= 0.0) return -a;\n"
		"  return a;\n"
		"}\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"  vec4 color = vec4(0.0, 0.0, 0.0, 1.0);\n"
		"  \n"
		"  for (int i = 0; i < sources_size; i++)\n"
		"  {\n"
		"    float multiplier;\n"
		"    float len = length(pos_vec + offset - light_pos[i]) / TILE_SIZE * 2;\n"
		"  \n"
		"    multiplier = (light_intensity[i] - len) / light_intensity[i];\n"
		"    \n"
		"    \n"
		"    if(multiplier <= 0.001)\n"
		"    {\n"
		"      continue;\n"
		"    }\n"
		"    \n"
		"    float shadow_intensity = 1;\n"
		"    for (int j = 0; j < shadows_size; j++)\n"
		"    {\n"
		"      float current_intensity = 1;\n"
		"      if (rect_segment_intersect(shadow_pos[j], shadow_size[j], pos_vec + offset, light_pos[i]) &&\n"
		"        !point_in_rectangle(shadow_pos[j], shadow_size[j], pos_vec + offset))\n"
		"      {\n"
		"        \n"
		"        float min_ra = get_min_rect_angle(pos_vec + offset, shadow_pos[j], shadow_size[j]);\n"
		"        float max_ra = get_max_rect_angle(pos_vec + offset, shadow_pos[j], shadow_size[j]);\n"
		"        float dist = point_rect_distance(shadow_pos[j], shadow_size[j], light_pos[i]);\n"
		"        \n"
		"        float light_angle = get_angle(pos_vec + offset, light_pos[i]);\n"
		"        float sphere_delta_angle = get_sphere_delta_angle(pos_vec + offset, light_pos[i], min(dist - 0.0001, 32));\n"
		"        float angularIntersection = get_angular_intersection(\n"
		"              min_ra,\n"
		"              max_ra,\n"
		"              light_angle - sphere_delta_angle,\n"
		"              light_angle + sphere_delta_angle);\n"
		"        current_intensity = sphere_delta_angle * 2 - angularIntersection;\n"
		"        current_intensity /= sphere_delta_angle;\n"
		"        \n"
		"        if(current_intensity < 0.01)\n"
		"        {\n"
		"          shadow_intensity = 0;\n"
		"          break;\n"
		"        }\n"
		"      }\n"
		"      \n"
		"      shadow_intensity *= current_intensity;\n"
		"    }\n"
		"    shadow_intensity = min(shadow_intensity, 1.0);\n"
		"    \n"
		"    multiplier *= shadow_intensity;\n"
		"#ifdef " ADDITIVE_DEFINE "\n"
		"    multiplier *= light_additive_intensity[i];\n"
		"#endif\n"
		"    \n"
		"    color.r += light_color[i].r * multiplier;\n"
		"    color.g += light_color[i].g * multiplier;\n"
		"    color.b += light_color[i].b * multiplier;\n"
		"    \n"
		"  }\n"
		"  \n"
		"  gl_FragColor = color;\n"
		"}";

struct ShadowLightManagerImpl
{
	ShadowLightManagerImpl(float screenWidth, float screenHeight, float tileWidth);
	ShadowLightManagerImpl();
	void init(float screenWidth, float screenHeight, float tileSize);
	int addLightSource(sf::Vector2f pos, sf::Color color, float intensity);
	int addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size);
	void draw(sf::RenderTarget& renderTarget);
	void setPosition(int lightSourceIndex, sf::Vector2f pos);
	void removeLightSource(int lightSourceIndex);
	void onWindowResize(float screenWidth, float screenHeight);
private:
	struct LightData
	{
		sf::Vector2f position;
		sf::Vector3f color;
		float intensity;
		LightData() {}
		LightData(sf::Vector2f position, sf::Vector3f color, float intensity) :
				position(position),
				color(color),
				intensity(intensity)
		{}
	};
	std::map<int, int> idToShaderIndex; //can be replaced with bimap
	std::map<int, int> shaderIndexToId;
	std::map<int, LightData> idToData;
	int shaderArraySize;
	int shadowsArraySize;
	int counter;
	int textureDivider;
	sf::Shader shader;
	sf::Shader multiplyShader;
	sf::RectangleShape shape;
	sf::RenderTexture renderTexture;
	float screenWidth;
	float screenHeight;
	float tileWidth;
};

ShadowLightManagerImpl::ShadowLightManagerImpl(float screenWidth, float screenHeight, float tileWidth) :
		shaderArraySize(0),
		shadowsArraySize(0),
		textureDivider(1)
{
	init(screenWidth, screenHeight, tileWidth);
}

ShadowLightManagerImpl::ShadowLightManagerImpl() :
	shaderArraySize(0),
	shadowsArraySize(0),
	textureDivider(1)
{}

static std::string prepareLightMultiplyShader()
{
	std::string result = lightFragmentShader;
	std::string definePlaceholderStr(DEFINE_PLACEHOLDER);
	std::string defines =
		"#define " LIGHT_SIZE_DEFINE " " + std::to_string(DEFAULT_LIGHTS) + "\n"
		"#define " ADDITIVE_DEFINE "\n"
		"#define " OBSTACLE_SIZE_DEFINE " " + std::to_string(DEFAULT_OBSTACLES) +"\n";
	;
	result.replace(result.find(DEFINE_PLACEHOLDER), definePlaceholderStr.size(), defines);
	std::cout << result << std::endl;
	return result;
}

void ShadowLightManagerImpl::init(float screenWidth, float screenHeight, float tileSize)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->tileWidth = tileSize;

	sf::FileInputStream vertexStream;
	vertexStream.open(LIGHT_VERTEX_SHADER);
	sf::MemoryInputStream fragmentStream;
	std::string multiplyFragmentShader = prepareLightMultiplyShader();
	fragmentStream.open(multiplyFragmentShader.c_str(), multiplyFragmentShader.size());
	shader.loadFromStream(vertexStream, fragmentStream);

	sf::FileInputStream multiplyFragmentStream;
	multiplyFragmentStream.open(MULTIPLY_FRAGMENT_SHADER);
	multiplyShader.loadFromStream(multiplyFragmentStream, sf::Shader::Fragment);
	renderTexture.create(screenWidth / textureDivider, screenHeight / textureDivider);
	renderTexture.setSmooth(true);

	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

int ShadowLightManagerImpl::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
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

int ShadowLightManagerImpl::addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size)
{
	shader.setUniform("shadow_pos[" + std::to_string(shadowsArraySize) + "]", pos);
	shader.setUniform("shadow_size[" + std::to_string(shadowsArraySize) + "]", size);
	shadowsArraySize++;
	shader.setUniform("shadows_size", shadowsArraySize);
	return counter++;
}

void ShadowLightManagerImpl::draw(sf::RenderTarget& renderTarget)
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


	/*
	multiplyRenderStates.blendMode = sf::BlendAdd;
	multiplyShader.setUniform("multiplier", 0.3f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);
	*/


	multiplyRenderStates.blendMode = sf::BlendMultiply;
	multiplyShader.setUniform("multiplier", 1.0f);
	multiplyRenderStates.shader = &multiplyShader;
	renderTarget.draw(shape, multiplyRenderStates);

}

void ShadowLightManagerImpl::setPosition(int lightSourceID, sf::Vector2f pos)
{
	shader.setUniform("light_pos[" + std::to_string(idToShaderIndex[lightSourceID]) + "]", pos);
}

void ShadowLightManagerImpl::removeLightSource(int lightSourceID)
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

void ShadowLightManagerImpl::onWindowResize(float screenWidth, float screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	shape.setSize(sf::Vector2f(screenWidth / tileWidth, screenHeight / tileWidth));
	shape.setOrigin(screenWidth / tileWidth / 2, screenHeight / tileWidth / 2);
}

ShadowLightManager::ShadowLightManager(float screenWidth, float screenHeight, float tileWidth)
{
	impl = new ShadowLightManagerImpl(screenWidth, screenHeight, tileWidth);
}

ShadowLightManager::ShadowLightManager()
{
	impl = new ShadowLightManagerImpl();
}

ShadowLightManager::~ShadowLightManager()
{
	delete impl;
}

void ShadowLightManager::init(float screenWidth, float screenHeight, float tileSize)
{
	impl->init(screenWidth, screenHeight, tileSize);
}

int ShadowLightManager::addLightSource(sf::Vector2f pos, sf::Color color, float intensity)
{
	return impl->addLightSource(pos, color, intensity);
}

int ShadowLightManager::addRectangleObstacle(sf::Vector2f pos, sf::Vector2f size)
{
	return impl->addRectangleObstacle(pos, size);
}

void ShadowLightManager::draw(sf::RenderTarget& renderTarget)
{
	impl->draw(renderTarget);
}

void ShadowLightManager::setPosition(int lightSourceIndex, sf::Vector2f pos)
{
	impl->setPosition(lightSourceIndex, pos);
}

void ShadowLightManager::removeLightSource(int lightSourceIndex)
{
	impl->removeLightSource(lightSourceIndex);
}

void ShadowLightManager::onWindowResize(float screenWidth, float screenHeight)
{
	impl->onWindowResize(screenWidth, screenHeight);
}