#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *file;
    char buffer[256];

    file = fopen(argv[1], "r");

    // check if file was succesefully opened
    if (file == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        printf("%s", buffer);
    }



    fclose(file);
    
    return 0;
}
