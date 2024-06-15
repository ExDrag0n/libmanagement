#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef enum { ADMINISTRATOR, USER } UserType;
typedef struct {
    char name[50];
    char surname[50];
    char patronymic[50];
    char email[100];
    char password[50];
    char phone[20];
    char passport_data[100];
    int employee_code;
    UserType user_type;
} Employee;
typedef struct {
    char book_name[50];
    int available_count;
    int borrowed_count;
} LibraryBook;
LibraryBook books[100]; // Declare books globally
int book_count = 0; // Declare book_count globally
int has_access(Employee users[], int user_count, UserType required_type) {
    for (int i = 0; i < user_count; i++) {
        if (users[i].user_type == required_type) {
            return 1;
        }
    }
    return 0;
}
struct Date {
    int day;
    int month;
    int year;
};
struct Date calculate_due_date(struct Date* current_date) {
    struct Date due_date = *current_date;
    due_date.month += 1;

    if (due_date.month > 12) {
        due_date.month = 1;
        due_date.year += 1;
    }

    return due_date;
}

void save_employee(Employee emp) {
    FILE *file = fopen("employees.txt", "a");
    if (file == NULL) return;
    fprintf(file, "%s %s %s %s %s %s %s %d\n", emp.name, emp.surname, emp.patronymic, emp.email, emp.password, emp.phone, emp.passport_data, emp.employee_code);
    fclose(file);
}
void add_book(LibraryBook books[], int *book_count, LibraryBook new_book) {
    books[*book_count] = new_book;
    (*book_count)++;
}
int find_book(LibraryBook books[], int book_count, const char* book_name) {
    for (int i = 0; i < book_count; i++) {
        if (strcmp(books[i].book_name, book_name) == 0) {
            return i;
        }
    }
    return -1;
}
int check_availability(LibraryBook books[], int book_count, const char* book_name) {
    int index = find_book(books, book_count, book_name);
    if (index!= -1) {
        return books[index].available_count > 0? 1 : 0;
    }
    return 0;
}
void return_book(LibraryBook books[], int book_count, const char* book_name) {
    int index = find_book(books, book_count, book_name);
    if (index!= -1 && books[index].borrowed_count > 0) {
        books[index].available_count++;
        books[index].borrowed_count--;
        printf("Книга '%s' успешно возвращена.\n", book_name);
    } else {
        printf("Ошибка: книга '%s' не найдена или уже возвращена.\n", book_name);
    }
}
Employee load_employee(char* email) {
    Employee emp;
    FILE *file = fopen("employees.txt", "r");
    if (file == NULL) return emp;
    while (fscanf(file, "%*s %*s %*s %*s %*s %*s %*s %d", emp.name, emp.surname, emp.patronymic, emp.email, emp.password, emp.phone, emp.passport_data, &emp.employee_code)!= EOF) {
        if (strcmp(emp.email, email) == 0) {
            fclose(file);
            return emp;
        }
    }
    fclose(file);
    return emp;
}
void print_all_books(LibraryBook books[], int book_count) {
    for (int i = 0; i < book_count; i++) {
        printf("Название книги: %s\n", books[i].book_name);
        printf("Количество книг в наличии: %d\n", books[i].available_count);
        printf("Количество книг, взято в аренду: %d\n", books[i].borrowed_count);
        printf("-----------------------------\n");
    }
}
void serialize_books(LibraryBook books[], int book_count, FILE* file) {
    for (int i = 0; i < book_count; i++) {
        fprintf(file, "%s,%d,%d\n", books[i].book_name, books[i].available_count, books[i].borrowed_count);
    }
}
void deserialize_books(LibraryBook books[], int* book_count, FILE* file) {
    rewind(file); // Перемещаем указатель файла в начало
    while (!feof(file)) {
        fscanf(file, "%[^,],%d,%d\n", books[*book_count].book_name, &books[*book_count].available_count, &books[*book_count].borrowed_count);
        (*book_count)++;
    }
}
void take_book(LibraryBook books[], int book_count, int* borrowed_count, int** due_dates, int continue_selection, struct Date current_date) {
    if (continue_selection) {
        printf("Введите название книги для взятия: ");
        char book_name[100];
        scanf("%s", book_name);
        int index = find_book(books, book_count, book_name);
        if (index != -1 && books[index].available_count > 0) {
            books[index].available_count--;
            (*borrowed_count)++;
            if ((*due_dates) == NULL) {
                *due_dates = malloc(sizeof(int) * (*borrowed_count)); // Инициализация указателя на массив
            } else {
                *due_dates = realloc(*due_dates, sizeof(int) * (*borrowed_count)); // Увеличение размера массива, если необходимо
            }
            (*due_dates)[*borrowed_count - 1] = calculate_due_date(&current_date).day; // Теперь это должно работать, так как due_dates является указателем на массив
            printf("Книга '%s' успешно взята. Дата возвращения: %d\n", book_name, (*due_dates)[*borrowed_count - 1]);
            printf("Хотите взять ещё одну книгу? (1 - да, 0 - нет): ");
            int choice;
            scanf("%d", &choice);
            take_book(books, book_count, borrowed_count, due_dates, choice, current_date); // Pass current_date to the recursive call
        } else {
            printf("Книга '%s' не доступна.\n", book_name);
        }
    }
}

// Функция для входа в систему
int login(Employee employees[], int employee_count, char* email, char* password) {
    for (int i = 0; i < employee_count; i++) {
        if (strcmp(employees[i].email, email) == 0 && strcmp(employees[i].password, password) == 0) {
            return employees[i].user_type; // Возвращаем тип пользователя
        }
    }
    return -1; // Возвращаем -1, если сотрудник не найден
}
void display_borrowed_books(int borrowed_count, int* due_dates) {
    printf("Всего взято книг: %d\n", borrowed_count);
    for (int i = 0; i < borrowed_count; i++) {
        printf("Дата возвращения книги %d: %d\n", i + 1, due_dates[i]);
    }
}
// Функция для регистрации нового сотрудника
void register_employee(Employee employees[], int* employee_count, Employee new_emp) {
    // Запрос данных от пользователя
    printf("Введите имя: ");
    scanf("%s", new_emp.name);
    printf("Введите фамилию: ");
    scanf("%s", new_emp.surname);
    printf("Введите отчество: ");
    scanf("%s", new_emp.patronymic);
    printf("Введите email: ");
    scanf("%s", new_emp.email);
    printf("Введите пароль: ");
    scanf("%s", new_emp.password);
    printf("Введите телефон: ");
    scanf("%s", new_emp.phone);
    printf("Введите паспортные данные: ");
    scanf("%s", new_emp.passport_data);
    printf("Введите код сотрудника: ");
    scanf("%d", &new_emp.employee_code);
    new_emp.user_type = ADMINISTRATOR;
    // Сохраняем нового сотрудника в массив и файл
    employees[*employee_count] = new_emp;
    (*employee_count)++;
    save_employee(new_emp); // Используем ранее определенную функцию для сохранения в файл
}
int main() {
    FILE* writeFile = fopen("library.txt", "w");
    if (writeFile!= NULL) {
        serialize_books(books, book_count, writeFile);
        fclose(writeFile);
    } else {
        printf("Не удалось открыть файл для записи.\n");
    }    
    FILE* readFile = fopen("library.txt", "r"); 
    if (readFile!= NULL) {                
        deserialize_books(books, &book_count, readFile);                
        fclose(readFile);
    } else {
        printf("Файл не найден.\n");
    }
    char email[100], password[50];
    int choice, book_choice;
    Employee employees[100]; // Массив для хранения информации о сотрудниках
    int employee_count = 0; // Переменная для подсчета сотрудников
    struct Date current_date = {12, 6, 2024};
    while (1) {
        printf("Здраствуйте! Это терминальное приложение библиотеки «Знания - сила»\nПожалуйста, выберите действие:\n");
        printf("\nГлавное меню:\n");
        printf("1. Вход в систему\n");
        printf("2. Регистрация нового сотрудника\n");
        printf("3. Добавить книгу\n");
        printf("4. Взять книгу\n");
        printf("5. Вернуть книгу\n");
        printf("6. Проверить наличие книги\n");
        printf("7. Получить информацию обо всех книгах\n");
        printf("8. Выход\n");
        printf("Выберите действие: ");
        scanf("%d", &choice);
        Employee new_emp;
        switch (choice) {
            case 1: // Вход в систему
                printf("Введите email и пароль: ");
                scanf("%s %s", email, password);
                int employee_index = login(employees, employee_count, email, password);
                if (employee_index!= -1) {
                    printf("Вход выполнен.\n");
                } else {
                    printf("Неверные учетные данные.\n");
                }
                break;
            case 2:
                printf("Введите информацию о сотруднике:\n");
                register_employee(employees, &employee_count, new_emp);
                printf("Сотрудник успешно зарегистрирован.\n");
                break;
            case 3: // Добавить книгу
                if (has_access(employees, employee_count, ADMINISTRATOR)) {
                    printf("Введите название книги: ");
                    scanf("%s", books[book_count].book_name);
                    printf("Введите количество книг в наличии: ");
                    scanf("%d", &books[book_count].available_count);
                    printf("Введите количество книг, взято в аренду: ");
                    scanf("%d", &books[book_count].borrowed_count);
                    add_book(books, &book_count, books[book_count]);
                    printf("Книга успешно добавлена.\n");
                break;} 
                else 
                    {
                    printf("У вас недостаточно прав для выполнения этого действия.\n");
                    }
                    break;
                break;
            case 4: // Взять книгу
                {
                    int borrowed_count = 0;
                    int* due_dates = NULL;
                    take_book(books, book_count, &borrowed_count, &due_dates, 1, current_date);
                    display_borrowed_books(borrowed_count, due_dates);
                    free(due_dates);
                }
                break;
            case 5: // Вернуть книгу
                printf("Введите название книги: ");
                scanf("%s", books[book_count].book_name);
                return_book(books, book_count, books[book_count].book_name);
                break;
                break;
            case 6: // Проверить наличие книги
                printf("Введите название книги: ");
                scanf("%s", books[book_count].book_name);
                int availability = check_availability(books, book_count, books[book_count].book_name);
                if (availability) {
                    printf("Книга '%s' доступна.\n", books[book_count].book_name);
                } else {
                    printf("Книга '%s' не доступна.\n", books[book_count].book_name);
                }
                break;
                break;
            case 7:
                print_all_books(books, book_count);
                break;
            case 8: // Выход
                return 0;
            default:
                printf("Неверный выбор. Попробуйте еще раз.\n");
        }
    }
    return 0;
}