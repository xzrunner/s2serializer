#pragma once

#include "sns/NodeSym.h"

#include <bs/typedef.h>

namespace mm { class LinearAllocator; }
namespace bs { class ImportStream; class ExportStream; }

namespace sns
{

class IconSym : public NodeSym
{
public:
	IconSym();

	//
	// serialization
	//
	virtual void StoreToBin(const std::string& dir, uint8_t** data, size_t& length) const override;
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val,
		rapidjson::MemoryPoolAllocator<>& alloc) const override;

	//
	// deserialization
	//
	static IconSym* Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is);
	static IconSym* Create(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val);

	const char* GetBaseSymPath() const { return m_base_sym_path; }

	int GetType() const { return m_type; }

	int GetVerticesNum() const { return m_vertices_n; }
	const uint16_t* GetVertices() const { return m_vertices; }

public:
	enum IconType
	{
		ICON_UNKNOWN = 0,
		ICON_RECT,
		ICON_QUAD,
		ICON_CHANGED_RECT,
		ICON_CHANGED_SECTOR,

	}; // IconType

	static const int FIXED_TRANS_PRECISION = 1024;

private:
	size_t GetBinSize(const std::string& dir) const;

	static size_t MemSize() {
		return ALIGN_4BYTE(sizeof(IconSym) + bs::PTR_SIZE_DIFF * 2);
	}

private:
	uint16_t  m_type;

	uint16_t  m_vertices_n;
	uint16_t* m_vertices;

	const char* m_base_sym_path;

}; // IconSym

}