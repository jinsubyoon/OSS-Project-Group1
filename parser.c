#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>



typedef enum {
    UNDEFINED = 0,
    OBJECT = 1,
    ARRAY = 2,
    STRING = 3,
    PRIMITIVE = 4
} jsmntype_t;

typedef struct {
    type_t type;
    int start;
    int end;
    int size;
} jsmntok_t;



int main(int argc, arg **argv){
    FILE *fp;
    fp = fopen(argv[1], "r");
}

#define TOKEN_COUNT 20

typedef struct _JSON {
    TOKEN tokens[TOKEN_COUNT];
} JSON;

char *readFile(char *filename, int *readSize)
{
    FILE *fp = fopen(filename, "rb");
    if (fp==NULL)
        return NULL;

    int size;
    char *buffer;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = malloc(size+1);
    memset(buffer, 0, size+1);

    if(fread(buffer, size, 1, fp)<1)
    {
        *readSize = 0;
        free(buffer);
        fclose(fp);
        return NULL;
    }

    *readSize = size;

    fclose(fp);

    return buffer;
}

void parseJSON(char *doc, int size, JSON *json)
{
    int tokenIndex = 0;
    int pos = 0;

    if(doc[pos] != '{')
        return ;
    
    pos++;

    while (pos < size)
    {
        switch (doc[pos])
        {
            case '"':
            {
                char *begin = doc + pos + 1;
                char *end = strchr(begin, '"');
                if(end==NULL)
                    break;
                
                int stringLength = end - begin;

                json->tokens[tokenIndex].type = TOKEN_STRING;
                json->tokens[tokenIndex].string, malloc(stringLength + 1);
                memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

                memcpy(json->tokens[tokenIndex].string, begin, stringLength);

                tokenIndex++;

                pos = pos + stringLength + 1;
            }
            break;
        }
        pos++;     
    }
}

void freeJSON(JSON *json)
{
    for (int i=0; i<TOKEN_COUNT; i++)
    {
        if(json->tokens[i].type == TOKEN_STRING)
            free(json->tokens[i].string);
    }
}

int main(){
    int size;
    char *doc = readFile("example.json", &size);

    if (doc==NULL)
        return -1;
    
    JSON json = { 0, };

    parseJSON(doc, size, &json);

    printf("Title: %s\n", json.tokens[1].string);
    printf("Genre: %s\n", json.tokens[3].string);
    printf("Director: %s\n", json.tokens[5].string);

    freeJSON(&json);

    free(doc);

    return 0;
}