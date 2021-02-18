#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

typedef struct
{
    char name[128];
    char email[128];
    char password[32];

    uint16_t id;
} Account;

Account *accounts[32];
uint16_t account_buffer_size = 0;

//=============Input Buffer===================

InputBuffer *new_input_buffer()
{
    InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));

    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}
void read_input_buffer(InputBuffer *input_buffer)
{
    ssize_t byte_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    input_buffer->input_length = byte_read - 1;
    input_buffer->buffer[byte_read - 1] = 0;
}
void print_prompt(bool admin)
{
    printf("\033[34m[1] \033[0mShow my information\n\033[34m[2] \033[0mCreate account\n\033[34m[3] \033[0mLogin\n\033[34m[4] \033[0mExit\n\033[34m[5] \033[0mAdmin mode\n\n[\033[32m%s\033[0m] > ", admin ? "Admin" : "Client");
}
void close_input_buffer(InputBuffer *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}
//==============Close Input Buffer===============

//==============Account=================

Account *new_account(const char *name, const char *email, const char *password)
{
    Account *account = (Account *)malloc(sizeof(Account));

    strcpy(account->name, name);
    strcpy(account->email, email);
    strcpy(account->password, password);

    return account;
}
void show_account(Account *account)
{
    printf("\033[32mAccount ID: \033[33m%d\033[0m\n\033[32mName: \033[33m%s\033[0m\n\033[32mEmail: \033[33m%s\033[0m\n\033[32mPassword: \033[33m%s\033[0m\n\n",
           account->id,
           account->name,
           account->email,
           account->password);
}

bool compare_account_email(Account *a, Account *b)
{
    if(!strcmp(a->email, b->email))
        return true;
    return false;
}

void close_account(Account *account)
{
    free(account);
}

void create_account(const char *name, const char *email, const char *password)
{
    bool can_create = true;
    if (strlen(email) >= 6 && strlen(name) >= 3 && strlen(password) > 7)
    {

        Account *account = new_account(name, email, password);
        account->id = account_buffer_size + 1;

        accounts[account_buffer_size] = (Account *)malloc(sizeof(Account));

        for (size_t i = 0; i < account_buffer_size; i++)
            if(compare_account_email(account, accounts[i]))
            {
                puts("\033[31mThis account already exists.\033[0m");
                can_create = false;
            }
        if(!strchr(account->email, '@') || !(account->email[strlen(account->email) - 4] == '.'))
        {
            puts("\033[31mYou email is wrong.\033[0m");
            can_create = false;
        }
        if(can_create) 
        {
            accounts[account_buffer_size++] = account;
            puts("\033[32mAccount successfully created.\033[0m");
        }
    }
    else
    {
        puts("\033[31mError creating an account. Please try again.\033[0m");
    }
}
void show_all_acount()
{
    for (int i = 0; i < account_buffer_size; i++)
        show_account(accounts[i]);
}

Account *login_account(const char *email, const char *password)
{
    system("clear");

    for (int i = 0; i < account_buffer_size; i++)
        if (strcmp(accounts[i]->email, email) == 0 && strcmp(accounts[i]->password, password) == 0)
        {
            puts("\033[32m\nSuccessfully logged in.\033[0m\n");
            return accounts[i];
        }

    puts("\033[31m\nThis account is not logged.\033[0m\n");
    return NULL;
}

void get_input_for_account_create(InputBuffer *input_buffer)
{
    char name[128];
    char email[128];
    char password[32];

    printf("Name: ");
    read_input_buffer(input_buffer);
    strcpy(name, input_buffer->buffer);

    printf("Email: ");
    read_input_buffer(input_buffer);
    strcpy(email, input_buffer->buffer);

    printf("Password: ");
    read_input_buffer(input_buffer);
    strcpy(password, input_buffer->buffer);

    create_account(name, email, password);
}

void close_all_account()
{
    for (int i = 0; i < account_buffer_size; i++)
        if (account_buffer_size > 0)
            close_account(accounts[i]);
}

Account *init_account()
{
    Account *account = new_account("Undefined", "Undefined", "Undefined");
    return account;
}

//======================================

//================Command======+++======

void execute_commands(InputBuffer *input_buffer, Account **account_current, bool *running_status, bool *admin)
{
    if (!strcmp(input_buffer->buffer, "1"))
    {
        system("clear");
        if (*account_current)
            show_account(*account_current);
        else
            puts("\033[31mYou're not registered. Please create account or login\033[0m");
    }
    else if (!strcmp(input_buffer->buffer, "2"))
    {
        system("clear");
        get_input_for_account_create(input_buffer);
    }
    else if (!strcmp(input_buffer->buffer, "3"))
    {
        char email[128];
        char password[32];

        system("clear");

        printf("Email: ");
        read_input_buffer(input_buffer);
        strcpy(email, input_buffer->buffer);

        printf("Password: ");
        read_input_buffer(input_buffer);
        strcpy(password, input_buffer->buffer);

        if(login_account(email, password))
            *account_current = login_account(email, password);
    }
    else if (!strcmp(input_buffer->buffer, "4"))
    {
        system("clear");

        puts("Clean buffers...");
        close_all_account();

        *running_status = false;
    }
    else if (!strcmp(input_buffer->buffer, "5"))
    {
        system("clear");
        if (!strcmp((*account_current)->name, "Deuzivan") && !strcmp((*account_current)->email, "deuzivan@gmail.com"))
            *admin = true;
        else
            puts("\033[33mYou're not an administrator.\033[0m");
    }
    else if (!strcmp(input_buffer->buffer, "show_all_account()"))
    {
        system("clear");
        show_all_acount();
    } else {
        system("clear");
        puts("\033[31mCommand don't found..\033[0m");
    }
}

//======================================

int main()
{
    bool running = true;
    bool admin = false;

    InputBuffer *input_buffer = new_input_buffer();

    create_account("Visitante", "visitante@gmail.com", "00000000");
    Account *account_current = login_account("visitante@gmail.com", "00000000");

    create_account("Deuzivan", "deuzivan@gmail.com", "12345678");
    system("clear");

    while (running)
    {
        print_prompt(admin);
        read_input_buffer(input_buffer);

        execute_commands(input_buffer, &(account_current), &running, &admin);
    }


    close_input_buffer(input_buffer);
    return EXIT_SUCCESS;
}
