#include "main.h"
#include "auth.h"

// Global variables
Guest guests[100];
int guestCount = 0;
Room rooms[100];
int roomCount = 0;

void print_centered_box(const char *text)
{
    int text_length = strlen(text);
    int padding = 2;
    int box_width = text_length + padding * 2 + 2;
    int margin = (TERMINAL_WIDTH - box_width) / 2;

    // Top border
    printf("%*s", margin, "");
    printf(COLOR_BORDER);
    for (int i = 0; i < box_width; i++)
        printf("*");
    printf(COLOR_RESET "\n");

    // Text line
    printf("%*s", margin, "");
    printf(COLOR_BORDER "*");
    printf(COLOR_RESET COLOR_TEXT " %*s%-*s ", 0, "", text_length, text);
    printf(COLOR_BORDER "*\n");

    // Bottom border
    printf("%*s", margin, "");
    printf(COLOR_BORDER);
    for (int i = 0; i < box_width; i++)
        printf("*");
    printf(COLOR_RESET "\n");
}

// Get current date and time in YYYY-MM-DD HH:MM:SS format
void getCurrentDateTime(char *dateTime)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// Parse user-input date (YYYY-MM-DD) and append current time
void parseDateWithCurrentTime(const char *inputDate, char *outputDateTime)
{
    int year, month, day;
    if (sscanf(inputDate, "%d-%d-%d", &year, &month, &day) != 3) {
        set_text_color(12); // Red
        strcpy(outputDateTime, "Invalid Date");
        printf("Invalid date format.\n");
        set_text_color(7); // Reset
        return;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(outputDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            year, month, day, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// Calculate stay duration in days between two date-times
int calculateStayDuration(const char *checkIn, const char *checkOut)
{
    int inYear, inMonth, inDay, outYear, outMonth, outDay;
    sscanf(checkIn, "%d-%d-%d", &inYear, &inMonth, &inDay);
    sscanf(checkOut, "%d-%d-%d", &outYear, &outMonth, &outDay);

    struct tm inTime = {0}, outTime = {0};
    inTime.tm_year = inYear - 1900;
    inTime.tm_mon = inMonth - 1;
    inTime.tm_mday = inDay;
    outTime.tm_year = outYear - 1900;
    outTime.tm_mon = outMonth - 1;
    outTime.tm_mday = outDay;

    time_t in = mktime(&inTime);
    time_t out = mktime(&outTime);
    double seconds = difftime(out, in);
    return (int)(seconds / (60 * 60 * 24)); // Convert seconds to days
}

int main()
{
    printf(COLOR_HEADER);
    print_centered_box("TUX GLOBAL INSTITUTE");
    print_centered_box("Booking Hotel Management Systems");
    print_centered_box("Welcome to TGI Hotel");
    printf(COLOR_RESET);
    loadGuestsFromFile();
    loadRoomsFromFile();
    int choice;
    char email[100], password[50], role[10];

    while (1)
    {
        printf("\nWelcome to TGI Hotels\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n'); // Clear input buffer

        switch (choice)
        {
        case 1:
            register_user();
            break;
        case 2:
            if (login_user(email, password, role))
            {
                set_text_color(10); // Green
                printf("Login successful! Role: %s\n", role);
                set_text_color(7);  // Reset
                if (strcmp(role, "admin") == 0)
                {
                    displayAdminMenu();
                }
                else if (strcmp(role, "user") == 0)
                {
                    displayGuestMenu();
                }
            }
            else
            {
                set_text_color(12); // Red
                printf("Login failed! Invalid email or password.\n");
                set_text_color(7);  // Reset
            }
            break;
        case 3:
            saveGuestsToFile();
            saveRoomsToFile();
            set_text_color(10); // Green
            printf("Exiting...\n");
            set_text_color(7); // Reset
            exit(0);
            break;
        default:
            set_text_color(12); // Red
            printf("Invalid choice. Please try again.\n");
            set_text_color(7);  // Reset
        }
    }

    return 0;
}

void addRoom()
{
    if (roomCount >= 100)
    {
        set_text_color(12); // Red
        printf("Room list is full.\n");
        set_text_color(7); // Reset
        return;
    }

    Room newRoom;
    int status;

    printf("Enter room number: ");
    scanf("%d", &newRoom.roomNumber);
    if (findRoomByNumber(newRoom.roomNumber) != -1)
    {
        set_text_color(12); // Red
        printf("Room number already exists.\n");
        set_text_color(7); // Reset
        return;
    }

    printf("Enter room type: ");
    scanf("%s", newRoom.roomType);

    printf("Enter room rent per night($) : ");
    scanf("%f", &newRoom.rent);

    printf("Are you sure to make the room? (1 for Yes / 0 for No): ");
    scanf("%d", &status);

    if (status == 1)
    {
        rooms[roomCount++] = newRoom;
        saveRoomsToFile();
        set_text_color(10); // Green
        printf("Room added successfully.\n");
        set_text_color(7); // Reset
    }
    else
    {
        set_text_color(12); // Red
        printf("Room addition cancelled.\n");
        set_text_color(7); // Reset
    }
}

void viewRooms()
{
    if (roomCount == 0)
    {
        set_text_color(12); // Red
        printf("No rooms to display.\n");
        set_text_color(7); // Reset
    }
    else
    {
        printf("\nAvailable Rooms:\n");
        printf("+-----------------+--------------------+-----------------+-------------+\n");
        printf("| Room Number     | Room Type          | Rent per Night  | Status      |\n");
        printf("+-----------------+--------------------+-----------------+-------------+\n");

        for (int i = 0; i < roomCount; i++)
        {
            printf("| %-15d | %-18s | $%-14.2f | %-11s |\n",
                   rooms[i].roomNumber,
                   rooms[i].roomType,
                   rooms[i].rent,
                   isRoomOccupied(rooms[i].roomNumber) ? "Occupied" : "Available");
        }

        printf("+-----------------+--------------------+-----------------+-------------+\n");
        printf("\n");
        printf("Press any keys to continue to admin menu...\n");
        getch();
    }
}

void updateRoom()
{
    int roomNumber;
    printf("Enter room number to update: ");
    scanf("%d", &roomNumber);

    int index = findRoomByNumber(roomNumber);
    if (index != -1)
    {
        printf("\nCurrent Room:\n");
        printf("Room Number: %d\n", roomNumber);
        printf("Room Type: %s\n", rooms[index].roomType);
        printf("Rent per Night ($): %.2f\n\n", rooms[index].rent);

        printf("Press any keys to continue updating room...\n");
        getch();

        printf("Enter new room type: ");
        scanf("%s", rooms[index].roomType);
        printf("Enter new rent per night: ");
        scanf("%f", &rooms[index].rent);
        saveRoomsToFile();
        set_text_color(10); // Green
        printf("Room updated successfully.\n");
        set_text_color(7); // Reset
    }
    else
    {
        set_text_color(12); // Red
        printf("Room number not found.\n");
        set_text_color(7); // Reset
    }
}

void deleteRoom()
{
    int roomNumber;
    printf("Enter room number to delete: ");
    scanf("%d", &roomNumber);

    int index = findRoomByNumber(roomNumber);
    if (index != -1)
    {
        printf("\nRoom Details:\n");
        printf("Room Number: %d\n", rooms[index].roomNumber);
        printf("Room Type: %s\n", rooms[index].roomType);
        printf("Rent per Night ($): %.2f\n", rooms[index].rent);
        printf("Status: %s\n\n", isRoomOccupied(rooms[index].roomNumber) ? "Occupied" : "Available");

        if (isRoomOccupied(roomNumber))
        {
            set_text_color(12); // Red
            printf("Cannot delete an occupied room.\n");
            set_text_color(7); // Reset
            return;
        }

        printf("Are you sure you want to delete this room? (1 for Yes / 0 for No): ");
        int confirm;
        scanf("%d", &confirm);
        if (confirm != 1)
        {
            set_text_color(12); // Red
            printf("Room deletion cancelled.\n");
            set_text_color(7); // Reset
            return;
        }

        for (int i = index; i < roomCount - 1; i++)
        {
            rooms[i] = rooms[i + 1];
        }
        roomCount--;
        saveRoomsToFile();
        set_text_color(10); // Green
        printf("Room deleted successfully.\n");
        set_text_color(7); // Reset
    }
    else
    {
        set_text_color(12); // Red
        printf("Room number not found.\n");
        set_text_color(7); // Reset
    }
}

void viewBookingHistory()
{
    FILE *file = fopen("Bookingusers.txt", "r");
    if (file == NULL)
    {
        set_text_color(12); // Red
        printf("No booking history found.\n");
        set_text_color(7); // Reset
        return;
    }

    Guest tempGuests[100];
    int tempGuestCount = 0;
    char line[256];

    // Read and parse guest records
    while (fgets(line, sizeof(line), file) && tempGuestCount < 100)
    {
        if (sscanf(line, "Guest ID: %d, Name: %49[^,], Room: %d, Bill: $%f, Rent: $%f, Check-in: %19[^,], Check-out: %19[^,], Payment: %19s",
                   &tempGuests[tempGuestCount].id,
                   tempGuests[tempGuestCount].name,
                   &tempGuests[tempGuestCount].roomNumber,
                   &tempGuests[tempGuestCount].billAmount,
                   &tempGuests[tempGuestCount].roomRent,
                   tempGuests[tempGuestCount].checkInDateTime,
                   tempGuests[tempGuestCount].checkOutDateTime,
                   tempGuests[tempGuestCount].paymentMethod) == 8)
        {
            tempGuestCount++;
        }
    }
    fclose(file);

    if (tempGuestCount == 0)
    {
        set_text_color(12); // Red
        printf("No booking history to display.\n");
        set_text_color(7); // Reset
        return;
    }

    // Display table
    printf("\nBooking History:\n");
    printf("+-------+--------------------+------+----------+----------+----------+-------------------+-------------------+---------------+\n");
    printf("| %-5s | %-18s | %-4s | %-8s | %-8s | %-8s | %-17s | %-17s | %-13s |\n",
           "ID", "Guest Name", "Room", "Duration", "Bill", "Rent", "Check-in", "Check-out", "Payment");
    printf("+-------+--------------------+------+----------+----------+----------+-------------------+-------------------+---------------+\n");

    for (int i = 0; i < tempGuestCount; i++)
    {
        int stayDuration = calculateStayDuration(tempGuests[i].checkInDateTime, tempGuests[i].checkOutDateTime);
        printf("| %-5d | %-18s | %-4d | %-8d | $%-7.2f | $%-7.2f | %-17s | %-17s | %-13s |\n",
               tempGuests[i].id,
               tempGuests[i].name,
               tempGuests[i].roomNumber,
               stayDuration,
               tempGuests[i].billAmount,
               tempGuests[i].roomRent,
               tempGuests[i].checkInDateTime,
               tempGuests[i].checkOutDateTime,
               tempGuests[i].paymentMethod);
    }

    printf("+-------+--------------------+------+----------+----------+----------+-------------------+-------------------+---------------+\n");
    printf("\nPress any key to continue...\n");
    getch();
}

void viewGuestBillingStatus()
{
    int id;
    printf("Enter guest ID: ");
    scanf("%d", &id);

    int index = findGuestByID(id);
    if (index != -1)
    {
        int stayDuration = calculateStayDuration(guests[index].checkInDateTime, guests[index].checkOutDateTime);
        printf("\nGuest Billing Status:\n");
        printf("+-------+--------------------+------+----------+----------+----------+-------------------+-------------------+---------------+\n");
        printf("| %-5s | %-18s | %-4s | %-8s | %-8s | %-8s | %-17s | %-17s | %-13s |\n",
               "ID", "Guest Name", "Room", "Duration", "Bill", "Rent", "Check-in", "Check-out", "Payment");
        printf("+-------+--------------------+------+----------+----------+----------+-------------------+-------------------+---------------+\n");
        printf("| %-5d | %-18s | %-4d | %-8d | $%-7.2f | $%-7.2f | %-17s | %-17s | %-13s |\n",
               guests[index].id,
               guests[index].name,
               guests[index].roomNumber,
               stayDuration,
               guests[index].billAmount,
               guests[index].roomRent,
               guests[index].checkInDateTime,
               guests[index].checkOutDateTime,
               guests[index].paymentMethod);
        printf("+-------+--------------------+------+----------+----------+----------+-------------------+-------------------+---------------+\n");
    }
    else
    {
        set_text_color(12); // Red
        printf("Guest ID not found.\n");
        set_text_color(7); // Reset
    }
    printf("\nPress any key to continue...\n");
    getch();
}

void saveGuestsToFile()
{
    FILE *file = fopen("Bookingusers.txt", "a");
    if (file == NULL)
    {
        set_text_color(12); // Red
        printf("Error opening file.\n");
        set_text_color(7); // Reset
        return;
    }

    for (int i = 0; i < guestCount; i++)
    {
        fprintf(file, "Guest ID: %d, Name: %s, Room: %d, Bill: $%.2f, Rent: $%.2f, Check-in: %s, Check-out: %s, Payment: %s\n",
                guests[i].id, guests[i].name, guests[i].roomNumber,
                guests[i].billAmount, guests[i].roomRent,
                guests[i].checkInDateTime, guests[i].checkOutDateTime,
                guests[i].paymentMethod);
    }

    fclose(file);
    set_text_color(10); // Green
    printf("Guests saved successfully.\n");
    set_text_color(7); // Reset
}

void loadGuestsFromFile()
{
    guestCount = 0; // Reset guest count as we maintain history separately
}

void saveRoomsToFile()
{
    FILE *file = fopen("rooms.txt", "w");
    if (file == NULL)
    {
        set_text_color(12); // Red
        printf("Error opening file.\n");
        set_text_color(7); // Reset
        return;
    }

    fprintf(file, "%-10s %-20s %-10s\n", "RoomNumber", "RoomType", "Rent");
    for (int i = 0; i < roomCount; i++)
    {
        fprintf(file, "%-10d %-20s %-10.2f\n", rooms[i].roomNumber, rooms[i].roomType, rooms[i].rent);
    }

    fclose(file);
    set_text_color(10); // Green
    printf("Rooms saved successfully.\n");
    set_text_color(7); // Reset
}

void loadRoomsFromFile()
{
    FILE *file = fopen("rooms.txt", "r");
    if (file == NULL)
    {
        set_text_color(12); // Red
        printf("No rooms file found.\n");
        set_text_color(7); // Reset
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file); // Skip header
    roomCount = 0;
    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%d %29s %f", &rooms[roomCount].roomNumber,
                   rooms[roomCount].roomType, &rooms[roomCount].rent) == 3)
        {
            roomCount++;
        }
    }

    fclose(file);
    set_text_color(10); // Green
    printf("Rooms loaded successfully.\n");
    set_text_color(7); // Reset
}

void displayAdminMenu()
{
    int choice;

    while (1)
    {
        printf("\nAdmin Menu\n");
        printf("1. View All Rooms\n");
        printf("2. Add Room\n");
        printf("3. Update Room\n");
        printf("4. Delete Room\n");
        printf("5. View Booking History\n");
        printf("6. View Guest Billing Status\n");
        printf("7. Exit to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            viewRooms();
            break;
        case 2:
            addRoom();
            break;
        case 3:
            updateRoom();
            break;
        case 4:
            deleteRoom();
            break;
        case 5:
            viewBookingHistory();
            break;
        case 6:
            viewGuestBillingStatus();
            break;
        case 7:
            set_text_color(10); // Green
            printf("Returning to main menu...\n");
            set_text_color(7); // Reset
            return;
        default:
            set_text_color(12); // Red
            printf("Invalid choice. Please try again.\n");
            set_text_color(7); // Reset
        }
    }
}

void displayGuestMenu()
{
    int choice;

    while (1)
    {
        printf("\nGuest Menu\n");
        printf("1. Book Room\n");
        printf("2. Check Out\n");
        printf("3. Modify Guest Information\n");
        printf("4. View Receipt\n");
        printf("5. Exit to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            bookRoom();
            break;
        case 2:
            checkOut();
            break;
        case 3:
            modifyGuestInfo();
            break;
        case 4:
            viewReceipt();
            break;
        case 5:
            set_text_color(10); // Green
            printf("Returning to main menu...\n");
            set_text_color(7); // Reset
            return;
        default:
            set_text_color(12); // Red
            printf("Invalid choice. Please try again.\n");
            set_text_color(7); // Reset
        }
    }
}

void bookRoom()
{
    if (guestCount >= 100)
    {
        set_text_color(12); // Red
        printf("Guest list is full.\n");
        set_text_color(7); // Reset
        return;
    }

    Guest newGuest;
    newGuest.id = guestCount + 1;
    printf("Enter your name: ");
    scanf(" %49s", newGuest.name);
    while (getchar() != '\n');

    printf("Available rooms:\n");
    if (roomCount == 0)
    {
        set_text_color(12); // Red
        printf("No rooms available.\n");
        set_text_color(7); // Reset
        return;
    }

    for (int i = 0; i < roomCount; i++)
    {
        if (!isRoomOccupied(rooms[i].roomNumber))
        {
            printf("%d. %s - $%.2f per night\n", i + 1, rooms[i].roomType, rooms[i].rent);
        }
    }

    int roomChoice;
    printf("Enter the room type number: ");
    if (scanf("%d", &roomChoice) != 1)
    {
        set_text_color(12); // Red
        printf("Invalid input. Please enter a number.\n");
        set_text_color(7); // Reset
        while (getchar() != '\n');
        return;
    }

    if (roomChoice < 1 || roomChoice > roomCount)
    {
        set_text_color(12); // Red
        printf("Invalid room type selected.\n");
        set_text_color(7); // Reset
        return;
    }

    int selectedRoomNumber = rooms[roomChoice - 1].roomNumber;
    if (isRoomOccupied(selectedRoomNumber))
    {
        set_text_color(12); // Red
        printf("Selected room is already occupied. Please choose another room.\n");
        set_text_color(7); // Reset
        return;
    }

    newGuest.roomNumber = selectedRoomNumber;
    newGuest.roomRent = rooms[roomChoice - 1].rent;

    char inputDate[11];
    printf("Enter check-in date (YYYY-MM-DD): ");
    scanf("%s", inputDate);
    parseDateWithCurrentTime(inputDate, newGuest.checkInDateTime);

    printf("Enter check-out date (YYYY-MM-DD): ");
    scanf("%s", inputDate);
    parseDateWithCurrentTime(inputDate, newGuest.checkOutDateTime);

    int stayDuration = calculateStayDuration(newGuest.checkInDateTime, newGuest.checkOutDateTime);
    if (stayDuration <= 0)
    {
        set_text_color(12); // Red
        printf("Invalid dates. Check-out must be after check-in.\n");
        set_text_color(7); // Reset
        return;
    }

    newGuest.billAmount = newGuest.roomRent * stayDuration;
    strcpy(newGuest.paymentMethod, "Not Paid");

    printf("\n+-------------------+-------------------+\n");
    printf("| Field             | Value             |\n");
    printf("+-------------------+-------------------+\n");
    printf("| Guest Name        | %-17s |\n", newGuest.name);
    printf("| Room Number       | %-17d |\n", newGuest.roomNumber);
    printf("| Room Type         | %-17s |\n", rooms[roomChoice - 1].roomType);
    printf("| Stay Duration     | %-17d |\n", stayDuration);
    printf("| Total Bill        | $%-16.2f |\n", newGuest.billAmount);
    printf("| Check-in Date     | %-17s |\n", newGuest.checkInDateTime);
    printf("| Check-out Date    | %-17s |\n", newGuest.checkOutDateTime);
    printf("| Payment Status    | %-17s |\n", newGuest.paymentMethod);
    printf("+-------------------+-------------------+\n\n");

    int confirm;
    printf("Confirm booking? (1 for Yes / 0 for No): ");
    if (scanf("%d", &confirm) != 1 || (confirm != 0 && confirm != 1))
    {
        set_text_color(12); // Red
        printf("Invalid input. Booking cancelled.\n");
        set_text_color(7); // Reset
        while (getchar() != '\n');
        return;
    }

    if (confirm == 1)
    {
        guests[guestCount++] = newGuest;
        set_text_color(10); // Green
        printf("Room booked successfully. Your guest ID is %d.\n", newGuest.id);
        set_text_color(7); // Reset
    }
    else
    {
        set_text_color(12); // Red
        printf("Booking cancelled.\n");
        set_text_color(7); // Reset
    }
}

int isRoomOccupied(int roomNumber)
{
    for (int i = 0; i < guestCount; i++)
    {
        if (guests[i].roomNumber == roomNumber)
        {
            return 1;
        }
    }
    return 0;
}

void checkOut()
{
    int id;
    printf("Enter your guest ID: ");
    scanf("%d", &id);

    int index = findGuestByID(id);
    if (index != -1)
    {
        int stayDuration = calculateStayDuration(guests[index].checkInDateTime, guests[index].checkOutDateTime);
        printf("Billing Status:\n");
        printf("Guest ID: %d\n", guests[index].id);
        printf("Guest Name: %s\n", guests[index].name);
        printf("Room Number: %d\n", guests[index].roomNumber);
        printf("Stay Duration: %d days\n", stayDuration);
        printf("Room Rent: $%.2f\n", guests[index].roomRent);
        printf("Total Bill Amount: $%.2f\n", guests[index].billAmount);
        printf("Check-in Date: %s\n", guests[index].checkInDateTime);
        printf("Check-out Date: %s\n", guests[index].checkOutDateTime);

        int payChoice;
        printf("Do you want to pay the bill? (1 for Yes / 0 for No): ");
        scanf("%d", &payChoice);

        if (payChoice == 1)
        {
            int paymentMethod;
            printf("Select payment method:\n");
            printf("1. Credit Card\n");
            printf("2. Cash\n");
            printf("3. Online Payment\n");
            printf("Enter choice: ");
            scanf("%d", &paymentMethod);

            switch (paymentMethod)
            {
            case 1:
                strcpy(guests[index].paymentMethod, "Credit Card");
                break;
            case 2:
                strcpy(guests[index].paymentMethod, "Cash");
                break;
            case 3:
                strcpy(guests[index].paymentMethod, "Online Payment");
                break;
            default:
                set_text_color(12); // Red
                printf("Invalid payment method. Defaulting to Cash.\n");
                set_text_color(7); // Reset
                strcpy(guests[index].paymentMethod, "Cash");
            }

            saveGuestsToFile();
            for (int i = index; i < guestCount - 1; i++)
            {
                guests[i] = guests[i + 1];
            }
            guestCount--;
            set_text_color(10); // Green
            printf("Payment successful. Checked out successfully.\n");
            set_text_color(7); // Reset
        }
        else
        {
            set_text_color(12); // Red
            printf("Check out cancelled.\n");
            set_text_color(7); // Reset
        }
    }
    else
    {
        set_text_color(12); // Red
        printf("Guest ID not found.\n");
        set_text_color(7); // Reset
    }
}

void modifyGuestInfo()
{
    int id;
    printf("Enter your guest ID: ");
    scanf("%d", &id);

    int index = findGuestByID(id);
    if (index != -1)
    {
        printf("Enter new name: ");
        scanf("%s", guests[index].name);
        printf("Enter new room number: ");
        scanf("%d", &guests[index].roomNumber);

        char inputDate[11];
        printf("Enter new check-in date (YYYY-MM-DD): ");
        scanf("%s", inputDate);
        parseDateWithCurrentTime(inputDate, guests[index].checkInDateTime);

        printf("Enter new check-out date (YYYY-MM-DD): ");
        scanf("%s", inputDate);
        parseDateWithCurrentTime(inputDate, guests[index].checkOutDateTime);

        int stayDuration = calculateStayDuration(guests[index].checkInDateTime, guests[index].checkOutDateTime);
        if (stayDuration <= 0)
        {
            set_text_color(12); // Red
            printf("Invalid dates. Update cancelled.\n");
            set_text_color(7); // Reset
            return;
        }

        int roomIndex = findRoomByNumber(guests[index].roomNumber);
        if (roomIndex != -1)
        {
            guests[index].roomRent = rooms[roomIndex].rent;
            guests[index].billAmount = guests[index].roomRent * stayDuration;
            set_text_color(10); // Green
            printf("Guest information updated successfully.\n");
            set_text_color(7); // Reset
        }
        else
        {
            set_text_color(12); // Red
            printf("Invalid room number. Update cancelled.\n");
            set_text_color(7); // Reset
        }
    }
    else
    {
        set_text_color(12); // Red
        printf("Guest ID not found.\n");
        set_text_color(7); // Reset
    }
}

int findGuestByID(int id)
{
    for (int i = 0; i < guestCount; i++)
    {
        if (guests[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

int findRoomByNumber(int roomNumber)
{
    for (int i = 0; i < roomCount; i++)
    {
        if (rooms[i].roomNumber == roomNumber)
        {
            return i;
        }
    }
    return -1;
}

void viewReceipt()
{
    int id;
    printf("Enter your guest ID: ");
    scanf("%d", &id);

    int index = findGuestByID(id);
    if (index != -1)
    {
        int stayDuration = calculateStayDuration(guests[index].checkInDateTime, guests[index].checkOutDateTime);
        printf("\n--- Receipt ---\n");
        printf("Guest ID : %d\n", guests[index].id);
        printf("Guest Name : %s\n", guests[index].name);
        printf("Room Number : %d\n", guests[index].roomNumber);
        printf("Stay Duration : %d days\n", stayDuration);
        printf("Price Room : $%.2f\n", guests[index].roomRent);
        printf("Total Bill Amount : $%.2f\n", guests[index].billAmount);
        printf("Check-in Date : %s\n", guests[index].checkInDateTime);
        printf("Check-out Date : %s\n", guests[index].checkOutDateTime);
        printf("Payment Method : %s\n", guests[index].paymentMethod);

        char fileName[20];
        sprintf(fileName, "receipts%d.txt", guests[index].id);
        FILE *file = fopen(fileName, "a");
        if (file == NULL)
        {
            set_text_color(12); // Red
            printf("Error opening file.\n");
            set_text_color(7); // Reset
            return;
        }
        fprintf(file, "--- Receipt ---\n");
        fprintf(file, "Guest ID: %d\n", guests[index].id);
        fprintf(file, "Guest Name: %s\n", guests[index].name);
        fprintf(file, "Room Number: %d\n", guests[index].roomNumber);
        fprintf(file, "Stay Duration: %d days\n", stayDuration);
        fprintf(file, "Room Rent: $%.2f\n", guests[index].roomRent);
        fprintf(file, "Total Bill Amount: $%.2f\n", guests[index].billAmount);
        fprintf(file, "Check-in Date: %s\n", guests[index].checkInDateTime);
        fprintf(file, "Check-out Date: %s\n", guests[index].checkOutDateTime);
        fprintf(file, "Payment Method: %s\n", guests[index].paymentMethod);

        fclose(file);
        set_text_color(10); // Green
        printf("Receipt saved to %s\n", fileName);
        set_text_color(7); // Reset
    }
    else
    {
        set_text_color(12); // Red
        printf("Guest ID not found.\n");
        set_text_color(7); // Reset
    }
}