#pragma once
#include <stdint.h>
#include <Windows.h>
#include "concurrentqueue.h"
#include "Args.h"

enum PenState
{
    GONE,
    HOVER,
    CONTACTED
};

enum PenEnd
{
    DRAWING,
    ERASING
};

class PenHandler
{
    HSYNTHETICPOINTERDEVICE SyntheticPointer;
    POINTER_TYPE_INFO PointerInfo;

    moodycamel::ConcurrentQueue<POINTER_TYPE_INFO> events;
    std::thread eventProcessorThread;

    PenState lastState = GONE;
public:
    int32_t x_size = 20967;
    int32_t y_size = 15725;
    int32_t x_offset = 0;
    int32_t y_offset = 0;
    Orientation orientation = Landscape;

    PenState state = GONE;
    PenEnd end = DRAWING;
    uint64_t time = 0;
    int32_t x = 0;
    int32_t y = 0;
    int32_t tilt_x = 0;
    int32_t tilt_y = 0;
    int32_t pressure = 0;
    int32_t distance = 0;

    void Init();
    void StartEventProcessor();
    void AddEvent();
};