#pragma once

#include "sns/NodeSpr.h"
#include "sns/Textbox.h"

namespace sns
{

class TextboxSpr : public NodeSpr
{
public:
	TextboxSpr();

	//
	// serialization
	//
	virtual size_t GetBinSize(const std::string& dir) const override;
	virtual void StoreToBin(const std::string& dir, bs::ExportStream& es) const override;
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, 
		rapidjson::MemoryPoolAllocator<>& alloc) const override;

	//
	// deserialization
	//
	virtual void LoadFromBin(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is) override;
	virtual void LoadFromJson(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val) override;

private:
	Textbox m_tb;

	std::string m_text;
	std::string m_tid;

}; // TextboxSpr

}