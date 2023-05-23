#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEAMS 20
#define MAX_MATCHES 600

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
      }}

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


