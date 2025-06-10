#ifndef L3_HOST_H
#define L3_HOST_H

#include <vector>
#include <cstdint>

extern uint8_t main_state;

enum Role {
    ROLE_CITIZEN = 0x01,
    ROLE_MAFIA   = 0x02,
    ROLE_POLICE  = 0x03,
    ROLE_DOCTOR  = 0x04
};

struct Player {
    Role role;
    uint8_t id;
    bool isAlive;
    bool sentVoteId;
    bool sentNightId;

    Player() : role(ROLE_CITIZEN), id(0), isAlive(true), sentVoteId(false), sentNightId(false) {}
    Player(Role r, uint8_t i, bool alive)
        : role(r), id(i), isAlive(alive), sentVoteId(false), sentNightId(false) {}
};

// 전역 플레이어 목록
extern std::vector<Player> players;

// 전역 변수
static int now_try = 0;

// FSM state
#define MODE_1    1
#define DAY       2
#define VOTE      3
#define MAFIA     4
#define DOCTOR    5
#define POLICE    6
#define MODE_2    7
#define OVER      8

// 메소드 선언
void assignRandomRoles(std::vector<Player>& players);
const char* roleToString(Role role);
bool handleTestState(int hostId, uint8_t myId, uint8_t myDestId, const char* msg, uint8_t* sdu, uint8_t mystate, uint8_t myDestState);
Player* findPlayerById(std::vector<Player>& players, uint8_t id);

#endif