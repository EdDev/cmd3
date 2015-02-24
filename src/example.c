/*
 *The MIT License (MIT)
 *
 *Copyright (c) 2015 EdwardH
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 *
 *
 * example.c
 *
 *  Created on: Feb 23, 2015
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd3/cmd3.h"
#include "linenoise/linenoise.h"

static int sys_info(int argc, const char **argv, char *buf, size_t buf_size)
{
	int bytes_writen;

	bytes_writen = snprintf(buf, buf_size, "Info: argc=%d, arg[0]=%s""\n", argc, argv[0]);

	return bytes_writen;
}

static void register_commands()
{
    new_cmdtree_create("info", "System Information", sys_info, CMDTREE_NO_PARENT);
}

static void completion(const char *buf, linenoiseCompletions *lc)
{
//    if (buf[0] == 'h') {
//        linenoiseAddCompletion(lc,"hello");
//        linenoiseAddCompletion(lc,"hello there");
//    }
}

int main(int argc, char **argv)
{
    char *line;
    char *prgname = argv[0];
    char report_buf[256];

    register_commands();

    if(argc > 1)
    {
        argc--;
        argv++;
        if (!strcmp(*argv,"-c"))
        {
            argc--;
            argv++;

        	const char *arg_vdata[CMD_TREE_MAX_DEPTH];
        	const char **arg_vec = arg_vdata;
        	int   arg_count = 0;
        	cmdtree_stov(*argv, &arg_count, arg_vec);

            report_buf[0] = '\0';
            cmdtree_exec(arg_count, arg_vec, report_buf, sizeof(report_buf));
            printf("\r%s", report_buf);
        }
        else
        {
            fprintf(stderr, "Usage: %s [-c \"command\"]\n", prgname);
            exit(1);
        }
    }

    /* Set the completion callback. This will be called every time the
     * user uses the <tab> key. */
    linenoiseSetCompletionCallback(completion);

    /* Load history from file. The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

    /*
     * The typed string is returned as a malloc() allocated string by
     * linenoise, so the user needs to free() it. */
    while((line = linenoise("\r" "console> ")) != NULL)
    {
    	printf("\r");
        if (line[0] != '\0' && line[0] != '/')
        {
            linenoiseHistoryAdd(line); /* Add to the history. */

        	/*
        	 * Split the cmd string using white space delimiters, ending up with an argv/argc format.
        	 */
        	const char *arg_vdata[CMD_TREE_MAX_DEPTH];
        	const char **arg_vec = arg_vdata;
        	int   arg_count = 0;
        	cmdtree_stov(line, &arg_count, arg_vec);

        	report_buf[0] = '\0';
        	cmdtree_exec(arg_count, arg_vec, report_buf, sizeof(report_buf));
            printf("%s\r\n", report_buf);

            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        }
        else if (!strncmp(line,"/q",2))
        {
        	printf("Exit console.\r\n");
        	free(line);
        	exit(0);
        }
        else if (line[0] == '/')
        {
            printf("Unreconized command: %s\n", line);
        }
        free(line);
    }
    return 0;
}
