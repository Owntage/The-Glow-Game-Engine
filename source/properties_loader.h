#ifndef PROPERTIES_LOADER_H
#define PROPERTIES_LOADER_H

#include "singleton.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <iostream>
#include <map>

struct XmlVisitor
{
	virtual ~XmlVisitor() {}
	virtual std::string getPropertiesFilename() const = 0;
	virtual void onVisit(const boost::property_tree::ptree& node, std::string nodeName) const = 0;
};

void walkXml(std::string filename, const XmlVisitor& visitor);

/*
template<class VISITOR_T, class STRUCT_T>
struct XmlLoader : Singleton<XmlLoader<VISITOR_T, STRUCT_T> >
{
private:
	friend class Singleton<XmlLoader<VISITOR_T, STRUCT_T> >;
	//friend class VISITOR_T;
	XmlLoader()
	{
		VISITOR_T visitor;
		walkXml(visitor, visitor.getPropertiesFilename());
	}
	std::map<std::string, STRUCT_T> properties;
};
*/

#endif