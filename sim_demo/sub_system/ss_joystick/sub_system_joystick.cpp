#include "sub_system_joystick.h"

#include <json.hpp>
#include <fstream>

#include "util.h"


DLL_EXPORT
SubSystem* create_sub_system()
{
	return new SubSystemJoystick();
}


DLL_EXPORT
void destroy_sub_system(SubSystem* ss)
{
	delete ss;
}


bool SubSystemJoystick::init(const json& vehicle_config, const json& sub_system_config)
{
	logi("SubSystemJoystick::init\n");

	// 读取操纵杆数量
    int num_dev = joyGetNumDevs();
    if (num_dev < 1)
    {
        loge("No joystick found\n");
        return false;
    }

	// 获取操纵杆的能力信息
    if (joyGetDevCaps(JOYSTICKID1, &_jc, sizeof(JOYCAPS)) != JOYERR_NOERROR)
    {
        loge("Failed to get joystick capabilities\n");
        return false;
    }

	// 打印操纵杆的能力信息
    logi("joy_id={}, DevCap:\n"
        "wMid={:5d}                // manufacturer ID\n"
        "wPid={:5d}                // product ID\n"
        "wCaps={}=0x{:04X}           // joystick capabilities\n"
        "wMaxAxes={:5d}            // maximum number of axes supported\n"
        "wNumAxes={:5d}            // number of axes in use\n"
        "wMaxButtons={:5d}         // maximum number of buttons supported\n"
        "wNumButtons={:5d}         // number of buttons\n"
        "wXmin={:5d}               // minimum x position value\n"
        "wXmax={:5d}               // maximum x position value\n"
        "wYmin={:5d}               // minimum y position value\n"
        "wYmax={:5d}               // maximum y position value\n"
        "wZmin={:5d}               // minimum z position value\n"
        "wZmax={:5d}               // maximum z position value\n"
        "szPname={}                // product name\n"
        "szRegKey={}               // registry key\n",
        JOYSTICKID1,
        _jc.wMid,
        _jc.wPid,
        _jc.wCaps, _jc.wCaps,
        _jc.wMaxAxes,
        _jc.wNumAxes,
        _jc.wMaxButtons,
        _jc.wNumButtons,
        _jc.wXmin,
        _jc.wXmax,
        _jc.wYmin,
        _jc.wYmax,
        _jc.wZmin,
        _jc.wZmax,
        _jc.szPname,
        _jc.szRegKey
    );

    _trim_push = read_json_double(sub_system_config, "trim_push", 0.0);
    _trim_push_button = read_json_int(sub_system_config, "trim_push_button", -1);
    _trim_pull_button = read_json_int(sub_system_config, "trim_pull_button", -1);

	// 导出数据
	reg_data("stick_right", _stick_right);
	reg_data("stick_push", _stick_push);
	reg_data("pedal_right", _pedal_right);
	reg_data("throttle", _throttle);

	return true;
}


void SubSystemJoystick::step(double dt, double t)
{
    JOYINFOEX joy_info;
    if (joyGetPosEx(JOYSTICKID1, &joy_info) != JOYERR_NOERROR)
    {
        loge("Failed to get joystick position\n");
        return;
    }

	// 配平量处理
	if (_trim_push_button >= 0)
	{
		if (joy_info.dwButtons & (1 << _trim_push_button))
		{
			_trim_push += 0.01; // 推杆按钮按下，增加配平量
		}
	}
	if (_trim_pull_button >= 0)
	{
		if (joy_info.dwButtons & (1 << _trim_pull_button))
		{
			_trim_push -= 0.01; // 拉杆按钮按下，减少配平量
		}
	}

    _stick_right = (2.0 * joy_info.dwXpos - _jc.wXmax - _jc.wXmin) / (_jc.wXmax - _jc.wXmin);
    _stick_push = -(2.0 * joy_info.dwYpos - _jc.wYmax - _jc.wYmin) / (_jc.wYmax - _jc.wYmin);
    _pedal_right = (2.0 * joy_info.dwRpos - _jc.wRmax - _jc.wRmin) / (_jc.wRmax - _jc.wRmin);
    _throttle = static_cast<double>(_jc.wZmax - joy_info.dwZpos) / (_jc.wZmax - _jc.wZmin);

    _stick_push += _trim_push;
}