#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable :4996)
int free_k = 0, malloc_k = 0, realloc_k = 0, calloc_k = 0;

// Определение статусов через enum
typedef enum {
    normal,
    only_incoming_calls,
    no_calls,
    disabled
} Status;

// Функция преобразования строки в enum
Status enum_from_string(char* str) {
    if (strcmp(str, "normal") == 0) return normal;
    if (strcmp(str, "only_incoming_calls") == 0) return only_incoming_calls;
    if (strcmp(str, "no_calls") == 0) return no_calls;
    if (strcmp(str, "disabled") == 0) return disabled;
    return 0;
}

// Функция преобразования enum в строку
const char* enum_to_string(Status status) {
    switch (status) {
    case normal: return "normal";
    case only_incoming_calls: return "only_incoming_calls";
    case no_calls: return "no_calls";
    case disabled: return "disabled";
    default: return "unknown";
    }
}

// Структура абонента
typedef struct SUB {
    char last_name[50];
    char first_name[50];
    char middle_name[50];
    char number[20];
    float money;
    float min_money;
    Status status;
    struct SUB* next;
} SUB;

SUB* head = NULL;
FILE* output_file = NULL;

// Функция проверки корректности ввода денежных значений
int is_valid_money(const char* str) {
    int has_digit = 0;
    int dot_count = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            has_digit = 1;
        } else if (str[i] == '.') {
            if (dot_count++) return 0;
        } else {
            return 0;
        }
    }

    return has_digit;
}

// Функция преобразования строки в число с плавающей точкой
float parse_float(const char* str) {
    float result = 0.0;
    float fraction = 0.1;
    int sign = 1;
    int i = 0;

    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    if (str[i] == '.') {
        i++;
        while (str[i] >= '0' && str[i] <= '9') {
            result += (str[i] - '0') * fraction;
            fraction *= 0.1;
            i++;
        }
    }

    return result * sign;
}

// Функция открытия файла для вывода
void open_output_file() {
    output_file = fopen("output.txt", "w");
    if (!output_file) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }
}

// Функция записи в консоль и файл
void write_output(const char* message) {
    printf("%s", message);
    if (output_file) {
        fprintf(output_file, "%s", message);
        fflush(output_file);
    }
}

// Функция добавления нового абонента
void insert(const char* inf) {
    char last_name[50] = "", first_name[50] = "", middle_name[50] = "", number[20] = "", status_str[20] = "";
    float money = 0, min_money = 0;

    int parsed = sscanf(inf, "last_name=%49[^,], first_name=%49[^,], middle_name=%49[^,], number=%19[^,], money=%f, min_money=%f, status=%19s",
                        last_name, first_name, middle_name, number, &money, &min_money, status_str);

    if (parsed < 7) {
        char error_msg[30];
        snprintf(error_msg, sizeof(error_msg), "incorrect: %.20s\n", inf);
        write_output(error_msg);
        return;
    }

    SUB* new_sub = (SUB*)malloc(sizeof(SUB));
    if (!new_sub) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    malloc_k++;

    Status status = enum_from_string(status_str);
    strcpy(new_sub->last_name, last_name);
    strcpy(new_sub->first_name, first_name);
    strcpy(new_sub->middle_name, middle_name);
    strcpy(new_sub->number, number);
    new_sub->money = money;
    new_sub->min_money = min_money;
    new_sub->status = status;
    new_sub->next = NULL;

    if (head == NULL) {
        head = new_sub;
    } else {
        SUB* current = head;
        while (current->next != NULL) current = current->next;
        current->next = new_sub;
    }

    int count = 0;
    SUB* temp = head;
    while (temp) {
        count++;
        temp = temp->next;
    }

    char buf[50];
    snprintf(buf, sizeof(buf), "select: %d\n", count);
    write_output(buf);
}

// Функция проверки условия
int cond(const SUB* sub, const char* cond) {
    char what[20], op[3], val[50];

    if (sscanf(cond, "%19[^<>=!]%2[<>=!]%49s", what, op, val) != 3) {
        return 0;
    }

    if (strcmp(what, "last_name") == 0) {
        if (strcmp(op, "=") == 0) return strcmp(sub->last_name, val) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(sub->last_name, val) != 0;
    }
    if (strcmp(what, "first_name") == 0) {
        if (strcmp(op, "=") == 0) return strcmp(sub->first_name, val) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(sub->first_name, val) != 0;
    }
    if (strcmp(what, "middle_name") == 0) {
        if (strcmp(op, "=") == 0) return strcmp(sub->middle_name, val) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(sub->middle_name, val) != 0;
    }
    if (strcmp(what, "number") == 0) {
        if (strcmp(op, "=") == 0) return strcmp(sub->number, val) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(sub->number, val) != 0;
    }
    if (strcmp(what, "money") == 0 || strcmp(what, "min_money") == 0) {
        float value = parse_float(val);
        float field_val = (strcmp(what, "money") == 0) ? sub->money : sub->min_money;

        if (strcmp(op, ">") == 0) return field_val > value;
        if (strcmp(op, "<") == 0) return field_val < value;
        if (strcmp(op, "=") == 0) return field_val == value;
        if (strcmp(op, "!=") == 0) return field_val != value;
        if (strcmp(op, ">=") == 0) return field_val >= value;
        if (strcmp(op, "<=") == 0) return field_val <= value;
    }

    if (strcmp(what, "status") == 0) {
        if (strcmp(op, "=") == 0) return strcmp(enum_to_string(sub->status), val) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(enum_to_string(sub->status), val) != 0;
        if (strcmp(op, "in") == 0) {
            char* values = strchr(val, '[');
            if (!values) return 0;
            values++;

            char* token = strtok(values, "[],' ");
            while (token) {
                if (strcmp(enum_to_string(sub->status), token) == 0) return 1;
                token = strtok(NULL, "[],' ");
            }
            return 0;
        }
    }

    return 0;
}

// Функция выполнения команды select
void select(const char* uslov) {
    char fields[100], condit[100] = "";

    int parsed = sscanf(uslov, "%99[^ ] %99[^\n]", fields, condit);
    if (parsed == 1) strcpy(condit, "");

    char* selected_fields[10];
    int field_count = 0;

    char* tok = strtok(fields, ",");
    while (tok) {
        while (*tok == ' ') tok++;
        selected_fields[field_count++] = tok;
        tok = strtok(NULL, ",");
    }

    int count = 0;
    SUB* temp = head;

    while (temp) {
        int match = 1;
        if (strlen(condit) > 0) {
            char* condit_tok = strtok(condit, " ");
            while (condit_tok) {
                if (!cond(temp, condit_tok)) {
                    match = 0;
                    break;
                }
                condit_tok = strtok(NULL, " ");
            }
        }

        if (match) {
            char line[256] = "";
            for (int i = 0; i < field_count; i++) {
                char field_data[100] = "";
                if (strcmp(selected_fields[i], "last_name") == 0) snprintf(field_data, sizeof(field_data), "last_name=%s ", temp->last_name);
                else if (strcmp(selected_fields[i], "first_name") == 0) snprintf(field_data, sizeof(field_data), "first_name=%s ", temp->first_name);
                else if (strcmp(selected_fields[i], "middle_name") == 0) snprintf(field_data, sizeof(field_data), "middle_name=%s ", temp->middle_name);
                else if (strcmp(selected_fields[i], "number") == 0) snprintf(field_data, sizeof(field_data), "number=%s ", temp->number);
                else if (strcmp(selected_fields[i], "money") == 0) snprintf(field_data, sizeof(field_data), "money=%.2f ", temp->money);
                else if (strcmp(selected_fields[i], "min_money") == 0) snprintf(field_data, sizeof(field_data), "min_money=%.2f ", temp->min_money);
                else if (strcmp(selected_fields[i], "status") == 0) snprintf(field_data, sizeof(field_data), "status=%s ", enum_to_string(temp->status));

                strcat(line, field_data);
            }
            strcat(line, "\n");

            write_output(line);
            count++;
        }
        temp = temp->next;
    }

    char buf[50];
    snprintf(buf, sizeof(buf), "select: %d\n", count);
    write_output(buf);
}

// Функция выполнения команды delete
void delete(const char* condit) {
    SUB* temp = head;
    SUB* prev = NULL;

    int k = 0;
    while (temp) {
        if (cond(temp, condit)) {
            if (prev) {
                prev->next = temp->next;
            } else {
                head = temp->next;
            }
            SUB* to_delete = temp;
            temp = temp->next;
            free(to_delete);
            free_k++;
            k++;
        } else {
            prev = temp;
            temp = temp->next;
        }
    }

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "delete: %d\n", k);
    write_output(buffer);
}

// Функция выполнения команды update
void update(const char* uslov) {
    char up[99], condit[99];
    sscanf(uslov, "%99[^ ] %99[^\n]", up, condit);

    int k = 0;
    SUB* temp = head;

    while (temp) {
        if (strlen(condit) == 0 || cond(temp, condit)) {
            char* tok = strtok(up, ",");
            while (tok) {
                char fie[50], val[50];
                sscanf(tok, "%49[^=]=%49s", fie, val);

                if (strcmp(fie, "money") == 0) temp->money = parse_float(val);
                else if (strcmp(fie, "min_money") == 0) temp->min_money = parse_float(val);
                else if (strcmp(fie, "status") == 0) temp->status = enum_from_string(val);
                else if (strcmp(fie, "number") == 0) strncpy(temp->number, val, sizeof(temp->number));

                tok = strtok(NULL, ",");
            }
            k++;
        }
        temp = temp->next;
    }

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "update: %d\n", k);
    write_output(buffer);
}

// Функция выполнения команды uniq
void uniq(const char* arr) {
    char* fields[10];
    int field_count = 0;

    char* token = strtok(arr, ",");
    while (token) {
        fields[field_count++] = token;
        token = strtok(NULL, ",");
    }

    int count = 0;
    SUB* current = head;

    while (current) {
        SUB* runner = current;
        while (runner->next) {
            int duplicate = 1;

            for (int i = 0; i < field_count; i++) {
                char* field = fields[i];

                if (strcmp(field, "last_name") == 0 && strcmp(current->last_name, runner->next->last_name) != 0)
                    duplicate = 0;
                if (strcmp(field, "first_name") == 0 && strcmp(current->first_name, runner->next->first_name) != 0)
                    duplicate = 0;
                if (strcmp(field, "number") == 0 && strcmp(current->number, runner->next->number) != 0)
                    duplicate = 0;
                if (strcmp(field, "money") == 0 && (current->money != runner->next->money))
                    duplicate = 0;
                if (strcmp(field, "min_money") == 0 && (current->min_money != runner->next->min_money))
                    duplicate = 0;
                if (strcmp(field, "status") == 0 && strcmp(enum_to_string(current->status), enum_to_string(runner->next->status)) != 0)
                    duplicate = 0;
            }

            if (duplicate) {
                SUB* duplicate_node = runner->next;
                runner->next = runner->next->next;
                free(duplicate_node);
                free_k++;
                count++;
            } else {
                runner = runner->next;
            }
        }
        current = current->next;
    }

    char buf[50];
    snprintf(buf, sizeof(buf), "uniq: %d\n", count);
    write_output(buf);
}

// Функция обработки команд
void process_command(const char* line) {
    if (strncmp(line, "insert ", 7) == 0) {
        insert(line + 7);
    } else if (strncmp(line, "select ", 7) == 0) {
        select(line + 7);
    } else if (strncmp(line, "delete ", 7) == 0) {
        delete(line + 7);
    } else if (strncmp(line, "update ", 7) == 0) {
        update(line + 7);
    } else if (strncmp(line, "uniq ", 5) == 0) {
        uniq(line + 5);
    } else {
        printf("incorrect: %.20s\n", line);
    }
}

// Функция чтения команд из консоли
void read_commands_from_console() {
    char line[256];

    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0) {
            process_command(line);
        }
    }
}

// Функция чтения команд из файла
void read_commands_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) > 0) {
            process_command(line);
        }
    }

    fclose(file);
}

// Функция сохранения статистики памяти
void save_memory_stat() {
    FILE* file = fopen("memstat.txt", "w");
    if (!file) {
        perror("Failed to open memstat file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "malloc:%d\nrealloc:%d\ncalloc:%d\nfree:%d\n", malloc_k, realloc_k, calloc_k, free_k);
    fclose(file);
}

int main(int argc, char* argv[]) {
    open_output_file();

    if (argc > 1) {
        read_commands_from_file(argv[1]);
    } else {
        read_commands_from_console();
    }

    save_memory_stat();

    if (output_file) fclose(output_file);

    return 0;
}