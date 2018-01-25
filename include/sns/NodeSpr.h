#pragma once

#include "sns/NodeSprCommon.h"

#include <rapidjson/document.h>

namespace mm { class LinearAllocator; }
namespace bs { class ImportStream; class ExportStream; }

namespace sns
{

class NodeSpr : private cu::Uncopyable
{
public:
	virtual ~NodeSpr() {}

	//
	// serialization
	//
	virtual size_t GetBinSize(const std::string& dir) const = 0;
	virtual void StoreToBin(const std::string& dir, bs::ExportStream& es) const = 0;
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const = 0;

	//
	// deserialization
	//
	virtual void LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is) = 0;
	virtual void LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val) = 0;

	const NodeSprCommon& GetCommon() const { return m_common; }

protected:
	NodeSprCommon m_common;

}; // NodeSpr

}
