#include "sns/ComplexSym.h"
#include "sns/NodeFactory.h"
#include "sns/NodeSpr.h"

#include <bs/ImportStream.h>
#include <bs/ExportStream.h>
#include <bs/Serializer.h>
#include <memmgr/LinearAllocator.h>

namespace sns
{

void ComplexSym::StoreToBin(uint8_t** data, size_t& length) const
{
	length = GetBinSize();
	*data = new uint8_t[length];
	bs::ExportStream es(*data, length);
	
	// type
	es.Write(static_cast<uint8_t>(NODE_COMPLEX));

	// scissor
	for (size_t i = 0; i < 4; ++i) {
		es.Write(static_cast<int16_t>(m_scissor[i]));
	}

	// children
	es.Write(static_cast<uint16_t>(m_children_n));
	for (size_t i = 0; i < m_children_n; ++i) {
		m_children[i]->StoreToBin(es);
	}

	// actions
	es.Write(static_cast<uint16_t>(m_actions_n));
	for (size_t i = 0; i < m_actions_n; ++i) {
		m_actions[i].StoreToBin(es);
	}

	GD_ASSERT(es.Empty(), "error bin sz");
}

void ComplexSym::StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{
	// scissor
	val["xmin"] = m_scissor[0];
	val["ymin"] = m_scissor[1];
	val["xmax"] = m_scissor[2];
	val["ymax"] = m_scissor[3];

	// children
	for (size_t i = 0; i < m_children_n; ++i) {
		m_children[i]->StoreToJson(val["sprite"][i], alloc);
	}
}

ComplexSym* ComplexSym::Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	void* ptr = alloc.alloc<char>(MemSize());
	ComplexSym* sym = new (ptr) ComplexSym();

	// scissor
	for (size_t i = 0; i < 4; ++i) {
		sym->m_scissor[i] = is.UInt16();
	}

	// children
	sym->m_children_n = is.UInt16();
	sym->m_children = static_cast<NodeSpr**>(alloc.alloc<char>(sizeof(NodeSpr*) * sym->m_children_n));
	for (size_t i = 0; i < sym->m_children_n; ++i) {
		sym->m_children[i] = NodeFactory::CreateSprFromBin(alloc, dir, is);
	}

	// actions
	uint16_t actions_n = is.UInt16();
	sym->m_actions_n = static_cast<uint16_t>(actions_n);
	sym->m_actions = static_cast<Action*>(alloc.alloc<char>(Action::MemSize() * actions_n));
	for (size_t i = 0; i < actions_n; ++i)
	{
		Action* dst = &sym->m_actions[i];
		dst->name = is.String(alloc);
		dst->idx = bs::unpack_array16(alloc, is, 1, dst->n);
	}

	return sym;
}

ComplexSym* ComplexSym::Create(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	void* ptr = alloc.alloc<char>(MemSize());
	ComplexSym* sym = new (ptr) ComplexSym();

	// scissor
	sym->m_scissor[0] = static_cast<int16_t>(val["xmin"].GetFloat());
	sym->m_scissor[1] = static_cast<int16_t>(val["ymin"].GetFloat());
	sym->m_scissor[2] = static_cast<int16_t>(val["xmax"].GetFloat());
	sym->m_scissor[3] = static_cast<int16_t>(val["ymax"].GetFloat());

	// children
	auto children = val["sprite"].GetArray();
	sym->m_children_n = children.Size();
	sym->m_children = static_cast<NodeSpr**>(alloc.alloc<char>(sizeof(NodeSpr*) * sym->m_children_n));
	int idx = 0;
	for (auto& child : children) {
		sym->m_children[idx++] = NodeFactory::CreateSprFromJson(alloc, dir, child);
	}

	// actions
	// todo	
	sym->m_actions_n = 0;
	sym->m_actions = nullptr;

	return sym;
}

void ComplexSym::GetScissor(int16_t& xmin, int16_t& ymin, int16_t& xmax, int16_t& ymax) const
{
	xmin = m_scissor[0];
	ymin = m_scissor[1];
	xmax = m_scissor[2];
	ymax = m_scissor[3];
}

size_t ComplexSym::GetBinSize() const
{
	size_t sz = 0;

	// type
	sz += sizeof(uint8_t);
	// scissor
	sz += sizeof(int16_t) * 4;
	// children
	sz += sizeof(m_children_n);
	for (size_t i = 0; i < m_children_n; ++i) {
		sz += m_children[i]->GetBinSize();
	}
	// actions
	sz += sizeof(m_actions_n);
	for (size_t i = 0; i < m_actions_n; ++i) {
		sz += m_actions[i].GetBinSize();
	}

	return sz;
}

size_t ComplexSym::Action::GetBinSize() const
{
	size_t sz = 0;
	// todo
	sz += bs::pack_size(name);
	sz += sizeof(uint16_t);
	sz += sizeof(uint16_t) * n;
	return sz;
}

void ComplexSym::Action::StoreToBin(bs::ExportStream& es) const
{
	es.Write(name);
	es.Write(static_cast<uint16_t>(n));
	for (size_t i = 0; i < n; ++i) {
		es.Write(static_cast<uint16_t>(idx[i]));
	}
}

}