#include "sns/AnimSym.h"
#include "sns/NodeFactory.h"
#include "sns/NodeSpr.h"

#include <bs/ImportStream.h>
#include <bs/ExportStream.h>
#include <bs/Serializer.h>
#include <memmgr/LinearAllocator.h>

namespace sns
{

static const float DEG_TO_RAD = 3.1415926f / 180.0f;

void AnimSym::StoreToBin(uint8_t** data, size_t& length) const
{
	length = GetBinSize();
	*data = new uint8_t[length];
	bs::ExportStream es(*data, length);

	// type
	es.Write(static_cast<uint8_t>(NODE_ANIMATION));

	// fps
	es.Write(static_cast<uint16_t>(m_fps));

	// layers
	es.Write(static_cast<uint16_t>(m_layers_n));
	for (size_t i = 0; i < m_layers_n; ++i) {
		m_layers[i].StoreToBin(es);
	}

	GD_ASSERT(es.Empty(), "error bin sz");
}

void AnimSym::StoreToJson(rapidjson::Value& val, rapidjson::MemoryPoolAllocator<>& alloc) const
{

}

AnimSym* AnimSym::Create(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	void* ptr = alloc.alloc<char>(MemSize());
	AnimSym* sym = new (ptr) AnimSym();

	// fps
	sym->m_fps = is.UInt16();
	
	// layers
	sym->m_layers_n = is.UInt16();
	sym->m_layers = static_cast<Layer*>(alloc.alloc<char>(Layer::MemSize() * sym->m_layers_n));
	for (size_t i = 0; i < sym->m_layers_n; ++i) {
		sym->m_layers[i].Create(alloc, is);
	}

	return sym;
}

AnimSym* AnimSym::Create(mm::LinearAllocator& alloc, const rapidjson::Value& val)
{
	void* ptr = alloc.alloc<char>(MemSize());
	AnimSym* sym = new (ptr) AnimSym();

	sym->m_fps = val["fps"].GetInt();

	auto layers = val["layer"].GetArray();
	sym->m_layers_n = layers.Size();
	size_t sz = Layer::MemSize() * sym->m_layers_n;
	sym->m_layers = static_cast<Layer*>(alloc.alloc<char>(sz));
	int layer_idx = 0;
	for (auto& layer : layers) {
		sym->m_layers[layer_idx++].Create(alloc, layer);
	}

	return sym;
}

size_t AnimSym::GetBinSize() const
{
	size_t sz = 0;

	// type
	sz += sizeof(uint8_t);
	// fps
	sz += sizeof(uint16_t);
	// layers
	sz += sizeof(uint16_t); // m_layers_n
	for (size_t i = 0; i < m_layers_n; ++i) {
		sz += m_layers[i].GetBinSize();
	}

	return sz;
}

/************************************************************************/
/* class AnimSym::Lerp                                                  */
/************************************************************************/

size_t AnimSym::Lerp::GetBinSize() const
{
	size_t sz = 0;
	sz += sizeof(uint8_t);	// spr data
	sz += sizeof(uint8_t);	// lerp type
	sz += sizeof(uint16_t);	// data n
	sz += sizeof(uint32_t) * GetDataNum(type); // data
	return sz;
}

void AnimSym::Lerp::StoreToBin(bs::ExportStream& es) const
{
	es.Write(static_cast<uint8_t>(spr_data));
	es.Write(static_cast<uint8_t>(type));
	es.Write(static_cast<uint16_t>(data_n));
	es.WriteBlock((uint8_t*)(data), sizeof(uint32_t) * data_n);
}

AnimSym::Lerp* AnimSym::Lerp::Create(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	void* ptr = alloc.alloc<char>(Lerp::MemSize());
	Lerp* lerp = new (ptr) Lerp();

	lerp->spr_data = is.UInt8();
	lerp->type     = is.UInt8();
	lerp->data_n   = is.UInt16();
	size_t sz = sizeof(uint32_t) * lerp->data_n;
	lerp->data = static_cast<uint32_t*>(alloc.alloc<char>(sz));
	memcpy(lerp->data, is.Block(sz), sz);

	return lerp;
}

AnimSym::Lerp* AnimSym::Lerp::Create(mm::LinearAllocator& alloc, const rapidjson::Value& lerp_val)
{
	void* ptr = alloc.alloc<char>(Lerp::MemSize());
	Lerp* lerp = new (ptr) Lerp();

	lerp->spr_data = lerp_val["key"].GetInt();

	auto& val = lerp_val["val"];
	lerp->type = LERP_UNKNOWN;
	lerp->data_n = 0;
	auto lerp_type = val["type"].GetString();
	if (strcmp(lerp_type, "circle") == 0) 
	{
		lerp->type = LERP_CIRCLE;
		lerp->data_n = GetDataNum(lerp->type);
		lerp->data = static_cast<uint32_t*>(alloc.alloc<char>(sizeof(uint32_t) * lerp->data_n));

		float scale = val["scale"].GetInt() * 0.01f;
		memcpy(&lerp->data[0], &scale, sizeof(uint32_t));
	} 
	else if (strcmp(lerp_type, "spiral") == 0) 
	{
		lerp->type = LERP_SPIRAL;
		lerp->data_n = GetDataNum(lerp->type);
		lerp->data = static_cast<uint32_t*>(alloc.alloc<char>(sizeof(uint32_t) * lerp->data_n));
	
		float begin = val["angle_begin"].GetInt() * DEG_TO_RAD,
		      end   = val["angle_end"].GetInt() * DEG_TO_RAD;
		float scale = val["scale"].GetInt() * 0.01f;
		memcpy(&lerp->data[0], &begin, sizeof(uint32_t));
		memcpy(&lerp->data[1], &end, sizeof(uint32_t));
		memcpy(&lerp->data[2], &scale, sizeof(uint32_t));
	} 
	else if (strcmp(lerp_type, "wiggle") == 0) 
	{
		lerp->type = LERP_WIGGLE;
		lerp->data_n = GetDataNum(lerp->type);
		lerp->data = static_cast<uint32_t*>(alloc.alloc<char>(sizeof(uint32_t) * lerp->data_n));

		float freq = val["freq"].GetFloat();
		float amp  = val["amp"].GetFloat();
		memcpy(&lerp->data[0], &freq, sizeof(uint32_t));
		memcpy(&lerp->data[1], &amp, sizeof(uint32_t));
	} 
	else if (strcmp(lerp_type, "ease") == 0) 
	{
		lerp->type = LERP_EASE;
		lerp->data_n = GetDataNum(lerp->type);
		lerp->data = static_cast<uint32_t*>(alloc.alloc<char>(sizeof(uint32_t) * lerp->data_n));

		int type = val["ease"].GetInt();
		memcpy(&lerp->data[0], &type, sizeof(uint32_t));
	}

	return lerp;
}

int AnimSym::Lerp::GetDataNum(int type)
{
	int num = 0;
	switch (type)
	{
	case LERP_CIRCLE:
		num = 1;
		break;
	case LERP_SPIRAL:
		num = 3;
		break;
	case LERP_WIGGLE:
		num = 2;
		break;
	case LERP_EASE:
		num = 1;
		break;
	}
	return num;
}

/************************************************************************/
/* class AnimSym::Frame                                                 */
/************************************************************************/

size_t AnimSym::Frame::GetBinSize() const
{
	size_t sz = 0;
	sz += sizeof(int16_t);	// index
	sz += sizeof(uint8_t);	// tween
	// actors
	sz += sizeof(uint16_t);
	for (size_t i = 0; i < actors_n; ++i) {
		sz += actors[i]->GetBinSize();
	}
	// lerps
	sz += sizeof(uint16_t);
	for (size_t i = 0; i < lerps_n; ++i) {
		sz += lerps[i]->GetBinSize();
	}
	return sz;
}

void AnimSym::Frame::StoreToBin(bs::ExportStream& es) const
{
	es.Write(static_cast<int16_t>(index)); // index
	es.Write(static_cast<int8_t>(tween));  // tween
	// actors
	es.Write(static_cast<int16_t>(actors_n));
	for (size_t i = 0; i < actors_n; ++i) {
		actors[i]->StoreToBin(es);
	}
	// lerps
	es.Write(static_cast<int16_t>(lerps_n));
	for (size_t i = 0; i < lerps_n; ++i) {
		lerps[i]->StoreToBin(es);
	}
}

void AnimSym::Frame::Create(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	index = is.UInt16(); // index
	tween = is.UInt8();  // tween
	// actors
	actors_n = is.UInt16();
	actors = static_cast<NodeSpr**>(alloc.alloc<char>(sizeof(NodeSpr*) * actors_n));
	for (size_t i = 0; i < actors_n; ++i) {
		actors[i] = NodeFactory::CreateSprFromBin(alloc, is);
	}
	// lerps
	lerps_n = is.UInt16();
	lerps = static_cast<Lerp**>(alloc.alloc<char>(sizeof(Lerp*) * lerps_n));
	for (size_t i = 0; i < lerps_n; ++i) {
		lerps[i] = Lerp::Create(alloc, is);
	}
}

void AnimSym::Frame::Create(mm::LinearAllocator& alloc, const rapidjson::Value& val)
{
	index = val["time"].GetInt();
	tween = val["tween"].GetBool();

	// load actors
	auto actors_val = val["actor"].GetArray();
	actors_n = actors_val.Size();
	size_t sz = bs::SIZEOF_POINTER * actors_n;
	actors = static_cast<NodeSpr**>(alloc.alloc<char>(sz));
	memset(actors, 0, sz);
	int actor_idx = 0;
	for (auto& src_actor : actors_val) {
		actors[actor_idx++] = NodeFactory::CreateSprFromJson(alloc, src_actor);
	}

	// load lerps
	if (val.HasMember("lerp") && val["lerp"].IsArray())
	{
		auto lerps_val = val["lerp"].GetArray();
		lerps_n = lerps_val.Size();
		sz = bs::SIZEOF_POINTER * lerps_n;
		lerps = static_cast<Lerp**>(alloc.alloc<char>(sz));
		memset(lerps, 0, sz);
		int lerp_idx = 0;
		for (auto& lerp : lerps_val) {
			lerps[lerp_idx++] = Lerp::Create(alloc, lerp);
		}
	}
	else
	{
		lerps_n = 0;
	}
}

/************************************************************************/
/* class AnimSym::Layer                                                 */
/************************************************************************/

size_t AnimSym::Layer::GetBinSize() const
{
	size_t sz = 0;
	sz += sizeof(uint16_t);	// frames_n
	for (size_t i = 0; i < frames_n; ++i) {
		sz += frames[i].GetBinSize();
	}
	return sz;
}

void AnimSym::Layer::StoreToBin(bs::ExportStream& es) const
{
	es.Write(static_cast<uint16_t>(frames_n));
	for (size_t i = 0; i < frames_n; ++i) {
		frames[i].StoreToBin(es);
	}
}

void AnimSym::Layer::Create(mm::LinearAllocator& alloc, bs::ImportStream& is)
{
	frames_n = is.UInt16();
	frames = static_cast<Frame*>(alloc.alloc<char>(Frame::MemSize() * frames_n));
	for (size_t i = 0; i < frames_n; ++i) {
		frames[i].Create(alloc, is);
	}
}

void AnimSym::Layer::Create(mm::LinearAllocator& alloc, const rapidjson::Value& val)
{
	auto frames_val = val["frame"].GetArray();
	frames_n = frames_val.Size();

	size_t sz = Frame::MemSize() * frames_n;
	frames = static_cast<Frame*>(alloc.alloc<char>(sz));
	memset(frames, 0, sz);
	int frame_idx = 0;
	for (auto& frame : frames_val) {
		frames[frame_idx++].Create(alloc, frame);
	}
}

}