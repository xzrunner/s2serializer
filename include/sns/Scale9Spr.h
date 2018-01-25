#pragma once

#include "sns/NodeSpr.h"

namespace sns
{

class Scale9Spr : public NodeSpr
{
public:
	Scale9Spr();

	//
	// serialization
	//
	virtual size_t GetBinSize(const std::string& dir) const override;
	virtual void StoreToBin(const std::string& dir, bs::ExportStream& es) const override;
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, 
		rapidjson::MemoryPoolAllocator<>& alloc) const override;

	//
	// deserialization
	//
	virtual void LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is) override;
	virtual void LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val) override;

	uint16_t Width() const { return m_width; }
	uint16_t Height() const { return m_height; }

private:
	uint16_t m_width, m_height;

}; // Scale9Spr

}