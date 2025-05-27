#include <string.h>
#include <stdio.h>
#include "mbed.h"
#include "L3_host.h"
#include <vector>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time()


// 외부 선언된 Serial 객체 참조 
extern Serial pc;


// 플레이어 역할 배정 
Player globalPlayers[4];

const char* RoleNames[] = {
    "시민",    // ROLE_CITIZEN = 0
    "마피아",  // ROLE_MAFIA = 1
    "경찰",    // ROLE_POLICE = 2
    "의사"     // ROLE_DOCTOR = 3
};

void shuffle(Role* array, int size) {
    for (int i = size - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Role temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void handleMode1(char* originalWord, uint8_t id1, uint8_t id2, uint8_t id3, uint8_t id4) {
    pc.printf("-------------------yoon start---------------------\n");
    pc.printf("MODE_1\n");

    // 호스트 - 역할 배정 
    if (strcmp(originalWord, "0") == 0) {
        // 난수 초기화
        srand(time(NULL));  

        // 전달받은 플레이어 ID 저장
        uint8_t playerIds[4] = { id1, id2, id3, id4 };

        // 역할 목록 
        Role roleList[4] = { ROLE_CITIZEN, ROLE_MAFIA, ROLE_POLICE, ROLE_DOCTOR };
        shuffle(roleList, 4);  // 역할 섞기

        // 랜덤한 역할을 각 플레이어 ID에 매칭
        for (int i = 0; i < 4; ++i) {
            globalPlayers[i] = { roleList[i], playerIds[i], true, false, false };
        }
    }
    
}

// 역할 출력 
void printRoleFromInput(const char* input) {
    if (strcmp(input, "0") == 0) {
        pc.printf("역할은 호스트\n");
        printPlayerRoles(globalPlayers, 4);
    } else {
        int index = atoi(input);  // 문자열을 정수 인덱스로 변환
        pc.printf("%d 의 역할은 %s\n", index, RoleNames[globalPlayers[index].role]);
    }
}

// 전체 역할 출력 
void printPlayerRoles(Player* globalPlayers, int count) {
    printf("플레이어 역할 목록:\n");
    for (int i = 0; i < count; ++i) {
        printf("ID: %d - ", globalPlayers[i].id);
        switch (globalPlayers[i].role) {
            case ROLE_CITIZEN: printf("시민\n"); break;
            case ROLE_MAFIA:   printf("마피아\n"); break;
            case ROLE_POLICE:  printf("경찰\n"); break;
            case ROLE_DOCTOR:  printf("의사\n"); break;
            default:           printf("알 수 없음\n"); break;
        }
    }
}



// 투표 여부 확인 (모든 플레이어가 투표를 마쳤는지)
bool check_allSentVoted_true() {
    // 실제 구현 필요
    return true;
}

// 투표가 완료되었는지 확인
bool check_allSentVoted_false() {
    // 실제 구현 필요
    return false;
}


// 게임 오버 여부 확인 (투표 후 종료 조건)
bool check_isGameOver_false_vote() {
    // 실제 구현 필요
    return false;
}

bool check_isGameOver_true() {
    // 실제 구현 필요
    return true;
}


// 마피아 역할 확인
void rcved_id_mafia() {
    pc.printf("마피아 역할 정보 수신됨\n");
}

// 의사 역할 확인 
void rcved_id_doctor() {
    pc.printf("의사 역할 정보 수신됨\n");
}

// 경찰 역할 확인 
void rcved_id_police() {
    pc.printf("경찰 역할 정보 수신됨\n");
}


// 의사가 살아 있는지 체크
bool check_isDoctorAlive_true() {
    return true;
}

bool check_isDoctorAlive_false() {
    return false;
}


// 경찰이 살아 있는지 체크
bool check_isPoliceAlive_true() {
    return true;
}

bool check_isPoliceAlive_false() {
    return false;
}


// 밤에 게임 오버 조건 체크
bool check_isGameOver_false_night() {
    return false;
}


// 게임 오버 
void Host_StartGameOver() {
    pc.printf("게임이 종료 되었습니다\n");
}