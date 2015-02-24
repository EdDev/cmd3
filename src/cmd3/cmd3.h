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
 * cmd3.h
 *
 *  Created on: Feb 23, 2015
 */

#ifndef CMD3_H_
#define CMD3_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CMDTREE_NO_PARENT		NULL

#define CMD3_SUCCESS			0
#define CMD3_FAIL			-1

#define CMD_TREE_MAX_DEPTH 			32

typedef struct cmdtree *cmdtree_d;

typedef int (*cmdtree_cmdfunc)(int argc, const char **argv, char *buf, size_t buf_size);

typedef struct cmdtree_config
{
	const char 		*name;
	const char 		*comment;
	cmdtree_cmdfunc	 cmdfunc;

	const char 		*parent_name;
} cmdtree_config_t;


/*********************************************************************************//**
 * @note	Create a command tree entry,
 * 			representing a a tree junction or a command node (tree leaf).
 *
 * @param [in]  config - The configuration structure,
 * 						 describing the location of the entry in the tree,
 * 						 its name, comment and an optional command func.
 *
 * @return
 *  - On success, pointer to the cmdtree descriptor.
 *  - On failure, exit with process panic.
 *************************************************************************************/
cmdtree_d cmdtree_create(cmdtree_config_t *config);


/*********************************************************************************//**
 * @note	Destroy the command tree entry.
 * 			Entries with a child (subtree root) are not deleted.
 * 			Entries should be destroyed in the reverse order of creation.
 *
 * @param [in]  cmdtree - cmdtree descriptor.
 *
 * @return
 *  - N/A.
 *************************************************************************************/
void cmdtree_destroy(cmdtree_d cmdtree);

/*********************************************************************************//**
 * @note    Create a command tree entry, wrapping cmdtree_create() for cleaner usage.
 *
 * @param [in]  cmdname     - command name.
 * @param [in]  cmdcomment  - command comment/description.
 * @param [in]  cmdfunc     - command action/function.
 * @param [in]  parent_name - command parent name.
 *
 * @return
 *  - On success, pointer to the cmdtree descriptor.
 *  - On failure, returns NULL.
 *************************************************************************************/
cmdtree_d new_cmdtree_create(const char cmdname[], const char cmdcomment[], cmdtree_cmdfunc cmdfunc, const char parent_name[]);

/*********************************************************************************//**
 * @note	Retrieve the command tree root head.
 * 			In practice, the first root command added.
 *
 * @param   N/A
 *
 * @return
 *  - Pointer to the root cmdtree descriptor.
 *************************************************************************************/
cmdtree_d cmdtree_get_root(void);


/*********************************************************************************//**
 * @note	Execute the provided command.
 * 			If the provided entry is a subtree without an implementation, the cmd list of that level is reported.
 *
 * @param [in]  argc 	 - The number of additional arguments (not including the cmd name itself).
 * 		  [in]	argv	 - The vector of additional arguments (not including the cmd name itself).
 * 		  [out]	buf		 - Buffer to fill the report in.
 * 		  [in]	buf_size - The maximum size of the provided buffer.
 *
 * @return
 *  - The number of used buffer characters.
 *************************************************************************************/
int 		  cmdtree_exec(int argc, const char **argv, char *buf, size_t buf_size);


/*********************************************************************************//**
 * @note	Execute the provided command.
 * 			If the provided entry is a subtree without an implementation, the cmd list of that level is reported.
 *
 * @param [in]  string 	  - A string, with space delimiters, to be converted.
 * 		  [out]	arg_count - The number of vectors generated.
 * 		  [out]	arg_vec   - The vectors generated.
 *
 * @return
 *  - N/A
 *************************************************************************************/
void 		  cmdtree_stov(const char *string, int *arg_count, const char **arg_vec);

#ifdef __cplusplus
}
#endif

#endif /* CMD3_H_ */
