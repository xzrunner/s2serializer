#include "s2s/NodeSprCommon.h"
#include "s2s/ColorParser.h"
#include "s2s/Utility.h"

#include <bs/ImportStream.h>
#include <bs/ExportStream.h>
#include <bs/typedef.h>
#include <bs/Serializer.h>
#include <bs/FixedPointNum.h>
#include <memmgr/LinearAllocator.h>

#include <boost/filesystem.hpp>

namespace s2s
{

NodeSprCommon::NodeSprCommon()
	: m_sym_path(nullptr)
	, m_name(nullptr)
	, m_type(0)
	, m_data(nullptr)
{
}

size_t NodeSprCommon::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	auto path = GetSymRelativePath(dir);
	ChangeFileExtJsonToBin(path);
	sz += bs::pack_size(path);   // sym_path

	sz += bs::pack_size(m_name); // name
	sz += sizeof(uint32_t);      // type
	sz += DataSize(m_type);      // data

	return sz;
}

void NodeSprCommon::StoreToBin(const std::string& dir, bs::ExportStream& es) const
{
	auto path = GetSymRelativePath(dir);
	ChangeFileExtJsonToBin(path);
	es.Write(path);                          // sym_path
	es.Write(m_name);                        // name
	es.Write(static_cast<uint32_t>(m_type)); // type
	es.WriteBlock(reinterpret_cast<uint8_t*>(m_data), DataSize(m_type)); // data
}

void NodeSprCommon::StoreToJson(const std::string& dir, rapidjson::Value& val, 
	                            rapidjson::MemoryPoolAllocator<>& alloc) const
{
	// store name
	if (m_name) {
		val["name"].SetString(m_name, alloc);
	}

	// store filepath
	if (m_sym_path) {
		auto path = GetSymRelativePath(dir);
		ChangeFileExtJsonToBin(path);
		val["filepath"].SetString(path.c_str(), alloc);
	}

	int idx = 0;
	// store scale
	if ((m_type & SCALE_MASK) && (m_data[idx] != HIGH_FIXED_TRANS_PRECISION || m_data[idx + 1] != HIGH_FIXED_TRANS_PRECISION)) {
		val["x scale"] = bs::int2float(m_data[idx], HIGH_FIXED_TRANS_PRECISION);
		val["y scale"] = bs::int2float(m_data[idx], HIGH_FIXED_TRANS_PRECISION);
	}
	idx += 2;
	// store shear
	if ((m_type & SHEAR_MASK) && (m_data[idx] != 0 || m_data[idx + 1] != 0)) {
		val["x shear"] = bs::int2float(m_data[idx], HIGH_FIXED_TRANS_PRECISION);
		val["y shear"] = bs::int2float(m_data[idx], HIGH_FIXED_TRANS_PRECISION);
	}
	idx += 2;
	// store offset
	if ((m_type & OFFSET_MASK) && (m_data[idx] != 0 || m_data[idx + 1] != 0)) {
		val["x offset"] = bs::int2float(m_data[idx], LOW_FIXED_TRANS_PRECISION);
		val["y offset"] = bs::int2float(m_data[idx], LOW_FIXED_TRANS_PRECISION);
	}
	idx += 2;
	// store position
	if ((m_type & POSITION_MASK) && (m_data[idx] != 0 || m_data[idx + 1] != 0)) {
		val["position"]["x"] = bs::int2float(m_data[idx], LOW_FIXED_TRANS_PRECISION);
		val["position"]["y"] = bs::int2float(m_data[idx], LOW_FIXED_TRANS_PRECISION);
	}
	idx += 2;
	// store angle
	if ((m_type & ANGLE_MASK) && m_data[idx] != 0) {
		val["angle"]["x"] = bs::int2float(m_data[idx], HIGH_FIXED_TRANS_PRECISION);
	}
	idx += 1;
	// store multi color
	if ((m_type & COL_MUL_MASK) && m_data[idx] != 0xffffffff) { 
		val["multi color"] = rapidjson::Value(ColorParser::RGBAToString(m_data[idx], BGRA).c_str(), alloc);
	}
	idx += 1;
	// store add color
	if ((m_type & COL_ADD_MASK) && m_data[idx] != 0) {
		val["add color"] = rapidjson::Value(ColorParser::RGBAToString(m_data[idx], ABGR).c_str(), alloc);
	}
	idx += 1;
	// store r trans
	if (m_type & COL_R_MASK && m_data[idx] != 0xff000000) {
		val["r trans"] = rapidjson::Value(ColorParser::RGBAToString(m_data[idx], RGBA).c_str(), alloc);
	}
	idx += 1;
	// store g trans
	if (m_type & COL_G_MASK && m_data[idx] != 0x00ff0000) {
		val["g trans"] = rapidjson::Value(ColorParser::RGBAToString(m_data[idx], RGBA).c_str(), alloc);
	}
	idx += 1;
	// store b trans
	if (m_type & COL_R_MASK && m_data[idx] != 0x0000ff00) {
		val["b trans"] = rapidjson::Value(ColorParser::RGBAToString(m_data[idx], RGBA).c_str(), alloc);
	}
	idx += 1;
}

void NodeSprCommon::LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, 
	                            bs::ImportStream& is)
{
	auto filepath = is.String(alloc);
	m_sym_path = Utility::CopyStr2Alloc(alloc, GetSymAbsolutePath(dir, filepath));

	m_name = is.String(alloc);
	m_type = is.UInt32();

	m_data = static_cast<uint32_t*>(alloc.alloc<char>(DataSize(m_type)));
	int idx = 0;
	if (m_type & SCALE_MASK) {
		m_data[idx++] = is.UInt32();
		m_data[idx++] = is.UInt32();
	}
	if (m_type & SHEAR_MASK) {
		m_data[idx++] = is.UInt32();
		m_data[idx++] = is.UInt32();
	}
	if (m_type & OFFSET_MASK) {
		m_data[idx++] = is.UInt32();
		m_data[idx++] = is.UInt32();
	}
	if (m_type & POSITION_MASK) {
		m_data[idx++] = is.UInt32();
		m_data[idx++] = is.UInt32();
	}
	if (m_type & ANGLE_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & COL_MUL_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & COL_ADD_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & COL_R_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & COL_G_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & COL_B_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & BLEND_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & FAST_BLEND_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & FILTER_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & DOWNSMAPLE_MASK) {
		m_data[idx++] = is.UInt32();
	}
	if (m_type & CAMERA_MASK) {
		m_data[idx++] = is.UInt32();
	}
}

void NodeSprCommon::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir,
	                             const rapidjson::Value& val)
{
	// load name
	m_name = nullptr;
	if (val.HasMember("name")) {
		m_name = Utility::CopyStr2Alloc(alloc, val["name"]);
	}

	// load filepath
	m_sym_path = nullptr;
	if (val.HasMember("filepath")) {
		auto filepath = val["filepath"].GetString();
		m_sym_path = Utility::CopyStr2Alloc(alloc, GetSymAbsolutePath(dir, filepath));
	}

	m_type = 0;

	std::vector<uint32_t> data;

	// load scale
	float scale[2] = { 1, 1 };
	if (val.HasMember("x scale") && val.HasMember("y scale")) {
		scale[0] = val["x scale"].GetFloat();
		scale[1] = val["y scale"].GetFloat();
	} else if (val.HasMember("scale")) {
		scale[0] = scale[1] = val["scale"].GetFloat();
	}
	if (scale[0] != 1 || scale[1] != 1) {
		m_type |= SCALE_MASK;
		data.push_back(bs::float2int(scale[0], HIGH_FIXED_TRANS_PRECISION));
		data.push_back(bs::float2int(scale[1], HIGH_FIXED_TRANS_PRECISION));
	}

	// load shear
	float shear[2] = { 0, 0 };
	if (val.HasMember("x shear") && val.HasMember("y shear")) {
		shear[0] = val["x shear"].GetFloat();
		shear[1] = val["y shear"].GetFloat();
	}
	if (shear[0] != 0 || shear[1] != 0) {
		m_type |= SHEAR_MASK;
		data.push_back(bs::float2int(shear[0], HIGH_FIXED_TRANS_PRECISION));
		data.push_back(bs::float2int(shear[1], HIGH_FIXED_TRANS_PRECISION));
	}

	// load offset
	float offset[2] = { 0, 0 };
	if (val.HasMember("x offset") && val.HasMember("y offset"))
	{
		offset[0] = val["x offset"].GetFloat();
		offset[1] = val["y offset"].GetFloat();
	}
	if (offset[0] != 0 || offset[1] != 0) {
		m_type |= OFFSET_MASK;
		data.push_back(bs::float2int(offset[0], LOW_FIXED_TRANS_PRECISION));
		data.push_back(bs::float2int(offset[1], LOW_FIXED_TRANS_PRECISION));
	}

	// load position
	float position[2] = { 0, 0 };
	if (val.HasMember("position") && val["position"].HasMember("x") && val["position"].HasMember("y")) {
		position[0] = val["position"]["x"].GetFloat();
		position[1] = val["position"]["y"].GetFloat();
	}
	if (position[0] != 0 || position[1] != 0) {
		m_type |= POSITION_MASK;
		data.push_back(bs::float2int(position[0], LOW_FIXED_TRANS_PRECISION));
		data.push_back(bs::float2int(position[1], LOW_FIXED_TRANS_PRECISION));
	}

	// load rotate
	float angle = 0;
	if (val.HasMember("angle")) {
		angle = val["angle"].GetFloat();
	}
	if (angle != 0) {
		m_type |= ANGLE_MASK;
		data.push_back(bs::float2int(angle, HIGH_FIXED_TRANS_PRECISION));
	}

	// load¡¡color mul
	uint32_t col_mul = 0xffffffff;
	if (val.HasMember("multi color")) {
		col_mul = ColorParser::StringToRGBA(val["multi color"], BGRA);
	}
	if (col_mul != 0xffffffff) {
		m_type |= COL_MUL_MASK;
		data.push_back(col_mul);
	}

	// load color add
	uint32_t col_add = 0;
	if (val.HasMember("add color")) {
		col_add = ColorParser::StringToRGBA(val["add color"], ABGR);
	}
	if (col_add != 0) {
		m_type |= COL_ADD_MASK;
		data.push_back(col_add);
	}

	// load color rmap
	uint32_t col_rmap = 0xff000000;
	if (val.HasMember("r trans")) {
		col_rmap = ColorParser::StringToRGBA(val["r trans"], RGBA);
		col_rmap &= 0xffffff00;
	}
	if (col_rmap != 0xff000000) {
		m_type |= COL_R_MASK;
		data.push_back(col_rmap);
	}

	// load color gmap
	uint32_t col_gmap = 0x00ff0000;
	if (val.HasMember("g trans")) {
		col_gmap = ColorParser::StringToRGBA(val["g trans"], RGBA);
		col_gmap &= 0xffffff00;
	}
	if (col_gmap != 0x00ff0000) {
		m_type |= COL_G_MASK;
		data.push_back(col_gmap);
	}

	// load color bmap
	uint32_t col_bmap = 0x0000ff00;
	if (val.HasMember("b trans")) {
		col_bmap = ColorParser::StringToRGBA(val["b trans"], RGBA);
		col_bmap &= 0xffffff00;
	}
	if (col_bmap != 0x0000ff00) {
		m_type |= COL_B_MASK;
		data.push_back(col_bmap);
	}

	// store
	if (data.empty())
	{
		m_data = nullptr;
	}
	else
	{
		size_t sz = sizeof(uint32_t) * data.size();
		m_data = static_cast<uint32_t*>(alloc.alloc<char>(sz));
		memcpy(m_data, &data[0], sz);
	}
}

size_t NodeSprCommon::DataSize(uint32_t type)
{
	size_t sz = 0;

	if (type & SCALE_MASK) {
		sz += sizeof(uint32_t) * 2;
	}
	if (type & SHEAR_MASK) {
		sz += sizeof(uint32_t) * 2;
	}
	if (type & OFFSET_MASK) {
		sz += sizeof(uint32_t) * 2;
	}
	if (type & POSITION_MASK) {
		sz += sizeof(uint32_t) * 2;
	}
	if (type & ANGLE_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & COL_MUL_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & COL_ADD_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & COL_R_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & COL_G_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & COL_B_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & BLEND_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & FAST_BLEND_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & FILTER_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & DOWNSMAPLE_MASK) {
		sz += sizeof(uint32_t);
	}
	if (type & CAMERA_MASK) {
		sz += sizeof(uint32_t);
	}

	return ALIGN_4BYTE(sz);
}

std::string NodeSprCommon::GetSymAbsolutePath(const std::string& dir, const std::string& filepath)
{
	auto absolute = boost::filesystem::canonical(
		boost::filesystem::absolute(filepath, dir));
	return absolute.string();
}

std::string NodeSprCommon::GetSymRelativePath(const std::string& dir) const
{
	auto absolute_dir = boost::filesystem::absolute(dir);
	auto relative = boost::filesystem::relative(m_sym_path, absolute_dir);
	return relative.string();
}

void NodeSprCommon::ChangeFileExtJsonToBin(std::string& filepath)
{
	auto pos = filepath.find(".json");
	if (pos != std::string::npos) {
		filepath = filepath.substr(0, pos) + ".bin";
	}
}

}