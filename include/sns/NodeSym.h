#pragma once

#include <rapidjson/document.h>

#include <stdint.h>

namespace sns
{

class NodeSym
{
public:
	virtual ~NodeSym() {}

	//
	// serialization
	//
	virtual void StoreToBin(uint8_t** data, size_t& length) const = 0;
	virtual void StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const = 0;

}; // NodeSym

}