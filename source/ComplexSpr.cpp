#include "sns/ComplexSpr.h"
#include "sns/NodeType.h"

#include <bs/ImportStream.h>
#include <bs/ExportStream.h>

namespace sns
{

size_t ComplexSpr::GetBinSize() const
{
	size_t sz = 0;

	sz += sizeof(uint8_t);       // type
	sz += NodeSpr::GetBinSize(); // common

	sz += sizeof(uint16_t);      // action

	return sz;
}

void ComplexSpr::StoreToBin(bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(NODE_COMPLEX)); // type
	NodeSpr::StoreToBin(es);                      // common

	es.Write(static_cast<uint16_t>(m_action));    // action
}

void ComplexSpr::StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	NodeSpr::StoreToJson(val, alloc);
}

void ComplexSpr::LoadFromBin(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	NodeSpr::LoadFromBin(alloc, is);

	m_action = is.UInt16();
}

void ComplexSpr::LoadFromJson(mm::LinearAllocator& alloc, const rapidjson::Value& val)
{
	NodeSpr::LoadFromJson(alloc, val);
}

}