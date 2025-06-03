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
            main_state = MODE_1;

        case MODE_1:
        {
            players.clear();  // 기존 내용 초기화

            // 플레이어 4명 생성, id는 1, 3, 6, 7로 지정
            std::vector<uint8_t> ids = {1, 3, 6, 7};
            for (int i = 0; i < 4; ++i) {
                players.push_back(Player{ROLE_CITIZEN, ids[i], true});
            }

            assignRandomRoles(players);

            // 1번 플레이어(id==1) 역할 출력
            auto it = std::find_if(players.begin(), players.end(),
                                [](const Player& p){ return p.id == 1; });
            if (it != players.end()) {
                std::string roleStr = std::string("당신의 역할 : ") + roleToString(it->role);
                handleTestState(1, myId, myDestId, roleStr.c_str(), sdu, OVER, OVER);
            } else {
                std::cerr << "플레이어 ID 1을 찾을 수 없습니다." << std::endl;
            }

            break;
        }

        
        default :
            break;
    }
}