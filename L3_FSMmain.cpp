#include <iostream>
#include <algorithm>
#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"

#include "L3_host.h"


//FSM state -------------------------------------------------
#define L3STATE_IDLE                0

//state variables
uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//SDU (input)
static uint8_t originalWord[1030];
static uint8_t wordLen=0;

static uint8_t sdu[1030];

//serial port interface
static Serial pc(USBTX, USBRX);
static uint8_t myId;
static uint8_t myDestId;

//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_dataToSend))
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_dataToSend);
            debug_if(DBGMSG_L3,"word is ready! ::: %s\n", originalWord);
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
        }
    }
}



void L3_initFSM(uint8_t thisId, uint8_t destId)
{
    myId = thisId;
    myDestId = destId;
    //initialize service layer
    pc.attach(&L3service_processInputWord, Serial::RxIrq);
}

void L3_FSMrun(void)
{   
    if (prev_state != main_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %i to %i\n", prev_state, main_state);
        prev_state = main_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        case L3STATE_IDLE:
            main_state = MODE_1;
            break;

        case MODE_1:
        {
            static int currentIndex = 0; // for 상태 저장
            uint8_t hostId = 100;
            uint8_t destIds[4] = {1, 2, 3, 8};

            if (currentIndex < 4) {
                myDestId = destIds[currentIndex];
                L2_initFSM(myId); 
                L3_initFSM(myId, myDestId);

                // 완료될 때까지 기다림
                if (handleTestState(hostId, myId, myDestId, "hoelo", sdu, MODE_1, DAY)) {
                    currentIndex++; // 완료되면 다음 인덱스로
                }
            } else {
                // 호스트 상태 변경 
                main_state = DAY;
            }
            break;
        }

        case DAY:
            main_state = VOTE;
            break;

        case VOTE:
        {
            main_state = OVER;
            break;
        }

        
        default :
            break;
    }
}