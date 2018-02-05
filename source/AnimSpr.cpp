#include "s2s/AnimSpr.h"
#include "s2s/NodeType.h"

#include <guard/check.h>
#include <bs/ExportStream.h>
#include <bs/ImportStream.h>
#include <bs/FixedPointNum.h>

namespace s2s
{

static const int INTERVAL_PRECISION = 1000;

AnimSpr::AnimSpr()
	: m_interval(0)
	, m_fps(0)
	, m_loop(0)
	, m_start_random(0)
{
}

size_t AnimSpr::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	sz += sizeof(uint8_t);          // type
	sz += NodeSpr::GetBinSize(dir); // common

	sz += sizeof(uint32_t);         // interval
	sz += sizeof(uint16_t);         // fps
	sz += sizeof(uint8_t);          // loop
	sz += sizeof(uint8_t);          // start_random

	return sz;
}

void AnimSpr::StoreToBin(const std::string& dir, bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(NODE_ANIMATION)); // type
	NodeSpr::StoreToBin(dir, es);                   // common

	es.Write(static_cast<uint32_t>(m_interval));    // interval
	es.Write(static_cast<uint16_t>(m_fps));         // fps
	es.Write(static_cast<uint8_t>(m_loop));         // loop
	es.Write(static_cast<uint8_t>(m_start_random)); // start_random
}

void AnimSpr::StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	NodeSpr::StoreToJson(dir, val, alloc);

	val["animation"]["interval"].SetFloat(bs::int2float(m_interval, INTERVAL_PRECISION));
	val["animation"]["fps"].SetInt(m_fps);
	val["animation"]["loop"].SetBool(bs::int2bool(m_loop));
	val["animation"]["start_random"].SetBool(bs::int2bool(m_start_random));
}

void AnimSpr::LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	NodeSpr::LoadFromBin(alloc, dir, is);

	m_interval     = is.UInt32();
	m_fps          = is.UInt16();
	m_loop         = is.UInt8();
	m_start_random = is.UInt8();
}

void AnimSpr::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	NodeSpr::LoadFromJson(alloc, dir, val);

	GD_ASSERT(val.HasMember("animation"), "err anim val.");
	auto& anim_val = val["animation"];
	m_interval     = bs::float2int(anim_val["interval"].GetFloat(), INTERVAL_PRECISION);
	m_fps          = anim_val["fps"].GetInt();
	m_loop         = bs::bool2int(anim_val["loop"].GetBool());
	m_start_random = bs::bool2int(anim_val["start_random"].GetBool());
}

float AnimSpr::GetInterval() const
{
	return bs::int2float(m_interval, INTERVAL_PRECISION);
}

}