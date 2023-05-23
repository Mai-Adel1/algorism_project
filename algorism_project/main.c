#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEAMS 20
#define MAX_MATCHES 600

typedef struct TeamVertix {
  char name[20];
  struct Edge* edges;
  int num_edges;
  int matches_played;
  int wins;
  int draws;
  int losses;
  int goals_for;
  int goals_against;
  int goal_difference;
  int points;
  struct TeamVertix* next;  // For BFS traversal
  int visited;  // For BFS traversal
} TeamVertix;

typedef struct Edge {
  struct TeamVertix* opponent;
  int round;
  char date[11];
  int home_goals;
  int away_goals;
  struct Edge* next;
} Edge;

// Function to find a team node in the teams array
TeamVertix* findTeamVertix(TeamVertix* teams[], int num_teams, const char* team_name) {
  for (int i = 0; i < num_teams; i++) {
    if (strcmp(teams[i]->name, team_name) == 0) {
      return teams[i];
    }
  }
  return NULL;
}

// Function to add a team node to the teams array
void addTeamVertix(TeamVertix* teams[], int* num_teams, const char* team_name) {
  TeamVertix* team_vertix = (TeamVertix*)malloc(sizeof(TeamVertix));
  strcpy(team_vertix->name, team_name);
  team_vertix->edges = NULL;
  team_vertix->num_edges = 0;
  team_vertix->matches_played = 0;
  team_vertix->wins = 0;
  team_vertix->draws = 0;
  team_vertix->losses = 0;
  team_vertix->goals_for = 0;
  team_vertix->goals_against = 0;
  team_vertix->goal_difference = 0;
  team_vertix->points = 0;
  team_vertix->next = NULL;
  team_vertix->visited = 0;
  teams[*num_teams] = team_vertix;
  (*num_teams)++;
}

// Function to add an edge to the team node's edges list
void addEdge(TeamVertix* team, TeamVertix* opponent, int round, const char* date, int home_goals, int away_goals) {
  Edge* edge = (Edge*)malloc(sizeof(Edge));
  edge->opponent = opponent;
  edge->round = round;
  strcpy(edge->date, date);
  edge->home_goals = home_goals;
  edge->away_goals = away_goals;
  edge->next = NULL;

  if (team->edges == NULL) {
    team->edges = edge;
  } else {
    Edge* curr = team->edges;
    while (curr->next != NULL) {
      curr = curr->next;
    }
    curr->next = edge;
  }

  team->num_edges++;
}


// Function to parse and process the match results from the file
void parseMatchResults(FILE* file, TeamVertix* teams[], int* num_teams) {
  char line[100];
  fgets(line, sizeof(line), file);

  // Loop through the lines of the file and parse the match results
  while (fgets(line, sizeof(line), file)) {

    // Parse the fields of the line using strtok and atoi functions
    char* token = strtok(line, ",");
    int round = atoi(token);

    token = strtok(NULL, ",");
    char date[11];
    strcpy(date, token);

    token = strtok(NULL, ",");
    char home_team[20];
    strcpy(home_team, token);

    token = strtok(NULL, ",");
    char away_team[20];
    strcpy(away_team, token);

      token = strtok(NULL, ",");
      int home_goals;
      if (strcmp(token, "-") == 0) {
        continue; // skip processing if match was canceled
      } else {
        home_goals = atoi(token);
      }

      token = strtok(NULL, ",");
      int away_goals;
      if (strcmp(token, "-") == 0) {
        continue; // skip processing if match was canceled
      } else {
        away_goals = atoi(token);
      }
      TeamVertix* home_team_vertix = findTeamVertix(teams, *num_teams, home_team);
    if (home_team_vertix == NULL) {
      addTeamVertix(teams, num_teams, home_team);
      home_team_vertix = teams[*num_teams - 1];
    }

    TeamVertix* away_team_vertix = findTeamVertix(teams, *num_teams, away_team);
    if (away_team_vertix == NULL) {
      addTeamVertix(teams, num_teams, away_team);
      away_team_vertix = teams[*num_teams - 1];
    }


    addEdge(home_team_vertix, away_team_vertix, round, date, home_goals, away_goals);
    addEdge(away_team_vertix, home_team_vertix, round, date, away_goals, home_goals);
      }

}



// Function to perform breadth-first search and calculate the standings up to the specified round
void calculateStandingsBFS(TeamVertix* teams[], int num_teams, int round_number) {
  // Reset the visited flags
  for (int i = 0; i < num_teams; i++) {
    teams[i]->visited = 0;
  }

  // Initialize the BFS queue
  TeamVertix* queue[MAX_TEAMS];
  int front = 0;
  int rear = 0;

  // Enqueue the first team
  queue[rear++] = teams[0];
  teams[0]->visited = 1;

  // Perform BFS traversal
  while (front < rear) {
    // Dequeue a team
    TeamVertix* current_team = queue[front++];
    current_team->matches_played = 0;
    current_team->wins = 0;
    current_team->draws = 0;
    current_team->losses = 0;
    current_team->goals_for = 0;
    current_team->goals_against = 0;
    current_team->goal_difference = 0;
    current_team->points = 0;

    Edge* edge = current_team->edges;
    while (edge != NULL) {
      if (edge->round <= round_number) {
        current_team->matches_played++;
        current_team->goals_for += edge->home_goals;
        current_team->goals_against += edge->away_goals;
        current_team->goal_difference += (edge->home_goals - edge->away_goals);

        if (edge->home_goals > edge->away_goals) {
          current_team->wins++;
          current_team->points += 3;
        } else if (edge->home_goals < edge->away_goals) {
          current_team->losses++;
        } else {
          current_team->draws++;
          current_team->points++;
        }
      }

      // Enqueue the opponent team if not visited
      if (!edge->opponent->visited) {
        queue[rear++] = edge->opponent;
        edge->opponent->visited = 1;
      }

      edge = edge->next;
    }
  }
}


// Function to print the league standings up to the specified round

void printStandings(TeamVertix* teams[], int num_teams, int round_number) {

  // Sort the teams array in descending order of points and goal difference (GD)

  for (int i = 0; i < num_teams - 1; i++) {

    for (int j = i + 1; j < num_teams; j++) {

      if (teams[i]->points < teams[j]->points ||  (teams[i]->points == teams[j]->points && teams[i]->goal_difference < teams[j]->goal_difference)) {

        // Swap the positions of the two teams in the array

        TeamVertix* temp = teams[i];

        teams[i] = teams[j];

        teams[j] = temp;

      }

    }

  }

  printf("%-5s %-20s %-9s %-5s %-5s %-5s %-5s %-5s %-5s %-7s\n", "Pos", "Team", "Matches", "W", "D", "L", "GF", "GA", "GD", "Points");

  for (int i = 0; i < num_teams; i++) {

    TeamVertix*team= teams[i];

    if (team->matches_played > 0 && team->matches_played <= round_number) {

      printf("%-5d %-20s %-9d %-5d %-5d %-5d %-5d %-5d %-5d %-7d\n", i + 1, team->name, team->matches_played, team->wins, team->draws, team->losses, team->goals_for, team->goals_against, team->goal_difference, team->points);

    }

  }

}





      int main() {
  FILE* file = fopen("epl_results.CSV", "r");
  if (file == NULL) {
    printf("Failed to open the file.\n");
    return 1;
  }

  TeamVertix* teams[MAX_TEAMS];
  int num_teams = 0;

  parseMatchResults(file, teams, &num_teams);

  fclose(file);

  int round_number;

  // Keep asking for the round number until a valid input is provided
while (1) {
  printf("Enter the round number (1-30), or enter -1 to exit:");

  // Variable to store the result of scanf
  int scanf_result = scanf("%d", &round_number);

  // Check if the user wants to exit
  if (round_number == -1) {
    printf("Exiting...\n");
    break;
  }

  // Validate the input
  if (scanf_result != 1) {
    printf("Invalid input. Please enter a valid round number.\n");

    // Clear the input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}

    continue;
  }

  // Validate the round number
  if (round_number < 1 || round_number > 30) {
    printf("Invalid round number. Please try again.\n");
    continue;
  }

  // Calculate the standings up to the specified round using BFS
  calculateStandingsBFS(teams, num_teams, round_number);

  // Print the standings up to the specified round
  printStandings(teams, num_teams, round_number);
  printf("\n");
}

  return 0;
}


