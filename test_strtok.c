#include <stdio.h>
#include <string.h>

int main() {
    char test[] = "apple banana cherry";
    
    char *word = strtok(test, " ");
    while (word != NULL) {
        printf("Word: %s\n", word);
        word = strtok(NULL, " ");
    }
    
    return 0;
}
