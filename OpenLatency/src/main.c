#include <stdio.h>
#include <ctype.h>
#include <time.h>

#include "modules/InputMonitor/InputMonitor.h"
#include "modules/AudioInputMonitor/AudioInputMonitor.h"







const ma_uint32 SAMPLE_RATE = 200000;







int input_triggerMinDifference = 18;

int bTargetKeyDown = 0;
int bTargetKeyPrevState = 0;

int bMeasuringStart = 0;



int _bMeasuring = 0;

clock_t _startTime;

void AudioInputDataCallback(const void *pData, ma_uint32 frameCount) {

        if (bMeasuringStart && !_bMeasuring) { // Measuring init

                bMeasuringStart = 0;

                printf("MEASURING...");

                _startTime = clock();

                _bMeasuring = 1;

        }



        if (_bMeasuring) { // Measuring

                for (int i = 0; i < frameCount; i++) {

                        if (((short*)pData)[i] < 0) ((short*)pData)[i] = -((short*)pData)[i];

                        if (((short*)pData)[i] >= input_triggerMinDifference) {

                                printf("\r%f ms\n", (double)(clock() - _startTime) / CLOCKS_PER_SEC * 1000);

                                _bMeasuring = 0;
                                bMeasuringStart = 0;
                                return;

                        }

                }

        }

}



int main() {

        // TODO: Implement default (18) ...?
        printf("Trigger min. difference: ");
        if (scanf("%u", &input_triggerMinDifference) == EOF) {
                puts("ERROR: Failed to get user input for trigger min. difference");
                getchar(); return 1;
        }



        if (!InputMonitor__Bind(INPUTMONITOR__TARGET_MOUSE, MOUSE_BUTTON1, &bTargetKeyDown)) {
                puts("ERROR: Failed to hook & bind state of left mouse button");
                getchar(); return 1;
        }
        


        if (!AudioInputMonitor__Monitor(ma_format_s16, 1, SAMPLE_RATE, &AudioInputDataCallback)) {
                puts("ERROR: Failed to start monitoring audio input");
                getchar(); return 1;
        }



        for (;;) {

                if (bTargetKeyDown && bTargetKeyPrevState == 0) { bMeasuringStart = 1; }

                bTargetKeyPrevState = bTargetKeyDown;

                fflush(stdout);

        }
        // Rest of program takes place in AudioInputDataCallback()

}