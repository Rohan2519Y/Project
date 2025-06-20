#include <windows.h>
#include <stdio.h>
#include "WinRing0.h"

#define FAN_PORT 0x300
#define MIN_SPEED 30
#define MAX_TEMP 85

volatile BOOL running = TRUE;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    if (fdwCtrlType == CTRL_C_EVENT) {
        running = FALSE;
        return TRUE;
    }
    return FALSE;
}

DWORD GetCpuTemperature() {
    return 50; // Dummy value - replace with real sensor reading
}

void SetFanSpeed(BYTE percent) {
    if (percent > 100) percent = 100;
    BYTE raw = (BYTE)(percent * 2.55f);
    WriteIoPortByte(FAN_PORT, raw);
    printf("Set fan to %d%%\n", percent);
}

int main() {
    if (!InitializeOls()) {
        printf("Failed to initialize WinRing0\n");
        return 1;
    }

    if (!GetDllStatus()) {
        printf("Driver not running\n");
        DeinitializeOls();
        return 1;
    }

    SetConsoleCtrlHandler(CtrlHandler, TRUE);
    printf("Fan control started. Press Ctrl+C to exit.\n");

    while (running) {
        DWORD temp = GetCpuTemperature();
        printf("Temp: %lu°C\n", temp);

        if (temp >= MAX_TEMP) {
            SetFanSpeed(100);
            printf("EMERGENCY: Max temp reached!\n");
            break;
        } else if (temp > 80) {
            SetFanSpeed(100);
        } else if (temp > 70) {
            SetFanSpeed(80);
        } else if (temp > 60) {
            SetFanSpeed(60);
        } else {
            SetFanSpeed(MIN_SPEED);
        }

        Sleep(5000);
    }

    SetFanSpeed(100);  // Reset on exit
    DeinitializeOls();
    return 0;
}

