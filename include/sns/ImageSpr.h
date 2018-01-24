#pragma once

#include "sns/NodeSpr.h"

#include <string>

namespace sns
{
	
class ImageSpr : public NodeSpr
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
	virtual void LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is) override;
	virtual void LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val) override;
	
//private:
//	int16_t m_xmin, m_ymin, m_xmax, m_ymax;
//	int16_t m_offx, m_offy;

}; // NodeImgae

}