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
	static NodeSpr* CreateNodeSpr(mm::LinearAllocator& alloc, bs::ImportStream& is);
	static NodeSpr* CreateNodeSpr(mm::LinearAllocator& alloc, const rapidjson::Value& val);

	static NodeSym* CreateNodeSym(mm::LinearAllocator& alloc, bs::ImportStream& is);
	static NodeSym* CreateNodeSym(mm::LinearAllocator& alloc, const std::string& filepath);
	static NodeSym* CreateNodeSym(mm::LinearAllocator& alloc, const rapidjson::Value& val, NodeType type);

private:
	static NodeType GetNodeType(const std::string& filepath);

	static NodeSpr* CreateNodeSpr(mm::LinearAllocator& alloc, NodeType type);

}; // NodeFactory

}