#include "PenHandler.h"
#include <Windows.h>
#include <iostream>

void PenHandler::Init()
{
    SyntheticPointer = CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_DEFAULT);
    if (!SyntheticPointer)
    {
        printf("CreateSyntheticPointerDevice() failed");
    }
}

void PenHandler::StartEventProcessor()
{
    eventProcessorThread = std::thread([&]()->void{
        constexpr size_t eventListSize = 32;
        POINTER_TYPE_INFO eventList[eventListSize];
        while (true)
        {
            size_t count = events.try_dequeue_bulk(eventList, eventListSize);
            InjectSyntheticPointerInput(SyntheticPointer, eventList, count);
        }
    });
}

void PenHandler::AddEvent()
{
    memset(&PointerInfo, 0, sizeof(PointerInfo));
    PointerInfo.type = PT_PEN;
    PointerInfo.penInfo.pointerInfo.pointerFlags = 0;

    if (lastState == HOVER && state == CONTACTED)
        PointerInfo.penInfo.pointerInfo.pointerFlags |= POINTER_FLAG_DOWN;
    else if (lastState == CONTACTED && state == HOVER)
        PointerInfo.penInfo.pointerInfo.pointerFlags |= POINTER_FLAG_UP;
    else if (state == HOVER)
        PointerInfo.penInfo.pointerInfo.pointerFlags |= POINTER_FLAG_INRANGE;
    else if (state == CONTACTED)
        PointerInfo.penInfo.pointerInfo.pointerFlags |= POINTER_FLAG_INCONTACT;

    PointerInfo.penInfo.pointerInfo.pointerFlags |= POINTER_FLAG_UPDATE;

    if (state == CONTACTED && end == DRAWING)
    {
        PointerInfo.penInfo.pointerInfo.pointerFlags |= POINTER_FLAG_FIRSTBUTTON;
    }

    float fx = x / 20967.0f;
    float fy = y / 15725.0f;
    int32_t tiltx = tilt_x;
    int32_t tilty = tilt_y;
    {
        auto ft = fx;
        switch(orientation)
        {
        case Landscape:
            tiltx = tilt_x;
            tilty = tilt_y;
        case Portrait:
            fx = fy;
            fy = 1.0f - ft;
            tiltx = tilt_y;
            tilty = -tilt_x;
            break;
        case LandscapeFlipped:
            fx = 1.0f - fx;
            fy = 1.0f - fy;
            tiltx = -tilt_x;
            tilty = -tilt_y;
            break;
        case PortraitFlipped:
            fx = 1.0f - fy;
            fy = ft;
            tiltx = -tilt_y;
            tilty = tilt_x;
            break;
        }
        fx *= x_size;
        fy *= y_size;
    }

    PointerInfo.penInfo.pointerInfo.ptPixelLocation.x = (LONG)fx + x_offset;
    PointerInfo.penInfo.pointerInfo.ptPixelLocation.y = (LONG)fy + y_offset;
    PointerInfo.penInfo.penFlags = (end==DRAWING) ? PEN_FLAG_NONE : PEN_FLAG_INVERTED;
    PointerInfo.penInfo.penMask = PEN_MASK_PRESSURE | PEN_MASK_TILT_X | PEN_MASK_TILT_Y;
    PointerInfo.penInfo.pressure = pressure;
    PointerInfo.penInfo.tiltX = tiltx;
    PointerInfo.penInfo.tiltY = tilty;
    time *= 10;
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);

    static uint64_t difference = 0;
    time -= difference;
    if (int64_t(time) - li.QuadPart > 0)
    {
        difference += time - li.QuadPart;
        time = li.QuadPart;
    }

    PointerInfo.penInfo.pointerInfo.PerformanceCount = time;

    events.enqueue(PointerInfo);
}
