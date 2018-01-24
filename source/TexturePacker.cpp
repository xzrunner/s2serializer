#include "sns/TexturePacker.h"

#include <js/RapidJsonHelper.h>

#include <boost/filesystem.hpp>

namespace sns
{

TexturePacker::TexturePacker(const std::string& src_dir)
	: m_src_dir(src_dir)
{
}

void TexturePacker::AddTexture(const std::string& filepath)
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(filepath.c_str(), doc);

	auto tex = std::make_unique<Texture>(filepath);
	tex->idx = m_textures.size();
	tex->Load(doc, m_src_dir);
	m_textures.push_back(std::move(tex));
}

bool TexturePacker::Query(const std::string& filepath, float& xmin, float& xmax, float& ymin, float& ymax, float& offx, float& offy) const
{
	for (auto& tex : m_textures)
	{
		if (auto frame = tex->Query(filepath)) 
		{
			xmin = frame->dst.tex_coords[0];
			ymin = frame->dst.tex_coords[1];
			xmax = frame->dst.tex_coords[4];
			ymax = frame->dst.tex_coords[5];
			offx = frame->dst.offset[0];
			offy = frame->dst.offset[1];
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
// struct TexturePacker::Region
//////////////////////////////////////////////////////////////////////////

void TexturePacker::Region::
Load(const rapidjson::Value& val)
{
	x = val["x"].GetInt();
	y = val["y"].GetInt();
	w = val["w"].GetInt();
	h = val["h"].GetInt();
}

//////////////////////////////////////////////////////////////////////////
// struct TexturePacker::FrameSrcData
//////////////////////////////////////////////////////////////////////////

void TexturePacker::FrameSrcData::
Load(const rapidjson::Value& val, const std::string& src_dir)
{
	std::string filepath = val["filename"].GetString();
	filename = boost::filesystem::absolute(filepath, src_dir).string();

	frame.Load(val["frame"]);
	rotated = val["rotated"].GetBool();
	trimmed = val["trimmed"].GetBool();
	sprite_source_size.Load(val["spriteSourceSize"]);
	src_width = val["sourceSize"]["w"].GetInt();
	src_height = val["sourceSize"]["h"].GetInt();
}

//////////////////////////////////////////////////////////////////////////
// struct TexturePacker::FrameDstData
//////////////////////////////////////////////////////////////////////////

void TexturePacker::FrameDstData::
Load(const FrameSrcData& src, const Texture& tex)
{
	bool flip_y = tex.is_easydb;

	float left, right, up, down;
	if (src.rotated)
	{
		down = static_cast<float>(src.frame.x);
		up   = static_cast<float>(src.frame.x + src.frame.h);
		if (flip_y) {
			left  = static_cast<float>(tex.height - src.frame.y);
			right = static_cast<float>(tex.height - (src.frame.y + src.frame.w));
		} else {
			left  = static_cast<float>(src.frame.y);
			right = static_cast<float>(src.frame.y + src.frame.w);
		}

		tex_coords[0] = down; 
		tex_coords[1] = left;
		tex_coords[2] = up;
		tex_coords[3] = left;
		tex_coords[4] = up;
		tex_coords[5] = right;
		tex_coords[6] = down;
		tex_coords[7] = right;
	}
	else
	{
		left  = static_cast<float>(src.frame.x);
		right = static_cast<float>(src.frame.x + src.frame.w);
		if (flip_y) {
			up   = static_cast<float>(tex.height - src.frame.y);
			down = static_cast<float>(tex.height - (src.frame.y + src.frame.h));
		} else {
			up   = static_cast<float>(src.frame.y);
			down = static_cast<float>(src.frame.y + src.frame.h);
		}

		tex_coords[0] = left;
		tex_coords[1] = down;
		tex_coords[2] = left;
		tex_coords[3] = up;
		tex_coords[4] = right;
		tex_coords[5] = up;
		tex_coords[6] = right;
		tex_coords[7] = down;
	}

	offset[0] = (src.sprite_source_size.x + src.sprite_source_size.w * 0.5f) - src.src_width * 0.5f;
	offset[1] = src.src_height * 0.5f - (src.sprite_source_size.y + src.sprite_source_size.h * 0.5f);

}

//////////////////////////////////////////////////////////////////////////
// struct TexturePacker::Frame
//////////////////////////////////////////////////////////////////////////

void TexturePacker::Frame::
Load(const rapidjson::Value& val, const std::string& src_data_dir, const Texture& tex)
{
	tex_idx = tex.idx;
	src.Load(val, src_data_dir);
	dst.Load(src, tex);
}

//////////////////////////////////////////////////////////////////////////
// struct TexturePacker::Texture
//////////////////////////////////////////////////////////////////////////

void TexturePacker::Texture::
Load(const rapidjson::Value& val, const std::string& src_data_dir)
{
	width  = val["meta"]["size"]["w"].GetInt();
	height = val["meta"]["size"]["h"].GetInt();

	if (val["meta"].HasMember("format")) {
		format = val["meta"]["format"].GetString();
	} else {
		format = "png";
	}

// 	std::string scale = val["meta"]["scale"].asString();
// 	m_invscale = atof(scale.c_str());

// 	std::string app = val["meta"]["app"].asString();
// 	is_easydb = app.compare("easydb") == 0;
	is_easydb = false;

	for (auto& frame_val : val["frames"].GetArray())
	{
		Frame frame;
		frame.Load(frame_val, src_data_dir, *this);
		frames.insert(std::make_pair(frame.src.filename, frame));
	}
}

const TexturePacker::Frame* TexturePacker::Texture::
Query(const std::string& filepath) const
{
	auto itr = frames.find(filepath);
	if (itr != frames.end()) {
		return &itr->second;
	} else {
		return NULL;
	}
}

}