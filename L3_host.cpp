#include <string.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>  
#include <time.h>    
#include <algorithm>    
#include <random>      
#include <ctime> 
#include <iostream>
using namespace std;

#include "L3_host.h"
#include "L3_FSMevent.h"    
#include "L3_timer.h"       
#include "L3_LLinterface.h" 
#include "mbed.h"
#include "L3_host.h"    


// 역할 랜덤 배정 함수
std::vector<Player> players;
void assignRandomRoles(vector<Player>& players) {

    vector<Role> allRoles = { ROLE_CITIZEN, ROLE_MAFIA, ROLE_POLICE, ROLE_DOCTOR };

    // 랜덤 섞기
    random_device rd;
    mt19937 gen(rd());
    shuffle(allRoles.begin(), allRoles.end(), gen);

    // 역할 배정
    for (size_t i = 0; i < players.size(); ++i) {
        players[i].role = allRoles[i];
    }
    
}


// 역할 출력 함수
const char* roleToString(Role role) {
    switch(role) {
        case ROLE_CITIZEN: return "Citizen";
        case ROLE_MAFIA:   return "Mafia";
        case ROLE_POLICE:  return "Police";
        case ROLE_DOCTOR:  return "Doctor";
        default:           return "Unknown";
    }
}


// 메시지 전송 메소드 (호스트 id, 내 id, 상대방 id, 전달할 메시지, 상태)
bool handleTestState(int hostId, uint8_t myId, uint8_t myDestId, const char* msg, uint8_t* sdu, uint8_t mystate, uint8_t myDestState) {
    if (myId == hostId) {  // 호스트
        if (L3_timer_getTimerStatus() == 0) {
            int wordLen = strlen(msg);
            strcpy((char*)sdu, msg);
            debug("%u 에게 전송 중...", myDestId);
            L3_LLI_dataReqFunc(sdu, wordLen, myDestId);
            L3_timer_startTimer();

            if (L3_event_checkEventFlag(L3_event_dataSendCnf)) {
                L3_event_clearEventFlag(L3_event_dataSendCnf);
                debug("ACK 수신됨\n");

                now_try++;
                main_state = mystate;
                return true;  // 전송 성공
            }
        }
    } else {  // 게스트
        if (L3_event_checkEventFlag(L3_event_msgRcvd)) {
            uint8_t* dataPtr = L3_LLI_getMsgPtr();
            debug("\n\n받은 메시지 : %s \n", dataPtr);
            L3_event_clearEventFlag(L3_event_msgRcvd);

            // 호스트에게 ACK 전송
            const char* ack = "ACK";
            strcpy((char*)sdu, ack);
            L3_LLI_dataReqFunc(sdu, strlen(ack), hostId);

            main_state = myDestState;
        }
    }

    return false;
}


// ID로 플레이어를 찾는 함수
Player* findPlayerById(std::vector<Player>& players, uint8_t id) {
    for (auto& p : players) {
        if (p.id == id) return &p;
    }
    return nullptr;  // 못 찾으면 nullptr 반환
}