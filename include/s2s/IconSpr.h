#pragma once

#include "s2s/NodeSpr.h"

namespace s2s
{

class IconSpr : public NodeSpr
{
public:
	IconSpr();

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

	float GetProcess() const;

private:
	uint16_t m_process;

}; // IconSpr

}