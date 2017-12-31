#pragma once

#include "sns/NodeSpr.h"

namespace sns
{

class ComplexSpr : public NodeSpr
{
public:
	//
	// serialization
	//
	virtual size_t GetBinSize() const override;
	virtual void StoreToBin(bs::ExportStream& es) const override;
	virtual void StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const override;

	//
	// deserialization
	//
	virtual void LoadFromBin(mm::LinearAllocator& alloc, bs::ImportStream& is);
	virtual void LoadFromJson(mm::LinearAllocator& alloc, const rapidjson::Value& val);

private:
	uint32_t m_action = 0;

}; // ComplexSpr

}