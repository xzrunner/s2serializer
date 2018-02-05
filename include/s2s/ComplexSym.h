#pragma once

#include "s2s/NodeSym.h"

#include <bs/typedef.h>

#include <string>
#include <memory>

namespace mm { class LinearAllocator; }
namespace bs { class ImportStream; class ExportStream; }

namespace s2s
{

class NodeSpr;

class ComplexSym : public NodeSym
{
public:
	ComplexSym();

	//
	// serialization
	//
	virtual void StoreToBin(const std::string& dir, uint8_t** data, size_t& length) const override;
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, 
		rapidjson::MemoryPoolAllocator<>& alloc) const override;

	//
	// deserialization
	//
	static ComplexSym* Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is);
	static ComplexSym* Create(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val);
	
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
	size_t GetBinSize(const std::string& dir) const;

	static size_t MemSize() {
		return ALIGN_4BYTE(sizeof(ComplexSym) + bs::PTR_SIZE_DIFF * 2);
	}

public:
	// xmin, ymin, xmax, ymax
	int16_t   scissor[4];

	uint16_t  children_n;
	uint16_t  actions_n;

	Action*   actions;

	NodeSpr** children;

}; // ComplexSym

}
