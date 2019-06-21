#pragma once
#include<tinyxml2.h>
#include<string>


class SSDXML
{
public:
	static const bool OpenXMLFile(tinyxml2::XMLDocument* _Doc,const std::string& _Path);
	static tinyxml2::XMLNode* FindNode(tinyxml2::XMLDocument* _Doc, const std::string& _NodeName);
	static tinyxml2::XMLNode* FindNode(tinyxml2::XMLNode* _RootNode, const std::string& _NodeName);
private:
	SSDXML() {}
	virtual ~SSDXML() = 0{}
};
