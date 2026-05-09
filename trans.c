// Bank Account Management System
// Features: Login, CRUD, Transfer, Transaction Log, UI Formatting

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Structure ---
struct clientData {
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// --- Prototypes ---
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void transferMoney(FILE *fPtr);
void transactionHistory(void);
void viewAccount(FILE *fPtr);
void logTransaction(char *message);

// UI Helpers
void printDivider(void);
void printHeader(const char *title);
void printTableHeader(void);
void printTableRow(struct clientData c);
void clearInputBuffer(void);
int  confirmAction(const char *message);
int  getAccount(const char *prompt, unsigned int *acc);
int  readAccount(FILE *fPtr, unsigned int acc, struct clientData *c);

// ----------------------------------------------------------------
//  MAIN
// ----------------------------------------------------------------
int main(void)
{
    FILE *cfPtr;
    unsigned int choice;
    char password[20];
    int attempts = 0;

    printf("\n");
    printDivider();
    printf("       BANK ACCOUNT MANAGEMENT SYSTEM\n");
    printDivider();

    // Login — 3 attempts
    while (attempts < 3) {
        printf("  Enter admin password: ");
        scanf("%19s", password);
        clearInputBuffer();
        if (strcmp(password, "12345") == 0) {
            printDivider();
            break;
        }
        attempts++;
        if (attempts < 3)
            printf("  [ERROR] Wrong password! %d attempt(s) remaining.\n", 3 - attempts);
        else {
            printf("  [ERROR] Too many failed attempts. Exiting...\n");
            printDivider();
            exit(0);
        }
    }

    // Open or create file
    cfPtr = fopen("credit.dat", "rb+");
    if (cfPtr == NULL) {
        printf("  [!] credit.dat not found. Creating new file...\n");
        cfPtr = fopen("credit.dat", "wb+");
        if (cfPtr == NULL) {
            printf("  [ERROR] Could not create credit.dat. Exiting.\n");
            exit(-1);
        }
    } 

    // Main loop
    while ((choice = enterChoice()) != 8) {
        switch (choice) {
            case 1: textFile(cfPtr);        break;
            case 2: updateRecord(cfPtr);    break;
            case 3: newRecord(cfPtr);       break;
            case 4: deleteRecord(cfPtr);    break;
            case 5: transferMoney(cfPtr);   break;
            case 6: transactionHistory();   break;
            case 7: viewAccount(cfPtr);     break;
            default: printf("\n  [!] Invalid choice. Enter 1 to 8.\n"); break;
        }
    }

    printDivider();
    printf("  Thank you for using the system. Goodbye!\n");
    printDivider();
    fclose(cfPtr);
    return 0;
}

// ----------------------------------------------------------------
//  UI HELPERS
// ----------------------------------------------------------------
void printDivider(void) {
    printf("===================================================\n");
}

void printHeader(const char *title) {
    printf("\n");
    printDivider();
    printf("  %s\n", title);
    printDivider();
}

void printTableHeader(void) {
    printDivider();
    printf("| %-4s | %-14s | %-10s | %10s |\n", "Acct", "Last Name", "First Name", "Balance");
    printDivider();
}

void printTableRow(struct clientData c) {
    printf("| %-4d | %-14s | %-10s | %10.2f |\n", c.acctNum, c.lastName, c.firstName, c.balance);
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int confirmAction(const char *message) {
    char r;
    printf("\n  [?] %s (Y/N): ", message);
    scanf(" %c", &r);
    clearInputBuffer();
    return (r == 'Y' || r == 'y');
}

// Get + validate account number — returns 1 if valid, 0 if not
int getAccount(const char *prompt, unsigned int *acc) {
    printf("  %s (1 - 100): ", prompt);
    if (scanf("%u", acc) != 1 || *acc < 1 || *acc > 100) {
        clearInputBuffer();
        printf("  [ERROR] Invalid account number.\n");
        printDivider();
        return 0;
    }
    clearInputBuffer();
    return 1;
}

// Read a record — returns 1 if account exists, 0 if empty slot
int readAccount(FILE *fPtr, unsigned int acc, struct clientData *c) {
    fseek(fPtr, (acc - 1) * sizeof(struct clientData), SEEK_SET);
    fread(c, sizeof(struct clientData), 1, fPtr);
    return (c->acctNum != 0);
}

// Log with timestamp
void logTransaction(char *message) {
    FILE *fp = fopen("transactions.txt", "a");
    if (!fp) { printf("  [WARNING] Could not write to transactions.txt\n"); return; }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(fp, "%02d-%02d-%d %02d:%02d:%02d - %s\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec, message);
    fclose(fp);
}

// ----------------------------------------------------------------
//  FUNCTION 1: Export accounts to text file
// ----------------------------------------------------------------
void textFile(FILE *readPtr) {
    FILE *writePtr;
    int result, count = 0;
    struct clientData client = {0, "", "", 0.0};

    printHeader("GENERATING ACCOUNT REPORT");

    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        printf("  [ERROR] Could not open accounts.txt.\n");
        printDivider();
        return;
    }

    rewind(readPtr);
    fprintf(writePtr, "===================================================\n");
    fprintf(writePtr, "             BANK ACCOUNT REPORT\n");
    fprintf(writePtr, "===================================================\n");
    fprintf(writePtr, "| %-4s | %-14s | %-10s | %10s |\n", "Acct", "Last Name", "First Name", "Balance");
    fprintf(writePtr, "===================================================\n");

    printf("  Accounts found:\n");
    printTableHeader();

    while ((result = fread(&client, sizeof(struct clientData), 1, readPtr)) != 0) {
        if (client.acctNum != 0) {
            fprintf(writePtr, "| %-4d | %-14s | %-10s | %10.2f |\n",
                    client.acctNum, client.lastName, client.firstName, client.balance);
            printTableRow(client);
            count++;
        }
    }

    fprintf(writePtr, "===================================================\n");
    fprintf(writePtr, "  Total accounts: %d\n===================================================\n", count);
    printDivider();
    printf("  Total active accounts: %d\n", count);
    printf("  [OK] Saved to accounts.txt\n");
    printDivider();
    fclose(writePtr);
}

// ----------------------------------------------------------------
//  FUNCTION 2: Update account balance
// ----------------------------------------------------------------
void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};
    char log[200];

    printHeader("UPDATE ACCOUNT BALANCE");

    if (!getAccount("Enter account number to update", &account)) return;
    if (!readAccount(fPtr, account, &client)) {
        printf("  [ERROR] Account #%d does not exist.\n", account);
        printDivider();
        return;
    }

    printf("\n  Current details:\n");
    printTableHeader();
    printTableRow(client);
    printDivider();

    printf("  Enter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction) != 1) {
        clearInputBuffer();
        printf("  [ERROR] Invalid amount.\n");
        printDivider();
        return;
    }
    clearInputBuffer();

    printf("\n  Preview: $%.2f %+.2f = $%.2f\n",
           client.balance, transaction, client.balance + transaction);

    if (!confirmAction("Apply this transaction?")) {
        printf("  [CANCELLED] No changes made.\n");
        printDivider();
        return;
    }

    client.balance += transaction;
    fseek(fPtr, -(long)sizeof(struct clientData), SEEK_CUR);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    sprintf(log, "Account #%d updated by %.2f | New Balance: %.2f", account, transaction, client.balance);
    logTransaction(log);

    printf("\n  Updated details:\n");
    printTableHeader();
    printTableRow(client);
    printDivider();
    printf("  [OK] Account #%d updated.\n", account);
    printDivider();
}

// ----------------------------------------------------------------
//  FUNCTION 3: Create new account
// ----------------------------------------------------------------
void newRecord(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;
    char log[200];

    printHeader("CREATE NEW ACCOUNT");

    if (!getAccount("Enter new account number", &accountNum)) return;
    if (readAccount(fPtr, accountNum, &client)) {
        printf("  [ERROR] Account #%d already exists.\n", accountNum);
        printTableHeader();
        printTableRow(client);
        printDivider();
        return;
    }

    printf("  Enter last name  : "); scanf("%14s", client.lastName);  clearInputBuffer();
    printf("  Enter first name : "); scanf("%9s",  client.firstName); clearInputBuffer();
    printf("  Enter balance    : ");
    if (scanf("%lf", &client.balance) != 1) {
        clearInputBuffer();
        printf("  [ERROR] Invalid balance.\n");
        printDivider();
        return;
    }
    clearInputBuffer();

    client.acctNum = accountNum;
    printf("\n  New account preview:\n");
    printTableHeader();
    printTableRow(client);
    printDivider();

    if (!confirmAction("Confirm creating this account?")) {
        printf("  [CANCELLED] Account NOT created.\n");
        printDivider();
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    sprintf(log, "New account #%d | %s %s | Balance: %.2f",
            accountNum, client.firstName, client.lastName, client.balance);
    logTransaction(log);

    printf("  [OK] Account #%d created.\n", accountNum);
    printDivider();
}

// ----------------------------------------------------------------
//  FUNCTION 4: Delete account
// ----------------------------------------------------------------
void deleteRecord(FILE *fPtr) {
    struct clientData client, blankClient = {0, "", "", 0};
    unsigned int accountNum;
    char log[200];

    printHeader("DELETE ACCOUNT");

    if (!getAccount("Enter account number to delete", &accountNum)) return;
    if (!readAccount(fPtr, accountNum, &client)) {
        printf("  [ERROR] Account #%d does not exist.\n", accountNum);
        printDivider();
        return;
    }

    printf("\n  Account to delete:\n");
    printTableHeader();
    printTableRow(client);
    printDivider();

    printf("  [WARNING] This CANNOT be undone!\n");
    if (!confirmAction("Delete this account?") ||
        !confirmAction("Confirm AGAIN to permanently delete?")) {
        printf("  [CANCELLED] Account NOT deleted.\n");
        printDivider();
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);

    sprintf(log, "Account #%d deleted | %s %s | Balance: %.2f",
            accountNum, client.firstName, client.lastName, client.balance);
    logTransaction(log);

    printf("  [OK] Account #%d permanently deleted.\n", accountNum);
    printDivider();
}

// ----------------------------------------------------------------
//  FUNCTION 5: Transfer money
// ----------------------------------------------------------------
void transferMoney(FILE *fPtr) {
    unsigned int fromAcc, toAcc;
    double amount;
    struct clientData from = {0}, to = {0};
    char log[200];

    printHeader("TRANSFER MONEY");

    if (!getAccount("Enter sender account number  ", &fromAcc)) return;
    if (!getAccount("Enter receiver account number", &toAcc))   return;

    if (fromAcc == toAcc) {
        printf("  [ERROR] Sender and receiver cannot be the same.\n");
        printDivider();
        return;
    }

    printf("  Enter amount to transfer: $");
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        clearInputBuffer();
        printf("  [ERROR] Invalid amount.\n");
        printDivider();
        return;
    }
    clearInputBuffer();

    if (!readAccount(fPtr, fromAcc, &from)) {
        printf("  [ERROR] Sender account #%d does not exist.\n", fromAcc);
        printDivider();
        return;
    }
    if (!readAccount(fPtr, toAcc, &to)) {
        printf("  [ERROR] Receiver account #%d does not exist.\n", toAcc);
        printDivider();
        return;
    }
    if (from.balance < amount) {
        printf("  [ERROR] Insufficient balance! Available: $%.2f\n", from.balance);
        printDivider();
        return;
    }

    printf("\n  Transfer preview:\n");
    printf("  From: #%d %s %s | $%.2f\n", from.acctNum, from.firstName, from.lastName, from.balance);
    printf("  To  : #%d %s %s | $%.2f\n", to.acctNum,   to.firstName,   to.lastName,   to.balance);
    printf("  Amount: $%.2f\n", amount);

    if (!confirmAction("Confirm this transfer?")) {
        printf("  [CANCELLED] Transfer NOT made.\n");
        printDivider();
        return;
    }

    from.balance -= amount;
    to.balance   += amount;

    fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&from, sizeof(struct clientData), 1, fPtr);
    fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
    fwrite(&to, sizeof(struct clientData), 1, fPtr);

    sprintf(log, "Transferred $%.2f from #%d to #%d", amount, fromAcc, toAcc);
    logTransaction(log);

    printf("\n  Updated balances:\n");
    printTableHeader();
    printTableRow(from);
    printTableRow(to);
    printDivider();
    printf("  [OK] Transfer complete.\n");
    printDivider();
}

// ----------------------------------------------------------------
//  FUNCTION 6: Transaction history
// ----------------------------------------------------------------
void transactionHistory(void) {
    FILE *fp = fopen("transactions.txt", "r");
    char line[200];
    int count = 0;

    printHeader("TRANSACTION HISTORY");

    if (!fp) {
        printf("  [INFO] No transaction history found.\n");
        printDivider();
        return;
    }
    while (fgets(line, sizeof(line), fp)) {
        printf("  %s", line);
        count++;
    }
    printf("\n  Total transactions: %d\n", count);
    printDivider();
    fclose(fp);
}

// ----------------------------------------------------------------
//  FUNCTION 7: View account details
// ----------------------------------------------------------------
void viewAccount(FILE *fPtr) {
    unsigned int account;
    struct clientData client = {0};

    printHeader("VIEW ACCOUNT DETAILS");

    if (!getAccount("Enter account number", &account)) return;
    if (!readAccount(fPtr, account, &client)) {
        printf("  [ERROR] Account #%d not found.\n", account);
        printDivider();
        return;
    }

    printf("\n  Account Details:\n");
    printTableHeader();
    printTableRow(client);
    printDivider();
}

// ----------------------------------------------------------------
//  MENU
// ----------------------------------------------------------------
unsigned int enterChoice(void) {
    unsigned int menuChoice;

    printf("\n");
    printDivider();
    printf("  MAIN MENU\n");
    printDivider();
    printf("  1 - View all accounts\n");
    printf("  2 - Update an account\n");
    printf("  3 - Add a new account\n");
    printf("  4 - Delete an account\n");
    printf("  5 - Transfer money\n");
    printf("  6 - Transaction history\n");
    printf("  7 - View account details\n");
    printf("  8 - Exit\n");
    printDivider();
    printf("  Your choice: ");

    if (scanf("%u", &menuChoice) != 1) {
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();
    return menuChoice;
}