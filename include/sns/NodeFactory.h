#pragma once

#include "sns/NodeType.h"

#include <rapidjson/document.h>

#include <string>

namespace mm { class LinearAllocator; }
namespace bs { class ImportStream; }

namespace sns
{

class NodeSpr;
class NodeSym;

class NodeFactory
{
public:
	static NodeSpr* CreateSprFromBin(mm::LinearAllocator& alloc, const std::string& dir,
		bs::ImportStream& is);
	static NodeSpr* CreateSprFromJson(mm::LinearAllocator& alloc, const std::string& dir, 
		const rapidjson::Value& val);

	static NodeSym* CreateSymFromBin(mm::LinearAllocator& alloc, const std::string& dir,
		bs::ImportStream& is);
	static NodeSym* CreateSymFromBin(mm::LinearAllocator& alloc, const std::string& filepath);
	static NodeSym* CreateSymFromJson(mm::LinearAllocator& alloc, const std::string& filepath);
	static NodeSym* CreateSymFromJson(mm::LinearAllocator& alloc, const std::string& dir, 
		const rapidjson::Value& val, NodeType type);

private:
	static NodeType GetNodeType(const std::string& filepath);

	static NodeSpr* CreateNodeSpr(mm::LinearAllocator& alloc, NodeType type);

}; // NodeFactory

}