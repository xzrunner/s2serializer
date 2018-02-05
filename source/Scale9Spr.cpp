#include "s2s/Scale9Spr.h"
#include "s2s/NodeType.h"

#include <bs/ExportStream.h>
#include <bs/ImportStream.h>

namespace s2s
{

Scale9Spr::Scale9Spr()
	: m_width(0)
	, m_height(0)
{
}

size_t Scale9Spr::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	sz += sizeof(uint8_t);          // type
	sz += NodeSpr::GetBinSize(dir); // common

	sz += sizeof(uint16_t) * 2;     // size

	return sz;
}

void Scale9Spr::StoreToBin(const std::string& dir, bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(NODE_SCALE9)); // type
	NodeSpr::StoreToBin(dir, es);                // common

	es.Write(static_cast<uint16_t>(m_width));
	es.Write(static_cast<uint16_t>(m_height));
}

void Scale9Spr::StoreToJson(const std::string& dir, rapidjson::Value& val, 
	                        rapidjson::MemoryPoolAllocator<>& alloc) const
{
	NodeSpr::StoreToJson(dir, val, alloc);

	val["scale9"]["width"].SetFloat(m_width);
	val["scale9"]["height"].SetFloat(m_width);
}

void Scale9Spr::LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	NodeSpr::LoadFromBin(alloc, dir, is);

	m_width  = is.UInt16();
	m_height = is.UInt16();
}

void Scale9Spr::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	NodeSpr::LoadFromJson(alloc, dir, val);

	m_width  = val["scale9"]["width"].GetFloat();
	m_height = val["scale9"]["height"].GetFloat();
}

}