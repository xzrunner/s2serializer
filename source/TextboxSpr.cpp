#include "s2s/TextboxSpr.h"
#include "s2s/NodeType.h"
#include "s2s/ColorParser.h"
#include "s2s/NodeType.h"

#include <bs/ExportStream.h>
#include <bs/ImportStream.h>

namespace s2s
{

TextboxSpr::TextboxSpr()
{
}

size_t TextboxSpr::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	sz += sizeof(uint8_t);          // type
	sz += NodeSpr::GetBinSize(dir); // common

	sz += m_tb.GetBinSize();
	
	sz += bs::pack_size(m_text);
	sz += bs::pack_size(m_tid);

	return sz;
}

void TextboxSpr::StoreToBin(const std::string& dir, bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(NODE_TEXTBOX)); // type
	NodeSpr::StoreToBin(dir, es);                 // common

	m_tb.StoreToBin(es);

	es.Write(m_text);
	es.Write(m_tid);
}

void TextboxSpr::StoreToJson(const std::string& dir, rapidjson::Value& val, 
	                         rapidjson::MemoryPoolAllocator<>& alloc) const
{
	NodeSpr::StoreToJson(dir, val, alloc);

	m_tb.StoreToJson(val["text"], alloc);

	val["text"]["text"].SetString(m_text.c_str(), alloc);
	val["text"]["tid"].SetString(m_tid.c_str(), alloc);
}

void TextboxSpr::LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	NodeSpr::LoadFromBin(alloc, dir, is);

	m_tb.LoadFromBin(alloc, is);
	
	m_text = is.String();
	m_tid  = is.String();
}

void TextboxSpr::LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	NodeSpr::LoadFromJson(alloc, dir, val);

	m_tb.LoadFromJson(alloc, val["text"]);

	m_text = val["text"]["text"].GetString();
	m_tid  = val["text"]["tid"].GetString();
}

}