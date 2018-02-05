#pragma once

#include "s2s/NodeSym.h"

#include <bs/typedef.h>

namespace mm { class LinearAllocator; }
namespace bs { class ImportStream; class ExportStream; }

namespace s2s
{

class NodeSpr;

class AnimSym : public NodeSym
{
public:
	//
	// serialization
	//
	virtual void StoreToBin(const std::string& dir, uint8_t** data, size_t& length) const override;
	virtual void StoreToJson(const std::string& dir, rapidjson::Value& val, 
		rapidjson::MemoryPoolAllocator<>& alloc) const override;

	//
	// deserialization
	//
	static AnimSym* Create(mm::LinearAllocator& alloc, const std::string& dir, bs::ImportStream& is);
	static AnimSym* Create(mm::LinearAllocator& alloc, const std::string& dir, const rapidjson::Value& val);
	
private:
	size_t GetBinSize(const std::string& dir) const;

	static size_t MemSize() {
		return ALIGN_4BYTE(sizeof(AnimSym) + bs::PTR_SIZE_DIFF);
	}

public:
	enum LerpType
	{
		LERP_UNKNOWN = 0,
		LERP_CIRCLE,
		LERP_SPIRAL,
		LERP_WIGGLE,
		LERP_EASE,
	};

	struct Lerp
	{
		uint8_t   spr_data;
		uint8_t   type;
		uint16_t  data_n;
		uint32_t  padding;
		uint32_t* data;

		size_t GetBinSize() const;
		void StoreToBin(bs::ExportStream& es) const;

		static size_t MemSize() {
			return ALIGN_4BYTE(sizeof(Lerp) + bs::PTR_SIZE_DIFF);
		}

		static Lerp* Create(mm::LinearAllocator& alloc, bs::ImportStream& is);
		static Lerp* Create(mm::LinearAllocator& alloc, const rapidjson::Value& val);

	private:
		static int GetDataNum(int type);
	};

	struct Frame
	{
		NodeSpr** actors;
		Lerp**    lerps;
		uint16_t  actors_n;
		uint16_t  lerps_n;
		int16_t   index;
		uint8_t   tween;
		uint8_t   padding;

		size_t GetBinSize(const std::string& dir) const;
		void StoreToBin(const std::string& dir, bs::ExportStream& es) const;

		static size_t MemSize() {
			return ALIGN_4BYTE(sizeof(Frame) + bs::PTR_SIZE_DIFF * 2);
		}

		void Create(mm::LinearAllocator& alloc, const std::string& dir,
			bs::ImportStream& is);
		void Create(mm::LinearAllocator& alloc, const std::string& dir, 
			const rapidjson::Value& val);
	};

	struct Layer
	{
		Frame*   frames;
		uint16_t frames_n;
		uint16_t padding[3];

		size_t GetBinSize(const std::string& dir) const;
		void StoreToBin(const std::string& dir, bs::ExportStream& es) const;

		static size_t MemSize() {
			return ALIGN_4BYTE(sizeof(Layer) + bs::PTR_SIZE_DIFF);
		}

		void Create(mm::LinearAllocator& alloc, const std::string& dir,
			bs::ImportStream& is);
		void Create(mm::LinearAllocator& alloc, const std::string& dir,
			const rapidjson::Value& val);
	};

public:
	uint16_t m_fps;
	uint16_t m_layers_n;
	Layer*   m_layers;
	
}; // AnimSym

}