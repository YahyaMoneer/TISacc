#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Cross-platform directory checking headers
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

int items = 0;
int last_viewed_code = -1; 
char database_path[512];

// Checks if a path exists and is a directory
int is_valid_dir(const char *path) {
#ifdef _WIN32
    DWORD ftyp = GetFileAttributesA(path);
    if (ftyp == INVALID_FILE_ATTRIBUTES) return 0; // Does not exist
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY) return 1; // Is a directory
    return 0; // Is a file
#else
    struct stat info;
    if (stat(path, &info) != 0) return 0; // Does not exist
    return S_ISDIR(info.st_mode);         // Standard POSIX macro to check if directory
#endif
}

void get_number(int *val) {
    while (scanf("%d", val) != 1) {
        printf("invalid input. please enter a number: ");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
}

void new() {
    int price, year, month, day;
    char name[512];

    printf("price: ");
    get_number(&price);
    
    printf("name: ");
    scanf(" %511[^\n]", name); 
    
    printf("date:\n    year: ");
    get_number(&year);
    printf("    month: ");
    get_number(&month);
    printf("    day: ");
    get_number(&day);
    
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s%d.txt", database_path, items);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        printf("Error: Could not open file. Check if path is valid.\n");
        return;
    }

    fprintf(file, "code: %d\n", items);
    fprintf(file, "price: %d\n", price);
    fprintf(file, "name: %s\n", name);
    fprintf(file, "date:\n    year: %d\n    month: %d\n    day: %d\n", year, month, day);
    fclose(file);
    
    printf("Item saved to file: %s\n", filepath);
    items++;
}

void view() {
    int search_code;
    printf("Enter code to view: ");
    get_number(&search_code);

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s%d.txt", database_path, search_code);

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        printf("File not found.\n");
        last_viewed_code = -1; 
        return;
    }

    printf("\n--- Item %d ---\n", search_code);
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    printf("\n");
    fclose(file);

    last_viewed_code = search_code; 
}

void edit() {
    if (last_viewed_code == -1) {
        printf("Error: You must 'view' an item first before editing it.\n");
        return;
    }

    int price, year, month, day;
    char name[512];

    printf("Editing Item %d:\n", last_viewed_code);
    printf("new price: ");
    get_number(&price);
    
    printf("new name: ");
    scanf(" %511[^\n]", name);

    printf("new date:\n    year: ");
    get_number(&year);
    printf("    month: ");
    get_number(&month);
    printf("    day: ");
    get_number(&day);

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s%d.txt", database_path, last_viewed_code);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        printf("Error updating file.\n");
        return;
    }

    fprintf(file, "code: %d\n", last_viewed_code);
    fprintf(file, "price: %d\n", price);
    fprintf(file, "name: %s\n", name);
    fprintf(file, "date:\n    year: %d\n    month: %d\n    day: %d\n", year, month, day);
    fclose(file);

    printf("Item %d updated successfully.\n", last_viewed_code);
    last_viewed_code = -1; 
}

void delete_item() {
    if (last_viewed_code == -1) {
        printf("Error: You must 'view' an item first before deleting it.\n");
        return;
    }

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s%d.txt", database_path, last_viewed_code);

    if (remove(filepath) == 0) {
        printf("Item %d deleted successfully from storage.\n", last_viewed_code);
    } else {
        printf("Error: Could not delete the file.\n");
    }

    last_viewed_code = -1; 
}

int main() {
    char func[512];
    char choice[512];

    FILE *config = fopen("config.txt", "r");
    if (config != NULL) {
        fscanf(config, "%511[^\n]", database_path);
        fclose(config);
        
        if (!is_valid_dir(database_path)) {
            printf("\n[WARNING] The saved database path is invalid or missing: %s\n", database_path);
            printf("Please set a valid path using the 'path' command.\n\n");
        }
    } else {
        printf("\n[NOTICE] No configuration found. Please set a path using the 'path' command.\n\n");
    }

    int max_found = -1;
    for (int i = 0; i < 10000; i++) {
        char filepath[1024];
        snprintf(filepath, sizeof(filepath), "%s%d.txt", database_path, i);
        FILE *f = fopen(filepath, "r");
        if (f != NULL) {
            max_found = i;
            fclose(f);
        }
    }
    items = max_found + 1;

    while(1){
        printf("function: ");
        scanf("%511s", func); 

        if (strcmp(func, "new") == 0) {
            new();
            last_viewed_code = -1;
        } else if(strcmp(func, "view") == 0) {
            view();
        } else if(strcmp(func, "edit") == 0) {
            edit();
        } else if(strcmp(func, "delete") == 0) {
            delete_item();
        } else if(strcmp(func, "path") == 0) {
            char temp_path[512];
            printf("database path: ");
            scanf(" %511[^\n]", temp_path);
            
            if (is_valid_dir(temp_path)) {
                strcpy(database_path, temp_path);
                
                FILE *config_w = fopen("config.txt", "w");
                if (config_w != NULL) {
                    fprintf(config_w, "%s", database_path);
                    fclose(config_w);
                    printf("Path saved successfully.\n");
                }
            } else {
                printf("Error: Invalid path. The directory does not exist.\n");
            }
            last_viewed_code = -1;
        } else if(strcmp(func, "exit") == 0) {
            printf("exit?(y/n): ");
            scanf("%511s", choice);
            if(strcmp(choice, "0") == 0 || strcmp(choice, "n") == 0 || strcmp(choice, "no") == 0) {
                continue;
            } else if(strcmp(choice, "1") == 0 || strcmp(choice, "y") == 0 || strcmp(choice, "yes") == 0) {
                exit(0);
            }
        } else {
            printf("no such function\n");
        }
    }
    return 0;
}