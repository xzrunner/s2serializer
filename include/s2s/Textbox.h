#pragma once

#include <rapidjson/document.h>

namespace mm { class LinearAllocator; }
namespace bs { class ImportStream; class ExportStream; }

namespace s2s
{

class Textbox
{
public:
	Textbox();

	size_t GetBinSize() const;
	void StoreToBin(bs::ExportStream& es) const;
	void StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const;

	void LoadFromBin(mm::LinearAllocator& alloc, bs::ImportStream& is);
	void LoadFromJson(mm::LinearAllocator& alloc, const rapidjson::Value& val);

private:
	static const uint32_t FLAG_EDGE = 0x00000001;
	static const uint32_t FLAG_RICHTEXT = 0x00000002;
	static const uint32_t FLAG_OVERFLOW = 0x00000004;

private:
	uint16_t m_width;
	uint16_t m_height;

	uint16_t m_font;
	uint16_t m_font_size;
	uint32_t m_font_color;

	float    m_edge_size;
	uint32_t m_edge_color;

	uint16_t m_align_hori;
	uint16_t m_align_vert;

	float    m_space_hori;
	float    m_space_vert;

	uint32_t m_flags;

}; // Textbox

}