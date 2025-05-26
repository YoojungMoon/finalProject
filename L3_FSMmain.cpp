#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"
#include "L3_host.h"


//FSM state -------------------------------------------------
#define L3STATE_IDLE                0
#define MODE_1                      1
#define DAY                         2
#define VOTE                        3
#define MAFIA                       4
#define DOCTOR                      5
#define POLICE                      6
#define MODE_2                      7   
#define OVER                        8


//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//SDU (input)
static uint8_t originalWord[1030];
static uint8_t wordLen=0;

static uint8_t sdu[1030];

//serial port interface
static Serial pc(USBTX, USBRX);
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



void L3_initFSM(uint8_t destId)
{

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
        case L3STATE_IDLE: //IDLE state description
            if (L3_event_checkEventFlag(L3_event_msgRcvd)) //if data reception event happens
            {
                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                debug("\n -------------------------------------------------\nRCVD MSG : %s (length:%i)\n -------------------------------------------------\n", 
                            dataPtr, size);
                
                pc.printf("Give a word to send : ");
                
                L3_event_clearEventFlag(L3_event_msgRcvd);

                // 임시 : 바로 MODE_1 부터 시작
                main_state = MODE_1;
            }
            else if (L3_event_checkEventFlag(L3_event_dataToSend)) //if data needs to be sent (keyboard input)
            {
                //msg header setting
                strcpy((char*)sdu, (char*)originalWord);
                debug("[L3] msg length : %i\n", wordLen);
                L3_LLI_dataReqFunc(sdu, wordLen, myDestId);

                debug_if(DBGMSG_L3, "[L3] sending msg....\n");
                wordLen = 0;

                pc.printf("Give a word to send : ");

                L3_event_clearEventFlag(L3_event_dataToSend);

                // 임시 : 바로 MODE_1 부터 시작
                main_state = MODE_1;
            }
            break;


        case MODE_1:
            pc.printf("--------------------yoon start---------------------\n");
            pc.printf("MODE_1\n");
            printRoleFromInput((char*)originalWord); // 역할 출력
            main_state = DAY;
            break;


        case DAY:
            pc.printf("DAY\n");
            main_state = VOTE;
            break;


        case VOTE:
            pc.printf("VOTE\n");
            main_state = MAFIA;
            break;


        case MAFIA:
            pc.printf("MARIA\n");
            main_state = DOCTOR;
            break;


        case DOCTOR:
            pc.printf("DOCTOR\n");
            main_state = POLICE;
            break;


        case POLICE:
            pc.printf("POLICE\n");
            main_state = MODE_2;
            break;
        

        case MODE_2:
            pc.printf("MODE_2\n");
            main_state = OVER;
            break;

            
        case OVER:
            pc.printf("OVER\n");
            main_state = L3STATE_IDLE;
            break;


        default :
            break;
    }
}