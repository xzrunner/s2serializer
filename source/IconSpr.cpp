#include "s2s/IconSpr.h"
#include "s2s/NodeType.h"

#include <bs/ExportStream.h>
#include <bs/ImportStream.h>
#include <bs/FixedPointNum.h>

namespace s2s
{

static const int FIXED_TRANS_PRECISION = 1024;

IconSpr::IconSpr()
	: m_process(0)
{
}

size_t IconSpr::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	sz += sizeof(uint8_t);          // type
	sz += NodeSpr::GetBinSize(dir); // common

	sz += sizeof(uint16_t);         // process

	return sz;
}

void IconSpr::StoreToBin(const std::string& dir, bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(NODE_ICON)); // type
	NodeSpr::StoreToBin(dir, es);              // common

	es.Write(static_cast<uint16_t>(m_process));
}

void IconSpr::StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	NodeSpr::StoreToJson(dir, val, alloc);

	val["process"].SetFloat(bs::int2float(m_process, FIXED_TRANS_PRECISION));
}

void IconSpr::LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	NodeSpr::LoadFromBin(alloc, dir, is);

	m_process = is.UInt16();
}

void IconSpr::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	NodeSpr::LoadFromJson(alloc, dir, val);

	m_process = bs::float2int(val["process"].GetFloat(), FIXED_TRANS_PRECISION);
}

float IconSpr::GetProcess() const
{
	return bs::int2float(m_process, FIXED_TRANS_PRECISION);
}

}