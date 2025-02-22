#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable :4996)

#define MAX_LINE 256

// Структура записи абонента
typedef struct Subscriber {
    char last_name[50];
    char first_name[50];
    char middle_name[50];
    char number[15];
    float money;
    float min_money;
    char status[25];
    struct Subscriber* next;
} Subscriber;

Subscriber* head = NULL;  // Голова списка (БД)

// Подсчет вызовов malloc/free
int malloc_count = 0, free_count = 0;

// Обертки для контроля памяти
void* my_malloc(size_t size) {
    malloc_count++;
    return malloc(size);
}

void my_free(void* ptr) {
    if (ptr) {
        free_count++;
        free(ptr);
    }
}

// Функция вставки записи
void insert(char* last_name, char* first_name, char* middle_name, char* number, float money, float min_money, char* status) {
    Subscriber* new_sub = (Subscriber*)my_malloc(sizeof(Subscriber));
    if (!new_sub) return;

    strcpy(new_sub->last_name, last_name);
    strcpy(new_sub->first_name, first_name);
    strcpy(new_sub->middle_name, middle_name);
    strcpy(new_sub->number, number);
    new_sub->money = money;
    new_sub->min_money = min_money;
    strcpy(new_sub->status, status);
    new_sub->next = NULL;

    // Вставляем в конец списка
    if (!head) {
        head = new_sub;
    }
    else {
        Subscriber* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = new_sub;
    }

    printf("insert: OK\n");
}

// Функция вывода всех записей
void select_all() {
    Subscriber* temp = head;
    int count = 0;

    while (temp) {
        printf("%s %s %s %s %.2f %.2f %s\n",
            temp->last_name, temp->first_name, temp->middle_name,
            temp->number, temp->money, temp->min_money, temp->status);
        temp = temp->next;
        count++;
    }
    printf("select: %d\n", count);
}

// Функция обновления счета абонента
void update_money(char* number, float new_money) {
    Subscriber* temp = head;
    int count = 0;

    while (temp) {
        if (strcmp(temp->number, number) == 0) {
            temp->money = new_money;
            count++;
        }
        temp = temp->next;
    }
    printf("update: %d\n", count);
}

// Функция удаления абонента
void delete_record(char* number) {
    Subscriber* temp = head, * prev = NULL;
    int count = 0;

    while (temp) {
        if (strcmp(temp->number, number) == 0) {
            if (prev) prev->next = temp->next;
            else head = temp->next;
            my_free(temp);
            count++;
            break;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("delete: %d\n", count);
}

// Функция обработки команд
void execute_command(char* line) {
    if (strncmp(line, "insert", 6) == 0) {
        char last_name[50], first_name[50], middle_name[50], number[15], status[25];
        float money, min_money;
        sscanf(line, "insert last_name=%[^,],first_name=%[^,],middle_name=%[^,],number=%[^,],money=%f,min_money=%f,status=%s",
            last_name, first_name, middle_name, number, &money, &min_money, status);
        insert(last_name, first_name, middle_name, number, money, min_money, status);
    }
    else if (strcmp(line, "select_all") == 0) {
        select_all();
    }
    else if (strncmp(line, "update_money", 12) == 0) {
        char number[15];
        float new_money;
        sscanf(line, "update_money number=%[^,],new_money=%f", number, &new_money);
        update_money(number, new_money);
    }
    else if (strncmp(line, "delete", 6) == 0) {
        char number[15];
        sscanf(line, "delete number=%s", number);
        delete_record(number);
    }
    else {
        printf("incorrect: %.20s\n", line);
    }
}

// Функция чтения команд из файла
void process_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return;

    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            execute_command(line);
        }
    }

    fclose(file);
}

// Запись статистики памяти
void save_memstat() {
    FILE* file = fopen("memstat.txt", "w");
    if (!file) return;
    fprintf(file, "malloc: %d\nfree: %d\n", malloc_count, free_count);
    fclose(file);
}

// Освобождение памяти перед выходом
void delete_all() {
    Subscriber* temp;
    while (head) {
        temp = head;
        head = head->next;
        my_free(temp);
    }
}

// Главная функция
int main() {
    process_file("input.txt");
    delete_all();
    save_memstat();
    return 0;
}
