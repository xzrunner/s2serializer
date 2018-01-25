#include "sns/Scale9Sym.h"
#include "sns/NodeFactory.h"
#include "sns/NodeSpr.h"

#include <guard/check.h>
#include <memmgr/LinearAllocator.h>
#include <gimg_import.h>
#include <bs/ExportStream.h>
#include <bs/ImportStream.h>

namespace sns
{

Scale9Sym::Scale9Sym()
	: m_type(S9_NULL)
	, m_left(0)
	, m_right(0)
	, m_top(0)
	, m_down(0)
{
	memset(m_children, 0, sizeof(m_children));
}

void Scale9Sym::StoreToBin(const std::string& dir, uint8_t** data, size_t& length) const
{
	length = GetBinSize(dir);
	*data = new uint8_t[length];
	bs::ExportStream es(*data, length);

	// type
	es.Write(static_cast<uint8_t>(NODE_SCALE9));

	// scale9 type
	es.Write(static_cast<uint8_t>(m_type));

	// size
	es.Write(static_cast<uint16_t>(m_left));
	es.Write(static_cast<uint16_t>(m_right));
	es.Write(static_cast<uint16_t>(m_top));
	es.Write(static_cast<uint16_t>(m_down));

	// children
	for (int i = 0; i < 9; ++i) {
		if (m_children[i]) {
			m_children[i]->StoreToBin(dir, es);
		}
	}

	GD_ASSERT(es.Empty(), "error bin sz");
}

void Scale9Sym::StoreToJson(const std::string& dir, rapidjson::Value& val, 
	                        rapidjson::MemoryPoolAllocator<>& alloc) const
{
}

Scale9Sym* Scale9Sym::Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is)
{
	void* ptr = alloc.alloc<char>(MemSize());
	Scale9Sym* sym = new (ptr) Scale9Sym();

	// scale9 type
	sym->m_type = is.UInt8();

	// size
	sym->m_left  = is.UInt16();
	sym->m_right = is.UInt16();
	sym->m_top   = is.UInt16();
	sym->m_down  = is.UInt16();

	// children
	switch (sym->m_type)
	{
	case S9_9GRID:
		sym->m_children[S9_DOWN_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_DOWN_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_DOWN_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		break;
	case S9_9GRID_HOLLOW:
		sym->m_children[S9_DOWN_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_DOWN_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_DOWN_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		break;
	case S9_3GRID_HORI:
		sym->m_children[S9_MID_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		break;
	case S9_3GRID_VERT:
		sym->m_children[S9_DOWN_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		break;
	case S9_6GRID_UPPER:
		sym->m_children[S9_MID_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_MID_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_LEFT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_CENTER]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		sym->m_children[S9_TOP_RIGHT]	= NodeFactory::CreateSprFromBin(alloc, dir, is);
		break;
	default:
		GD_REPORT_ASSERT("Unknown type.");
	}

	return sym;
}

Scale9Sym* Scale9Sym::Create(mm::LinearAllocator& alloc, const std::string& dir, 
	                         const rapidjson::Value& val)
{
	void* ptr = alloc.alloc<char>(MemSize());
	Scale9Sym* sym = new (ptr) Scale9Sym();

	sym->m_type = static_cast<Scale9Type>(val["type"].GetInt());
	auto itr = val["sprite"].GetArray().Begin();
	switch (sym->m_type)
	{
	case S9_9GRID:
		sym->m_children[S9_DOWN_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_DOWN_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_DOWN_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		break;
	case S9_9GRID_HOLLOW:
		sym->m_children[S9_DOWN_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_DOWN_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_DOWN_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		break;
	case S9_3GRID_HORI:
		sym->m_children[S9_MID_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		break;
	case S9_3GRID_VERT:
		sym->m_children[S9_DOWN_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		break;
	case S9_6GRID_UPPER:
		sym->m_children[S9_MID_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_MID_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_LEFT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_CENTER]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		sym->m_children[S9_TOP_RIGHT]	= NodeFactory::CreateSprFromJson(alloc, dir, *itr++);
		break;
	default:
		GD_REPORT_ASSERT("Unknown type.");
	}

	NodeSpr* spr = nullptr;
	int w = 0, h = 0;
	// left
	if (spr = sym->m_children[S9_DOWN_LEFT]) {
	} else if (spr = sym->m_children[S9_MID_LEFT]) {
	} else if (spr = sym->m_children[S9_TOP_LEFT]) {
	}
	if (spr) {
		GetImgSize(spr, w, h);
		sym->m_left = w;
	}
	// right
	if (spr = sym->m_children[S9_DOWN_RIGHT]) {
	} else if (spr = sym->m_children[S9_MID_RIGHT]) {
	} else if (spr = sym->m_children[S9_TOP_RIGHT]) {
	}
	if (spr) {
		GetImgSize(spr, w, h);
		sym->m_right = w;
	}
	// top
	if (spr = sym->m_children[S9_TOP_LEFT]) {
	} else if (spr = sym->m_children[S9_TOP_CENTER]) {
	} else if (spr = sym->m_children[S9_TOP_RIGHT]) {
	}
	if (spr) {
		GetImgSize(spr, w, h);
		sym->m_top = h;
	}
	// down
	if (spr = sym->m_children[S9_DOWN_LEFT]) {
	} else if (spr = sym->m_children[S9_DOWN_CENTER]) {
	} else if (spr = sym->m_children[S9_DOWN_RIGHT]) {
	}
	if (spr) {
		GetImgSize(spr, w, h);
		sym->m_down = h;
	}

	return sym;
}

size_t Scale9Sym::GetBinSize(const std::string& dir) const
{
	size_t sz = 0;

	// type
	sz += sizeof(uint8_t);
	// scale9 type
	sz += sizeof(uint8_t);
	// size
	sz += sizeof(uint16_t) * 4;
	// children
	for (int i = 0; i < 9; ++i) {
		if (m_children[i]) {
			sz += m_children[i]->GetBinSize(dir);
		}
	}

	return sz;
}

void Scale9Sym::GetImgSize(const NodeSpr* spr, int& w, int& h)
{
	GD_ASSERT(spr, "null spr.");

	auto filepath = spr->GetCommon().GetFilepath();
	if (!gimg_read_header(filepath, &w, &h)) {
		GD_REPORT_ASSERT("Not supported image format.");
	}
}

}