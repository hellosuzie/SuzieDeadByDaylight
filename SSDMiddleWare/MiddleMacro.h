#pragma once

#define MAXSURVIVORUSER 4
#define MAXKILLERUSER 1

#define MAXROOMUSER 5

#define TRYCONNECTTIME 5.f
#define PINGTIME 0.25f

#define BROADCASTTIME 0.05f
#define SYNCROTIME   0.25f

#define LOBBYROOM -1

#define MAXLOGSTRING 256 

#define MAXROOMCOUNT 6					// 서버 상에 존재하는 룸 갯수 : 테스트용 룸(0번) + 실제 게임 룸(1번 ~ 5번)
#define MAXACCESSROOMCOUNT 5			// 실제 게임룸갯수


#define MAXMAGICCIRCLE 40

#define TIMING(Acc,Delta,Time) ((Acc < Time) && ((Acc + Delta) >= Time) )