#include "s2s/JsonResOP.h"

#include <sprite2/SymType.h>
#include <s2loader/SymbolFile.h>
#include <js/RapidJsonHelper.h>
#include <mt_2d.h>

#include <boost/filesystem.hpp>

#include <fstream>

namespace s2s
{

const std::string JsonResOP::KEY_SPR  = "sprite";
const std::string JsonResOP::KEY_COMP = "components";
const std::string JsonResOP::KEY_PATH = "filepath";

static const std::string SYM_GROUP_TAG = "group";
static const std::string SYM_SHAPE_TAG = "shape";
static const std::string SYM_TEXT_TAG  = "text";

JsonResOP::JsonResOP(const std::string& filepath)
	: m_filepath(filepath)
{
}

void JsonResOP::Do()
{
	Clear();

	int type = s2loader::SymbolFile::Instance()->Type(m_filepath.c_str());
	switch (type)
	{
	case s2::SYM_IMAGE:
		break;
	case s2::SYM_SCALE9:
		DoCommon(KEY_SPR);
		break;
	case s2::SYM_ICON:
		break;
	case s2::SYM_TEXTURE:
		DoTexture();
		break;
	case s2::SYM_COMPLEX:
		DoCommon(KEY_SPR);
		break;
	case s2::SYM_ANIMATION:
		DoAnim();
		break;
	case s2::SYM_ANIM2:
		// todo
		break;
	case s2::SYM_PARTICLE3D:
		DoCommon(KEY_COMP);
		break;
	case s2::SYM_PARTICLE2D:
		DoCommon(KEY_COMP);
		break;
	case s2::SYM_SHAPE:
		// todo
		break;
	case s2::SYM_MESH:
		DoMesh();
		break;
	case s2::SYM_MASK:
		DoMask();
		break;
	case s2::SYM_TRAIL:
		// todo		
		break;
	case s2::SYM_SKELETON:
		// todo		
		break;
	case s2::SYM_MODEL:
		// todo
		break;
	}
}

bool JsonResOP::DoFile(rapidjson::Value& val, const std::string& key)
{
	std::string filepath = val[key.c_str()].GetString();
	if (filepath == SYM_GROUP_TAG) {
		return DoGroup(val, key);
	} else {
		std::string absolute;
		if (!filepath.empty()) 
		{
			auto base_dir = boost::filesystem::path(m_filepath).parent_path();
			absolute = boost::filesystem::absolute(filepath, base_dir).string();
		}
		return OnDoFile(absolute, val, key);
	}
}

bool JsonResOP::DoGroup(rapidjson::Value& val, const std::string& key)
{
	bool dirty = false;
	for (auto& group : val["group"].GetArray()) {
		if (DoFile(group, "filepath")) {
			dirty = true;
		}
	}
	AfterDoGroup(dirty, val);
	return dirty;
}

void JsonResOP::DoCommon(const std::string& key)
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(m_filepath.c_str(), doc);

	BeforeDoCommon(doc);

	bool dirty = false;
	for (auto& item : doc[key.c_str()].GetArray()) {
		if (DoFile(item, KEY_PATH)) {
			dirty = true;
		}
	}

	AfterDoCommon(dirty, doc, key);

	if (dirty) {
		js::RapidJsonHelper::WriteToFile(m_filepath.c_str(), doc);
	}
}

void JsonResOP::DoUIWnd()
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(m_filepath.c_str(), doc);

	BeforeDoCommon(doc);

	bool dirty = false;
	for (auto& item : doc[KEY_SPR.c_str()].GetArray()) {
		if (DoFile(item, KEY_PATH)) {
			dirty = true;
		}
	}

	if (doc.HasMember("ref_spr"))
	{
		for (auto& item : doc["ref_spr"].GetArray()) {
			if (DoFile(item, KEY_PATH)) {
				dirty = true;
			}
		}
	}

	AfterDoCommon(dirty, doc, KEY_SPR);

	if (dirty) {
		js::RapidJsonHelper::WriteToFile(m_filepath.c_str(), doc);
	}
}

void JsonResOP::DoTexture()
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(m_filepath.c_str(), doc);

	bool dirty = false;
	for (auto& iteme : doc["shapes"].GetArray())
	{
		if (!iteme.HasMember("material")) {
			continue;
		}
		auto& val_mat = iteme["material"];
		if (val_mat["type"] == "texture") {
			if (DoFile(val_mat, "texture path")) {
				dirty = true;
			}
		}		
	}

	AfterDoTexture(dirty, doc);

	if (dirty) {
		js::RapidJsonHelper::WriteToFile(m_filepath.c_str(), doc);
	}
}

void JsonResOP::DoAnim()
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(m_filepath.c_str(), doc);

	BeforeDoAinm(doc);

	bool dirty = false;
	for (auto& layer : doc["layer"].GetArray()) {
		for (auto& frame : layer["frame"].GetArray()) {
			for (auto& actor : frame["actor"].GetArray()) {
				if (DoFile(actor, KEY_PATH)) {
					dirty = true;
				}
			}
		}
	}

	AfterDoAnim(dirty, doc);

	if (dirty) {
		js::RapidJsonHelper::WriteToFile(m_filepath.c_str(), doc);
	}
}

void JsonResOP::DoMesh()
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(m_filepath.c_str(), doc);

	if (DoFile(doc, "base_symbol")) {
		js::RapidJsonHelper::WriteToFile(m_filepath.c_str(), doc);
	}
}

void JsonResOP::DoMask()
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(m_filepath.c_str(), doc);

	bool dirty = false;
	if (DoFile(doc["base"], KEY_PATH)) {
		dirty = true;
	}
	if (DoFile(doc["mask"], KEY_PATH)) {
		dirty = true;
	}
	if (dirty) {
		js::RapidJsonHelper::WriteToFile(m_filepath.c_str(), doc);
	}
}

void JsonResOP::DoTrail()
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(m_filepath.c_str(), doc);

	int mode = doc["mode"].GetInt();
	if (mode != T2D_MODE_IMAGE) {
		return;
	}

	bool dirty = false;
	for (auto& item : doc[KEY_COMP.c_str()].GetArray()) {
		if (DoFile(item, KEY_PATH)) {
			dirty = true;
		}
	}

	if (dirty) {
		js::RapidJsonHelper::WriteToFile(m_filepath.c_str(), doc);
	}
}

void JsonResOP::DoUI()
{
	rapidjson::Document doc;
	js::RapidJsonHelper::ReadFromFile(m_filepath.c_str(), doc);

	bool dirty = false;
	if (DoFile(doc, "items filepath")) {
		dirty = true;
	}
	if (DoFile(doc, "wrapper filepath")) {
		dirty = true;
	}
	if (dirty) {
		js::RapidJsonHelper::WriteToFile(m_filepath.c_str(), doc);
	}
}

}