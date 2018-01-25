#include <sns/NodeSpr.h>

namespace sns
{

size_t NodeSpr::GetBinSize(const std::string& dir) const
{
	return m_common.GetBinSize(dir);
}

void NodeSpr::StoreToBin(const std::string& dir, bs::ExportStream& es) const
{
	m_common.StoreToBin(dir, es);
}

void NodeSpr::StoreToJson(const std::string& dir, rapidjson::Value& val, 
	                      rapidjson::MemoryPoolAllocator<>& alloc) const
{
	m_common.StoreToJson(dir, val, alloc);
}

void NodeSpr::LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	m_common.LoadFromBin(alloc, dir, is);
}

void NodeSpr::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	m_common.LoadFromJson(alloc, dir, val);
}

}