#include "sns/TextboxSpr.h"
#include "sns/NodeType.h"
#include "sns/ColorParser.h"
#include "sns/NodeType.h"

#include <bs/ExportStream.h>
#include <bs/ImportStream.h>

namespace sns
{

TextboxSpr::TextboxSpr()
{
}

size_t TextboxSpr::GetBinSize() const
{
	size_t sz = 0;

	sz += sizeof(uint8_t);       // type
	sz += NodeSpr::GetBinSize(); // common

	sz += m_tb.GetBinSize();
	
	sz += bs::pack_size(m_text);
	sz += bs::pack_size(m_tid);

	return sz;
}

void TextboxSpr::StoreToBin(bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(NODE_TEXTBOX)); // type
	NodeSpr::StoreToBin(es);                      // common

	m_tb.StoreToBin(es);

	es.Write(m_text);
	es.Write(m_tid);
}

void TextboxSpr::StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	NodeSpr::StoreToJson(val, alloc);

	m_tb.StoreToJson(val, alloc);

	val["text"].SetString(m_text.c_str(), alloc);
	val["tid"].SetString(m_tid.c_str(), alloc);
}

void TextboxSpr::LoadFromBin(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	NodeSpr::LoadFromBin(alloc, is);

	m_tb.LoadFromBin(alloc, is);
	
	m_text = is.String();
	m_tid  = is.String();
}

void TextboxSpr::LoadFromJson(mm::LinearAllocator& alloc, const rapidjson::Value& val)
{
	NodeSpr::LoadFromJson(alloc, val);

	m_tb.LoadFromJson(alloc, val["text"]);

	m_text = val["text"]["text"].GetString();
	m_tid  = val["text"]["tid"].GetString();
}

}