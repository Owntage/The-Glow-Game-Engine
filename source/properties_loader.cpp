#include "properties_loader.h"

void walkXml(std::string filename, const XmlVisitor& visitor)
{
	try
	{
		boost::property_tree::ptree tree;
		read_xml(filename, tree);
		BOOST_FOREACH(auto& v, tree)
		{
			visitor.onVisit(v.second, v.first);
		}

	}
	catch(...)
	{
		std::cout << "failed to read xml with name " << filename << std::endl;
	}
}
