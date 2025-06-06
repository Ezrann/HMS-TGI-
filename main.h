#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TERMINAL_WIDTH 80
#define COLOR_RESET "\x1b[0m"
#define COLOR_HEADER "\x1b[1;31m" // Bright Red
#define COLOR_TEXT "\x1b[1;34m"   // Bright Blue
#define COLOR_BORDER "\x1b[1;31m" // Bright Red

typedef struct
{
    int id;
    char name[50];
    int roomNumber;
    float billAmount;
    float roomRent;
    char checkInDateTime[20];  // Format: YYYY-MM-DD HH:MM:SS
    char checkOutDateTime[20]; // Format: YYYY-MM-DD HH:MM:SS
    char paymentMethod[20];
} Guest;

typedef struct
{
    int roomNumber;
    char roomType[30];
    float rent;
} Room;

// Global variables
extern Guest guests[100];
extern int guestCount;
extern Room rooms[100];
extern int roomCount;

// Function prototypes
void print_centered_box(const char *text);
void addRoom();
void viewRooms();
void updateRoom();
void deleteRoom();
void viewBookingHistory();
void viewGuestBillingStatus();
void saveGuestsToFile();
void loadGuestsFromFile();
void saveRoomsToFile();
void loadRoomsFromFile();
void displayAdminMenu();
void displayGuestMenu();
void bookRoom();
void checkOut();
void modifyGuestInfo();
int findGuestByID(int id);
int findRoomByNumber(int roomNumber);
int isRoomOccupied(int roomNumber);
void getCurrentDateTime(char *dateTime);
void parseDateWithCurrentTime(const char *inputDate, char *outputDateTime);
int calculateStayDuration(const char *checkIn, const char *checkOut);
void viewReceipt();

#endif