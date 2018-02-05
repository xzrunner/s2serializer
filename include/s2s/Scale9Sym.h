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

class Scale9Sym : public NodeSym
{
public:
	Scale9Sym();

	//
	// serialization
	//
	virtual void StoreToBin(const std::string& dir, uint8_t** data, size_t& length) const override;
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, 
		rapidjson::MemoryPoolAllocator<>& alloc) const override;

	//
	// deserialization
	//
	static Scale9Sym* Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is);
	static Scale9Sym* Create(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val);

	int Type() const { return m_type; }

	const NodeSpr* GetChildByIndex(size_t idx) const {
		return idx < 9 ? m_children[idx] : nullptr;
	}
	
	void GetSize(int& left, int& right, int& top, int& down) const {
		left = m_left; right = m_right; top = m_top; down = m_down;
	}

private:
	size_t GetBinSize(const std::string& dir) const;

	static size_t MemSize() {
		return ALIGN_4BYTE(sizeof(Scale9Sym) + bs::PTR_SIZE_DIFF);
	}

	static void GetImgSize(const NodeSpr* spr, int& w, int& h);

public:
	enum Scale9Type
	{
		S9_NULL = 0,
		S9_9GRID,
		S9_3GRID_HORI,
		S9_3GRID_VERT,
		S9_6GRID_UPPER,
		S9_9GRID_HOLLOW
	};

	enum Scale9Idx
	{
		S9_DOWN_LEFT   = 0,
		S9_DOWN_CENTER = 1,
		S9_DOWN_RIGHT  = 2,

		S9_MID_LEFT    = 3,
		S9_MID_CENTER  = 4,
		S9_MID_RIGHT   = 5,

		S9_TOP_LEFT    = 6,
		S9_TOP_CENTER  = 7,
		S9_TOP_RIGHT   = 8,
	};

private:
	uint32_t m_type;

	uint16_t m_left, m_right, m_top, m_down;

	NodeSpr* m_children[9];

}; // Scale9Sym

}