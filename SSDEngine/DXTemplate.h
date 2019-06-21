#pragma once

template<typename RETURN,typename MAP,typename KEY>
RETURN UMAPFINDSPTR(MAP& _Map, const KEY& _Key)
{
	typename MAP::iterator FindIter = _Map.find(_Key);

	if (_Map.end() == FindIter)
		return nullptr;

	return FindIter->second;
}

template<typename RETURN, typename MAP, typename KEY>
RETURN MAPFINDSPTR(MAP& _Map, const KEY& _Key)
{
	typename MAP::iterator FindIter = _Map.find(_Key);

	if (_Map.end() == FindIter)
		return nullptr;

	return FindIter->second;
}

template<typename VALUE, typename MAP, typename KEY>
VALUE MAPFINDVALUE(MAP& _Map, const KEY& _Key)
{
	typename MAP::iterator FindIter = _Map.find(_Key);

	if (_Map.end() == FindIter)
		return 0;

	return FindIter->second;
}
