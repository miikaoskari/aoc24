#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

#define MAX_LINE_LENGTH 4096
#define MAX_MATCHES 200

int get_muls(char *line, char *pattern)
{
    const char *error;
    int erroffset;
    pcre *re;
    int ovector[MAX_MATCHES]; /* output vector */
    int rc;
    int line_length = strlen(line);
    int utf8;
    int crlf_is_newline;
    unsigned int option_bits;
    
    /* compile the binary */
    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if (re == NULL)
    {
        printf("pcre failed compilation");
        exit(EXIT_FAILURE);
    }
    (void)pcre_fullinfo(re, NULL, PCRE_INFO_OPTIONS, &option_bits);
    utf8 = option_bits & PCRE_UTF8;
    option_bits &= PCRE_NEWLINE_CR|PCRE_NEWLINE_LF|PCRE_NEWLINE_CRLF|
               PCRE_NEWLINE_ANY|PCRE_NEWLINE_ANYCRLF;

    /* If no newline options were set, find the default newline convention from the
    build configuration. */
    if (option_bits == 0)
    {
        int d;
        (void)pcre_config(PCRE_CONFIG_NEWLINE, &d);
        /* Note that these values are always the ASCII ones, even in
        EBCDIC environments. CR = 13, NL = 10. */
        option_bits = (d == 13)? PCRE_NEWLINE_CR :
                (d == 10)? PCRE_NEWLINE_LF :
                (d == (13<<8 | 10))? PCRE_NEWLINE_CRLF :
                (d == -2)? PCRE_NEWLINE_ANYCRLF :
                (d == -1)? PCRE_NEWLINE_ANY : 0;
    }

    /* See if CRLF is a valid newline sequence. */

    crlf_is_newline =
        option_bits == PCRE_NEWLINE_ANY ||
        option_bits == PCRE_NEWLINE_CRLF ||
        option_bits == PCRE_NEWLINE_ANYCRLF;

    /* get all matches */
    for(int i = 0; i < MAX_MATCHES; i++)
    {
        int options = 0;
        int start_offset = ovector[1];

        if (ovector[0] == ovector[1])
        {
            if (ovector[0] == line_length)
            {
                break;
            }
            options = PCRE_NOTEMPTY_ATSTART | PCRE_ANCHORED;
        }

        rc = pcre_exec(
            re,
            NULL,
            line,
            line_length,
            start_offset,
            options,
            ovector,
            MAX_MATCHES);

        if (rc == PCRE_ERROR_NOMATCH)
        {
        if (options == 0) break;                    /* All matches found */
        ovector[1] = start_offset + 1;              /* Advance one byte */
        if (crlf_is_newline &&                      /* If CRLF is newline & */
            start_offset < line_length - 1 &&    /* we are at CRLF, */
            line[start_offset] == '\r' &&
            line[start_offset + 1] == '\n')
        ovector[1] += 1;                          /* Advance by one more. */
        else if (utf8)                              /* Otherwise, ensure we */
        {                                         /* advance a whole UTF-8 */
        while (ovector[1] < line_length)       /* character. */
            {
            if ((line[ovector[1]] & 0xc0) != 0x80) break;
            ovector[1] += 1;
            }
        }
        continue;    /* Go round the loop again */
        }

        if (rc < 0)
        {
            printf("Matching error %d\n", rc);
            pcre_free(re);
            return 1;
        }

        /* Match succeded */
        printf("\nMatch succeeded again at offset %d\n", ovector[0]);

        /* The match succeeded, but the output vector wasn't big enough. */
        if (rc == 0)
        {
            rc = MAX_MATCHES / 3;
            printf("ovector only has room for %d captured substrings\n", rc - 1);
        }

        /* As before, show substrings stored in the output vector by number, and then
        also any named substrings. */
        for (i = 0; i < rc; i++)
        {
            char *substring_start = line + ovector[2*i];
            int substring_length = ovector[2*i+1] - ovector[2*i];
            printf("%2d: %.*s\n", i, substring_length, substring_start);
        }
    }

    pcre_free(re);
    return 0;
}

int main(void)
{
    FILE *fp;
    int fsize = 0;

    fp = fopen("input.txt", "r");

    if (fp == NULL)
    {
        perror("file open error");
        return 1;
    }

    char *fcontent;
    char *pattern = "mul\\(\\d+,\\d+\\)";
    if (fp) {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        fcontent = (char*) malloc(sizeof(char) * fsize);
        fread(fcontent, 1, fsize, fp);

        fclose(fp);
    }
    
    get_muls(fcontent, pattern);

    free(fcontent);
    return 0;
}


