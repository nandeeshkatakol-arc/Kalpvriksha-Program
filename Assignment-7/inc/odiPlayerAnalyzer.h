#ifndef ODIPLAYERANALYZER_H
#define ODIPLAYERANALYZER_H

#include "Players_data.h"

#define MAX_TEAMS 10
#define MAX_PLAYERS_PER_TEAM 50
#define NAME_LEN 50

typedef enum 
{
    ROLE_BATSMAN = 1,
    ROLE_BOWLER,
    ROLE_ALLROUNDER
}ROLE;

typedef struct
{
    int id;
    char name[NAME_LEN];
    int teamID;
    ROLE role;

    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
}playerModel;

typedef struct
{
    int teamID;
    char name[NAME_LEN]; 
    playerModel players[MAX_PLAYERS_PER_TEAM];
    int playerCount;
    float avgStrikeRate;

}Team;

extern Team teamList[MAX_TEAMS];
extern int teamListCount;

void initSystem();
ROLE getRoleEnum(const char *rolestr);
void showMenu();
void handleChoice();
void displayTeamPlayers(int teamIndex);
void displayTeamsByAvgSR();

float computePerformanceIndex(const playerModel *p);
void computeTeamAvgStrikeRate(int teamIndex);
void sortTeamsByAvgStrikeRate();
void sortTeamPlayers(int teamIndex);
void displayTopKPlayers(int teamIndex, ROLE role, int K);
void addNewPlayer();
int findTeamByName(const char *teamName);



#endif