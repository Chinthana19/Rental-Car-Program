/*
 *   Author: Chinthana Sembakutti
 *   Date: October 22, 2022
 *   ID: 1177250
 *   Assignment 2 Question 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defining constants
#define DATA_FILE_NAME "cars.txt"
#define TOTAL_INCOME "total_income"
#define AVAILABLE_LIST_NAME "available"
#define RENTED_LIST_NAME "rented"
#define REPAIR_LIST_NAME "repair"

/*
 * The format of the datafile
 * total_income:<income>
 * available:<plate>,<mileage>,<return-date>
 * rented:<plate>,<mileage>,<return-date>
 * repair:<plate>,<mileage>,<return-date>
 */

// list contents
struct Car
{
    char plate[32];
    int mileage;
    int returnDate;
    struct Car *next;
};

// setting heads to null until information can be set
struct Car *available_head = NULL;
struct Car *rented_head = NULL;
struct Car *atWkshop_head = NULL;

// setting total income to 0 initially
float total_income = 0.0;

enum Codes
{
    INVALID = 0,
    ADD_NEW_AVAILABLE = 1,
    ADD_RETURN_AVAILABLE = 2,
    ADD_RETURN_REPAIR = 3,
    TRANSFER_REPAIR_TO_AVAILABLE = 4,
    RENT_FIRST_AVAILABLE = 5,
    PRINT_LIST = 6,
    QUIT = 7
};

// frees memory of each list
void freeMemory(struct Car *ptr)
{
    while (ptr != NULL)
    {
        struct Car *tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
}

void print_details(struct Car *car)
{
    printf("\tPlate Number: %s\n", car->plate);
    printf("\tMileage: %d\n", car->mileage);
    printf("\tReturn Date: %06d\n\n", car->returnDate);
}

// inserts car to the available list
void insertToAvailable(struct Car *car)
{
    if (available_head == NULL)
    {
        // in case its the first element in list
        available_head = car;
        return;
    }
    struct Car *temp = available_head;
    if (car->mileage < temp->mileage) // if the mileage is less, car will be added here
    {
        car->next = temp;
        available_head = car;
    }
    else
    {
        int inserted = 0;
        while (temp->next != NULL)
        {
            // will continue through the list until a car with greater mileage is found
            if (car->mileage > temp->next->mileage)
            {
                temp = temp->next;
                continue;
            }
            else
            {
                car->next = temp->next;
                temp->next = car;
                inserted = 1;
                break;
            }
        }
        // if the end of list is reached, the car will be inserted at the end
        if (!inserted)
        {
            temp->next = car;
        }
    }
}

int getCarPlateNumber(char *plate_number)
{
    printf("Enter plate number: ");

    char line[1024];
    while (fgets(line, sizeof(line), stdin) != NULL)
    {
        if (strlen(line) < 2)
        {
            // to avoid error of random symbols being read
            continue;
        }
        break;
    }
    char *token = &line[0];
    // splitting line at the end of line, storing as plate_str
    char *plate_str = strtok_r(token, "\n", &token);
    strcpy(plate_number, plate_str);
    return 0;
}

int getCarMileage()
{
    int mileage;
    printf("Enter mileage: ");
    if (scanf("%d", &mileage) != 1)
    {
        printf("Invalid Input\n");
        return -1;
    }
    return mileage;
}

// adding new car to available list
void addNewAvailable()
{
    char plateNum[32];
    if (getCarPlateNumber(plateNum) < 0)
    {
        // Invalid input
        return;
    }
    int mileage = getCarMileage();
    if (mileage < 0)
    {
        // Invalid input
        return;
    }
    // creating structure, copying information
    struct Car *car = (struct Car *)malloc(sizeof(struct Car));
    memset(car, 0, sizeof(struct Car));
    car->mileage = mileage;
    strcpy(car->plate, plateNum);
    insertToAvailable(car);
    printf("A new car is added to available list:\n");
    print_details(car);
}

// searches through repair list for car with specified plate number
struct Car *retrieveFromRepairList(char *plateNum)
{
    // If repair list is empty
    if (atWkshop_head == NULL)
    {
        return NULL;
    }
    struct Car *car = NULL;
    struct Car *temp = atWkshop_head;
    // If its the first in the list, makes following element the head
    if (strcmp(plateNum, temp->plate) == 0)
    {
        car = atWkshop_head;
        atWkshop_head = atWkshop_head->next;
        car->next = NULL;
        return car;
    }

    // Traverse and find from the list
    while (temp->next != NULL)
    {
        if (strcmp(plateNum, temp->next->plate) == 0)
        {
            car = temp->next;
            temp->next = temp->next->next;
            car->next = NULL;
            break;
        }
        else
        {
            temp = temp->next;
        }
    }
    return car;
}

// searches through rented list for car with specified plate number
struct Car *retrieveFromRentedList(char *plateNum)
{
    // If rented list is empty
    if (rented_head == NULL)
    {
        return NULL;
    }
    struct Car *car = NULL;
    struct Car *temp = rented_head;
    // If its the first element, second element of the list will become
    if (strcmp(plateNum, temp->plate) == 0)
    {
        car = rented_head;
        rented_head = rented_head->next;
        car->next = NULL;
        return car;
    }

    // Traverse and find from the list, previous element will point to next
    while (temp->next != NULL)
    {
        if (strcmp(plateNum, temp->next->plate) == 0)
        {
            car = temp->next;
            temp->next = temp->next->next;
            car->next = NULL; // rest set to null
            break;
        }
        else
        {
            temp = temp->next;
        }
    }
    return car;
}

// calculates income given distance driven
float calculateIncome(int mileage)
{
    float income = 80.0f;
    if (mileage > 200)
    {
        float excess_mileage = (float)(mileage - 200);
        income += excess_mileage * 0.15;
    }
    return income;
}

// adds returned car to available list
void addReturnToAvailable()
{
    char plateNum[32];
    if (getCarPlateNumber(plateNum) < 0)
    {
        // Invalid input
        return;
    }
    // Retrieve the car from rented list
    struct Car *car = retrieveFromRentedList(plateNum);
    if (car == NULL)
    {
        printf("\nError: plate '%s' is not found in the rented list\n\n", plateNum);
        return;
    }
    // gets new mileage
    int mileage = getCarMileage();
    if (mileage < 0)
    {
        // Invalid input
        return;
    }
    // Find the income from car
    if (mileage < car->mileage)
    {
        printf("Error: Lesser mileage is given than when rented!");
        return;
    }
    int mileage_used = mileage - car->mileage;
    float income = calculateIncome(mileage_used);
    // adds to total income
    total_income += income;
    // Set the next mileage
    car->mileage = mileage;
    // Add it to available list
    printf("Income for this rental is: %.2f\n", income);
    printf("Tranferring the car from rental list to available list:\n");
    print_details(car);
    insertToAvailable(car);
}

// adds car to repair list
void insertToRepair(struct Car *car)
{
    // If its the first element in list
    if (atWkshop_head == NULL)
    {
        atWkshop_head = car;
        return;
    }

    struct Car *temp = atWkshop_head;
    // adds car to the repair list (order not considered)
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = car;
}

// Adds a returned car to the repair list
void addReturnToRepair()
{
    char plateNum[32];
    if (getCarPlateNumber(plateNum) != 0)
    {
        // Invalid input
        return;
    }
    int mileage = getCarMileage();
    if (mileage < 0)
    {
        // Invalid input
        return;
    }
    // Retrieve the car from rented list
    struct Car *car = retrieveFromRentedList(plateNum);
    if (car == NULL)
    {
        printf("\nError: plate '%s' is not found in the rented list\n\n", plateNum);
        return;
    }
    // calculating distance traveled
    int mileage_used = mileage - car->mileage;

    // uses distance traveled to calculate income from car
    float income = calculateIncome(mileage_used);
    // adds income from car to total
    total_income += income;

    // setting new mileage
    car->mileage = mileage;

    // Add the car to the repair list
    insertToRepair(car);
    printf("Income for this rental is: %.2f\n", income);
    printf("Transferring the car from rented list to repair list:\n");
    print_details(car);
}

// move car from repair list to available list
void transferFromRepairToAvailable()
{
    char plateNum[32];
    if (getCarPlateNumber(plateNum) != 0)
    {
        // Invalid input
        return;
    }
    // retrieves car from repair list (given plate num)
    struct Car *car = retrieveFromRepairList(plateNum);
    if (car == NULL)
    {
        printf("Cannot find a car with plate '%s' from the repair list\n\n", plateNum);
        return;
    }
    // adds to available list (sorting done in insertToAvailable function)
    insertToAvailable(car);
    printf("Transferring the car from repair list to available list:\n");
    print_details(car);
}

// gets first available car from available list (the head)
struct Car *getFirstAvailable()
{
    struct Car *first_available = NULL;
    // makes first_avaible the first element of available list, moves head to next element
    if (available_head != NULL)
    {
        first_available = available_head;
        available_head = available_head->next;
        first_available->next = NULL;
    };
    return first_available;
}

// adds car to rental list
void addToRentalList(struct Car *car)
{
    // if rental list is empty
    if (rented_head == NULL)
    {
        rented_head = car;
        return;
    }
    struct Car *temp = rented_head;

    // if the return date is earlier than first element of rental list, it will become the head
    if (car->returnDate < temp->returnDate)
    {
        car->next = temp;
        rented_head = car;
    }
    else
    {
        int inserted = 0;
        // traverses through list until car with earlier return date is found, and added after
        while (temp->next != NULL)
        {
            if (car->returnDate > temp->next->returnDate)
            {
                temp = temp->next;
                continue;
            }
            else
            {
                car->next = temp->next;
                temp->next = car;
                inserted = 1;
                break;
            }
        }
        // if end of list is reached (rented list = NULL), car will be added to end
        if (!inserted)
        {
            temp->next = car;
        }
    }
}

// rents the first available car (the head of available list). Sets second element
// to head
void rentFirstAvailable()
{
    // if avaible list is empty
    if (available_head == NULL)
    {
        printf("No cars available\n");
        return;
    }
    // user input for return date
    printf("Expected return date: ");
    int return_date;
    if (scanf("%d", &return_date) != 1)
    {
        printf("Invalid Input\n");
        return;
    }
    // moves car to rented list, sets return date
    struct Car *first_available = getFirstAvailable();
    if (first_available != NULL)
    {
        first_available->returnDate = return_date;
        addToRentalList(first_available);
        printf("Transferring car from available list to rental list:\n");
        print_details(first_available);
    }
}

// prints information given list input (uses print_details function)
void printList(struct Car *list)
{
    while (list != NULL)
    {
        print_details(list);
        list = list->next;
    }
}

// prints information
void printLists()
{
    if (available_head != NULL)
    {
        printf("Available Cars List:\n");
        printf("======================================\n");
        printList(available_head);
        printf("======================================\n\n");
    }
    else
    {
        printf("No available cars\n\n");
    }
    if (rented_head != NULL)
    {
        printf("Rented Cars List: \n");
        printf("======================================\n");
        printList(rented_head);
        printf("======================================\n\n");
    }
    else
    {
        printf("No rented cars\n\n");
    }
    if (atWkshop_head != NULL)
    {
        printf("Repair Shop List: \n");
        printf("======================================\n");
        printList(atWkshop_head);
        printf("======================================\n\n");
    }
    else
    {
        printf("No cars to be repaired\n\n");
    }
}

// uses information from file to fill the 3 lists
int insertToLists(char *list_type, char *plate, int mileage, int return_date)
{
    struct Car *car = (struct Car *)malloc(sizeof(struct Car));
    // uses file information to fill mileage, return date, plate and list type information
    car->mileage = mileage;
    strcpy(car->plate, plate);
    car->returnDate = return_date;
    car->next = NULL;
    // for car to be added to available list
    if (strcmp(list_type, AVAILABLE_LIST_NAME) == 0)
    {
        insertToAvailable(car);
        return 0;
    }
    // for car to be added to rented list
    if (strcmp(list_type, RENTED_LIST_NAME) == 0)
    {
        addToRentalList(car);
        return 0;
    }
    // for car to be added to repair list
    if (strcmp(list_type, REPAIR_LIST_NAME) == 0)
    {
        insertToRepair(car);
        return 0;
    }
    // in case of error, returns -1. else returns 0
    return -1;
}

// reads from input file and calls function to fill lists given file information
int readFromFile()
{
    // opens file named 'cars.txt'
    FILE *fp = fopen(DATA_FILE_NAME, "r");
    if (fp == NULL)
    {
        // in case of error opening file
        printf("File '%s' cannot be opened.\n", DATA_FILE_NAME);
        return -1;
    }

    char line[1024];
    // flag to ensure that total income (top of file) is read
    int total_income_read_flag = 0;
    // flag to ensure that process is successful
    int success = 0;
    //
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        success = 0;
        // reading 1 line at a time from input file. using tokens to split string
        char *token = &line[0];
        // for the first line of the file (previous income). will only perform once,
        // then will be skipped for the rest of the file reading
        if (total_income_read_flag == 0)
        {
            // splits string at :
            char *total_income_token = strtok_r(token, ":", &token);
            if (total_income_token == NULL)
            {
                // in case of error
                printf("Invalid input from file '%s'\n", line);
                break;
            }
            // ensures that 'total_income' is read from the file. Error if not
            if (strcmp(total_income_token, TOTAL_INCOME) != 0)
            {
                printf("Invalid input from file '%s'\n", DATA_FILE_NAME);
                break;
            }
            // reads the value after the ':' and before the '\n' which will be the income from
            // previous session.
            char *total_income_str = strtok_r(token, "\n", &token);
            if (total_income_str == NULL)
            {
                printf("Invalid input from file '%s'\n", line);
                break;
            }
            // turns total income from data file to a float value, stores to
            // static variable which can be accessed anywhere
            total_income = atof(total_income_str);
            // flag becomes 1, so getting income from previous session process will not repeat
            total_income_read_flag = 1;
            // success = 1 if successful, 0 if not
            success = 1;
            continue;
        }

        // loop will run through every line of the data file and store data into required lists
        int mileage;
        int return_date;
        if (strlen(line) < 2)
        {
            success = 1;
            continue;
        }
        // reads first part of each line (list type). splits at the ':'
        char *list_type = strtok_r(token, ":", &token);
        if (list_type == NULL)
        {
            printf("Invalid input from file '%s'\n", line);
            break;
        }
        // reads plate number from data file
        char *plate = strtok_r(token, ",", &token);
        if (plate == NULL)
        {
            printf("Invalid input from file '%s'\n", line);
            break;
        }
        // reads mileage from data file
        char *mileage_str = strtok_r(token, ",", &token);
        if (mileage_str == NULL)
        {
            printf("Invalid input from file '%s'\n", line);
            break;
        }
        // turns string for mileage into integer
        mileage = atoi(mileage_str);
        // reads return date from data file
        char *return_date_str = strtok_r(token, "\n", &token);
        if (return_date_str == NULL)
        {
            printf("Invalid input from file '%s'\n", line);
            break;
        }
        // turns return date string into integer value
        return_date = atoi(return_date_str);
        // runs insertToLists function, which uses data from 1 line to
        // create car, and store it in specified list.
        if (insertToLists(list_type, plate, mileage, return_date) < 0)
        {
            printf("Invalid input from file '%s'\n", line);
            break;
        }
        // success flag = 1, means operation is successful and data file can
        // be safely closed
        success = 1;
    }

    // closes file
    if (fp)
    {
        fclose(fp);
    }
    if (success)
    {
        return 0;
    }
    return -1;
}

// takes a list input and writes the contents to file
void writeListToFile(struct Car *list, char *listname, FILE *fp)
{
    // ensures list isn't empty
    if (list != NULL)
    {
        char buffer[1024];
        struct Car *temp = list;
        while (temp != NULL)
        {
            // stores data from 1 car in buffer (list,plate,mileage,return date)
            // then prints it into the file. Then moves onto the next car in the
            // list until end of list is reached.
            sprintf(buffer, "%s:%s,%d,%d\n", listname,
                    temp->plate,
                    temp->mileage,
                    temp->returnDate);
            fputs(buffer, fp);
            temp = temp->next;
        }
    }
}

// function to write to files
int writeToFile()
{
    FILE *fp = fopen(DATA_FILE_NAME, "w");
    if (fp == NULL)
    {
        // in case of error opening file
        printf("File '%s' cannot be opened.\n", DATA_FILE_NAME);
        return -1;
    }
    char buffer[1024];
    // prints the total income as the first line of the data file
    // uses format "total_income:<income>"
    // sprintf stores into buffer, buffer is then printed to file
    sprintf(buffer, "%s:%.2f\n", TOTAL_INCOME, total_income);
    fputs(buffer, fp);
    // uses writeListToFile function to write complete lists to file
    writeListToFile(available_head, AVAILABLE_LIST_NAME, fp);
    writeListToFile(rented_head, RENTED_LIST_NAME, fp);
    writeListToFile(atWkshop_head, REPAIR_LIST_NAME, fp);
    // closes file
    fclose(fp);
    return 0;
}
// function for quitting. Calls printing to file function and frees memory afterwards
void quitFunction()
{
    // prints total income to user
    printf("Total income: %.2f\n", total_income);
    printf("Writing data to file '%s':\n", DATA_FILE_NAME);
    if (writeToFile() < 0)
    {
        printf("Writting to file '%s' failed\n", DATA_FILE_NAME);
        return;
    }
    // after writing to file, frees memory taken up by the 3 lists
    freeMemory(available_head);
    freeMemory(rented_head);
    freeMemory(atWkshop_head);
}

// processes transaction code inputs from user
void processInput(int transCode)
{
    printf("processing input %d\n\n", transCode);
    switch (transCode)
    {
    case ADD_NEW_AVAILABLE:
        addNewAvailable();
        break;
    case ADD_RETURN_AVAILABLE:
        addReturnToAvailable();
        break;
    case ADD_RETURN_REPAIR:
        addReturnToRepair();
        break;
    case TRANSFER_REPAIR_TO_AVAILABLE:
        transferFromRepairToAvailable();
        break;
    case RENT_FIRST_AVAILABLE:
        rentFirstAvailable();
        break;
    case PRINT_LIST:
        printLists();
        break;
    default:
        printf("Invalid Transaction Code: %d\n", transCode);
        break;
    }
}

int main(int argc, char *argv[])
{
    // calls read from file function
    if (readFromFile() < 0)
    {
        return 1;
    }
    int transCode = 0;
    // prints all transaction code options and waits for user inputs.
    // calls processInput function to process what was entered.
    // Also calls the quir function if '7' is entered
    while (1)
    {
        printf("1. Add a new car to the available for rent list.\n");
        printf("2. Add a returned car to the available for rent list.\n");
        printf("3. Add a returned car to the repairs list.\n");
        printf("4. Transfer repaired car to available list.\n");
        printf("5. Rent first available.\n");
        printf("6. Print list.\n");
        printf("7. Quit.\n");
        scanf("%d", &transCode);
        if (transCode == QUIT)
        {
            quitFunction();
            return 0;
        }
        processInput(transCode);
    }
}