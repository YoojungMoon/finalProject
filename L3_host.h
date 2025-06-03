#ifndef L3_HOST_H
#define L3_HOST_H

#ifdef __cplusplus
extern "C" {
#endif


// yoon - extern 변수
extern uint8_t main_state;


// yoon - FSM state
#define TEST    1
#define END     2


// yoon - 메소드 정의
void handleTestState(int hostId, uint8_t myId, uint8_t myDestId, const char* msg, uint8_t* sdu, uint8_t state);


#ifdef __cplusplus
}
#endif

#endif 