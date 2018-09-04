#include "light_manager.h"


#define LIGHT_VERTEX_SHADER "res/light_vertex_shader.txt"
#define MULTIPLY_FRAGMENT_SHADER "res/divide_fragment_shader.txt"


static std::string lightFragmentShader =
		"#version 120\n"
		"\n"
		"varying vec2 pos_vec;\n"
		"varying vec2 tex_vec;\n"
		"\n"
		"uniform vec2 offset;\n"
		"uniform vec2 light_pos[200];\n"
		"uniform vec3 light_color[200];\n"
		"uniform float light_intensity[200];\n"
		"\n"
		"uniform vec2 shadow_pos[100];\n"
		"uniform vec2 shadow_size[100];\n"
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
		"\tvec2 delta = target - base;\n"
		"\t\n"
		"\tif (delta.x < 0)\n"
		"\t{\n"
		"\t\treturn atan(delta.y / delta.x) + PI;\n"
		"\t}\n"
		"\telse\n"
		"\t{\n"
		"\t\tif (delta.y > 0)\n"
		"\t\t{\n"
		"\t\t\treturn atan(delta.y / delta.x);\n"
		"\t\t}\n"
		"\t\telse\n"
		"\t\t{\n"
		"\t\t\treturn atan(delta.y / delta.x) + PI * 2;\n"
		"\t\t}\n"
		"\t}\n"
		"}\n"
		"\n"
		"\n"
		"vec4 get_rectangle_angles(vec2 base, vec2 rect_pos, vec2 rect_size)\n"
		"{\n"
		"\treturn vec4(\n"
		"\t\tget_angle(base, vec2(rect_pos.x + rect_size.x / 2, rect_pos.y + rect_size.y / 2)),\n"
		"\t\tget_angle(base, vec2(rect_pos.x - rect_size.x / 2, rect_pos.y + rect_size.y / 2)),\n"
		"\t\tget_angle(base, vec2(rect_pos.x - rect_size.x / 2, rect_pos.y - rect_size.y / 2)),\n"
		"\t\tget_angle(base, vec2(rect_pos.x + rect_size.x / 2, rect_pos.y - rect_size.y / 2)));\n"
		"}\n"
		"\n"
		"\n"
		"\n"
		"\n"
		"float vec_min(vec4 v)\n"
		"{\n"
		"\tfloat res = v[0];\n"
		"\tfor\t(int i = 1; i < 4; i++)\n"
		"\t{\n"
		"\t\tres = min(res, v[i]);\n"
		"\t}\n"
		"\treturn res;\n"
		"}\n"
		"\n"
		"float vec_max(vec4 v)\n"
		"{\n"
		"\tfloat res = v[0];\n"
		"\tfor\t(int i = 1; i < 4; i++)\n"
		"\t{\n"
		"\t\tres = max(res, v[i]);\n"
		"\t}\n"
		"\treturn res;\n"
		"}\n"
		"\n"
		"float get_min_rect_angle(vec2 base, vec2 rect_pos, vec2 rect_size)\n"
		"{\n"
		"\tvec2 delta = rect_pos - base;\n"
		"\tvec2 a0 = vec2(delta.x - rect_size.x / 2, delta.y - rect_size.y / 2);\n"
		"\tvec2 a1 = vec2(delta.x + rect_size.x / 2, delta.y - rect_size.y / 2);\n"
		"\tvec2 a2 = vec2(delta.x + rect_size.x / 2, delta.y + rect_size.y / 2);\n"
		"\tvec2 a3 = vec2(delta.x - rect_size.x / 2, delta.y + rect_size.y / 2);\n"
		"\t\n"
		"\tif(a1.x > 0 && a1.y > 0) return get_angle(vec2(0, 0), a1);\n"
		"\tif(a2.x < 0 && a2.y > 0) return get_angle(vec2(0, 0), a2);\n"
		"\tif(a3.x < 0 && a3.y < 0) return get_angle(vec2(0, 0), a3);\n"
		"\treturn get_angle(vec2(0, 0), a0);\n"
		"}\n"
		"\n"
		"float get_max_rect_angle(vec2 base, vec2 rect_pos, vec2 rect_size)\n"
		"{\n"
		"\tvec2 delta = rect_pos - base;\n"
		"\tvec2 a0 = vec2(delta.x - rect_size.x / 2, delta.y - rect_size.y / 2);\n"
		"\tvec2 a1 = vec2(delta.x + rect_size.x / 2, delta.y - rect_size.y / 2);\n"
		"\tvec2 a2 = vec2(delta.x + rect_size.x / 2, delta.y + rect_size.y / 2);\n"
		"\tvec2 a3 = vec2(delta.x - rect_size.x / 2, delta.y + rect_size.y / 2);\n"
		"\t\n"
		"\tif(a3.x > 0 && a3.y > 0) return get_angle(vec2(0, 0), a3);\n"
		"\tif(a0.x < 0 && a0.y > 0) return get_angle(vec2(0, 0), a0);\n"
		"\tif(a1.x < 0 && a1.y < 0) return get_angle(vec2(0, 0), a1);\n"
		"\treturn get_angle(vec2(0, 0), a2);\n"
		"}\n"
		"\n"
		"float correctAngle(float angle)\n"
		"{\n"
		"\t\n"
		"\tif (angle > 2.0f * PI)\n"
		"\t{\n"
		"\t\tangle -= 2.0f * PI;\n"
		"\t}\n"
		"\tif (angle < 0)\n"
		"\t{\n"
		"\t\tangle += 2.0f * PI;\n"
		"\t}\n"
		"\treturn angle;\n"
		"\t\n"
		"}\n"
		"\n"
		"//copy-paste, because glsl doesnt allow recursion\n"
		"float getAngularIntersection1(float startAngleA, float endAngleA, float startAngleB, float endAngleB)\n"
		"{\n"
		"\tstartAngleA = correctAngle(startAngleA);\n"
		"\tendAngleA = correctAngle(endAngleA);\n"
		"\tstartAngleB = correctAngle(startAngleB);\n"
		"\tendAngleB = correctAngle(endAngleB);\n"
		"\t\n"
		"\tfloat startAngle = max(startAngleA, startAngleB);\n"
		"\tfloat endAngle = min(endAngleA, endAngleB);\n"
		"\treturn max(endAngle - startAngle, 0.0f);\n"
		"}\n"
		"\n"
		"\n"
		"float getAngularIntersection0(float startAngleA, float endAngleA, float startAngleB, float endAngleB)\n"
		"{\n"
		"\tstartAngleA = correctAngle(startAngleA);\n"
		"\tendAngleA = correctAngle(endAngleA);\n"
		"\tstartAngleB = correctAngle(startAngleB);\n"
		"\tendAngleB = correctAngle(endAngleB);\n"
		"\tif (startAngleB > endAngleB)\n"
		"\t{\n"
		"\t\treturn getAngularIntersection1(startAngleA, endAngleA, startAngleB, 2 * PI) +\n"
		"\t\t\tgetAngularIntersection1(startAngleA, endAngleA, 0, endAngleB);\n"
		"\t}\n"
		"\tfloat startAngle = max(startAngleA, startAngleB);\n"
		"\tfloat endAngle = min(endAngleA, endAngleB);\n"
		"\treturn max(endAngle - startAngle, 0.0f);\n"
		"}\n"
		"\n"
		"float get_angular_intersection(float startAngleA, float endAngleA, float startAngleB, float endAngleB)\n"
		"{\n"
		"\tstartAngleA = correctAngle(startAngleA);\n"
		"\tendAngleA = correctAngle(endAngleA);\n"
		"\tstartAngleB = correctAngle(startAngleB);\n"
		"\tendAngleB = correctAngle(endAngleB);\n"
		"\tif (startAngleA > endAngleA)\n"
		"\t{\n"
		"\t\treturn getAngularIntersection0(startAngleA, 2 * PI, startAngleB, endAngleB) +\n"
		"\t\t\tgetAngularIntersection0(0, endAngleA, startAngleB, endAngleB);\n"
		"\t}\n"
		"\tif (startAngleB > endAngleB)\n"
		"\t{\n"
		"\t\treturn getAngularIntersection1(startAngleA, endAngleA, startAngleB, 2 * PI) +\n"
		"\t\t\tgetAngularIntersection1(startAngleA, endAngleA, 0, endAngleB);\n"
		"\t}\n"
		"\tfloat startAngle = max(startAngleA, startAngleB);\n"
		"\tfloat endAngle = min(endAngleA, endAngleB);\n"
		"\treturn max(endAngle - startAngle, 0.0f);\n"
		"}\n"
		"\n"
		"float get_sphere_delta_angle(vec2 base, vec2 sphere, float radius)\n"
		"{\t\n"
		"\tfloat angleRadius = radius / length(base - sphere);\n"
		"\treturn asin(angleRadius);\n"
		"}\n"
		"\n"
		"float dotProduct(vec2 a, vec2 b)\n"
		"{\n"
		"\treturn a.x * b.x + a.y * b.y;\n"
		"}\n"
		"\n"
		"float crossProduct(vec2 a, vec2 b)\n"
		"{\n"
		"\treturn a.x * b.y - a.y * b.x;\n"
		"}\n"
		"\n"
		"float point_segment_distance(vec2 s1, vec2 s2, vec2 p)\n"
		"{\n"
		"\tvec2 s1p = p - s1;\n"
		"\tvec2 s1s2 = s2 - s1;\n"
		"\tfloat firstDotProduct = dotProduct(s1p, s1s2);\n"
		"\tif(firstDotProduct < 0)\n"
		"\t{\n"
		"\t\treturn length(p - s1);\n"
		"\t}\n"
		"\tvec2 s2p = p - s2;\n"
		"\tvec2 s2s1 = -s1s2;\n"
		"\tfloat secondDotProduct = dotProduct(s2p, s2s1);\n"
		"\tif(secondDotProduct < 0)\n"
		"\t{\n"
		"\t\treturn length(p - s2);\n"
		"\t}\n"
		"\tfloat parallelogramSquare = abs(crossProduct(s1p, s2p));\n"
		"\treturn parallelogramSquare / length(s1s2);\n"
		"}\n"
		"\n"
		"float point_rect_distance(vec2 rect_center, vec2 rect_size, vec2 point)\n"
		"{\n"
		"\tvec2 rect_p0 = vec2(rect_center.x - rect_size.x / 2, rect_center.y - rect_size.y / 2);\n"
		"\tvec2 rect_p1 = vec2(rect_center.x + rect_size.x / 2, rect_center.y - rect_size.y / 2);\n"
		"\tvec2 rect_p2 = vec2(rect_center.x + rect_size.x / 2, rect_center.y + rect_size.y / 2);\n"
		"\tvec2 rect_p3 = vec2(rect_center.x - rect_size.x / 2, rect_center.y + rect_size.y / 2);\n"
		"\t\n"
		"\tvec4 res;\n"
		"\tres[0] = point_segment_distance(rect_p0, rect_p1, point);\n"
		"\tres[1] = point_segment_distance(rect_p1, rect_p2, point);\n"
		"\tres[2] = point_segment_distance(rect_p2, rect_p3, point);\n"
		"\tres[3] = point_segment_distance(rect_p3, rect_p0, point);\n"
		"\t\n"
		"\treturn vec_min(res);\n"
		"}\n"
		"\n"
		"bool point_in_rectangle(vec2 rect_pos, vec2 rect_size, vec2 point)\n"
		"{\n"
		"\treturn (point.x >= rect_pos.x - rect_size.x / 2 &&\n"
		"\t\tpoint.x <= rect_pos.x + rect_size.x / 2 &&\n"
		"\t\tpoint.y >= rect_pos.y - rect_size.y / 2 &&\n"
		"\t\tpoint.y <= rect_pos.y + rect_size.y / 2);\n"
		"\t\n"
		"}\n"
		"\n"
		"bool left_turn(vec2 a, vec2 b, vec2 c)\n"
		"{\n"
		"\treturn ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x)) > 0;\n"
		"}\n"
		"\n"
		"bool segment_intersect(vec2 a0, vec2 a1, vec2 b0, vec2 b1)\n"
		"{\n"
		"\treturn left_turn(a0, a1, b0) != left_turn(a0, a1, b1) &&\n"
		"\t\t\tleft_turn(b0, b1, a0) != left_turn(b0, b1, a1);\n"
		"}\n"
		"\n"
		"bool rect_segment_intersect(vec2 rect_center, vec2 rect_size, vec2 a0, vec2 a1)\n"
		"{\n"
		"\tvec2 rect_p0 = vec2(rect_center.x - rect_size.x / 2, rect_center.y - rect_size.y / 2);\n"
		"\tvec2 rect_p1 = vec2(rect_center.x + rect_size.x / 2, rect_center.y - rect_size.y / 2);\n"
		"\tvec2 rect_p2 = vec2(rect_center.x + rect_size.x / 2, rect_center.y + rect_size.y / 2);\n"
		"\tvec2 rect_p3 = vec2(rect_center.x - rect_size.x / 2, rect_center.y + rect_size.y / 2);\n"
		"\t\n"
		"\treturn segment_intersect(rect_p0, rect_p1, a0, a1) ||\n"
		"\t\t\tsegment_intersect(rect_p1, rect_p2, a0, a1) ||\n"
		"\t\t\tsegment_intersect(rect_p2, rect_p3, a0, a1) ||\n"
		"\t\t\tsegment_intersect(rect_p3, rect_p0, a0, a1);\n"
		"}\n"
		"\n"
		"float my_abs(float a)\n"
		"{\n"
		"\tif (a <= 0.0) return -a;\n"
		"\treturn a;\n"
		"}\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"\tvec4 color = vec4(0.0, 0.0, 0.0, 1.0);\n"
		"\t\n"
		"\tfor (int i = 0; i < sources_size; i++)\n"
		"\t{\n"
		"\t\tfloat multiplier;\n"
		"\t\tfloat len = length(pos_vec + offset - light_pos[i]) / TILE_SIZE * 2;\n"
		"\t\n"
		"\t\tmultiplier = (light_intensity[i] - len) / light_intensity[i];\n"
		"\t\t\n"
		"\t\t\n"
		"\t\tif(multiplier <= 0.001)\n"
		"\t\t{\n"
		"\t\t\tcontinue;\n"
		"\t\t}\n"
		"\t\t\n"
		"\t\tfloat shadow_intensity = 1;\n"
		"\t\tfor (int j = 0; j < shadows_size; j++)\n"
		"\t\t{\n"
		"\t\t\tfloat current_intensity = 1;\n"
		"\t\t\tif (rect_segment_intersect(shadow_pos[j], shadow_size[j], pos_vec + offset, light_pos[i]) &&\n"
		"\t\t\t\t!point_in_rectangle(shadow_pos[j], shadow_size[j], pos_vec + offset))\n"
		"\t\t\t{\n"
		"\t\t\t\t\n"
		"\t\t\t\tfloat min_ra = get_min_rect_angle(pos_vec + offset, shadow_pos[j], shadow_size[j]);\n"
		"\t\t\t\tfloat max_ra = get_max_rect_angle(pos_vec + offset, shadow_pos[j], shadow_size[j]);\n"
		"\t\t\t\tfloat dist = point_rect_distance(shadow_pos[j], shadow_size[j], light_pos[i]);\n"
		"\t\t\t\t\n"
		"\t\t\t\tfloat light_angle = get_angle(pos_vec + offset, light_pos[i]);\n"
		"\t\t\t\tfloat sphere_delta_angle = get_sphere_delta_angle(pos_vec + offset, light_pos[i], min(dist - 0.0001, 32));\n"
		"\t\t\t\tfloat angularIntersection = get_angular_intersection(\n"
		"\t\t\t\t\t\t\tmin_ra,\n"
		"\t\t\t\t\t\t\tmax_ra,\n"
		"\t\t\t\t\t\t\tlight_angle - sphere_delta_angle,\n"
		"\t\t\t\t\t\t\tlight_angle + sphere_delta_angle);\n"
		"\t\t\t\tcurrent_intensity = sphere_delta_angle * 2 - angularIntersection;\n"
		"\t\t\t\tcurrent_intensity /= sphere_delta_angle;\n"
		"\t\t\t\t\n"
		"\t\t\t\tif(current_intensity < 0.01)\n"
		"\t\t\t\t{\n"
		"\t\t\t\t\tshadow_intensity = 0;\n"
		"\t\t\t\t\tbreak;\n"
		"\t\t\t\t}\n"
		"\t\t\t}\n"
		"\t\t\t\n"
		"\t\t\tshadow_intensity *= current_intensity;\n"
		"\t\t}\n"
		"\t\tshadow_intensity = min(shadow_intensity, 1.0);\n"
		"\t\t\n"
		"\t\tmultiplier *= shadow_intensity;\n"
		"\t\t\n"
		"\t\tcolor.r += light_color[i].r * multiplier;\n"
		"\t\tcolor.g += light_color[i].g * multiplier;\n"
		"\t\tcolor.b += light_color[i].b * multiplier;\n"
		"\t\t\n"
		"\t}\n"
		"\t\n"
		"\tgl_FragColor = color;\n"
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

void ShadowLightManagerImpl::init(float screenWidth, float screenHeight, float tileSize)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->tileWidth = tileSize;

	sf::FileInputStream vertexStream;
	vertexStream.open(LIGHT_VERTEX_SHADER);
	sf::MemoryInputStream fragmentStream;
	fragmentStream.open(lightFragmentShader.c_str(), lightFragmentShader.size());
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