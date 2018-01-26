#include "sns/IconSym.h"
#include "sns/NodeFactory.h"
#include "sns/Utility.h"

#include <memmgr/LinearAllocator.h>
#include <bs/FixedPointNum.h>
#include <bs/ImportStream.h>
#include <bs/ExportStream.h>

#include <boost/filesystem.hpp>

namespace sns
{

IconSym::IconSym()
	: m_type(0)
	, m_vertices_n(0)
	, m_vertices(nullptr)
{
}

void IconSym::StoreToBin(const std::string& dir, uint8_t** data, size_t& length) const
{
	length = GetBinSize(dir);
	*data = new uint8_t[length];
	bs::ExportStream es(*data, length);

	// type
	es.Write(static_cast<uint8_t>(NODE_ICON));

	// icon type
	es.Write(static_cast<uint8_t>(m_type));

	// sym path
	es.Write(m_base_sym_path);

	// vertices
	es.Write(static_cast<uint16_t>(m_vertices_n));
	for (int i = 0; i < m_vertices_n; ++i) {
		es.Write(m_vertices[i]);
	}

	GD_ASSERT(es.Empty(), "error bin sz");
}

void IconSym::StoreToJson(const std::string& dir, rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{

}

IconSym* IconSym::Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	void* ptr = alloc.alloc<char>(MemSize());
	IconSym* sym = new (ptr) IconSym();

	// icon type
	sym->m_type = is.UInt8();

	// sym path
	sym->m_base_sym_path = is.String(alloc);

	// vertices
	sym->m_vertices_n = is.UInt16();
	sym->m_vertices = static_cast<uint16_t*>(alloc.alloc<char>(sizeof(uint16_t) * sym->m_vertices_n));
	for (int i = 0; i < sym->m_vertices_n; ++i) {
		sym->m_vertices[i] = is.UInt16();
	}

	return sym;
}

IconSym* IconSym::Create(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val)
{
	void* ptr = alloc.alloc<char>(MemSize());
	IconSym* sym = new (ptr) IconSym();

	sym->m_base_sym_path = Utility::CopyStr2Alloc(alloc, val["image"]);

	std::string type = val["type"].GetString();
	if (type == "rect") 
	{
		sym->m_type = ICON_RECT;

		sym->m_vertices_n = 4;
		sym->m_vertices = static_cast<uint16_t*>(alloc.alloc<char>(sizeof(uint16_t) * sym->m_vertices_n));

		sym->m_vertices[0] = bs::float2int(val["xmin"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[1] = bs::float2int(val["ymin"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[2] = bs::float2int(val["xmax"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[3] = bs::float2int(val["ymax"].GetFloat(), FIXED_TRANS_PRECISION);
	}
	else if (type == "quad") 
	{
		sym->m_type = ICON_QUAD;

		sym->m_vertices_n = 16;
		sym->m_vertices = static_cast<uint16_t*>(alloc.alloc<char>(sizeof(m_vertices) * sym->m_vertices_n));

		for (int i = 0; i < 4; ++i) {
			sym->m_vertices[i * 4 + 0] = bs::float2int(val["src"][i]["x"].GetFloat(), FIXED_TRANS_PRECISION);
			sym->m_vertices[i * 4 + 1] = bs::float2int(val["src"][i]["y"].GetFloat(), FIXED_TRANS_PRECISION);
			sym->m_vertices[i * 4 + 2] = bs::float2int(val["screen"][i]["x"].GetFloat(), FIXED_TRANS_PRECISION);
			sym->m_vertices[i * 4 + 3] = bs::float2int(val["screen"][i]["y"].GetFloat(), FIXED_TRANS_PRECISION);
		}
	} 
	else if (type == "changed-rect") 
	{
		sym->m_type = ICON_CHANGED_RECT;

		sym->m_vertices_n = 8;
		sym->m_vertices = static_cast<uint16_t*>(alloc.alloc<char>(sizeof(m_vertices) * sym->m_vertices_n));

		auto& val_begin = val["begin"];
		sym->m_vertices[0] = bs::float2int(val_begin["xmin"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[1] = bs::float2int(val_begin["ymin"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[2] = bs::float2int(val_begin["xmax"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[3] = bs::float2int(val_begin["ymax"].GetFloat(), FIXED_TRANS_PRECISION);

		auto& val_end = val["end"];
		sym->m_vertices[4] = bs::float2int(val_end["xmin"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[5] = bs::float2int(val_end["ymin"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[6] = bs::float2int(val_end["xmax"].GetFloat(), FIXED_TRANS_PRECISION);
		sym->m_vertices[7] = bs::float2int(val_end["ymax"].GetFloat(), FIXED_TRANS_PRECISION);
	} 
	else if (type == "changed-sector") 
	{
		sym->m_type = ICON_CHANGED_SECTOR;
	}

	return sym;
}

size_t IconSym::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	// type
	sz += sizeof(uint8_t);
	// icon type
	sz += sizeof(uint8_t);
	// sym path
	sz += bs::pack_size(m_base_sym_path);
	// vertices
	sz += sizeof(uint16_t);
	sz += sizeof(uint16_t) * m_vertices_n;
	
	return sz;
}

}