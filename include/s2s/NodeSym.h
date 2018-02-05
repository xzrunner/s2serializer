#pragma once

#include <rapidjson/document.h>

#include <stdint.h>

namespace s2s
{

class NodeSym
{
public:
	virtual ~NodeSym() {}

	//
	// serialization
	//
	virtual void StoreToBin(const std::string& dir, uint8_t** data, size_t& length) const = 0;
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, 
		rapidjson::MemoryPoolAllocator<>& alloc) const = 0;

}; // NodeSym

}