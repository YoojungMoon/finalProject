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
uint8_t main_state = TEST; //protocol state
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

    pc.printf("Give a word to send : ");
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
            main_state = TEST;
        case TEST:

            // 게스트이면 (임시로 -> endnode를 1로 하면 호스트)
            if (myId!=1) {

                if (L3_event_checkEventFlag(L3_event_msgRcvd)) // 메시지를 받으면 
                {
                    uint8_t* dataPtr = L3_LLI_getMsgPtr();
                    uint8_t size = L3_LLI_getSize();

                    debug("\n --------------------\nyonnnnnnnnnn: %s (length:%i)\n -------------------------------\n", 
                                dataPtr, size);
                    
                    L3_event_clearEventFlag(L3_event_msgRcvd);

                    
                    main_state = END;
                }

            }
            // 호스트인 경우 (임시로 endnode == 1이면 호스트)
            else
            {
                if (L3_timer_getTimerStatus() == 0)  // 타이머가 꺼져 있으면 (즉, 보낼 수 있으면)
                {
                    // 메시지 정보
                    const char* originalWord = "gkgk";  // 전달할 내용 
                    int wordLen = strlen(originalWord); // 문자열 길이 
                    int myDestId = 31;                  // 목적지 ID

                    // 메시지 전송
                    strcpy((char*)sdu, (char*)originalWord);
                    debug("%u 에게 전송 중...", myDestId);
                    L3_LLI_dataReqFunc(sdu, wordLen, myDestId);

                    // 전송 후 타이머 시작 (예: 1초 동안 다시 전송 금지)
                    L3_timer_startTimer();

                    // 전송 확인되면 상태 종료
                    if (L3_event_checkEventFlag(L3_event_dataSendCnf)) {
                        main_state = END;
                    }
                }
                else {
                    // 타이머가 동작 중이면 아무 것도 하지 않음 (보내지 않음)
                }
            }


            break;

        case END:
            break;

        default :
            break;
    }
}