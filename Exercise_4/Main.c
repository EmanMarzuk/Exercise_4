#define _CRT_SECURE_NO_WARNINGS
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/ //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct StudentCourseGrade
{
    char courseName[35];
    int grade;
}StudentCourseGrade;

typedef struct Student
{
    char name[35];
    StudentCourseGrade* grades; //dynamic array of courses
    int numberOfCourses;
}Student;


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

int main()
{
    //Part A
    int* coursesPerStudent = NULL;
    int numberOfStudents = 0;
    char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
    factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
    printStudentArray(students, coursesPerStudent, numberOfStudents);
    //studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)

    //Part B
    Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
    writeToBinFile("students.bin", transformedStudents, numberOfStudents);
    Student* testReadStudents = readFromBinFile("students.bin");


    /*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
    // Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

    return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
    FILE* fp = fopen(fileName, "r");
    int lines = 0;
    char str[1023];

    // Counting number of line in the file which is number of students
    while (fgets(str, 1023, fp) != NULL) {
        (*numberOfStudents)++;
    }

    rewind(fp);

    // Allocation an array for courses of students
    *coursesPerStudent = (int*)malloc(*numberOfStudents);
    if (coursesPerStudent == NULL) {
        printf("Allocation failed!");
        return;
    }
    // Counting number of courses per student and updating the suitable array    
    int numofcou = 0;
    for (int i = 0; i < *numberOfStudents; i++) {
        fgets(str, 1023, fp);
        numofcou = countPipes(str, strlen(str));
        //*coursesPerStudent = countPipes(str, strlen(str));
       // (*coursesPerStudent)++;
        *(*coursesPerStudent) = numofcou;
        (*coursesPerStudent)++;
    }
    rewind(fp);
    fclose(fp);
}

int countPipes(const char* lineBuffer, int maxCount)
{
    if (maxCount <= 0)
        return 0;

    if (lineBuffer == NULL)
        return -1;

    int num = 0, count = 0, i = 0;
    while (*lineBuffer != '\0' && num < maxCount) {
        if (*lineBuffer == '|') {
            count++;
        }
        lineBuffer++;
        num++;
        i++;
    }
    return count;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
    char str[1023], st[30], token_c[30] = { '\0' };
    int j = 0, i = 0, cnt = 0, n = 0, stuNum = 0;

    int* temp = (int*)malloc(sizeof(int));
    char* token_p;

    countStudentsAndCourses(fileName, &temp, &stuNum);


    FILE* fp = fopen(fileName, "r");


    // Counting number of line in the file which is number of students
    while (fgets(str, 1023, fp) != NULL) {
        (*numberOfStudents)++;
    }

    // Allocation for the file to array
    char*** students = (char***)malloc(*numberOfStudents * sizeof(char));
    if (students == NULL) {
        printf("Allocation failed!");
        return;
    }

    // Allocation the course per student array 
    char** PerStudent = (char**)malloc(sizeof(char));
    rewind(fp);

    while (fgets(str, 1023, fp) != NULL) {
        if (PerStudent == NULL) {
            printf("Allocation failed!");
            return;
        }

        token_p = strtok(str, "|");         //get the first token 
        *PerStudent = token_p;
        token_p = strtok(NULL, "|");

        while (token_p != NULL) {          // walk through other tokens 
            strcpy(st, token_p);
            while (n <= strlen(st)) {
                if (st[j] != ',' && st[j] != '\0') {
                    token_c[i] = st[j];
                    i++;
                }
                else {
                    token_c[i] = '\0';
                    (PerStudent)++;
                    *PerStudent = token_c;
                    i = 0;
                }
                j++;
                n++;
            }
            j = 0;
            i = 0;
            n = 0;
            token_p = strtok(NULL, "|");
        }
        *(students) = PerStudent;
        PerStudent++;
        students++;
        coursesPerStudent++;
    }

    coursesPerStudent = temp;
    numberOfStudents = stuNum;

    return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
    if (factor < -20 || factor > 20) {
        printf("Invalid factor grade!");
        return;
    }

    int grade = 0;
    char** stud = (char**)malloc(sizeof(char));  // pointer for each student (**students)
    if (stud == NULL) {
        printf("Allocation failed!");
        return;
    }
    char* course = (char**)malloc(sizeof(char)); // pointer to the information of each student (***students)
    if (course == NULL) {
        printf("Allocation failed!");
        return;
    }

    stud = **students;
    for (int i = 0; i < numberOfStudents; i++) {
        course = *stud;
        for (int j = 0; j < (*coursesPerStudent * 2 + 1); j++) {
            if (strcmp(courseName, *course) == 0) {
                grade = atoi(*(course + 1)) + factor;
                if (grade > 100) grade = 100;
                if (grade < 0) grade = 0;
                _itoa(grade, *(course + 1), 10);
                break;
            }
            course++;
        }
        coursesPerStudent++;
        stud++;
    }
}

void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
    for (int i = 0; i < numberOfStudents; i++)
    {
        printf("name: %s\n*********\n", students[i][0]);
        for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
        {
            printf("course: %s\n", students[i][j]);
            printf("grade: %s\n", students[i][j + 1]);
        }
        printf("\n");
    }
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
    char str[1023];
    int check;
    FILE* fp = fopen("studentList.txt", "w");

    char** stud = (char**)malloc(sizeof(char)); // pointer for each student (**students)
    if (stud == NULL) {
        printf("Allocation failed!");
        return;
    }
    char* course = (char**)malloc(sizeof(char)); // pointer to the information of each student (***students)
    if (course == NULL) {
        printf("Allocation failed!");
        return;
    }
    stud = **students;

    for (int j = 0; j < numberOfStudents; j++) {
        course = *stud;
        strcat(str, *course);
        strcat(str, "|");
        course++;
        for (int i = 0; i < *coursesPerStudent; i++) {
            strcat(str, *course);
            strcat(str, ",");
            course++;
            strcat(str, *course);
            strcat(str, "|");
            course++;
        }
        check = fputs(str, fp);
        if (check <= 0) {
            printf("Writing failed!");
            return;
        }
        fputs("\n", fp);
        strcpy(str, "");
        coursesPerStudent++;
        stud++;
    }

    free(stud);
    free(course);
    fclose(fp);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
    char str[1023];

    _itoa(numberOfStudents, str, 10);
    fwrite(&str, 39, 1, fileName);
    strcpy(str, "");

    for (int i = 0; i < numberOfStudents; i++) {
        strcat(str, *students->name);
        strcat(str, " ");
        strcat(str, students->numberOfCourses);
        strcat(str, " ");
        for (int i = 0; i < students->numberOfCourses; i++) {
            strcat(str, students->grades->courseName);
            strcat(str, " ");
            students->grades++;
        }
        students++;
        fwrite(&str, 39, 1, fileName);
        strcpy(str, "");
    }
    fclose(fileName);
}

Student* readFromBinFile(const char* fileName)
{
    FILE* fp = fopen(fileName, "rb");
    char* str = (char*)malloc(sizeof(char));
    if (str == NULL) {
        printf("Allocation failed!");
        return;
    }
    char* token = (char*)malloc(sizeof(char));
    if (token == NULL) {
        printf("Allocation failed!");
        return;
    }
    fread(&str, sizeof(Student), 1, fp);
    int numStu = *str;
    Student* binStudents = (Student*)malloc(numStu * sizeof(Student));
    if (binStudents == NULL) {
        printf("Allocation failed!");
        return;
    }
    Student* temp = (Student*)malloc(numStu * sizeof(Student));
    if (temp == NULL) {
        printf("Allocation failed!");
        return;
    }
    char** strArr = (char**)malloc(sizeof(char));
    if (strArr == NULL) {
        printf("Allocation failed!");
        return;
    }
    token = strtok(str, " ");
    *strArr = token;
    strArr++;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        *strArr = token;
        strArr++;
    }

    while (*strArr != NULL) {
        strcpy(temp->name, *strArr);
        strArr++;
        temp->numberOfCourses = atoi(*strArr);
        strArr++;
        for (int i = 0; i < temp->numberOfCourses; i++) {
            strcpy(temp->grades->courseName, *strArr);
            strArr++;
        }
        strArr++;
    }
    fclose(fp);
    return binStudents;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
    char** stud = (char**)malloc(sizeof(char));  // pointer for each student (**students)
    if (stud == NULL) {
        printf("Allocation failed!");
        return;
    }
    char* course = (char**)malloc(sizeof(char));  // pointer to the information of each student (***students)
    if (course == NULL) {
        printf("Allocation failed!");
        return;
    }

    Student* studentsArray = (Student*)malloc(numberOfStudents * sizeof(Student));  // pointer for the students array (Output)
    if (studentsArray == NULL) {
        printf("Allocation failed!");
        return;
    }

    Student* temp = (Student*)malloc(numberOfStudents * sizeof(Student));  // pointer for the students array (Output)
    if (temp == NULL) {
        printf("Allocation failed!");
        return;
    }

    temp = studentsArray;

    stud = **students;

    for (int i = 0; i < numberOfStudents; i++) {
        course = *stud;
        temp->numberOfCourses = *coursesPerStudent;
        strcpy(temp->name, *course);
        course++;
        for (int j = 0; j < *coursesPerStudent; j++) {
            strcpy(temp->grades->courseName, *course);
            course++;
            temp->grades->grade = atoi(*course);
            course++;
        }
        coursesPerStudent++;
        stud++;
        temp++;
    }
    return studentsArray;
}
