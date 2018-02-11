#pragma once

#include <rapidjson/document.h>

namespace s2s
{

class JsonResOP
{
public:
	JsonResOP(const std::string& filepath);
	virtual ~JsonResOP() {}

	void Do();

protected:
	virtual void Clear() {}

	virtual bool OnDoFile(const std::string& filepath, rapidjson::Value& val, const std::string& key) = 0;

	virtual void BeforeDoCommon(const rapidjson::Value& value) {}
	virtual void BeforeDoAinm(const rapidjson::Value& value) {}

	virtual void AfterDoGroup(bool dirty, rapidjson::Value& val) {}
	virtual void AfterDoCommon(bool dirty, rapidjson::Value& val, const std::string& key) {}
	virtual void AfterDoTexture(bool dirty, rapidjson::Value& val) {}
	virtual void AfterDoAnim(bool dirty, rapidjson::Value& val) {}

protected:
	static const std::string KEY_SPR;
	static const std::string KEY_COMP;
	static const std::string KEY_PATH;

private:
	bool DoFile(rapidjson::Value& val, const std::string& key);
	bool DoGroup(rapidjson::Value& val, const std::string& key);

	void DoCommon(const std::string& key);
	void DoUIWnd();
	void DoTexture();
	void DoAnim();
	void DoMesh();
	void DoMask();
	void DoTrail();
	void DoUI();

protected:
	std::string m_filepath;

}; // JsonResOP

}