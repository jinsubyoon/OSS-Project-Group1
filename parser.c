#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

typedef enum _TOKEN_TYPE {
    TOKEN_UNDEFINED,
    TOKEN_OBJECT,
    TOKEN_ARRAY,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_PRIMITIVE
} TOKEN_TYPE;

typedef struct _TOKEN {
    TOKEN_TYPE type;
    union {
        char *string;
        double number;
    };
} TOKEN;

#define TOKEN_COUNT 25

typedef struct _JSON {
    TOKEN tokens[TOKEN_COUNT];
} JSON;

char *readFile(char *filename, int *readSize)
{
    FILE *fp = fopen(filename, "rb");
    if(fp==NULL)
        return NULL;
    
    int size;
    char *buffer;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = malloc(size + 1);
    memset(buffer, 0, size+1);

    if(fread(buffer, size, 1, fp)<1){
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
        return;
    pos++;

    while(pos<size)
    {
        switch(doc[pos])
        {
            case '"':
            {
                char *begin = doc + pos + 1;
                char *end = strchr(begin, '"');
                if(end==NULL)
                    break;
                
                int stringLength = end-begin;

                json->tokens[tokenIndex].type = TOKEN_STRING;
                json->tokens[tokenIndex].string = malloc(stringLength + 1);
                memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

                memcpy(json->tokens[tokenIndex].string, begin, stringLength);

                tokenIndex++;

                pos = pos + stringLength + 1;
            }
            break;
            case '[':
            {
                

                while(doc[pos] != ']')
                {
                    if(doc[pos]=='"')
                    {
                        char *begin = doc + pos + 1;
                        char *end = strchr(begin, '"');
                        if(end==NULL)
                            break;
                        int stringLength = end - begin;

                        json->tokens[tokenIndex].type = TOKEN_STRING;
                        json->tokens[tokenIndex].string = malloc(stringLength+1);
                        
                        memset(json->tokens[tokenIndex].string, 0, stringLength+1);

                        memcpy(json->tokens[tokenIndex].string, begin, stringLength);

                        tokenIndex++;

                        pos = pos + stringLength + 1;
                    }
                    pos++;
                }
            }
            break;

            case '0': case '1': case'2': case'3': case '4' : case '5' : case '6' : case '7' : case '8' : case '9' : case '-' :
            {
                char *begin = doc + pos;
                char *end;
                char *buffer;

                end = strchr(doc + pos, ',');
                if(end==NULL)
                {
                    end = strchr(doc + pos, '}');
                    if(end==NULL)
                        break;
                }
                int stringLength = end - begin;

                buffer = malloc(stringLength + 1);
                memset(buffer, 0, stringLength + 1);
                memcpy(buffer, begin, stringLength);

                json->tokens[tokenIndex].type = TOKEN_NUMBER;
                json->tokens[tokenIndex].number = atof(buffer);

                free(buffer);

                tokenIndex++;

                pos = pos+stringLength + 1;
            }
            break;
        }
        pos++;
    }
}

void freeJSON(JSON *json)
{
    for(int i=0; i<TOKEN_COUNT; i++)
    {
        if(json->tokens[i].type == TOKEN_STRING)
            free(json->tokens[i].string);
    }
}

int main(int argc, char **argv)
{
    int size;



    char *doc = readFile(argv[1], &size);
    if(doc==NULL)
        return -1;
    
    JSON json = { 0, };

    parseJSON(doc, size, &json);

    printf("***** ALL Tokens *******\n");

    for(int i=0; i<TOKEN_COUNT; i++)
    {
        printf("[%2d] %s (size=, , JSMN_STRING)\n", i, json.tokens[i].string);    
    }

    freeJSON(&json);
    free(doc);
    return 0;
}