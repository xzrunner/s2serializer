#pragma once

#include <rapidjson/document.h>

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace sns
{

class TexturePacker
{
public:
	TexturePacker(const std::string& src_dir);

	void AddTexture(const std::string& filepath);

	bool Query(const std::string& filepath, float& xmin, float& xmax, 
		float& ymin, float& ymax, float& offx, float& offy) const;

private:		
	struct Region
	{
		int x, y;
		int w, h;

		void Load(const rapidjson::Value& val);
	};

	struct FrameSrcData
	{
		std::string filename;

		Region frame;

		bool rotated;

		bool trimmed;
		Region sprite_source_size;

		int src_width, src_height;

		void Load(const rapidjson::Value& val, const std::string& src_dir);
	};

	struct Texture;
	struct FrameDstData
	{
		// 0 3
		// 1 2
		float tex_coords[8];

		float offset[2];

		void Load(const FrameSrcData& src, const Texture& tex);
	};

	struct Frame
	{
		FrameSrcData src;
		FrameDstData dst;

		int tex_idx;

		void Load(const rapidjson::Value& val, const std::string& src_dir, const Texture& tex);
	};

	struct Texture
	{
		const std::string filepath;

		int idx;

		int width, height;

		std::string format;

		std::map<std::string, Frame> frames;

		bool is_easydb;

		Texture(const std::string& filepath) : filepath(filepath) {}

		void Load(const rapidjson::Value& val, const std::string& src_dir);

		const Frame* Query(const std::string& filepath) const;
	};

private:
	std::string m_src_dir;

	std::vector<std::unique_ptr<Texture>> m_textures;

}; // TexturePacker

}