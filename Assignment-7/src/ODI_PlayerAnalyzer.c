#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ODI_PlayerAnalyzer.h"
#include "Players_data.h"

Team teamList[MAX_TEAMS];
int teamListCount = 0;

typedef enum
{
    MENU_EXIT = 0,
    MENU_DISPLAY_TEAM_PLAYERS = 1,
    MENU_DISPLAY_TEAMS_BY_AVG_SR = 2,
    MENU_TOP_K_PLAYERS = 3,
    MENU_GLOBAL_ROLE_DISPLAY = 4,
    MENU_ADD_NEW_PLAYER = 5
} MenuOption;

typedef struct
{
    int teamIndex;
    int playerIndex;
    float pi;
}HeapNode;

ROLE getRoleEnum(const char *rolestr)
{
    ROLE role;
    if(strcmp(rolestr,"Batsman") == 0)
    {
        role = ROLE_BATSMAN;
    }
    else if(strcmp(rolestr, "Bowler") == 0)
    {
        role = ROLE_BOWLER;
    }
    else
    {
        role = ROLE_ALLROUNDER;
    }
    return role;
}

static int getTeamIndexFromHeader(const char *teamName)
{
    int result = -1;

    for (int iterator = 0; iterator < teamCount; iterator++)
    {
        if (strcmp(teams[iterator], teamName) == 0)
        {
            result = iterator;
            break;
        }
    }

    return result;
}

void initSystem()
{
    teamListCount = teamCount;

    for(int iterator = 0; iterator < teamListCount; iterator++)
    {
        strcpy(teamList[iterator].name, teams[iterator]);
        teamList[iterator].teamID = iterator + 1;
        teamList[iterator].playerCount = 0;
        teamList[iterator].avgStrikeRate = 0.0f;
    }

    for (int iterator = 0; iterator < playerCount; iterator++)
    {
        int teamIndex = getTeamIndexFromHeader(players[iterator].team);

        if(teamIndex < 0 || teamIndex >= teamListCount)
            continue;

        if(teamList[teamIndex].playerCount >= MAX_PLAYERS_PER_TEAM)
            continue;

        playerModel *p = &teamList[teamIndex].players[teamList[teamIndex].playerCount++];

        p->id = players[iterator].id;
        strcpy(p->name, players[iterator].name);
        p->teamID = teamList[teamIndex].teamID;
        
        p->role = getRoleEnum(players[iterator].role);

        p->totalRuns = players[iterator].totalRuns;
        p->battingAverage = players[iterator].battingAverage;
        p->strikeRate = players[iterator].strikeRate;
        p->wickets = players[iterator].wickets;
        p->economyRate = players[iterator].economyRate;
        
        p->performanceIndex = computePerformanceIndex(p);
    }

    for (int iterator = 0; iterator < teamListCount; iterator++)
    {
        computeTeamAvgStrikeRate(iterator);
    }
    for (int iterator = 0; iterator < teamListCount; iterator++)
    {
        sortTeamPlayers(iterator);
    }

    printf("System initialized with %d teams and %d players loaded.\n",
            teamListCount, playerCount);         
}

float computePerformanceIndex(const playerModel *p)
{
    float result = 0.0f;

    float battingPI = (p->battingAverage * p->strikeRate) / 100.0f;
    float bowlingPI = (p->wickets * 2.0f) + (100.0f - p->economyRate);

    switch (p->role)
    {
        case ROLE_BATSMAN:
            result = battingPI;
            break;

        case ROLE_BOWLER:
            result = bowlingPI;
            break;

        case ROLE_ALLROUNDER:
            result = battingPI + (p->wickets * 2.0f);
            break;

        default:
            break;
    }

    return result;
}

void computeTeamAvgStrikeRate(int teamIndex)
{
    Team *t = &teamList[teamIndex];

    float avg = 0.0f;
    float sum = 0.0f;
    int count = t->playerCount;

    if (count > 0)
    {
        for (int iterator = 0; iterator < count; iterator++)
        {
            sum += t->players[iterator].strikeRate;
        }
        avg = sum / (float)count;
    }
    else
    {
        avg = 0.0f;
    }

    t->avgStrikeRate = avg;
    return;
}

void sortTeamsByAvgStrikeRate()
{
    for(int i = 0; i < teamListCount-1; i++)
    {
        for(int j = i+1; j < teamListCount; j++)
        {
            if(teamList[j].avgStrikeRate > teamList[i].avgStrikeRate)
            {
                Team temp = teamList[i];
                teamList[i] = teamList[j];
                teamList[j] = temp;
            }
        }
    }
    printf("\nTeams have been sorted by Average Strike Rate (Descending).\n");
}

void sortTeamPlayers(int teamIndex)
{
     Team *t = &teamList[teamIndex];
     for (int i = 0; i < t->playerCount-1; i++)
     {
        for(int j = i+1; j < t->playerCount; j++)
        {
            if (t->players[j].performanceIndex > t->players[i].performanceIndex)
            {
                playerModel temp = t->players[i];
                t->players[i] = t->players[j];
                t->players[j] = temp;
            }
        }
     }
}

void displayTopKPlayers(int teamIndex, ROLE role, int K)
{
    int valid = 1; 
    if (teamIndex < 0 || teamIndex >= teamListCount)
    {
        valid = 0;
        printf("Invalid team index!\n");
    }

    if (valid && K <= 0)
    {
        valid = 0;
        printf("Invalid K (must be > 0).\n");
    }

    if (valid)
    {
        Team *t = &teamList[teamIndex];
        printf("\nTop %d ", K);
        if (role == ROLE_BATSMAN)
            printf("Batsmen");
        else if (role == ROLE_BOWLER)
            printf("Bowlers");
        else
            printf("All-Rounders");

        printf(" of Team %s:\n", t->name);

        int count = 0;
        for (int i = 0; i < t->playerCount && count < K; i++)
        {
            if (t->players[i].role == role)
            {
                playerModel *p = &t->players[i];
                printf("%d  %-20s  PI: %.2f  Runs:%d  SR:%.1f  WK:%d\n",
                       p->id, p->name, p->performanceIndex, p->totalRuns, p->strikeRate, p->wickets);
                count++;
            }
        }
        if (count == 0)
        {
            printf("No players of this role found in this team.\n");
        }
    }

    return;
}

void heapSwap(HeapNode *a, HeapNode *b)
{
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyDown(HeapNode heap[], int size, int currentIndex)
{
    int largest = currentIndex;
    int left = 2*currentIndex+1;
    int right = 2*currentIndex+2;

    if(left < size && heap[left].pi > heap[largest].pi)
    {
        largest = left;
    }
    if(right < size && heap[right].pi > heap[largest].pi)
    {
        largest = right;
    }
    if(largest != currentIndex)
    {
        heapSwap(&heap[currentIndex], &heap[largest]);
        heapifyDown(heap, size, largest);
    }
}

int buildInitialHeap(HeapNode heap[], ROLE role)
{
    int heapSize = 0;

    for (int team = 0; team < teamCount; team++)
    {
        int firstRolePlayerIndex = -1;

        for(int playerIdx = 0; playerIdx < teamList[team].playerCount; playerIdx++)
        {
             if(teamList[team].players[playerIdx].role == role)
             {
                firstRolePlayerIndex = playerIdx;
                break;
             }
        }

        if(firstRolePlayerIndex != -1)
        {
            heap[heapSize].teamIndex = team;
            heap[heapSize].playerIndex = firstRolePlayerIndex;
            heap[heapSize].pi = teamList[team].players[firstRolePlayerIndex].performanceIndex;

            heapSize++;
        }
    }
    for (int iterator = heapSize / 2 - 1; iterator >= 0; iterator--)
    {
        heapifyDown(heap, heapSize, iterator);
    }
    return heapSize;

}

HeapNode heapPop(HeapNode heap[], int *size)
{
    HeapNode top = heap[0];
    heap[0] = heap[--(*size)];
    heapifyDown(heap, *size, 0);
    return top;
}

int findTeamByName(const char *teamName)
{
    int result = -1;

    for (int iterator = 0; iterator < teamListCount; iterator++)
    {
        if (strcmp(teamList[iterator].name, teamName) == 0)
        {
            result = iterator;
            break;
        }
    }

    return result;
}

void displayPlayersByRoleHeap(ROLE role)
{
    printf("\n=== All Players of Role %d (Heap Merge) ===\n", role);

    HeapNode heap[MAX_TEAMS];
    int heapSize = buildInitialHeap(heap, role);

    while (heapSize > 0)
    {
        HeapNode topNode = heapPop(heap, &heapSize);

        int teamIndex = topNode.teamIndex;
        int playerIndex = topNode.playerIndex;

        playerModel *player = &teamList[teamIndex].players[playerIndex];

        printf("%d %-20s Team:%s PI:%.2f SR:%.1f WK:%d\n",
               player->id, player->name,
               teamList[teamIndex].name,
               player->performanceIndex,
               player->strikeRate,
               player->wickets);

        int nextPlayerIndex = playerIndex + 1;

        while (nextPlayerIndex < teamList[teamIndex].playerCount &&
               teamList[teamIndex].players[nextPlayerIndex].role != role)
        {
            nextPlayerIndex++;
        }
        
        if (nextPlayerIndex < teamList[teamIndex].playerCount)
        {
            heap[heapSize].teamIndex = teamIndex;
            heap[heapSize].playerIndex = nextPlayerIndex;
            heap[heapSize].pi = teamList[teamIndex].players[nextPlayerIndex].performanceIndex;
            heapSize++;
        }

        for (int heapIndex = heapSize / 2 - 1; heapIndex >= 0; heapIndex--)
        {
            heapifyDown(heap, heapSize, heapIndex);
        }
    }
}

void addNewPlayer()
{
    playerModel newP;
    char teamName[50];
    char roleStr[20];

    printf("\nEnter Player ID (<=1500): ");
    scanf("%d", &newP.id);

    if (newP.id > 1500)
    {
        printf("Invalid ID. Must be <=1500.\n");
        return;
    }

    printf("Enter Player Name: ");
    scanf(" %[^\n]", newP.name);

    if (strlen(newP.name) > 50)
    {
        printf("Name too long!\n");
        return;
    }

    printf("Enter Team Name: ");
    scanf(" %[^\n]", teamName);

    int teamIndex = findTeamByName(teamName);
    if (teamIndex == -1)
    {
        printf("Team not found!\n");
        return;
    }

    if (teamList[teamIndex].playerCount >= MAX_PLAYERS_PER_TEAM)
    {
        printf("Team full! Cannot add more players.\n");
        return;
    }

    printf("Enter Role (Batsman/Bowler/All-rounder): ");
    scanf(" %[^\n]", roleStr);

    newP.role = getRoleEnum(roleStr);

    printf("Enter Total Runs: ");
    scanf("%d", &newP.totalRuns);

    printf("Enter Batting Average: ");
    scanf("%f", &newP.battingAverage);

    printf("Enter Strike Rate: ");
    scanf("%f", &newP.strikeRate);

    printf("Enter Wickets: ");
    scanf("%d", &newP.wickets);

    printf("Enter Economy Rate: ");
    scanf("%f", &newP.economyRate);

    newP.performanceIndex = computePerformanceIndex(&newP);

    int pos = teamList[teamIndex].playerCount;
    teamList[teamIndex].players[pos] = newP;
    teamList[teamIndex].playerCount++;

    computeTeamAvgStrikeRate(teamIndex);
    sortTeamPlayers(teamIndex);

    printf("\nPlayer added successfully to %s!\n", teamList[teamIndex].name);
}

void displayTeamPlayers(int teamIndex)
{
    if(teamIndex < 0 || teamIndex >= MAX_TEAMS)
    {
        printf("\n==========Invalid team index=========\n");
        return;
    }

    Team *t = &teamList[teamIndex];
    for(int i = 0; i < t->playerCount; i++)
    {
        playerModel *p = &t->players[i];
        printf("%d  %-25s  Role: %d  SR: %.2f  WK: %d  PI: %.2f\n",p->id, p->name, p->role, p->strikeRate, p->wickets, p->performanceIndex);
    }
}

void displayTeamsByAvgSR()
{
    sortTeamsByAvgStrikeRate();  

    printf("\n===== Teams Sorted by Average Strike Rate =====\n\n");

    for (int i = 0; i < teamListCount; i++)
    {
        printf("%d. %-15s  Avg SR = %.2f\n",
               i,
               teamList[i].name,
               teamList[i].avgStrikeRate);
    }
}

void showMenu()
{
    printf("\n========== MAIN MENU ==========\n");
    printf("1. Display players of a team\n");
    printf("2. Display teams by average strike rate\n");
    printf("3. Display Top K players of a team by role\n");
    printf("4. Display players of a role (global, PI descending)\n");
    printf("5. Add New Player\n");
    printf("0. Exit\n");
    printf("Enter choice: ");
}

void handleChoice()
{
    int ch;
    scanf("%d", &ch);

switch ((MenuOption)ch)
{
    case MENU_DISPLAY_TEAM_PLAYERS:
    {
        printf("Enter team index (0-9): ");
        int t;
        scanf("%d", &t);
        displayTeamPlayers(t);
        break;
    }

    case MENU_DISPLAY_TEAMS_BY_AVG_SR:
        displayTeamsByAvgSR();
        break;

    case MENU_TOP_K_PLAYERS:
    {
        int t, role, K;
        printf("Enter team index (0-9): ");
        scanf("%d", &t);

        printf("Enter role (1=Batsman, 2=Bowler, 3=All-rounder): ");
        scanf("%d", &role);

        printf("Enter K: ");
        scanf("%d", &K);

        displayTopKPlayers(t, (ROLE)role, K);
        break;
    }

    case MENU_GLOBAL_ROLE_DISPLAY:
    {
        int role;
        printf("Enter role (1=Batsman,2=Bowler,3=All-rounder): ");
        scanf("%d", &role);

        displayPlayersByRoleHeap((ROLE)role);
        break;
    }

    case MENU_ADD_NEW_PLAYER:
        addNewPlayer();
        break;

    case MENU_EXIT:
        printf("Exiting...\n");
        exit(0);

    default:
        printf("Invalid choice.\n");
        break;
}

}