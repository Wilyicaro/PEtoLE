/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

#pragma once

#include <stdint.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <common/Mth.hpp>
#include <functional>

enum eKeyMappingIndex
{
	KM_FORWARD,
	KM_LEFT,
	KM_BACKWARD,
	KM_RIGHT,
	KM_JUMP,
	KM_CRAFTING,
	KM_INVENTORY,
	KM_DROP,
	KM_CHAT,
	KM_FOG,
	KM_SNEAK,
	KM_DESTROY,
	KM_PLACE,
	KM_MENU_NEXT,
	KM_MENU_PREVIOUS,
	KM_MENU_OK,
	KM_MENU_CANCEL, KM_BACK = KM_MENU_CANCEL,
	KM_SLOT_1,
	KM_SLOT_2,
	KM_SLOT_3,
	KM_SLOT_4,
	KM_SLOT_5,
	KM_SLOT_6,
	KM_SLOT_7,
	KM_SLOT_8,
	KM_SLOT_9,
	KM_SLOT_L,
	KM_SLOT_R,
	KM_TOGGLEGUI,
	KM_SCREENSHOT,
	KM_TOGGLEDEBUG,
	KM_TOGGLEAO,
	KM_TOGGLE3RD,
	KM_FLY_UP,
	KM_FLY_DOWN,
	KM_CHAT_CMD, // called "Open Chat" in Release 1.8
	KM_COUNT,
};

struct KeyMapping
{
	std::string key;
	int value;

	KeyMapping() : value(-1) {} // key is automatically clear when constructed
	KeyMapping(const char* keyName, int keyCode) : key(keyName), value(keyCode) {}
};

class Button;
class Minecraft;

class Options
{
public:
	struct Option;
	struct KeyBind;
private:
	static bool readBool(const std::string& str);
	static int readInt(const std::string& str);
	static float readFloat(const std::string& str);
	static std::string saveBool(bool b);
	static std::string saveInt(int i);
	static std::string saveFloat(float f);
	static std::vector<std::string> readPropertiesFromFile(const std::string& filePath);
	static void savePropertiesToFile(const std::string& filePath, std::vector<std::string> properties);

private:
	void _load();
public:
	Options(Minecraft*);
	Options(Minecraft*, const std::string& folderPath);

	class OptionEntry
	{
	private:
		std::string m_key;
		std::string m_name;

	public:
		OptionEntry(const std::string& key, const std::string& name) : m_key(key), m_name(name) {}

		virtual const std::string& getKey() const { return m_key; }
		virtual const std::string& getName() const { return m_name; }
		virtual std::string getDisplayValue() const { return save(); }
		virtual std::string save() const = 0;
		virtual void load(const std::string& value) = 0;
		virtual void toggle() = 0;


		virtual void addButton(std::vector<Button*>&, const std::string&);
	};

	void save();
	std::string getMessage(const Options::OptionEntry&);

	void add(OptionEntry&);
	std::vector<std::string> getOptionStrings();
	
	int getKey(eKeyMappingIndex idx) const { return m_keyMappings[idx].value; }
	bool isKey(eKeyMappingIndex idx, int keyCode) const { return getKey(idx) == keyCode; }

	void loadControls();

private:
	std::string m_filePath;
	KeyMapping m_keyMappings[KM_COUNT];


public:
	template <typename V>
	class OptionInstance : public OptionEntry
	{
	private:
		V m_value;
		std::function<void(V)> m_onApply;

	public:
		OptionInstance(const std::string& key, const std::string& name, V initial, std::function<void(V)> onApply = {}) : OptionEntry(key, name), m_value(initial), m_onApply(std::move(onApply)) {}

		virtual void toggle()
		{
		}

		void set(V v)
		{
			V oldValue = m_value;
			m_value = v;
			if (oldValue != v && m_onApply)
				m_onApply(v);
		}

		V get() const { return m_value; }
	};

	class BoolOption : public OptionInstance<bool>
	{
	public:
		BoolOption(const std::string& key, const std::string& name, bool initial = true, std::function<void(bool)> onApply = {}) : OptionInstance(key, name, initial, std::move(onApply)) {}

		void load(const std::string& value) override { set(readBool(value)); }
		std::string save() const override { return saveBool(get()); }

		void toggle() override
		{
			set(get() ^ 1);
		}

		std::string getDisplayValue() const override
		{
			return get() ? "options.on" : "options.off";
		}
	};

	class FloatOption : public OptionInstance<float>
	{
	public:
		FloatOption(const std::string& key, const std::string& name, float initial = 0.0f, std::function<void(float)> onApply = {}) : OptionInstance(key, name, initial, std::move(onApply)) {}

		void load(const std::string& value) override { set(readFloat(value)); }
		std::string save() const override { return saveFloat(get()); }


		std::string getDisplayValue() const override
		{
			return get() == 0.0f ? "options.off" : saveInt(get() * 100) + "%";
		}

		void addButton(std::vector<Button*>&, const std::string&) override;
	};

	//@NOTE: A better solution is really needed for this, maybe use std::function<std::string(V)>> in the display in OptionInstance?
	class SensibilityOption : public FloatOption
	{
	public:
		SensibilityOption(const std::string& key, const std::string& name, float initial = 0.0f, std::function<void(float)> onApply = {}) : FloatOption(key, name, initial, std::move(onApply)) {}

		std::string getDisplayValue() const override
		{
			return get() == 0.0f ? "options.sensitivity.min" : get() == 1.0f ? "options.sensitivity.max" : saveInt(get() * 200) + "%";
		}
	};

	class IntOption : public OptionInstance<int>
	{
	public:
		IntOption(const std::string& key, const std::string& name, int initial = 0, std::function<void(int)> onApply = {}) : OptionInstance(key, name, initial, std::move(onApply)) {}

		void load(const std::string& value) override { set(readInt(value)); }
		std::string save() const override { return saveInt(get()); }
	};

	class StringOption : public OptionInstance<std::string>
	{
	public:
		StringOption(const std::string& key, const std::string& name, std::string initial = "", std::function<void(std::string)> onApply = {}) : OptionInstance(key, name, initial, std::move(onApply)) {}

		void load(const std::string& value) override { set(value); }
		std::string save() const override { return get(); }
	};

	class ValuesOption : public IntOption
	{
	public:
		std::vector<std::string> m_values;
		ValuesOption(const std::string& key, const std::string& name, int initial, std::vector<std::string> values, std::function<void(int)> onApply = {}) : IntOption(key, name, initial, std::move(onApply)), m_values(values) {}

		void toggle() override
		{
			set((get() + 1) % m_values.size());
		}

		std::string getDisplayValue() const override
		{
			return m_values[Mth::min(get(), m_values.size())];
		}
	};

public:
	Minecraft* m_pMinecraft;
	std::unordered_map<std::string, OptionEntry*> m_options;
	FloatOption m_fMusicVolume;
	FloatOption m_fMasterVolume;
	SensibilityOption m_fSensitivity;
	BoolOption m_bInvertMouse;
	ValuesOption m_iViewDistance;
	BoolOption m_bViewBobbing;
	BoolOption m_bAnaglyphs;
	BoolOption m_moreWorldOptions;
	ValuesOption m_limitFramerate;
	uint8_t field_16;
	BoolOption m_bFancyGraphics;
	BoolOption m_bAmbientOcclusion;
	BoolOption m_splitControls;
	uint8_t field_19; // use Mouse as input for breaking
	std::string field_1C;
	ValuesOption m_difficulty;
	bool m_bDontRenderGui;
	bool m_bThirdPerson;
	ValuesOption m_guiScale;
	StringOption m_lang;
	bool m_bFlyCheat;
	uint8_t field_240;
	bool field_241;
	float field_244;
	float field_248;
	int field_24C;
	StringOption m_playerName;
	bool m_bServerVisibleDefault;
	BoolOption m_bAutoJump;
	bool m_bDebugText;
	BoolOption m_bBlockOutlines;
	bool m_bFancyGrass;
	bool m_bBiomeColors;
	bool m_bUseController;
	BoolOption m_bDynamicHand;
	bool m_bOldTitleLogo;
	BoolOption m_bMenuPanorama;
	BoolOption m_bMipmaps;
	BoolOption m_vSync;
};

