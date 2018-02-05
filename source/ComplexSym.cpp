#include "s2s/ComplexSym.h"
#include "s2s/NodeFactory.h"
#include "s2s/NodeSpr.h"

#include <bs/ImportStream.h>
#include <bs/ExportStream.h>
#include <bs/Serializer.h>
#include <memmgr/LinearAllocator.h>

namespace s2s
{

ComplexSym::ComplexSym()
{
	memset(this, 0, sizeof(*this));
}

void ComplexSym::StoreToBin(const std::string& dir, uint8_t** data, size_t& length) const
{
	length = GetBinSize(dir);
	*data = new uint8_t[length];
	bs::ExportStream es(*data, length);
	
	// type
	es.Write(static_cast<uint8_t>(NODE_COMPLEX));

	// scissor
	for (size_t i = 0; i < 4; ++i) {
		es.Write(static_cast<int16_t>(scissor[i]));
	}

	// children
	es.Write(static_cast<uint16_t>(children_n));
	for (size_t i = 0; i < children_n; ++i) {
		children[i]->StoreToBin(dir, es);
	}

	// actions
	es.Write(static_cast<uint16_t>(actions_n));
	for (size_t i = 0; i < actions_n; ++i) {
		actions[i].StoreToBin(es);
	}

	GD_ASSERT(es.Empty(), "error bin sz");
}

void ComplexSym::StoreToJson(const std::string& dir, rapidjson::Value& val, 
	                         rapidjson::MemoryPoolAllocator<>& alloc) const
{
	// scissor
	val["xmin"] = scissor[0];
	val["ymin"] = scissor[1];
	val["xmax"] = scissor[2];
	val["ymax"] = scissor[3];

	// children
	for (size_t i = 0; i < children_n; ++i) {
		children[i]->StoreToJson(dir, val["sprite"][i], alloc);
	}
}

ComplexSym* ComplexSym::Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	void* ptr = alloc.alloc<char>(MemSize());
	ComplexSym* sym = new (ptr) ComplexSym();

	// scissor
	for (size_t i = 0; i < 4; ++i) {
		sym->scissor[i] = is.UInt16();
	}

	// children
	sym->children_n = is.UInt16();
	sym->children = static_cast<NodeSpr**>(alloc.alloc<char>(sizeof(NodeSpr*) * sym->children_n));
	for (size_t i = 0; i < sym->children_n; ++i) {
		sym->children[i] = NodeFactory::CreateSprFromBin(alloc, dir, is);
	}

	// actions
	uint16_t actions_n = is.UInt16();
	sym->actions_n = static_cast<uint16_t>(actions_n);
	sym->actions = static_cast<Action*>(alloc.alloc<char>(Action::MemSize() * actions_n));
	for (size_t i = 0; i < actions_n; ++i)
	{
		Action* dst = &sym->actions[i];
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
	sym->scissor[0] = static_cast<int16_t>(val["xmin"].GetInt());
	sym->scissor[1] = static_cast<int16_t>(val["ymin"].GetInt());
	sym->scissor[2] = static_cast<int16_t>(val["xmax"].GetInt());
	sym->scissor[3] = static_cast<int16_t>(val["ymax"].GetInt());

	// children
	auto children = val["sprite"].GetArray();
	sym->children_n = children.Size();
	sym->children = static_cast<NodeSpr**>(alloc.alloc<char>(sizeof(NodeSpr*) * sym->children_n));
	int idx = 0;
	for (auto& child : children) {
		sym->children[idx++] = NodeFactory::CreateSprFromJson(alloc, dir, child);
	}

	// actions
	// todo	
	sym->actions_n = 0;
	sym->actions = nullptr;

	return sym;
}

size_t ComplexSym::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	// type
	sz += sizeof(uint8_t);
	// scissor
	sz += sizeof(int16_t) * 4;
	// children
	sz += sizeof(children_n);
	for (size_t i = 0; i < children_n; ++i) {
		sz += children[i]->GetBinSize(dir);
	}
	// actions
	sz += sizeof(actions_n);
	for (size_t i = 0; i < actions_n; ++i) {
		sz += actions[i].GetBinSize();
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