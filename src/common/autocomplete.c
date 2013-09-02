#include "autocomplete.h"

#include <sys/types.h>
#include <dirent.h>

#undef  ENABLE_DEBUG
#define ENABLE_DEBUG 1

// modify acc_match s.t. the prefix of both opt_match and acc_match are equal
static void merge_matches(char *acc_match, char *opt_match)
{
    size_t acc_len = strlen(acc_match);
    size_t opt_len = strlen(opt_match);
    size_t min_len = acc_len < opt_len ? acc_len : opt_len;
    acc_match[min_len] = '\0';

    for(; *acc_match; acc_match++, opt_match++) {
        if(*acc_match != *opt_match) {
            *acc_match = '\0';
            return;
        }
    }
}

char *autocomplete_filename(const char *s, void *usr)
{
    // extract the directory part
    size_t len = strlen(s);
    char *mem = malloc((len+1) * sizeof(char));
    char *mem_ptr = mem;
    const char *s_ptr = s;
    char *last_slash = NULL;
    const char *dirname = NULL, *fname = NULL;
    size_t dirname_len, fname_len;

    while(*s_ptr) {
        *mem_ptr = *s_ptr;
        if(*mem_ptr == '/')
            last_slash = mem_ptr;
        mem_ptr++;
        s_ptr++;
    }
    *mem_ptr = '\0';

    // no slash?
    if(last_slash == NULL) {
        dirname = ".";
        dirname_len = 1;
        fname = mem;
        fname_len = len;
    }

    // has slash?
    else {
        *last_slash = '\0'; // split it into two strings with the '\0'
        dirname = mem;  // first string
        dirname_len = strlen(dirname);
        fname = last_slash+1; // second string
        fname_len = strlen(fname);
    }


    DIR *d = opendir(dirname);
    if(d == NULL) {
        ERROR("Failed to open '%s' as a directory\n", s);
        return NULL;
    }


    char *match = NULL;
    size_t match_len;
    int match_count = 0;

    struct dirent *ent;
    while((ent = readdir(d))) {
        DEBUG("%s\n", ent->d_name);
        if(strncmp(fname, ent->d_name, fname_len) == 0) {
            DEBUG("Match found: %s => %s\n", fname, ent->d_name);
            if(match == NULL) {
                match = strdup(ent->d_name);
                match_len = strlen(match);
            } else  {
                merge_matches(match, ent->d_name);
            }

            match_count++;
        }
    }

    closedir(d);

    // construct the returned value
    char *ret = NULL;
    if(match != NULL) {
        ret = malloc((dirname_len + match_len + 2) * sizeof(char));
        char *ret_ptr = ret;

        // add the dirname part of the path
        if(strcmp(dirname, ".") != 0) {
            strcpy(ret_ptr, dirname);
            ret_ptr += dirname_len;
            *ret_ptr = '/';
            ret_ptr++;
        }

        // add the match part of the path
        strcpy(ret_ptr, match);
    }

    // cleanup and return
    free(match);
    free(mem);
    return ret;
}
