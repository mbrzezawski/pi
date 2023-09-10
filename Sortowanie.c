#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	int day; 
	int month; 
	int year;
} Date;

// 1 bsearch2

#define FOOD_MAX  30   // max. number of goods
#define ART_MAX   15   // max. length of name's name + 1
#define RECORD_MAX 40  // max. line length

typedef struct {
	char name[20];
	float price;
	int amount;
	Date valid_date;
} Food;

typedef int (*ComparFp)(const void *, const void *);

// compare dates
int cmp_date(const void *d1, const void *d2) {
    const Date *date1 = (const Date *)d1;
    const Date *date2 = (const Date *)d2;

    if (date1->year != date2->year)
        return date1->year - date2->year;
    if (date1->month != date2->month)
        return date1->month - date2->month;
    return date1->day - date2->day;
}

int compare_dates(Date date1, Date date2) {
    if (date1.year != date2.year) {
        return date1.year - date2.year;
    } else if (date1.month != date2.month) {
        return date1.month - date2.month;
    } else {
        return date1.day - date2.day;
    }
}
// compare foods
int cmp(const void *a, const void *b) {
    const Food *food1 = (const Food *)a;
    const Food *food2 = (const Food *)b;
    //return strcmp(food1->name, food2->name);

    int name_cmp = strcmp(food1->name, food2->name);
    if (name_cmp != 0) {
        return name_cmp;
    }


    if (food1->price < food2->price) {
        return -1;
    } else if (food1->price > food2->price) {
        return 1;
    }


    if (compare_dates(food1->valid_date, food2->valid_date) < 0) {
        return -1;
    } else if (compare_dates(food1->valid_date, food2->valid_date) > 0) {
        return 1;
    }

    return 0;
}

// bsearch returning address where to insert new element
void *bsearch2 (const void *key, const void *base, size_t nitems, size_t size, ComparFp compar, char *result) {
    const char* ptr = (const char*)base;
    for (size_t i = 0; i < nitems; i++) {
        if (compar(key, ptr) == 0) {
            *result = 1;
            return (void*)ptr;
        }
        ptr += size;
    }
    *result = 0;

    return (void*)ptr;
}


// print goods of given name
void print_art(Food *p, int n, char *art) {
    int i;
    for (i = 0; i < n; i++) {
        if (strcmp(p[i].name, art) == 0) {
            printf("%.2f %d %02d.%02d.%04d\n", p[i].price, p[i].amount, p[i].valid_date.day, p[i].valid_date.month, p[i].valid_date.year);
        }
    }
}

// add record to table if absent
Food* add_record(Food *tab, int *np, ComparFp compar, Food *new) {
    char result;
    Food *found = bsearch2(new, tab, *np, sizeof(Food), compar, &result);

    if (found != NULL && result == 1) {
        found->amount += new->amount;
        return found;
    } else {
        size_t index = ((const char*)found - (const char*)tab) / sizeof(Food);
        memmove(found + 1, found, (*np - index) * sizeof(Food));
        memcpy(found, new, sizeof(Food));
        (*np)++;
        return found;
    }
}


void sort_goods(Food *tab, int no) {
    int i, j;
    for (i = 0; i < no - 1; i++) {
        for (j = 0; j < no - i - 1; j++) {
            if (compare_dates(tab[j].valid_date, tab[j + 1].valid_date) > 0) {
                Food temp = tab[j];
                tab[j] = tab[j + 1];
                tab[j + 1] = temp;
            }
        }
    }
}


// read no lines of data
// calls add_record if sorted = 1 and just adds element if sorted = 0
int read_goods(Food *tab, int no, FILE *stream, int sorted) {
    int i;
    for (i = 0; i < no; i++) {
        Food new_food;
        int np=i;
        fscanf(stream, "%s %f %d %d.%d.%d", new_food.name, &new_food.price, &new_food.amount,
                   &new_food.valid_date.day, &new_food.valid_date.month, &new_food.valid_date.year);
        //printf("New food added: %s\n",new_food.name);
        //printf("New food added: %f\n",new_food.price);
        //printf("i=%d\n",i);
        add_record(tab, &np, cmp, &new_food);

    }


    if (sorted) {
        sort_goods(tab, no);
    }

    return no;

}

int cmp_qs(const void *a, const void *b) {
	Food *fa = (Food*)a, *fb = (Food*)b;
	return cmp_date(&fa->valid_date, &fb->valid_date);
}

int cmp_bs(const void *a, const void *b) {
	Date *pd = (Date*)a;
	Food *fb = (Food*)b;
	return cmp_date(pd, &fb->valid_date);
}

int is_leap_year(int year) {
    if (year % 4 != 0) {
        return 0;
    } else if (year % 100 != 0) {
        return 1;
    } else if (year % 400 != 0) {
        return 0;
    } else {
        return 1;
    }
}

int days_in_month(int month, int year) {
    static const int days_per_month[] = {
            31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    int days = days_per_month[month - 1];

    if (month == 2 && is_leap_year(year)) {
        days++;
    }

    return days;
}

// finds the value of goods due on 'curr_date'
float value(Food *food_tab, size_t n, Date curr_date, int days) {
    float total_value = 0.0;

    Date expiration_date = curr_date;
    expiration_date.day += days;

    while (expiration_date.day > days_in_month(expiration_date.month, expiration_date.year)) {
        expiration_date.day -= days_in_month(expiration_date.month, expiration_date.year);
        expiration_date.month++;

        // Adjust the year if the month exceeds December
        if (expiration_date.month > 12) {
            expiration_date.month = 1;
            expiration_date.year++;
        }
    }

    for (size_t i = 0; i < n; i++) {
        const Food *food = &food_tab[i];
        Date food_expiration_date = food->valid_date;

        if (compare_dates(food_expiration_date, expiration_date) == 0) {
            float food_value = food->price * food->amount;
            total_value += food_value;
        }
    }

    return total_value;
}

/////////////////////////////////////////////////////////////////
// 3 Succession

#define CHILD_MAX 20

enum Sex {F, M};
enum BOOL {no, yes};

struct Bit_data {
	enum Sex sex:1;
	enum BOOL in_line:1;
};

typedef struct {
	char *name;
	struct Bit_data bits;
	Date born;
	char *parent;
} Person;

typedef struct {
	char *par_name;
	int index;
} Parent;

const Date primo_date = { 28, 10, 2011 }; // new succession act

int cmp_primo(Person *person1, Person *person2) {
	if (person1->bits.sex == person2->bits.sex) return 0;
	if (person1->bits.sex == F && cmp_date(&person1->born, &primo_date) > 0) return 0;
	if (person2->bits.sex == F && cmp_date(&person2->born, &primo_date) > 0) return 0;
	return person2->bits.sex - person1->bits.sex;
}

// compare persons
int cmp_person(const void *a, const void *b) {
    const Person *person_a = (const Person *)a;
    const Person *person_b = (const Person *)b;

    // Check if person_a is the first person
    if (person_a->parent == NULL && person_b->parent != NULL)
        return -1;
    if (person_a->parent != NULL && person_b->parent == NULL)
        return 1;

    // Compare the names lexicographically
    return strcmp(person_a->name, person_b->name);
}

int fill_indices_tab(Parent *idx_tab, Person *pers_tab, int size) {
    int index = 0;
    for (int i = 0; i < size; i++) {
        if (pers_tab[i].parent != NULL) {
            idx_tab[index].par_name = pers_tab[i].parent;
            idx_tab[index].index = i;
            index++;
        }
    }
    printf("index:%d\n",index);
    printf("size:%d\n",size);
    return index;
}

void persons_shifts(Person *person_tab, int size, Parent *idx_tab, int no_parents) {
    for (int i = 0; i < no_parents; i++) {
        Parent parent = idx_tab[i];
        Person *parent_person = NULL;

        for (int j = 0; j < size; j++) {
            if (strcmp(person_tab[j].name, parent.par_name) == 0) {
                parent_person = &person_tab[j];
                break;
            }
        }

        if (parent_person != NULL) {
            int parent_index = -1;
            for (int j = 0; j < size; j++) {
                if (&person_tab[j] == parent_person) {
                    parent_index = j;
                    break;
                }
            }

            if (parent_index != -1) {
                int next_index = parent_index + 1;

                int num_children = 0;
                for (int k = 0; k < size; k++) {
                    if (person_tab[k].parent != NULL && strcmp(person_tab[k].parent, parent.par_name) == 0) {
                        num_children++;
                    }
                }

                for (int k = 0; k < num_children; k++) {
                    int child_index = -1;
                    for (int m = 0; m < size; m++) {
                        if (person_tab[m].parent != NULL && strcmp(person_tab[m].parent, parent.par_name) == 0) {
                            child_index = m;
                            break;
                        }
                    }

                    if (child_index != -1) {
                        Person temp = person_tab[child_index];
                        for (int m = child_index; m > next_index; m--) {
                            person_tab[m] = person_tab[m - 1];
                        }
                        person_tab[next_index] = temp;
                        next_index++; // Update the next_index value
                    }
                }
            }
        }
    }
}

int cleaning(Person *person_tab, int n) {
    int i, j;
    int cleaned_size = n;

    for (i = 0, j = 0; i < n; i++) {
        if (person_tab[i].bits.in_line == no) {
            cleaned_size--;
        } else {
            if (i != j) {
                person_tab[j] = person_tab[i];
            }
            j++;
        }
    }

    return cleaned_size;
}

void print_person(const Person *p) {
	printf("%s\n", p->name);
}

int create_list(Person *person_tab, int n) {
    // Sort the person_tab array based on the comparison function
    qsort(person_tab, n, sizeof(Person), cmp_person);
    printf("1\n");
    for (int i=0;i<32;i++) printf("%d %s\n",i, person_tab[i].name);
    // Fill the indices table
    Parent idx_tab[n];
    int no_parents=fill_indices_tab(idx_tab, person_tab, n);
    printf("2\n");
    for (int i=0;i<31;i++) printf("%d %s\n",i, idx_tab[i].par_name);
    // Shift persons based on their parent-child relationship
    persons_shifts(person_tab, n, idx_tab, no_parents);
    printf("3\n");
    for (int i=0;i<32;i++) printf("%d %s\n",i, person_tab[i].name);
    // Clean up the person array
    int cleaned_size = cleaning(person_tab, n);

    return cleaned_size;
}


int comparePersons(const void *a, const void *b) {
    const Person *person1 = (const Person *)a;
    const Person *person2 = (const Person *)b;

    // Sortowanie osób bez rodzica na początku
    if (person1->parent == NULL && person2->parent != NULL)
        return -1;
    else if (person1->parent != NULL && person2->parent == NULL)
        return 1;

    if (person1->bits.in_line && !person2->bits.in_line)
        return -1;
    else if (!person1->bits.in_line && person2->bits.in_line)
        return 1;
    else {
        int dateComparison = compare_dates(person1->born, person2->born);
        if (dateComparison != 0)
            return dateComparison;
        else
            return strcmp(person1->name, person2->name);
    }
}

void sortSuccession(Person *person_tab, int size) {
    qsort(person_tab, size, sizeof(Person), comparePersons);
}






int main(void) {
	Person person_tab[] = {
		{"Charles III", {M, no}, {14, 11, 1948},"Elizabeth II"},
		{"Anne", {F,yes}, {15, 8, 1950},"Elizabeth II"},
		{"Andrew", {M,yes}, {19, 2, 1960},"Elizabeth II"},
		{"Edward", {M,yes}, {10, 3, 1964} ,"Elizabeth II"},
		{"David", {M,yes}, {3, 11, 1961} ,"Margaret"},
		{"Sarah", {F,yes}, {1, 5, 1964}, "Margaret"},
		{"William", {M,yes}, {21, 6, 1982}, "Charles III"},
		{"Henry", {M,yes}, {15, 9, 1984}, "Charles III"},
		{"Peter", {M,yes}, {15, 11, 1977}, "Anne"},
		{"Zara", {F,yes}, {15, 5, 1981}, "Anne"},
		{"Beatrice", {F,yes}, {8, 8, 1988}, "Andrew"},
		{"Eugenie", {F,yes}, {23, 3, 1990}, "Andrew"},
		{"James", {M,yes}, {17, 12, 2007}, "Edward"},
		{"Louise", {F,yes}, {8, 11, 2003}, "Edward"},
		{"Charles", {M,yes}, {1, 7, 1999}, "David"},
		{"Margarita", {F,yes}, {14, 5, 2002}, "David"},
		{"Samuel", {M,yes}, {28, 7, 1996}, "Sarah"},
		{"Arthur", {M,yes}, {6, 5, 2019}, "Sarah"},
		{"George", {M,yes}, {22, 7, 2013}, "William"},
		{"George VI", {M,no}, {14, 12, 1895}, NULL},
		{"Charlotte", {F,yes}, {2, 5, 2015}, "William"},
		{"Louis", {M,yes}, {23, 4, 2018}, "William"},
		{"Archie", {M,yes}, {6, 5, 2019}, "Henry"},
		{"Lilibet", {F,yes}, {4, 6, 2021}, "Henry"},
		{"Savannah", {F,yes}, {29, 12, 2010}, "Peter"},
		{"Isla", {F,yes}, {29, 3, 2012}, "Peter"},
		{"Mia", {F,yes}, {17, 1, 2014}, "Zara"},
		{"Lena", {F,yes}, {18, 6, 2018}, "Zara"},
		{"Elizabeth II", {F,no}, {21, 4, 1925}, "George VI"},
		{"Margaret", {F,no}, {21, 8, 1930}, "George VI"},
		{"Lucas", {M,yes}, {21, 3, 2021}, "Zara"},
		{"Sienna", {F,yes}, {18, 9, 2021}, "Beatrice"},
		{"August", {M,yes}, {9, 2, 2021}, "Eugenie"}
	};

	int to_do, no;
	int n;
	Food food_tab[FOOD_MAX];
	char buff[ART_MAX];
	fgets(buff, ART_MAX, stdin);
	sscanf(buff, "%d", &to_do);

	switch (to_do) {
		case 1:  // bsearch2
			fgets(buff, ART_MAX, stdin);
			sscanf(buff, "%d", &no);
			n = read_goods(food_tab, no, stdin, 1);
            scanf("%s",buff);
			print_art(food_tab, n, buff);
			break;
		case 2: // qsort
			fgets(buff, ART_MAX, stdin);
			sscanf(buff, "%d", &no);
			n = read_goods(food_tab, no, stdin, 0);
			Date curr_date;
			int days;
			scanf("%d %d %d", &curr_date.day, &curr_date.month, &curr_date.year);
			scanf("%d", &days);
			printf("%.2f\n", value(food_tab, (size_t)n, curr_date, days));
			break;
		case 3: // succession
			scanf("%d",&no);
			int no_persons = sizeof(person_tab) / sizeof(Person);
            no_persons--;
            printf("number of persons:%d\n",no_persons);
            sortSuccession(person_tab, no_persons);
			//create_list(person_tab,no_persons);
            printf("Succession to the throne:\n");
            for (int i = 0; i < no_persons; i++) {
                printf("%d %s\n",i, person_tab[i].name);
            }
			print_person(person_tab + no - 1);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
	}
	return 0;
}
