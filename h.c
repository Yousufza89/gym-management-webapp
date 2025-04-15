#include <stdio.h>
#include <stdlib.h> //to use filing functions
#include <string.h> //to use string manipulation fucntions
#include <ctype.h>  //to use tolower and toupper functions

// Constants
#define MAX_USERS 20               // could be changes here
#define MAX_QUIZZES 10             // could be changed here
#define MAX_QUESTIONS 10           // could be changed here
#define MAX_LEADERBOARD_ENTRIES 20 // could be changed here
#define TEACHER_CODE "chumantar"   // teacher code for additional security

// Structures
// users data (teacher and student) are stored here
struct LoginInfo
{
    char firstName[50];
    char lastName[50];
    char username[50];
    char password[20];
    char profession[10];
};

// questions
struct Question
{
    char question[500];
    char option1[50];
    char option2[50];
    char option3[50];
    char option4[50];
    char answer;
};

// quizes are stored here
struct Quiz
{
    char quizName[50];
    char teacherName[100];
    struct Question questions[MAX_QUESTIONS];
    int negativeMarking; // 0 for off and 1 for on
};

// leaderboard data is stored here
struct Leaderboard
{
    char name[100];
    char quizName[100];
    char quizMaker[100];
    int score;
};

// Global Variables
struct LoginInfo users[MAX_USERS];
struct Quiz quiz[MAX_QUIZZES];
struct Leaderboard leaderboard[MAX_LEADERBOARD_ENTRIES];
int userCounter = 0;       // it tracks the number of users till now
int currentUserIndex = -1; // -1 so it does not point to anything in the beginning // it tracks the current user we are on
int currentQuizIndex = 0;  // it tracks the current quiz we are on
int leaderboardCount = 0;  // it tracks the total number leaderboard rankings

// Utility Functions
// Clear the buffer before using fgets
void clearBuffer()
{
    while (getchar() != '\n')
        ;
}

// lowercasing the string so it can be comapred easily
void toLowerString(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// returns 1 when the option is validated else -1
int validateCorrectOption(char option)
{
    if (option < 'A' || option > 'D')
    {
        return -1;
    }
    return 1;
}

void DisplayBoxedMessage(const char *message)
{
    int len = strlen(message);
    printf("\n+");
    for (int i = 0; i < len + 6; i++)
        printf("=");
    printf("+\n");
    printf("|   %-*s   |\n", len, message);
    printf("+");
    for (int i = 0; i < len + 6; i++)
        printf("=");
    printf("+\n\n");
}

void DisplayWelcome()
{
    DisplayBoxedMessage("Welcome to the Enhanced Quiz Application");
    printf("Let's get started!\n");
}

// File Handling Funtions
//  Function to load data from the files into the structures
void loadUsersFromFile()
{
    FILE *file = fopen("users.txt", "r");
    if (file == NULL)
    {
        printf("Error: Could not open users.txt for reading.\n");
        return;
    }

    userCounter = 0;
    while (fscanf(file, "%[^|]|%[^|]|%[^|]|%[^|]|%s\n",
                  users[userCounter].firstName,
                  users[userCounter].lastName,
                  users[userCounter].username,
                  users[userCounter].password,
                  users[userCounter].profession) == 5)
    {
        toLowerString(users[userCounter].profession); // normalize profession to lowercase
        userCounter++;

        if (userCounter >= MAX_USERS)
        {
            printf("Maximum user limit reached while loading from file.\n");
            break;
        }
    }

    fclose(file);
    printf("Users loaded successfully from users.txt.\n");
}

void loadQuizzesFromFile()
{
    FILE *file = fopen("quiz.txt", "r");
    if (file == NULL)
    {
        printf("Error: Could not open quiz.txt for reading.\n");
        return;
    }

    currentQuizIndex = 0;
    while (fscanf(file, "%[^|]|%[^|]|", quiz[currentQuizIndex].quizName, quiz[currentQuizIndex].teacherName) == 2)
    {
        for (int i = 0; i < MAX_QUESTIONS; i++)
        {
            if (fscanf(file, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%c|",
                       quiz[currentQuizIndex].questions[i].question,
                       quiz[currentQuizIndex].questions[i].option1,
                       quiz[currentQuizIndex].questions[i].option2,
                       quiz[currentQuizIndex].questions[i].option3,
                       quiz[currentQuizIndex].questions[i].option4,
                       &quiz[currentQuizIndex].questions[i].answer) != 6)
            {
                break; // No more questions in this quiz
            }
        }

        // Read the negative marking flag
        fscanf(file, "%d\n", &quiz[currentQuizIndex].negativeMarking);

        currentQuizIndex++;
        if (currentQuizIndex >= MAX_QUIZZES)
        {
            printf("Maximum quiz limit reached while loading from file.\n");
            break;
        }
    }

    fclose(file);
    printf("Quizzes loaded successfully from quiz.txt.\n");
}

void loadLeaderboardFromFile()
{
    FILE *file = fopen("leaderboard.txt", "r");
    if (file == NULL)
    {
        printf("Error: Could not open leaderboard.txt for reading.\n");
        return;
    }

    leaderboardCount = 0; // Reset leaderboard count

    while (fscanf(file, "%[^|]|%[^|]|%[^|]|%d\n",
                  leaderboard[leaderboardCount].name,
                  leaderboard[leaderboardCount].quizName,
                  leaderboard[leaderboardCount].quizMaker,
                  &leaderboard[leaderboardCount].score) == 4)
    {
        leaderboardCount++;
        if (leaderboardCount >= MAX_LEADERBOARD_ENTRIES)
        {
            printf("Maximum leaderboard limit reached while loading from file.\n");
            break;
        }
    }

    fclose(file);
    printf("Leaderboard loaded successfully from leaderboard.txt.\n");
}

// Function to update leaderboard.txt with the current list of leaderboard entries
void updateLeaderboardFile()
{
    FILE *file = fopen("leaderboard.txt", "w");
    if (file == NULL)
    {
        printf("Error: Could not open leaderboard.txt for writing.\n");
        return;
    }

    for (int i = 0; i < leaderboardCount; i++)
    {
        fprintf(file, "%s|%s|%s|%d\n",
                leaderboard[i].name,
                leaderboard[i].quizName,
                leaderboard[i].quizMaker,
                leaderboard[i].score);
    }

    fclose(file);
    printf("Leaderboard file updated successfully.\n");
}

// Function to update users.txt with the current list of users
void updateUsersFile()
{
    FILE *file = fopen("users.txt", "w");
    if (file == NULL)
    {
        printf("Error: Could not open users.txt for writing.\n");
        return;
    }

    for (int i = 0; i < userCounter; i++)
    {
        fprintf(file, "%s|%s|%s|%s|%s\n",
                users[i].firstName,
                users[i].lastName,
                users[i].username,
                users[i].password,
                users[i].profession);
    }

    fclose(file);
    printf("Users file updated successfully.\n");
}

// Function to update quiz.txt with the current list of quizzes
void updateQuizzesFile()
{
    FILE *file = fopen("quiz.txt", "w");
    if (file == NULL)
    {
        printf("Error: Could not open quiz.txt for writing.\n");
        return;
    }

    for (int i = 0; i < currentQuizIndex; i++)
    {
        fprintf(file, "%s|%s|", quiz[i].quizName, quiz[i].teacherName);

        for (int j = 0; j < MAX_QUESTIONS; j++)
        {
            if (strlen(quiz[i].questions[j].question) == 0) // Skip empty questions
                break;

            fprintf(file, "%s|%s|%s|%s|%s|%c|",
                    quiz[i].questions[j].question,
                    quiz[i].questions[j].option1,
                    quiz[i].questions[j].option2,
                    quiz[i].questions[j].option3,
                    quiz[i].questions[j].option4,
                    quiz[i].questions[j].answer);
        }

        fprintf(file, "%d\n", quiz[i].negativeMarking);
    }

    fclose(file);
    printf("Quizzes file updated successfully.\n");
}

void printCertificate(int score, int wrongQuestions)
{
    // Ensure a user is logged in
    if (currentUserIndex == -1)
    {
        printf("Error: No user is currently logged in.\n");
        return;
    }

    // Generate the filename
    char fileName[100];
    sprintf(fileName, "Congratulations_%s_%s.txt", users[currentUserIndex].firstName, users[currentUserIndex].lastName);

    // Open the file for writing
    FILE *file = fopen(fileName, "w");
    if (file == NULL)
    {
        printf("Error: Could not create the file %s.\n", fileName);
        return;
    }

    // Write the congratulatory message
    fprintf(file, "Congratulations %s %s!\n\n", users[currentUserIndex].firstName, users[currentUserIndex].lastName);
    fprintf(file, "Your quiz results are as follows:\n");
    fprintf(file, "- Score before negative marking: %d\n", score);
    fprintf(file, "- Score after negative marking: %d\n", score - wrongQuestions);
    fprintf(file, "\nWell done and keep up the great work!\n");

    fclose(file);
    printf("Congratulatory file created: %s\n", fileName);
}

// Account Management
// Creating a User
void createUser()
{
    if (userCounter >= MAX_USERS)
    {
        printf("\n+---------------------------------------------+\n");
        printf("|              USER LIMIT REACHED            |\n");
        printf("+---------------------------------------------+\n");
        return;
    }

    printf("\n+---------------------------------------------+\n");
    printf("|               CREATING A USER               |\n");
    printf("+---------------------------------------------+\n");
    printf("| Please follow the steps below:              |\n");
    printf("+---------------------------------------------+\n");

    printf("| Step 1: Enter First Name                    |\n");
    printf("+---------------------------------------------+\n");
    printf("> ");
    scanf("%s", users[userCounter].firstName);

    printf("\n+---------------------------------------------+\n");
    printf("| Step 2: Enter Last Name                     |\n");
    printf("+---------------------------------------------+\n");
    printf("> ");
    scanf("%s", users[userCounter].lastName);

    printf("\n+---------------------------------------------+\n");
    printf("| Step 3: Enter Username                     |\n");
    printf("+---------------------------------------------+\n");
    printf("> ");
    scanf("%s", users[userCounter].username);

    printf("\n+---------------------------------------------+\n");
    printf("| Step 4: Enter Password                      |\n");
    printf("+---------------------------------------------+\n");
    printf("> ");
    scanf("%s", users[userCounter].password);

    printf("\n+---------------------------------------------+\n");
    printf("| Step 5: Enter Profession                    |\n");
    printf("| ('teacher' or 'student')                    |\n");
    printf("+---------------------------------------------+\n");
    printf("> ");
    scanf("%s", users[userCounter].profession);
    toLowerString(users[userCounter].profession);

    userCounter++;

    printf("\n+---------------------------------------------+\n");
    printf("|          USER CREATED SUCCESSFULLY!         |\n");
    printf("+---------------------------------------------+\n");

    updateUsersFile();
}

// Returns 1 is user is verified else 0
int verifyUser()
{
    char username[50], password[20];

    printf("\n+---------------------------------------------+\n");
    printf("|                  LOGIN                      |\n");
    printf("+---------------------------------------------+\n");
    printf("| Please enter your login details:            |\n");
    printf("+---------------------------------------------+\n");

    printf("| Step 1: Enter Username                      |\n");
    printf("+---------------------------------------------+\n");
    printf("> ");
    scanf("%s", username);

    printf("\n+---------------------------------------------+\n");
    printf("| Step 2: Enter Password                      |\n");
    printf("+---------------------------------------------+\n");
    printf("> ");
    scanf("%s", password);

    for (int i = 0; i < userCounter; i++)
    {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0)
        {
            printf("\n+---------------------------------------------+\n");
            printf("|         LOGIN SUCCESSFUL! WELCOME           |\n");
            printf("+---------------------------------------------+\n");

            currentUserIndex = i;
            return 1;
        }
    }

    printf("\n+---------------------------------------------+\n");
    printf("|          INVALID USERNAME OR PASSWORD       |\n");
    printf("+---------------------------------------------+\n");

    return 0;
}

// Quiz Management
// it checks whether the answer option is between the range of A and D
void createAnswer(int i)
{
    printf("Correct Answer (A-D): ");
    scanf(" %c", &quiz[currentQuizIndex].questions[i].answer);
    if (validateCorrectOption(quiz[currentQuizIndex].questions[i].answer) == -1)
    {
        printf("\nInvalid option\n");
        createAnswer(i);
    }
}

void createQuiz()
{
    if (currentQuizIndex >= MAX_QUIZZES)
    {
        DisplayBoxedMessage("\nQuiz limit reached!\n");
        return;
    }

    DisplayBoxedMessage("\nEnter the name of the quiz: ");

    clearBuffer();
    fgets(quiz[currentQuizIndex].quizName, sizeof(quiz[currentQuizIndex].quizName), stdin);
    quiz[currentQuizIndex].quizName[strcspn(quiz[currentQuizIndex].quizName, "\n")] = 0;

    strcat(quiz[currentQuizIndex].teacherName, users[currentUserIndex].firstName);
    strcat(quiz[currentQuizIndex].teacherName, " ");
    strcat(quiz[currentQuizIndex].teacherName, users[currentUserIndex].lastName);
    printf("\nEnable negative marking? (yes/no): ");
    char choice[10];
    scanf("%s", choice);
    toLowerString(choice);

    quiz[currentQuizIndex].negativeMarking = (strcmp(choice, "yes") == 0) ? 1 : 0;

    for (int i = 0; i < MAX_QUESTIONS; i++)
    {
        printf("\nCreating Question %d:\n", i + 1);
        printf("Enter the question: ");
        printf("\n---------------------------------\n");
        clearBuffer();
        fgets(quiz[currentQuizIndex].questions[i].question, sizeof(quiz[currentQuizIndex].questions[i].question), stdin);
        quiz[currentQuizIndex].questions[i].question[strcspn(quiz[currentQuizIndex].questions[i].question, "\n")] = '\0'; // Clears the newline nothing more

        printf("A): ");
        scanf("%s", quiz[currentQuizIndex].questions[i].option1);

        printf("B): ");
        scanf("%s", quiz[currentQuizIndex].questions[i].option2);
        printf("C): ");
        scanf("%s", quiz[currentQuizIndex].questions[i].option3);
        printf("D): ");
        scanf("%s", quiz[currentQuizIndex].questions[i].option4);
        createAnswer(i);
        quiz[currentQuizIndex].questions[i].answer = toupper((unsigned char)quiz[currentQuizIndex].questions[i].answer);
    }
    DisplayBoxedMessage("\nQuiz created successfully!\n");
    currentQuizIndex++;
    updateQuizzesFile();
}

void takeQuiz()
{
    int quizIndex, score = 0, skipped[MAX_QUESTIONS], skippedCount = 0, wrongQuestions = 0;
    char choice;

    if (currentQuizIndex == 0)
    {
        DisplayBoxedMessage("\nNo quizzes available..\n");
        return;
    }

    DisplayBoxedMessage("\nAvailable quizzes:\n");
    for (int i = 0; i < currentQuizIndex; i++)
    {
        printf(">%d. %s\n", i + 1, quiz[i].quizName);
    }

    printf("\nSelect a quiz (1-%d): ", currentQuizIndex);
    scanf("%d", &quizIndex);
    quizIndex--; // to Allign it with indexing number

    for (int i = 0; i < MAX_QUESTIONS; i++)
    {
        printf("\nQuestion %d: %s", i + 1, quiz[quizIndex].questions[i].question);
        printf("\nA. %s\nB. %s\nC. %s\nD. %s", quiz[quizIndex].questions[i].option1, quiz[quizIndex].questions[i].option2, quiz[quizIndex].questions[i].option3, quiz[quizIndex].questions[i].option4);
        DisplayBoxedMessage("\nEnter your answer (A-D) or 'S' to skip: ");
        scanf(" %c", &choice);
        choice = toupper((unsigned char)choice);

        if (choice == 'S')
        {
            skipped[skippedCount] = i;
            skippedCount++;
            continue;
        }

        if (choice == quiz[quizIndex].questions[i].answer)
        {
            score += 4;
        }
        else if (quiz[quizIndex].negativeMarking)
        {
            wrongQuestions++;
        }
    }

    if (skippedCount > 0)
    {
        printf("\nSkipped questions will now be asked.\n");
        for (int i = 0; i < skippedCount; i++)
        {
            int qIndex = skipped[i];
            printf("\nQuestion %d: %s", qIndex + 1, quiz[quizIndex].questions[qIndex].question);
            printf("\nA. %s\nB. %s\nC. %s\nD. %s", quiz[quizIndex].questions[qIndex].option1, quiz[quizIndex].questions[qIndex].option2, quiz[quizIndex].questions[qIndex].option3, quiz[quizIndex].questions[qIndex].option4);
            printf("\nEnter your answer (A-D): ");
            scanf(" %c", &choice);
            choice = toupper((unsigned char)choice);

            if (choice == quiz[quizIndex].questions[qIndex].answer)
            {
                score += 4;
            }
            else if (quiz[quizIndex].negativeMarking)
            {
                wrongQuestions++;
            }
        }
        char haveACertificate[3];
        DisplayBoxedMessage("\nDo you want to have a certificate (yes or no): ");
        scanf("%s", haveACertificate);
        toLowerString(haveACertificate);
        if (strcmp(haveACertificate, "yes") == 0)
        {
            printCertificate(score, wrongQuestions);
        }
    }

    score = score < 0 ? 0 : score;

    printf("\n+---------------------------------------+\n");
    printf("|Your score before negative marking: %d   |\n", score);
    printf("|Total Negative Marking: %d               |\n", wrongQuestions);
    printf("|Your score after negative marking: %d    |\n", score - wrongQuestions);
    printf("+----------------------------------------+\n");

    strcpy(leaderboard[leaderboardCount].name, users[currentUserIndex].firstName);
    leaderboard[leaderboardCount].score = score - wrongQuestions;
    strcpy(leaderboard[leaderboardCount].quizMaker, quiz[quizIndex].teacherName);
    strcpy(leaderboard[leaderboardCount].quizName, quiz[quizIndex].quizName);
    leaderboardCount++;
    updateLeaderboardFile();
}

void sortLeaderboard()
{
    for (int i = 0; i < leaderboardCount - 1; i++)
    {
        for (int j = 0; j < leaderboardCount - i - 1; j++)
        {
            if (leaderboard[j].score < leaderboard[j + 1].score)
            {
                struct Leaderboard temp = leaderboard[j];
                leaderboard[j] = leaderboard[j + 1];
                leaderboard[j + 1] = temp;
            }
        }
    }
}

void displayLeaderboard(int choice)
{
    // Sort the leaderboard by score
    sortLeaderboard();

    switch (choice)
    {
    case 1:
        printf("\nEnter the subject you want to filter by: ");
        char subject[10];
        scanf("%s", subject);

        printf("\n\n\t\t\tLEADERBOARD\n");
        printf("Rank\tName\tSubject\t\tTeacher\t\tScore\n");

        int flag = 0; // flag to track if the leaderboard ha not even a single entry of that subject
        for (int i = 0; i < leaderboardCount; i++)
        {
            if (strcmp(subject, leaderboard[i].quizName) == 0)
            {
                printf("%d\t%s\t%s\t\t%s\t\t%d\n", i + 1, leaderboard[i].name, leaderboard[i].quizName, leaderboard[i].quizMaker, leaderboard[i].score);
                flag++;
            }
        }

        if (flag == 0)
        {
            printf("\nNo matches found.");
        }

        break;

    case 2:

        DisplayBoxedMessage("\n\n\t\t\tLEADERBOARD\n");
        DisplayBoxedMessage("Rank\tName\tSubject\t\tTeacher\t\tScore\n");

        for (int i = 0; i < leaderboardCount; i++)
        {

            printf("%d\t%s\t%s\t\t%s\t\t%d\n", i + 1, leaderboard[i].name, leaderboard[i].quizName, leaderboard[i].quizMaker, leaderboard[i].score);
        }
        break;

    default:
        printf("\nInvalid choice.\n\n\n");
        clearBuffer();
    }
}

void EditQuiz()
{
    int quizIndex, questionIndex;

    printf("\nEnter the quiz number you want to edit (1 - %d): ", currentQuizIndex);
    scanf("%d", &quizIndex);
    quizIndex--; // Adjust to zero-based index

    if (quizIndex < 0 || quizIndex >= currentQuizIndex)
    {
        printf("Invalid quiz number.\n");
        return;
    }

    printf("\nEditing Quiz: %s\n", quiz[quizIndex].quizName);

    // Ask for the question to edit
    while (1)
    {
        DisplayBoxedMessage("\nEnter the question number to edit (1 - 10)(Press -1 to exit): ");
        scanf("%d", &questionIndex);
        if (questionIndex == -1)
        {
            break;
        }
        questionIndex--; // Adjust to zero-based index

        if (questionIndex < 0 || questionIndex >= 10)
        {
            printf("Invalid question number. Please enter a number between 1 and 10.\n");
            return;
        }

        printf("\n+----------------------+\n");
        printf("|Editing Question %d:  |\n", questionIndex + 1);
        printf("+----------------------+\n");

        // Edit the selected question
        printf("Current Question: %s\n", quiz[quizIndex].questions[questionIndex].question);
        printf("Enter new question: ");
        scanf(" %[^\n]", quiz[quizIndex].questions[questionIndex].question);

        // Edit the options
        printf("Current Option 1: %s\n", quiz[quizIndex].questions[questionIndex].option1);
        printf("Enter new Option 1: ");
        scanf(" %[^\n]", quiz[quizIndex].questions[questionIndex].option1);

        printf("Current Option 2: %s\n", quiz[quizIndex].questions[questionIndex].option2);
        printf("Enter new Option 2: ");
        scanf(" %[^\n]", quiz[quizIndex].questions[questionIndex].option2);

        printf("Current Option 3: %s\n", quiz[quizIndex].questions[questionIndex].option3);
        printf("Enter new Option 3: ");
        scanf(" %[^\n]", quiz[quizIndex].questions[questionIndex].option3);

        printf("Current Option 4: %s\n", quiz[quizIndex].questions[questionIndex].option4);
        printf("Enter new Option 4: ");
        scanf(" %[^\n]", quiz[quizIndex].questions[questionIndex].option4);

        // Edit the correct answer
        printf("Current Answer: %c\n", quiz[quizIndex].questions[questionIndex].answer);
        printf("Enter new correct answer (A, B, C, D): ");
        scanf(" %c", &quiz[quizIndex].questions[questionIndex].answer);

        printf("\nQuestion %d has been successfully edited.\n", questionIndex + 1);

        // Save changes
        updateQuizzesFile();
    }
}

void teacherMenu()
{
    char teacherCode[20];
    printf("\nEnter the teacher code: ");
    scanf("%s", teacherCode);

    int teacherChoice = 0;
    if (strcmp(teacherCode, TEACHER_CODE) == 0)
    {
        while (teacherChoice != 4)
        {
            int leaderboardChoice;
            printf("\n");
            printf("+======================================+\n");
            printf("|               TEACHER PORTAL         |\n");
            printf("+======================================+\n");
            printf("| [1] Create Quiz                      |\n");
            printf("| [2] Edit Quiz                        |\n");
            printf("| [3] View Leaderboard                 |\n");
            printf("| [4] Exit                             |\n");
            printf("+======================================+\n");
            printf("\nEnter your choice here: ");
            scanf("%d", &teacherChoice);

            switch (teacherChoice)
            {
            case 1:
                createQuiz();
                break;
            case 2:
                EditQuiz();
                break;
            case 3:
                printf("\nPress 1 to filter the leaderboard by subject.\nPress 2 to view all the rankings.\n");
                scanf("%d", &leaderboardChoice);

                displayLeaderboard(leaderboardChoice);
                break;
            case 4:
                break;
            default:
                printf("\nInvalid choice.\n");
            }
        }
    }
    else
    {
        printf("\n+---------------------------------+\n");
        printf("|Invalid Teacher's code...        |\n");
        printf("+---------------------------------+\n");
    }
}

void studentMenu()
{
    int studentChoice = 0;
    while (studentChoice != 3)
    {
        int leaderboardChoice;

        printf("\n");
        printf("+======================================+\n");
        printf("|            STUDENT PORTAL            |\n");
        printf("+======================================+\n");
        printf("| [1] Take Quiz                        |\n");
        printf("| [2] View Leaderboard                 |\n");
        printf("| [3] Exit                             |\n");
        printf("+======================================+\n");
        printf("\nEnter your choice here: ");
        scanf("%d", &studentChoice);

        switch (studentChoice)
        {
        case 1:
            takeQuiz();
            break;
        case 2:
            printf("\nPress 1 to filter the leaderboard by subject.\nPress 2 to view all the rankings.\nEnter you choice: ");
            scanf("%d", &leaderboardChoice);
            displayLeaderboard(leaderboardChoice);
            break;
        case 3:
            break;
        default:
            printf("\nInvalid choice.\n");
        }
    }
}

// Main Function
int main()
{
    loadUsersFromFile();
    loadQuizzesFromFile();
    loadLeaderboardFromFile();

    int choice;
    DisplayWelcome();
    while (1)
    {
        printf("\n");
        printf("+======================================+\n");
        printf("|               MAIN MENU              |\n");
        printf("+======================================+\n");
        printf("| [1] Create Account                   |\n");
        printf("| [2] Login                            |\n");
        printf("| [3] Exit                             |\n");
        printf("+======================================+\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createUser();
            break;
        case 2:
            if (verifyUser())
            {
                if (strcmp(users[currentUserIndex].profession, "teacher") == 0)
                {
                    printf("\n+---------------------------------+\n");
                    printf("|Welcome %s.                       |\n", users[currentUserIndex].firstName);
                    printf("+---------------------------------+\n");
                    teacherMenu();
                }
                else
                {
                    printf("\n+---------------------------------+\n");
                    printf("|Welcome %s.                       |\n", users[currentUserIndex].firstName);
                    printf("+---------------------------------+\n");
                    studentMenu();
                }
            }
            else
            {
                printf("\n+---------------------------------+\n");
                printf("|Invalid login credentials...      |\n");
                printf("+---------------------------------+\n");
            }
            break;
        case 3:
            printf("\nExiting program.\n");
            DisplayBoxedMessage("Thank you for using the Quiz Application. Goodbye!");
            exit(0); // it ends the program
        default:
            printf("\nInvalid choice.\n");
        }
    }

    return 0;
}