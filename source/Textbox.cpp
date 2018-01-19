#include "sns/Textbox.h"
#include "sns/NodeType.h"
#include "sns/ColorParser.h"

#include <bs/Serializer.h>
#include <bs/ImportStream.h>
#include <bs/ExportStream.h>

namespace sns
{

Textbox::Textbox()
	: m_width(0)
	, m_height(0)
	, m_font(0)
	, m_font_size(0)
	, m_font_color(0)
	, m_edge_size(0)
	, m_edge_color(0)
	, m_align_hori(0)
	, m_align_vert(0)
	, m_space_hori(0)
	, m_space_vert(0)
	, m_flags(0)
{
}

size_t Textbox::GetBinSize() const
{
	size_t sz = 0;

	sz += sizeof(uint16_t); // m_width
	sz += sizeof(uint16_t); // m_height

	sz += sizeof(uint16_t); // m_font
	sz += sizeof(uint16_t); // m_font_size
	sz += sizeof(uint32_t); // m_font_color

	sz += sizeof(float);    // m_edge_size
	sz += sizeof(uint32_t); // m_edge_color

	sz += sizeof(uint16_t); // m_align_hori
	sz += sizeof(uint16_t); // m_align_vert

	sz += sizeof(float);    // m_space_hori
	sz += sizeof(float);    // m_space_vert

	sz += sizeof(uint32_t); // m_flags

	return sz;
}

void Textbox::StoreToBin(bs::ExportStream& es) const
{
	es.Write(static_cast<uint16_t>(m_width));
	es.Write(static_cast<uint16_t>(m_height));

	es.Write(static_cast<uint16_t>(m_font));
	es.Write(static_cast<uint16_t>(m_font_size));
	es.Write(static_cast<uint32_t>(m_font_color));

	es.Write(static_cast<float>(m_edge_size));
	es.Write(static_cast<uint32_t>(m_edge_color));

	es.Write(static_cast<uint16_t>(m_align_hori));
	es.Write(static_cast<uint16_t>(m_align_vert));

	es.Write(static_cast<float>(m_space_hori));
	es.Write(static_cast<float>(m_space_vert));

	es.Write(static_cast<uint32_t>(m_flags));
}

void Textbox::StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	val.AddMember("width", m_width, alloc);
	val.AddMember("height", m_height, alloc);

	val.AddMember("font", m_font, alloc);
	val.AddMember("font_size", m_font_size, alloc);
	//val.AddMember("font_color", ColorParser::RGBAToString(m_font_color, RGBA), alloc);
	val["font_color"].SetString(ColorParser::RGBAToString(m_font_color, RGBA).c_str(), alloc);

	val.AddMember("edge", m_flags & FLAG_EDGE, alloc);
	val.AddMember("edge_size", m_edge_size, alloc);
	//val.AddMember("edge_color", ColorParser::RGBAToString(m_edge_color, RGBA), alloc);
	val["edge_color"].SetString(ColorParser::RGBAToString(m_edge_color, RGBA).c_str(), alloc);

	val.AddMember("align_hori", m_align_hori, alloc);
	val.AddMember("align_vert", m_align_vert, alloc);

	val.AddMember("space_hori", m_space_hori, alloc);
	val.AddMember("space_vert", m_space_vert, alloc);

	val.AddMember("overflow", m_flags & FLAG_OVERFLOW, alloc);
	val.AddMember("richtext", m_flags & FLAG_RICHTEXT, alloc);
}

void Textbox::LoadFromBin(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	m_width      = is.UInt16();	
	m_height     = is.UInt16();

	m_font       = is.UInt16();
	m_font_size  = is.UInt16();
	m_font_color = is.UInt32();

	m_edge_size  = is.Float();
	m_edge_color = is.UInt32();

	m_align_hori = is.UInt16();
	m_align_vert = is.UInt16();

	m_space_hori = is.Float();
	m_space_vert = is.Float();

	m_flags      = is.UInt32();
}

void Textbox::LoadFromJson(mm::LinearAllocator& alloc, const rapidjson::Value& val)
{
	m_width  = val["width"].GetInt();
	m_height = val["height"].GetInt();

	m_font       = val["font"].GetInt();
	m_font_size  = val["font_size"].GetInt();
	m_font_color = ColorParser::StringToRGBA(val["font_color"], RGBA);

	if (val["edge"].GetBool()) {
		m_flags |= FLAG_EDGE;
	} else {
		m_flags &= ~FLAG_EDGE;
	}
	m_edge_size  = val["edge_size"].GetFloat();
	m_edge_color = ColorParser::StringToRGBA(val["edge_color"], RGBA);

	m_align_hori = val["align_hori"].GetInt();
	m_align_vert = val["align_vert"].GetInt();

	m_space_hori = val["space_hori"].GetFloat();
	m_space_vert = val["space_vert"].GetFloat();

	m_flags |= FLAG_OVERFLOW;
	if (val.HasMember("overflow")) 
	{
		if (val["overflow"].GetBool()) {
			m_flags |= FLAG_OVERFLOW;
		} else {
			m_flags &= ~FLAG_OVERFLOW;
		}
	}

	m_flags |= FLAG_RICHTEXT;
	if (val.HasMember("richtext"))
	{
		if (val["richtext"].GetBool()) {
			m_flags |= FLAG_RICHTEXT;
		} else {
			m_flags &= ~FLAG_RICHTEXT;
		}
	}
}

}