#include <sns/NodeSpr.h>

namespace sns
{

size_t NodeSpr::GetBinSize() const
{
	return m_common.GetBinSize();
}

void NodeSpr::StoreToBin(bs::ExportStream& es) const
{
	m_common.StoreToBin(es);
}

void NodeSpr::StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	m_common.StoreToJson(val, alloc);
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