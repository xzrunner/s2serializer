#include "sns/NodeFactory.h"
#include "sns/NodeType.h"

#include "sns/ImageSpr.h"
#include "sns/TextboxSpr.h"
#include "sns/ComplexSpr.h"
#include "sns/ComplexSym.h"
#include "sns/AnimSpr.h"
#include "sns/AnimSym.h"

#include <guard/check.h>
#include <bs/ImportStream.h>
#include <js/RapidJsonHelper.h>

#include <algorithm>
#include <fstream>

namespace sns
{

NodeSpr* NodeFactory::CreateSprFromBin(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	NodeType type = static_cast<NodeType>(is.UInt8());
	GD_ASSERT(type != NODE_INVALID, "err node type.");

	NodeSpr* spr = CreateNodeSpr(alloc, type);
	spr->LoadFromBin(alloc, is);
	
	return spr;
}

NodeSpr* NodeFactory::CreateSprFromJson(mm::LinearAllocator& alloc, const rapidjson::Value& val)
{
	NodeType type = GetNodeType(val["filepath"].GetString());
	GD_ASSERT(type != NODE_INVALID, "err node type.");

	NodeSpr* spr = CreateNodeSpr(alloc, type);
	spr->LoadFromJson(alloc, val);
	
	return spr;
}

NodeSym* NodeFactory::CreateSymFromBin(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	NodeSym* sym = nullptr;

	NodeType type = static_cast<NodeType>(is.UInt8());
	GD_ASSERT(type != NODE_INVALID, "err node type.");
	switch (type)
	{
	case NODE_COMPLEX:
		sym = ComplexSym::Create(alloc, is);
		break;
	case NODE_ANIMATION:
		sym = AnimSym::Create(alloc, is);
		break;
	default:
		GD_REPORT_ASSERT("unknown node type.");
	}

	return sym;
}

NodeSym* NodeFactory::CreateSymFromBin(mm::LinearAllocator& alloc, const std::string& filepath)
{
	std::ifstream fin(filepath, std::ios::binary);
	fin.seekg(0, std::ios::end);
	size_t len = static_cast<size_t>(fin.tellg());
	fin.seekg(0, std::ios::beg);
	char* data = new char[len];
	fin.read(data, len);
	fin.close();

	auto sym = CreateSymFromBin(alloc, bs::ImportStream(data, len));
	delete[] data;

	return sym;
}

NodeSym* NodeFactory::CreateSymFromJson(mm::LinearAllocator& alloc, const std::string& filepath)
{
	NodeType type = GetNodeType(filepath);
	GD_ASSERT(type != NODE_INVALID, "err node type.");
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);
	return CreateSymFromJson(alloc, doc, type);
}

NodeSym* NodeFactory::CreateSymFromJson(mm::LinearAllocator& alloc, const rapidjson::Value& val, NodeType type)
{
	NodeSym* sym = nullptr;

	GD_ASSERT(type != NODE_INVALID, "err node type.");
	switch (type)
	{
	case NODE_IMAGE:
		break;
	case NODE_COMPLEX:
		sym = ComplexSym::Create(alloc, val);
		break;
	case NODE_ANIMATION:
		sym = AnimSym::Create(alloc, val);
		break;
	case NODE_TEXTBOX:
		break;
	default:
		GD_REPORT_ASSERT("unknown node type.");
	}

	return sym;
}

NodeType NodeFactory::GetNodeType(const std::string& filepath)
{
	NodeType type = NODE_INVALID;
	std::string ext = filepath.substr(filepath.rfind('.') + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
	if (ext == "png" || ext == "jpg" || ext == "bmp" || ext == "ppm" || ext == "pvr" || ext == "pkm") 
	{
		type = NODE_IMAGE;
	} 
	else if (ext == "json") 
	{
		std::string tag = filepath.substr(filepath.rfind('_') + 1);
		tag = tag.substr(0, tag.rfind('.'));
		std::transform(tag.begin(), tag.end(), tag.begin(), tolower);
		if (tag == "scale9") {
			type = NODE_SCALE9;
		} else if (tag == "icon") {
			type = NODE_ICON;
		} else if (tag == "texture") {
			type = NODE_TEXTURE;
		} else if ((tag == "text")) {
			type = NODE_TEXTBOX;
		} else if (tag == "complex") {
			type = NODE_COMPLEX;
		} else if (tag == "anim") {
			type = NODE_ANIMATION;
		} else if (tag == "particle") {
			type = NODE_PARTICLE3D;
		} else if (tag == "particle2d") {
			type = NODE_PARTICLE2D;
		} else if (tag == "shape") {
			type = NODE_SHAPE;
		} else if (tag == "mesh") {
			type = NODE_MESH;
		} else if (tag == "mask") {
			type = NODE_MASK;
		} else if (tag == "trail") {
			type = NODE_TRAIL;
		} else if (tag == "skeleton") {
			type = NODE_SKELETON;
		}
	}
	else if (ext == "mp3") 
	{
		type = NODE_AUDIO;
	}

	return type;
}

#define CREATE_NODE_SPR(type) \
	spr = new (alloc.alloc<char>(sizeof(type))) type();
NodeSpr* NodeFactory::CreateNodeSpr(mm::LinearAllocator& alloc, NodeType type)
{
	NodeSpr* spr = nullptr;
	switch (type)
	{
	case NODE_IMAGE:
		CREATE_NODE_SPR(ImageSpr);
		break;
	case NODE_COMPLEX:
		CREATE_NODE_SPR(ComplexSpr);
		break;
	case NODE_TEXTBOX:
		CREATE_NODE_SPR(TextboxSpr);
		break;
	default:
		GD_REPORT_ASSERT("unknown node type.");
	}
	return spr;
}

}