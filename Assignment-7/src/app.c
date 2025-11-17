#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "Players_data.h"

Team teamList[MAX_TEAMS];
int teamListCount = 0;

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
    for(int i = 0; i < teamCount; i++)
    {
        if(strcmp(teams[i], teamName) == 0)
        {
            return i;
        }
    }
    return -1;
}

void initSystem()
{
    teamListCount = teamCount;

    for(int i = 0; i < teamListCount; i++)
    {
        strcpy(teamList[i].name, teams[i]);
        teamList[i].teamID = i + 1;
        teamList[i].playerCount = 0;
        teamList[i].avgStrikeRate = 0.0f;
    }

    for (int i = 0; i < playerCount; i++)
    {
        int teamIndex = getTeamIndexFromHeader(players[i].team);

        if(teamIndex < 0 || teamIndex >= teamListCount)
            continue;

        if(teamList[teamIndex].playerCount >= MAX_PLAYERS_PER_TEAM)
            continue;

        playerModel *p = &teamList[teamIndex].players[teamList[teamIndex].playerCount++];

        p->id = players[i].id;
        strcpy(p->name, players[i].name);
        p->teamID = teamList[teamIndex].teamID;
        
        p->role = getRoleEnum(players[i].role);

        p->totalRuns = players[i].totalRuns;
        p->battingAverage = players[i].battingAverage;
        p->strikeRate = players[i].strikeRate;
        p->wickets = players[i].wickets;
        p->economyRate = players[i].economyRate;
        
        p->performanceIndex = computePerformanceIndex(p);
    }

    for (int i = 0; i < teamListCount; i++)
    {
        computeTeamAvgStrikeRate(i);
    }
    for (int i = 0; i < teamListCount; i++)
    {
        sortTeamPlayers(i);
    }

    printf("System initialized with %d teams and %d players loaded.\n",
            teamListCount, playerCount);         
}

float computePerformanceIndex(playerModel *p)
{
    float battingPI = (p->battingAverage * p->strikeRate) / 100.0f;
    float bowlingPI = (p->wickets * 2.0f) + (100.0f - p->economyRate);

    switch(p->role)
    {
        case ROLE_BATSMAN:
            return battingPI; 

        case ROLE_BOWLER:
            return bowlingPI;

        case ROLE_ALLROUNDER:
            return battingPI + (p->wickets * 2.0f);
        
        default:
            printf("\nInvalid player type\n");

    }
     return 0.0f;
}

void computeTeamAvgStrikeRate(int teamIndex)
{
    Team *t = &teamList[teamIndex];

    if (t->playerCount == 0) 
    {
        t->avgStrikeRate = 0;
        return;
    }

    float sum = 0;
    for (int i = 0; i < t->playerCount; i++)
    {
        sum += t->players[i].strikeRate;
    }

    t->avgStrikeRate = sum / t->playerCount;
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
    if(teamIndex < 0 || teamIndex >= teamListCount)
    {
        printf("Invalid team index!\n");
        return;
    }

    Team *t = &teamList[teamIndex];
    printf("\nTop %d ", K);
    if (role == ROLE_BATSMAN)
    {
        printf("Batsmen");
    }     
    else if (role == ROLE_BOWLER)
    {
        printf("Bowlers");
    }  
    else
    {
        printf("All-Rounders");
    }
    printf(" of Team %s:\n", t->name);

    int count = 0;
    for ( int i = 0; i < t->playerCount && count <= K; i++)
    {
        if(t->players[i].role == role)
        {
            playerModel *p = &t->players[i];
            printf("%d  %-20s  PI: %.2f  Runs:%d  SR:%.1f  WK:%d\n",p->id, p->name, p->performanceIndex,p->totalRuns, p->strikeRate, p->wickets);

            count++;
        }
    }
    if(count == 0)
    {
        printf("No players of this role found in this team.\n");
    }   
}

void heapSwap(HeapNode *a, HeapNode *b)
{
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyDown(HeapNode heap[], int size, int i)
{
    int largest = i;
    int left = 2*i+1;
    int right = 2*i+2;

    if(left < size && heap[left].pi > heap[largest].pi)
    {
        largest = left;
    }
    if(right < size && heap[right].pi > heap[largest].pi)
    {
        largest = right;
    }
    if(largest != i)
    {
        heapSwap(&heap[i], &heap[largest]);
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
    for (int i = heapSize / 2 - 1; i >= 0; i--)
    {
        heapifyDown(heap, heapSize, i);
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

int binarySearchTeamByName(const char *teamName)
{
    int low = 0, high = teamListCount - 1;

    while (low <= high)
    {
        int mid = (low + high) / 2;
        int cmp = strcmp(teamList[mid].name, teamName);

        if (cmp == 0)
        {
            return mid;
        }
        else if (cmp < 0)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }
    return -1; 
}

void displayPlayersByRoleHeap(ROLE role)
{
    printf("\n=== All Players of Role %d (Heap Merge) ===\n", role);

    HeapNode heap[MAX_TEAMS];
    int size = buildInitialHeap(heap, role);

    while (size > 0)
    {
        HeapNode top = heapPop(heap, &size);

        int t = top.teamIndex;
        int i = top.playerIndex;

        playerModel *p = &teamList[t].players[i];

        printf("%d %-20s Team:%s PI:%.2f SR:%.1f WK:%d\n",p->id, p->name, teamList[t].name,p->performanceIndex, p->strikeRate, p->wickets);

        int next = i + 1;

        while (next < teamList[t].playerCount &&
               teamList[t].players[next].role != role)
        {
            next++;
        }

        if (next < teamList[t].playerCount)
        {
            heap[size].teamIndex = t;
            heap[size].playerIndex = next;
            heap[size].pi = teamList[t].players[next].performanceIndex;
            size++;
        }
        for (int j = size/2 - 1; j >= 0; j--)
            heapifyDown(heap, size, j);
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

    int teamIndex = binarySearchTeamByName(teamName);
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

    switch (ch)
    {
        case 1:
        {
            printf("Enter team index (0-9): ");
            int t;
            scanf("%d", &t);
            displayTeamPlayers(t);
            break;
        }

        case 2:
            displayTeamsByAvgSR();
            break;

        case 3:
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

        case 4:
        {
            int role;
            printf("Enter role (1=Batsman,2=Bowler,3=All-rounder): ");
            scanf("%d", &role);
        
            displayPlayersByRoleHeap((ROLE)role);
            break;
        }
        case 5:
            addNewPlayer();
            break;

        case 0:
            printf("Exiting...\n");
            exit(0);

        default:
            printf("Invalid choice.\n");
    }
}