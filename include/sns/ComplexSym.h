#pragma once

#include "sns/NodeSym.h"

#include <bs/typedef.h>

#include <string>
#include <memory>

namespace mm { class LinearAllocator; }
namespace bs { class ImportStream; class ExportStream; }

namespace sns
{

class NodeSpr;

class ComplexSym : public NodeSym
{
public:
	//
	// serialization
	//
	virtual void StoreToBin(uint8_t** data, size_t& length) const override;
	virtual void StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const override;

	//
	// deserialization
	//
	static ComplexSym* Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is);
	static ComplexSym* Create(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val);

	void GetScissor(int16_t& xmin, int16_t& ymin, int16_t& xmax, int16_t& ymax) const;

	size_t GetChildrenNum() const { return m_children_n; }
	const NodeSpr* GetChildByIndex(size_t idx) const {
		return idx < m_children_n ? m_children[idx] : nullptr;
	}
	
public:
	struct Action
	{
		const char* name;
		uint16_t* idx;
		uint16_t n;

		size_t GetBinSize() const;
		void StoreToBin(bs::ExportStream& es) const;

		static size_t MemSize() {
			return ALIGN_4BYTE(sizeof(Action) + bs::PTR_SIZE_DIFF * 2);
		}

	}; // Action

private:
	size_t GetBinSize() const;

	static size_t MemSize() {
		return ALIGN_4BYTE(sizeof(ComplexSym) + bs::PTR_SIZE_DIFF * 2);
	}

private:
	int16_t m_scissor[4];

	uint16_t m_children_n;
	uint16_t m_actions_n;

	Action* m_actions;

	NodeSpr** m_children;

}; // ComplexSym

}
