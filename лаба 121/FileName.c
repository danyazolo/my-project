
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable :4996)
int free_k = 0, malloc_k = 0, realloc_k = 0, calloc_k = 0;


//Определение статусов через enum
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
char* enum_to_string(Status status) {
    switch (status) {
    case normal: return "normal";
    case only_incoming_calls: return "only_incoming_calls";
    case no_calls: return "no_calls";
    case disabled: return "disabled";
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

// функция проверяет не ввели ли в money чтото кроме цифр и одной точки
int prov(char* str) {
    int flag = 0;
    int flag_dot = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            flag = 1;
        }
        else if (str[i] == '.') {
            if (flag_dot) return 0;
            flag_dot = 1;
        }
        else {
            return 0;
        }
    }

    return flag;
}
float prost(char* str) {
    float result = 0.0;   // итоговое число
    float fraction = 0.1; // разряды для дробной части
    int sign = 1;         // знак числа
    int i = 0;            // индекс строки

    //богдан лох
    if (str[i] == '-') {
        sign = -1;
        i++;
    }
    else if (str[i] == '+') {
        i++;
    }
    // читаем целую часть
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    // читаем дробную часть 
    if (str[i] == '.') {
        i++;
        while (str[i] >= '0' && str[i] <= '9') {
            result += (str[i] - '0') * fraction;
            fraction *= 0.1;
            i++;
        }
    }

    // возвращаем число с учетом знака
    return result * sign;
}
FILE* output_file = NULL;

void open_output_file() {
    output_file = fopen("output.txt", "w");
}
void write_output(char* message) {
    printf("%s", message);
    if (output_file) {
        fprintf(output_file, "%s", message);  // Запись в файл
        fflush(output_file);  // Принудительная запись
    }
}

void insert(char* inf) {
    char last_name[50] = "", first_name[50] = "", middle_name[50] = "", number[20] = "", status_str[20] = "";
    float money = 0, min_money = 0;

    // Используем sscanf() для безопасного разбора строки
    int parsed = sscanf(inf, "last_name=%49[^,], first_name=%49[^,], middle_name=%49[^,], number=%19[^,], money=%f, min_money=%f, status=%19s",
        last_name, first_name, middle_name, number, &money, &min_money, status_str);

    // Проверяем, все ли нужные поля были считаны
    if (parsed < 7) {

        char error_msg[30];
        snprintf(error_msg, sizeof(error_msg), "incorrect: %.20s\n", inf);
        write_output(error_msg);
        return;
    }

    // Создаем новый элемент списка
    SUB* new_sub = (SUB*)malloc(sizeof(SUB));
    malloc_k++;


    Status status = enum_from_string(status_str);
    strcpy(new_sub->last_name, last_name);
    strcpy(new_sub->first_name, first_name);
    strcpy(new_sub->middle_name, middle_name);
    strcpy(new_sub->number, number);
    new_sub->money = money;
    new_sub->min_money = min_money;
    new_sub->status=status;
    new_sub->next = NULL;

    /*printf("Final values: %s %s %s %s %.2f %.2f %s\n",
        new_sub->last_name, new_sub->first_name, new_sub->middle_name, new_sub->number,
        new_sub->money, new_sub->min_money, enum_to_string(new_sub->status));*/

    // Добавляем в конец списка
    if (head == NULL) {
        head = new_sub;
    }
    else {
        SUB* buck = head;
        while (buck->next != NULL) buck = buck->next;
        buck->next = new_sub;
    }

    // Подсчет количества элементов
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




//сначала условия 
int cond(SUB* sub, char* cond) {
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
        float value = prost(val);
        float field_val = (strcmp(what, "money") == 0) ? sub->money : sub->min_money;

        if (strcmp(op, ">") == 0) return field_val > value;
        if (strcmp(op, "<") == 0) return field_val < value;
        if (strcmp(op, "=") == 0) return field_val == value;
        if (strcmp(op, "!=") == 0) return field_val != value;
        if (strcmp(op, ">=") == 0) return field_val >= value;
        if (strcmp(op, "<=") == 0) return field_val <= value;
    }


    // Проверяем строковые поля
    if (strcmp(what, "status") == 0) {
        if (strcmp(op, "=") == 0) return strcmp(enum_to_string(sub->status), val) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(enum_to_string(sub->status), val) != 0;
        if (strcmp(op, "in") == 0) {
            // Убираем скобки
            char* values = strchr(val, '[');
            if (!values) return 0;
            values++; // Пропустить '['

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


//с условиями селект
void select(char* uslov) {
    char fields[100], condit[100] = "";

    int parsed = sscanf(uslov, "%99[^ ] %99[^\n]", fields, condit);
    if (parsed == 1) strcpy(condit, "");  // Если условия нет, очищаем

    char* selected_fields[10];  // Массив указателей для полей
    int field_count = 0;        // Количество полей

    char* tok = strtok(fields, ",");
    while (tok) {
        while (*tok == ' ') tok++;  // Убираем пробелы перед словом
        selected_fields[field_count++] = tok;
        tok = strtok(NULL, ",");
    }
    /*printf("[DEBUG] Selected fields: ");
    for (int i = 0; i < field_count; i++) {
      printf("'%s' ", selected_fields[i]);
    }*/
    printf("\n");


    int count = 0;
    SUB* temp = head;

    while (temp) {
        int match = 1;
        if (strlen(condit) > 0) {
            char* condit_tok = strtok(condit, " ");
            while (condit_tok) {
                if (!cond(temp, condit_tok)) {
                    match = 0; //значит не то
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

void delete(char* condit) {
    SUB* temp = head;
    SUB* buf = NULL;

    int k = 0;
    //запись в начале
    while (temp && cond(temp, condit)) {
        head = temp->next;
        free(temp);
        free_k++;
        temp = head;
        k++;
    }

    //запись не в начале
    while (temp) {
        //этот цикл пробегает все хорошие записи и останавливается когда нашел плохую запись
        while (temp && !cond(temp, condit)) {
            buf = temp;
            temp = temp->next;
        }

        if (!temp) break;//если конец списка

        //ну и удаляем
        buf->next = temp->next;
        free(temp);
        free_k++;
        temp = buf->next;
        k++;
    }

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "delete: %d\n", k);
    write_output(buffer);
}

//апдейт
void update(char* uslov) {
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


                if (strcmp(fie, "money") == 0) temp->money = prost(val);
                else if (strcmp(fie, "min_money") == 0) temp->min_money = prost(val);
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
void uniq(char* arr) {
    char* fields[10];
    int field_count = 0;


    char* token = strtok(arr, ",");
    while (token) {
        fields[field_count++] = token;
        token = strtok(NULL, ",");
    }

    int count = 0;
    SUB* current = head;

    // 2. Проходим по списку
    while (current) {
        SUB* runner = current;
        while (runner->next) {
            int duplicate = 1;

            // 3. Проверяем, совпадают ли поля
            for (int i = 0; i < field_count; i++) {
                char* field = fields[i];

                if (strcmp(field, "last_name") == 0 && strcmp(current->last_name, runner->next->last_name) != 0)
                    duplicate = 0;
                if (strcmp(field, "first_name") == 0 && strcmp(current->first_name, runner->next->first_name) != 0)
                    duplicate = 0;
                if (strcmp(field, "number") == 0 && strcmp(current->number, runner->next->number) != 0)
                    duplicate = 0;
            }

            // 4. Если дубликат найден, удаляем старый узел
            if (duplicate) {
                SUB* duplicate_node = runner->next;
                runner->next = runner->next->next;
                free(duplicate_node);
                free_k++;
                count++;
            }
            else {
                runner = runner->next;
            }
        }
        current = current->next;
    }

    char buf[50];
    snprintf(buf, sizeof(buf), "update: %d\n", count);
    write_output(buf);
}

void process_command(char* line) {
    if (strncmp(line, "insert ", 7) == 0) {
        insert(line + 7);
    }
    else if (strncmp(line, "select ", 7) == 0) {  // Добавили пробел lol
        select(line + 7);
    }
    else if (strncmp(line, "delete ", 7) == 0) {
        delete(line + 7);
    }
    else if (strncmp(line, "update ", 7) == 0) {
        update(line + 7);
    }
    else if (strncmp(line, "uniq ", 5) == 0) {
        uniq(line + 5);
    }
    else {
        printf("incorrect: %.20s\n", line);
    }
}




//// Функция записи в консоль и файл
//void write_output(char* message) {
//  if (output_file) {
//    fprintf(output_file, "%s", message);  // Запись в файл
//    fflush(output_file);  // Принудительная запись
//  }
//}
void read_commands_from_console() {
    char line[256];

    while (1) {
        if (!fgets(line, sizeof(line), stdin)) {
            break;  // Если ввод прерван (EOF или Ctrl+C) — выходим
        }

        line[strcspn(line, "\n")] = '\0';  // Удаляем символ новой строки
        if (strlen(line) > 0) {  // Игнорируем пустые строки
            process_command(line);
        }
    }
}

void read_commands_from_file(char* filename) {
    FILE* file = fopen(filename, "r");

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  // Удаление символа новой строки
        if (strlen(line) > 0) {            // Игнорируем пустые строки
            process_command(line);
        }
    }

    fclose(file);
}

// Функция сохранения статистики памяти
void save_memory_stat() {
    FILE* file = fopen("memstat.txt", "w");

    fprintf(file, "malloc:%d\nrealloc:%d\ncalloc:%d\nfree:%d\n ", malloc_k, realloc_k, calloc_k, free_k);
    fclose(file);
}

// Основная функция
int main(int argc, char* argv[]) {
    open_output_file();  // Открываем файл для вывода

    if (argc > 1) {
        read_commands_from_file(argv[1]);  // Читаем команды из файла
    }
    else {
        read_commands_from_console();      // Читаем команды из консоли
    }

    save_memory_stat();  // Сохраняем статистику памяти

    if (output_file) fclose(output_file);  // Закрываем output.txt

    return 0;
}
