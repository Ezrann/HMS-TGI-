#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <windows.h> // For console color

#define ADMIN_EMAIL "admin@tgi.com"
#define USER_FILE "users.txt"
#define SHIFT 3 // Caesar cipher shift

// Set console text color
static void set_text_color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Print message with specific color and reset
static void print_colored(const char *message, int color) {
    set_text_color(color);
    printf("%s", message);
    set_text_color(7); // Reset to default color (gray/white)
}

// Encrypt password using Caesar cipher
static void encrypt_password(const char *password, char *encrypted) {
    int i = 0;
    while (password[i] != '\0') {
        encrypted[i] = password[i] + SHIFT;
        i++;
    }
    encrypted[i] = '\0';
}

// Decrypt password using Caesar cipher
static void decrypt_password(const char *encrypted, char *decrypted) {
    int i = 0;
    while (encrypted[i] != '\0') {
        decrypted[i] = encrypted[i] - SHIFT;
        i++;
    }
    decrypted[i] = '\0';
}

// Read password with asterisk masking
static void read_password(char *password, int max_len) {
    int i = 0;
    char ch;
    while (i < max_len - 1) {
        ch = getch();
        if (ch == '\r' || ch == '\n') {
            break;
        } else if (ch == 8 && i > 0) { // Backspace
            i--;
            printf("\b \b");
        } else if (ch >= 32 && ch <= 126) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

// Register new user
static void register_user() {
    char email[100], password[50], encrypted[100], role[10];

    FILE *file = fopen(USER_FILE, "a");
    if (file == NULL) {
        print_colored("Error opening file.\n", 12); // Red
        return;
    }

    printf("Enter email: ");
    fgets(email, sizeof(email), stdin);
    email[strcspn(email, "\n")] = '\0';

    printf("Enter password: ");
    read_password(password, sizeof(password));

    if (strcmp(email, ADMIN_EMAIL) == 0)
        strcpy(role, "admin");
    else
        strcpy(role, "user");

    encrypt_password(password, encrypted);
    fprintf(file, "%s %s %s\n", email, encrypted, role);
    fclose(file);

    set_text_color(10); // Green
    printf("Registration successful! Role: %s\n", role);
    set_text_color(7);  // Reset
}

// Login existing user
static int login_user(char *email, char *password, char *role) {
    char file_email[100], file_encrypted[100], file_role[10], decrypted[100];
    FILE *file = fopen(USER_FILE, "r");
    if (file == NULL) {
        print_colored("Error opening file.\n", 12); // Red
        return 0;
    }

    printf("Enter email: ");
    fgets(email, 100, stdin);
    email[strcspn(email, "\n")] = '\0';

    printf("Enter password: ");
    read_password(password, 50);

    while (fscanf(file, "%s %s %s", file_email, file_encrypted, file_role) != EOF) {
        decrypt_password(file_encrypted, decrypted);
        if (strcmp(email, file_email) == 0 && strcmp(password, decrypted) == 0) {
            strcpy(role, file_role);
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

#endif