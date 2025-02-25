#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable :4996)

//создание структуры//
typedef struct subscribe {
	char last_name[50];
	char first_name[50];
	char middle_name[50];
	char number[20];
	float money;
	float min_money;
	char status[20];
	SUB* next;
}SUB ;
SUB* head = NULL;

//функция добавления нового клиента в конец//
void insert(char* last_name, char* first_name, char* middle_name,
	char* number, float money, float min_money, char* status) {
	SUB* new_sub = (SUB*)malloc(sizeof(SUB));

	//заполняем данные//

	strcpy(new_sub->last_name, last_name);
	strcpy(new_sub->first_name, first_name);
	strcpy(new_sub->middle_name, middle_name);
	strcpy(new_sub->number, number);
	new_sub->money = money;
	new_sub->min_money = min_money;
	strcpy(new_sub->status, status);
	new_sub->next = NULL;

	//обновляем голову//
	if (head == NULL) {
		head = new_sub;
	}
	//богдануох
	else {
		SUB* buck = head;
		while (buck->next!= NULL) buck = buck->next;
		buck->next = new_sub;
	}
}

//функция 2 select
void select(SUB)


















//int main() {
//	SUB clie;
//
//
//	strcpy(clie.name, "DAN");
//	strcpy(clie.last, "ZOL");
//	strcpy(clie.number, "123");
//	clie.money = 100000000;
//
//	printf("sub %s %s with %s sm cock have %d money", clie.name, clie.last,clie.number,clie.money);
//	return 0;
//}
