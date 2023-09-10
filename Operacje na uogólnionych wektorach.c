#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_STR_LEN 64

typedef struct Vector {
    void *data;
    size_t element_size;
    size_t size;
    size_t capacity;
} Vector;

typedef struct Person {
    int age;
    char first_name[MAX_STR_LEN];
    char last_name[MAX_STR_LEN];
} Person;

typedef int (*cmp_ptr)(const void *, const void *);
typedef int (*predicate_ptr)(void *);
typedef void (*read_ptr)(void *);
typedef void (*print_ptr)(const void *);

void init_vector(Vector *vector, size_t block_size, size_t element_size) {
    vector->data = malloc(block_size * element_size);
    vector->element_size = element_size;
    vector->size = 0;
    vector->capacity = block_size;
}

void reserve(Vector *vector, size_t new_capacity) {
    if (new_capacity > vector->capacity) {
        vector->data = realloc(vector->data, new_capacity * vector->element_size);
        vector->capacity = new_capacity;
    }
}

void resize(Vector *vector, size_t new_size) {
    if (new_size < vector->size) {
        vector->size = new_size;
    } else if (new_size > vector->size) {
        reserve(vector, new_size);
        size_t additional_elements = new_size - vector->size;
        memset((char *)vector->data + vector->size * vector->element_size, 0, additional_elements * vector->element_size);
        vector->size = new_size;
    }
}

void push_back(Vector *vector, void *value) {
    if (vector->size == vector->capacity) {
        reserve(vector, vector->capacity * 2);
    }
    memcpy((char *)vector->data + vector->size * vector->element_size, value, vector->element_size);
    vector->size++;
}

void clear(Vector *vector) {
    vector->size = 0;
}

void insert(Vector *vector, size_t idx, void *val) {
    if (idx > vector->size) {
        return;
    }
    if (vector->size == vector->capacity) {
        reserve(vector, vector->capacity << 1);
    }
    memmove(&((char *)vector->data)[(idx + 1) * vector->element_size],
            &((char *)vector->data)[idx * vector->element_size],
            (vector->size - idx) * vector->element_size);
    memcpy(&((char *)vector->data)[idx * vector->element_size], val, vector->element_size);
    vector->size++;
}

void erase(Vector *vector, size_t idx) {
    if (idx >= vector->size) {
        return;
    }
    memmove(&((char *)vector->data)[idx * vector->element_size],
            &((char *)vector->data)[(idx + 1) * vector->element_size],
            (vector->size - idx - 1) * vector->element_size);
    vector->size--;
}

void erase_value(Vector *vector, void *value, cmp_ptr cmp) {
    size_t index = 0;
    while (index < vector->size) {
        if (cmp((char *)vector->data + index * vector->element_size, value) == 0) {
            erase(vector, index);
        } else {
            index++;
        }
    }
}

void erase_if(Vector *vector, int (*cond)(void *)) {
    for (size_t i = 0; i < vector->size;) {
        void *element = (char *)vector->data + i * vector->element_size;
        if (cond(element)) {
            erase(vector, i);
        } else {
            ++i;
        }
    }
}

void shrink_to_fit(Vector *vector) {
    if (vector->size < vector->capacity) {
        void *new_data = realloc(vector->data, vector->size * vector->element_size);
        if (new_data) {
            vector->data = new_data;
            vector->capacity = vector->size;
        }
    }
}

int int_cmp(const void *v1, const void *v2) {
    int *i1 = (int *)v1;
    int *i2 = (int *)v2;
    return (*i1 > *i2) - (*i1 < *i2);
}

int char_cmp(const void *v1, const void *v2) {
    char *c1 = (char *)v1;
    char *c2 = (char *)v2;
    return (*c1 > *c2) - (*c1 < *c2);
}

int person_cmp(const void *p1, const void *p2) {
    const Person *person1 = (const Person *)p1;
    const Person *person2 = (const Person *)p2;

    if (person1->age > person2->age)
        return -1;
    else if (person1->age < person2->age)
        return 1;
    else {
        int cmp_result = strcmp(person1->first_name, person2->first_name);
        if (cmp_result != 0)
            return cmp_result;
        else
            return strcmp(person1->last_name, person2->last_name);
    }
}

int is_even(void *value) {
    int *num = (int *)value;
    return (*num % 2) == 0;
}

int is_vowel(void *value) {
    char *c = (char *)value;
    return *c == 'a' || *c == 'e' || *c == 'i' || *c == 'o' || *c == 'u' || *c == 'y' ||
           *c == 'A' || *c == 'E' || *c == 'I' || *c == 'O' || *c == 'U' || *c == 'Y';
}

int is_older_than_25(void *person) {
    Person *p = (Person *)person;
    return p->age > 25;
}

void print_int(const void *v) {
    int *num = (int *)v;
    printf("%d ", *num);
}

void print_char(const void *v) {
    char *c = (char *)v;
    printf("%c ", *c);
}

void print_person(const void *v) {
    const Person *person = (const Person *)v;
    printf("%d %s %s\n", person->age, person->first_name, person->last_name);
}

void print_vector(Vector *vector, print_ptr print) {
    printf("%zu\n", vector->capacity);
    for (size_t i = 0; i < vector->size; i++) {
        print((char *)vector->data + i * vector->element_size);
    }
    printf("\n");
}

void read_int(void *value) {
    int *num = (int *)value;
    scanf("%d", num);
}

void read_char(void *value) {
    char *c = (char *)value;
    scanf(" %c", c);
}

void read_person(void *value) {
    Person *person = (Person *)value;
    scanf("%d", &(person->age));
    scanf("%s", person->first_name);
    scanf("%s", person->last_name);
}

void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void vector_test(Vector *vector, size_t block_size, size_t elem_size, int n, read_ptr read,
                 cmp_ptr cmp, predicate_ptr predicate, print_ptr print) {
    init_vector(vector, block_size, elem_size);
    void *v = (void *)safe_malloc(vector->element_size);
    size_t index, size;
    for (int i = 0; i < n; ++i) {
        char op;
        scanf(" %c", &op);
        switch (op) {
            case 'p': // push_back
                read(v);
                push_back(vector, v);
                break;
            case 'i': // insert
                scanf("%zu", &index);
                read(v);
                insert(vector, index, v);
                break;
            case 'e': // erase
                scanf("%zu", &index);
                erase(vector, index);
                break;
            case 'v': // erase
                read(v);
                erase_value(vector, v, cmp);
                break;
            case 'd': // erase (predicate)
                erase_if(vector, predicate);
                break;
            case 'r': // resize
                scanf("%zu", &size);
                resize(vector, size);
                break;
            case 'c': // clear
                clear(vector);
                break;
            case 'f': // shrink
                shrink_to_fit(vector);
                break;
            case 's': // sort
                qsort(vector->data, vector->size, vector->element_size, cmp);
                break;
            default:
                printf("No such operation: %c\n", op);
                break;
        }
    }
    print_vector(vector, print);
    free(vector->data);
    free(v);
}

int main(void) {
    int to_do, n;
    Vector vector_int, vector_char, vector_person;

    scanf("%d%d", &to_do, &n);

    switch (to_do) {
        case 1:
            vector_test(&vector_int, 4, sizeof(int), n, read_int, int_cmp, is_even, print_int);
            break;
        case 2:
            vector_test(&vector_char, 2, sizeof(char), n, read_char, char_cmp, is_vowel, print_char);
            break;
        case 3:
            vector_test(&vector_person, 2, sizeof(Person), n, read_person, person_cmp, is_older_than_25, print_person);
            break;
        default:
            printf("Nothing to do for %d\n", to_do);
            break;
    }

    return 0;
}
