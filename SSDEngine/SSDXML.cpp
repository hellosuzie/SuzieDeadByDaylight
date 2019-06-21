#include"SSDXML.h"



const bool SSDXML::OpenXMLFile(tinyxml2::XMLDocument* _Doc, const std::string& _Path)
{
	if (nullptr == _Doc)
		return false;

	tinyxml2::XMLError Error = _Doc->LoadFile(_Path.c_str());
	
	if (Error != tinyxml2::XMLError::XML_SUCCESS)
		return false;
	
	return true;
}
tinyxml2::XMLNode* SSDXML::FindNode(tinyxml2::XMLDocument* _Doc, const std::string& _NodeName)
{
	if (nullptr == _Doc)
		return nullptr;

	tinyxml2::XMLNode* Node = _Doc->FirstChild();
	std::string Name;

	for (; nullptr != Node; Node = Node->NextSibling())
	{
		Name = Node->Value();

		if (Name == _NodeName)
			return Node;
	}
	return nullptr;
}
 tinyxml2::XMLNode* SSDXML::FindNode(tinyxml2::XMLNode* _RootNode, const std::string& _NodeName)
{
	 if (nullptr == _RootNode)
		 return nullptr;

	 tinyxml2::XMLNode* Node = _RootNode->FirstChild();
	 std::string Name;

	 for (; nullptr != Node; Node = Node->NextSibling())
	 {
		 Name = Node->Value();

		 if (Name == _NodeName)
			 return Node;
	 }
	 return nullptr;
}