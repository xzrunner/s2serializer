#include "sns/ImageSpr.h"
#include "sns/NodeType.h"

#include <bs/ExportStream.h>

namespace sns
{

size_t ImageSpr::GetBinSize() const
{
	size_t sz = 0;
	sz += sizeof(uint8_t);       // type
	sz += NodeSpr::GetBinSize(); // common
	return sz;
}

void ImageSpr::StoreToBin(bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(NODE_IMAGE)); // type
	NodeSpr::StoreToBin(es);                    // common
}

void ImageSpr::StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	NodeSpr::StoreToJson(val, alloc);
}

void ImageSpr::LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	NodeSpr::LoadFromBin(alloc, dir, is);
}

void ImageSpr::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	NodeSpr::LoadFromJson(alloc, dir, val);
}

}