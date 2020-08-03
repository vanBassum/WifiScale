/*
 * settings.h
 *
 *  Created on: 2 Aug 2020
 *      Author: Bas
 */

#ifndef MAIN_SETTINGS_H_
#define MAIN_SETTINGS_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "stdint.h"
#include <string>

template <typename T>
class SettingBase
{
protected:
	std::string key;
	T value;

public:

	SettingBase(const char *keyName)
	{
		key = std::string(keyName);
	}

	virtual ~SettingBase()
	{

	}


	SettingBase<T>& operator = (const T& val)
    {
        value = val;
        return *this;
    };


	void Set(T f)
	{
		value = f;
	}

	T Get()
	{
		return value;
	}


	/*
	//https://stackoverflow.com/a/35478633
    T& operator= (const T& f) { return value = f; }
    const T& operator() () const { return value; }
    explicit operator const T& () const { return value; }
    T* operator->() { return &value; }
*/

	virtual void Save(nvs_handle_t handle) = 0;
	virtual void Load(nvs_handle_t handle) = 0;

};


template<typename T = void> class Setting;



template<>
class Setting<uint8_t> : public SettingBase<uint8_t>
{
public:

	Setting(const char *keyName) : SettingBase(keyName)
	{
	}

	void Save(nvs_handle_t handle)
	{
		ESP_ERROR_CHECK(nvs_set_u8(handle, key.c_str(), value));
	}

	void Load(nvs_handle_t handle)
	{
		ESP_ERROR_CHECK(nvs_get_u8(handle, key.c_str(), &value));
	}
};


template<>
class Setting<std::string> : public SettingBase<std::string>
{
public:

	Setting(const char *keyName) : SettingBase(keyName)
	{
	}

	void Save(nvs_handle_t handle)
	{
		ESP_ERROR_CHECK(nvs_set_str(handle, key.c_str(), value.c_str()));
	}

	void Load(nvs_handle_t handle)
	{
		size_t required_size;
		ESP_ERROR_CHECK(nvs_get_str(handle, key.c_str(), NULL, &required_size));
		char temp[required_size];
		ESP_ERROR_CHECK(nvs_get_str(handle, key.c_str(), temp, &required_size));
		value = std::string(temp);
	}
};




class Settings
{

public:

	static Setting<uint8_t> someSetting;
	//static Setting<std::string> Wifi_SSID;
	//static Setting<std::string> Wifi_PASS;

	static void Init()
	{
		esp_err_t err = nvs_flash_init();
		if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
		{
			// NVS partition was truncated and needs to be erased
			// Retry nvs_flash_init
			ESP_ERROR_CHECK(nvs_flash_erase());
			err = nvs_flash_init();
		}
		ESP_ERROR_CHECK( err );
	}

	static void Load()
	{
		nvs_handle_t handle;
		ESP_ERROR_CHECK(nvs_open("settings", NVS_READWRITE, &handle));


		//someSetting.Load(handle);
		//Wifi_SSID.Load(handle);
		//Wifi_PASS.Load(handle);


		ESP_ERROR_CHECK(nvs_commit(handle));
		nvs_close(handle);
	}

	static void Save()
	{
		nvs_handle_t handle;
		ESP_ERROR_CHECK(nvs_open("settings", NVS_READWRITE, &handle));


		//someSetting.Save(handle);
		//Wifi_SSID.Save(handle);
		//Wifi_PASS.Save(handle);


		ESP_ERROR_CHECK(nvs_commit(handle));
		nvs_close(handle);
	}


};



#endif /* MAIN_SETTINGS_H_ */
