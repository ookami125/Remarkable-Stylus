#include "types.h"
#include "PenHandler.h"
#include "SSHHandler.h"
#include "Utils.h"
#include "Args.h"

PenHandler pen;

void Process(const Event& e)
{
    const std::string& type = types.find(e.type)->second;
    const std::string& code = codes.find(e.type)->second.find(e.code)->second;
    const std::string typeCode = type + "/" + code;
    
    if (typeCode == "EV_ABS/ABS_X") {
        pen.x = e.value;
    }
    else if (typeCode == "EV_ABS/ABS_Y") {
        pen.y = e.value;
    }
    else if (typeCode == "EV_ABS/ABS_DISTANCE") {
        pen.distance = e.value;
    }
    else if (typeCode == "EV_ABS/ABS_PRESSURE") {
        if (e.value == 0) {
            pen.pressure = 0;
        }
        else {
            if (pen.end == DRAWING) {
                pen.pressure = map<int32_t>(e.value, 1023, 4095, 1, 1024);
            }
            else {
                pen.pressure = map<int32_t>(e.value, 1023, 1792, 1, 1024);
            }
        }
    }
    else if (typeCode == "EV_ABS/ABS_TILT_X") {
        pen.tilt_x = map<int32_t>(e.value, -6400, 6400, -90, 90);
    }
    else if (typeCode == "EV_ABS/ABS_TILT_Y") {
        pen.tilt_y = map<int32_t>(e.value, -6400, 6400, -90, 90);
    }
    else if (typeCode == "EV_SYN/SYN_REPORT") {
        pen.time = ((uint64_t)e.time) * 1000000 + e.millis;
        pen.AddEvent();
    }
    else if (typeCode == "EV_KEY/BTN_TOOL_PEN") {
        pen.state = e.value == 0 ? GONE : HOVER;
        if (e.value == 1)
            pen.end = DRAWING;
    }
    else if (typeCode == "EV_KEY/BTN_TOOL_RUBBER") {
        pen.state = e.value == 0 ? GONE : HOVER;
        if (e.value == 1)
            pen.end = ERASING;
    }
    else if (typeCode == "EV_KEY/BTN_TOUCH") {
        pen.state = e.value == 0 ? HOVER : CONTACTED;
    }
    else printf("Unhandled Code: %s\n", typeCode.c_str());
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
}

int main(int argc, char** argv)
{
    Args args;
    ParseArgs(args, argc, argv);

    int monitor_width;
    int monitor_height;
    GetDesktopResolution(monitor_width, monitor_height);

    if (args.xSize != 0) monitor_width = args.xSize;
    if (args.ySize != 0) monitor_height = args.ySize;

    pen.Init();
    pen.orientation = args.orientation;
    pen.x_offset = args.xOffset;
    pen.y_offset = args.xOffset;
    pen.x_size = monitor_width;
    pen.y_size = monitor_height;
    pen.StartEventProcessor();

    SSHHandler ssh;
    ssh.hostname = args.hostname;
    ssh.username = args.username;
    ssh.password = args.password;
    ssh.ConnectToRemarkable();
    ssh.SetCallback(Process);
    ssh.Process();

    return 0;
}