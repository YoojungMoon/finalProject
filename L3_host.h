#ifndef L3_HOST_H
#define L3_HOST_H

#ifdef __cplusplus
extern "C" {
#endif


// 역할 정의
typedef enum {
    ROLE_CITIZEN = 0x01,
    ROLE_MAFIA   = 0x02,
    ROLE_POLICE  = 0x03,
    ROLE_DOCTOR  = 0x04
} Role;

// 역할 이름
extern const char* RoleNames[];

// 플레이어 구조체 
typedef struct {
    Role role;           // 역할
    uint8_t id;          // 게스트 ID
    bool isAlive;        // 생존 여부
    bool sentVoteId;     // 투표 메시지 수신 여부
    bool sentNightId;    // 밤 메시지 수신 여부
} Player;

// 플레이어 리스트 
extern Player globalPlayers[4];



// 함수 선언
void printRoleFromInput(const char* input);

void handleMode1(char* originalWord, uint8_t id1, uint8_t id2, uint8_t id3, uint8_t id4);
void printRoleFromInput(const char* input);
void printPlayerRoles(Player* globalPlayers, int count);

bool check_allSentVoted_true();
bool check_allSentVoted_false();
bool check_isGameOver_false_vote();
bool check_isGameOver_true();

void rcved_id_mafia();
void rcved_id_doctor();
void rcved_id_police();

bool check_isDoctorAlive_true();
bool check_isDoctorAlive_false();
bool check_isPoliceAlive_true();
bool check_isPoliceAlive_false();

bool check_isGameOver_false_night();

void Host_StartGameOver();



#ifdef __cplusplus
}
#endif

#endif 