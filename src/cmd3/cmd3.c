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
 * cmd3.c
 *
 *  Created on: Feb 23, 2015
 */

#include <stdio.h>

#include "cmd3.h"
#include "uthash.h"

#define CMD_NAME_MAX_LENGTH 	32		// Command tree 'name' attribute maximum size
#define CMD_COMMENT_MAX_LENGTH 	80		// Command tree 'comment' attribute maximum size

#define CMD_TERMINATING_CHAR_LEN 1

// Command tree node structure
struct cmdtree
{
	char 		 		name[CMD_NAME_MAX_LENGTH];			// Command tree name
	char 		 		comment[CMD_COMMENT_MAX_LENGTH];	// Command tree comment
	cmdtree_cmdfunc	cmdfunc;		// Command tree optional command function

	struct cmdtree *parent;			// Command tree parent node pointer
	struct cmdtree *child;			// Command tree child node pointer

	UT_hash_handle 		hh; 			// Makes this structure hashable
};

static cmdtree_d cmd_root = NULL;	// Command tree root node

static int   cmdtree_report_tree(cmdtree_d cmd_start, char *buf);
static cmdtree_d cmdtree_lookup(const char *cmd_base_name);



cmdtree_d cmdtree_create(cmdtree_config_t *config)
{
	cmdtree_d cmdtree;

	cmdtree = calloc(1, sizeof(struct cmdtree));
	if(NULL == cmdtree)
	    return NULL;

	strncpy(cmdtree->name, config->name, CMD_NAME_MAX_LENGTH-1);
	strncpy(cmdtree->comment, config->comment, CMD_COMMENT_MAX_LENGTH-1);
	cmdtree->cmdfunc = config->cmdfunc;

	if(config->parent_name == NULL)
	{
		/* This is a root level cmd */
		cmdtree->parent = NULL;

		HASH_ADD_STR(cmd_root, name, cmdtree);
	}
	else
	{	/* This is a sub cmd and a parent exist */
		cmdtree_d cmd_parent = NULL;
		char parent_name[1024];

		strncpy(parent_name, config->parent_name, sizeof(parent_name)-1);
		parent_name[sizeof(parent_name)-1] = '\0';

		/* Look for the parent */
		cmd_parent = cmdtree_lookup(parent_name);
		if(cmd_parent)
		{
			HASH_ADD_STR(cmd_parent->child, name, cmdtree);

			cmdtree->parent   = cmd_parent;
		}
		else
		{
			printf("Unable to detect parent cmd %s.""\n", parent_name);
			free(cmdtree);
			cmdtree = NULL;
		}
	}

	return cmdtree;
}

/* String to Vector convert */
void cmdtree_stov(const char *string, int *arg_count, const char **arg_vec)
{
	/*
	 * Split the cmd string using white space delimiters, ending up with an argv/argc format.
	 */
	const char **ap;

	*arg_count = 0;

	for (ap = arg_vec; (*ap = strsep((char **)&string, " \t")) != NULL;)
	{
		if (**ap != '\0')
		{
			(*arg_count)++;
		   if (++ap >= &arg_vec[CMD_TREE_MAX_DEPTH])
			   break;
		}
	}
}

static cmdtree_d cmdtree_lookup(const char *cmd_base_name)
{
	cmdtree_d cmd = NULL;

	/*
	 * Split the cmd string using white space delimiters, ending up with an argv/argc format.
	 */;
	const char *arg_vdata[CMD_TREE_MAX_DEPTH];
	const char **arg_vec = arg_vdata;
	int   arg_count = 0;

	cmdtree_stov(cmd_base_name, &arg_count, arg_vec);

	if (arg_count > 0)
	{
		cmdtree_d cmd_base;

		cmd_base = cmd_root;
		do
		{
			HASH_FIND_STR(cmd_base, *arg_vec, cmd);
			if(cmd)
			{
				arg_count--;
				arg_vec++;

				cmd_base = cmd->child;
			}
		} while (cmd_base && arg_count && cmd);
	}

	return cmd;
}

void cmdtree_destroy(cmdtree_d cmdtree)
{
	cmdtree_d cmdtree_head = NULL;

	/*
	 * The cmd entry should not be deleted in these cases:
	 * - The entry has a child. (the child sub-tree needs to be deleted first)
	 */
	if(cmdtree->child)
	{
		return;
	}

	if (NULL == cmdtree->parent)
	{
		/* In case this is a root level cmd, cmd_root is the hash table head. */
		cmdtree_head = cmd_root;

		if(1 == HASH_COUNT(cmdtree_head))
		{
			cmd_root = NULL;
		}
	}
	else
	{
		/* Lookup for the hash table (head): The parent "first child" points to the head. */
		cmdtree_head = cmdtree->parent->child;

		if(1 == HASH_COUNT(cmdtree_head))
		{
			cmdtree->parent->child = NULL;
		}
	}

	HASH_DEL(cmdtree_head, cmdtree);
	free(cmdtree);
}

cmdtree_d new_cmdtree_create(const char cmdname[], const char cmdcomment[], cmdtree_cmdfunc cmdfunc, const char parent_name[])
{
    cmdtree_config_t cmd_cfg;

    memset(&cmd_cfg, 0, sizeof(cmd_cfg));

    cmd_cfg.name        = cmdname;
    cmd_cfg.cmdfunc     = cmdfunc;
    cmd_cfg.comment     = cmdcomment;
    cmd_cfg.parent_name = parent_name;
    cmdtree_d cmdtree = cmdtree_create(&cmd_cfg);

    return cmdtree;
}

cmdtree_d cmdtree_get_root(void)
{
	return cmd_root;
}

int cmdtree_exec(int argc, const char **argv, char *buf, size_t buf_size)
{
	cmdtree_d cmd;
	int ret = 0;

	cmd = cmdtree_get_root();

	if(0 == argc)
	{
		ret = cmdtree_report_tree(cmd, buf);
	}
	else
	{
		cmdtree_d cmd_tree;

		do
		{
			HASH_FIND_STR(cmd, *argv, cmd_tree);
			if(cmd_tree)
			{
				argc--;
				argv++;

				cmd = cmd_tree->child;
			}
		} while (cmd && argc && cmd_tree);

		if(cmd_tree)
		{
			if(NULL != cmd_tree->cmdfunc)
				ret = cmd_tree->cmdfunc(argc, argv, buf, buf_size);
			else if(cmd_tree->child)
				ret = cmdtree_report_tree(cmd_tree->child, buf);
		}
		else
		{
			ret = cmdtree_report_tree(cmd, buf);
		}
	}

	if(ret <= 0)
	{
		ret = sprintf(buf,"Missing parameter or unsupported command.\n");
	}

	return ret + CMD_TERMINATING_CHAR_LEN;
}


static int cmdtree_report_tree(cmdtree_d cmd_start, char *buf)
{
	cmdtree_d cmd_iterate;
	cmdtree_d cmd_temp;
	int buf_len;
	char *buf_base = buf;

	HASH_ITER(hh, cmd_start, cmd_iterate, cmd_temp)
	{
		buf += sprintf(buf, "%s - %s\n", cmd_iterate->name, cmd_iterate->comment);
	}

	buf_len = buf - buf_base;
	return buf_len;
}
