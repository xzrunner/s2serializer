#include "sns/ComplexSpr.h"
#include "sns/NodeType.h"

#include <bs/ImportStream.h>
#include <bs/ExportStream.h>

namespace sns
{

ComplexSpr::ComplexSpr()
	: m_action(0)
{
}

size_t ComplexSpr::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	sz += sizeof(uint8_t);          // type
	sz += NodeSpr::GetBinSize(dir); // common

	sz += sizeof(uint16_t);         // action

	return sz;
}

void ComplexSpr::StoreToBin(const std::string& dir, bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(NODE_COMPLEX)); // type
	NodeSpr::StoreToBin(dir, es);                 // common

	es.Write(static_cast<uint16_t>(m_action));    // action
}

void ComplexSpr::StoreToJson(const std::string& dir, rapidjson::Value& val, 
	                         rapidjson::MemoryPoolAllocator<>& alloc) const
{
	NodeSpr::StoreToJson(dir, val, alloc);
}

void ComplexSpr::LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	NodeSpr::LoadFromBin(alloc, dir, is);

	m_action = is.UInt16();
}

void ComplexSpr::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	NodeSpr::LoadFromJson(alloc, dir, val);
}

}