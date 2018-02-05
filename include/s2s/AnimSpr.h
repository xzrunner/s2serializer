#pragma once

#include "s2s/NodeSpr.h"

namespace s2s
{

class AnimSpr : public NodeSpr
{
public:
	AnimSpr();

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

	float GetInterval() const;
	int   GetFPS() const { return m_fps; }
	bool  IsLoop() const { return m_loop; }
	bool  IsStartRandom() const { return m_start_random; }
	
private:
	uint32_t m_interval;
	uint16_t m_fps;
	uint8_t  m_loop;
	uint8_t  m_start_random;

}; // AnimSpr

}