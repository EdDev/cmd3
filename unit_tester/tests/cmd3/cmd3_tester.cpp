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
 * cmd3_tester.cpp
 *
 *  Created on: Feb 23, 2015
 */


#include <CppUTest/TestHarness.h>

#include <stdio.h>
#include <string.h>

#include "cmd3.h"

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

extern "C"
{

/* MOCK/s */
}

static int cmdtest1(int argc, const char **argv, char *buf, size_t buf_size)
{
	UNUSED(buf_size);
	int bytes_writen;

	bytes_writen = sprintf(buf, "cmdtest1: argc=%d, arg[0]=%s""\n", argc, argv[0]);

	return bytes_writen;
}

static int cmdtest2(int argc, const char **argv, char *buf, size_t buf_size)
{
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(buf);
	UNUSED(buf_size);
	return CMD3_FAIL;
}

static int cmdtest3(int argc, const char **argv, char *buf, size_t buf_size)
{
	UNUSED(argc);
	UNUSED(argv);
	UNUSED(buf);
	UNUSED(buf_size);
	return CMD3_FAIL;
}


TEST_GROUP(cmd3_creation)
{

    void setup()
    {

    }

    void teardown()
    {

    }
};

TEST(cmd3_creation, cmd_tree_create_destroy)
{
	cmdtree_d cmdtree;

	cmdtree = new_cmdtree_create("cmdtest1", "cmd test 1", cmdtest1, CMDTREE_NO_PARENT);

	CHECK(cmdtree != NULL);

	cmdtree_destroy(cmdtree);
}

TEST(cmd3_creation, cmd_tree_get_root)
{
	cmdtree_d cmdtree;

	cmdtree = new_cmdtree_create("cmdtest1", "cmd test 1", cmdtest1, CMDTREE_NO_PARENT);

	POINTERS_EQUAL(cmdtree, cmdtree_get_root());

	cmdtree_destroy(cmdtree);
}


TEST_GROUP(cmd3)
{

    void setup()
    {
    	new_cmdtree_create("cmdtest1", "cmd test 1", cmdtest1, CMDTREE_NO_PARENT);
    }

    void teardown()
    {
    	cmdtree_d cmdtree_root = cmdtree_get_root();

    	cmdtree_destroy(cmdtree_root);
    }
};

TEST(cmd3, add_2nd_root_cmd__two_root_cmd_exist_seen_in_usage)
{
	int argc;
	const char *argv[1];
	char report_buf[256];

	char report_expected[256] = 	"cmdtest1 - cmd test 1""\n"
								 	"cmdtest2 - cmd test 2""\n";

	cmdtree_d cmdtree = new_cmdtree_create("cmdtest2", "cmd test 2", cmdtest2, CMDTREE_NO_PARENT);

	argc 	= 0;
	argv[0] = NULL;
	memset(report_buf, 0, sizeof(report_buf));
	cmdtree_exec(argc, argv, report_buf, sizeof(report_buf));

	STRCMP_EQUAL(report_expected, report_buf);

	cmdtree_destroy(cmdtree);
}

TEST(cmd3, add_3rd_root_cmd__three_root_cmd_exist_seen_in_usage)
{
	int argc;
	const char *argv[1];
	char report_buf[256];

	char report_expected[256] = 	"cmdtest1 - cmd test 1""\n"
									"cmdtest2 - cmd test 2""\n"
								 	"cmdtest3 - cmd test 3""\n";

	cmdtree_d cmdtree2 = new_cmdtree_create("cmdtest2", "cmd test 2", cmdtest2, CMDTREE_NO_PARENT);
	cmdtree_d cmdtree3 = new_cmdtree_create("cmdtest3", "cmd test 3", cmdtest3, CMDTREE_NO_PARENT);

	argc 	= 0;
	argv[0] = NULL;
	memset(report_buf, 0, sizeof(report_buf));
	cmdtree_exec(argc, argv, report_buf, sizeof(report_buf));

	STRCMP_EQUAL(report_expected, report_buf);

	cmdtree_destroy(cmdtree3);
	cmdtree_destroy(cmdtree2);
}

TEST(cmd3, add_one_child_cmd__child_cmd_exist_seen_in_usage)
{
	int argc;
	const char *argv[1];
	char report_buf[256];

	const char arg0[] = "cmdtest2";
	char report_expected[256]  = "cmdtest2.1 - cmd test 2.1""\n";

	cmdtree_d cmdtree2  = new_cmdtree_create("cmdtest2",   "cmd test 2",   NULL, 	 CMDTREE_NO_PARENT);
	cmdtree_d cmdtree21 = new_cmdtree_create("cmdtest2.1", "cmd test 2.1", cmdtest2, "cmdtest2");

	argc 	= 1;
	argv[0] = (char *)arg0;
	memset(report_buf, 0, sizeof(report_buf));
	cmdtree_exec(argc, argv, report_buf, sizeof(report_buf));

	STRCMP_EQUAL(report_expected, report_buf);

	cmdtree_destroy(cmdtree21);
	cmdtree_destroy(cmdtree2);

}

TEST(cmd3, add_tree_child_cmd__child_cmd_exist_seen_in_usage)
{
	int argc;
	const char *argv[1];
	char report_buf[256];

	const char arg0[] = "cmdtest2";
	char report_expected[256]  = "cmdtest2.1 - cmd test 2.1""\n"
								 "cmdtest2.2 - cmd test 2.2""\n"
								 "cmdtest2.3 - cmd test 2.3""\n";

	cmdtree_d cmdtree2  = new_cmdtree_create("cmdtest2", 	 "cmd test 2", 	 NULL, CMDTREE_NO_PARENT);
	cmdtree_d cmdtree21 = new_cmdtree_create("cmdtest2.1", "cmd test 2.1", cmdtest2, "cmdtest2");
	cmdtree_d cmdtree22 = new_cmdtree_create("cmdtest2.2", "cmd test 2.2", cmdtest2, "cmdtest2");
	cmdtree_d cmdtree23 = new_cmdtree_create("cmdtest2.3", "cmd test 2.3", cmdtest2, "cmdtest2");

	argc 	= 1;
	argv[0] = (char *)arg0;
	memset(report_buf, 0, sizeof(report_buf));
	cmdtree_exec(argc, argv, report_buf, sizeof(report_buf));

	STRCMP_EQUAL(report_expected, report_buf);

	cmdtree_destroy(cmdtree23);
	cmdtree_destroy(cmdtree22);
	cmdtree_destroy(cmdtree21);
	cmdtree_destroy(cmdtree2);
}

TEST(cmd3, execute_cmd_from_root)
{
	int argc;
	const char *argv[2];
	char report_buf[256];

	const char arg0[] = "cmdtest1";
	const char arg1[] = "arg1";
	char report_expected[256]  = "cmdtest1: argc=1, arg[0]=arg1""\n";

	argc 	= 2;
	argv[0] = (char *)arg0;
	argv[1] = (char *)arg1;
	memset(report_buf, 0, sizeof(report_buf));
	cmdtree_exec(argc, argv, report_buf, sizeof(report_buf));

	STRCMP_EQUAL(report_expected, report_buf);
}

TEST(cmd3, execute_cmd_from_3_level_deep)
{
	int argc;
	const char *argv[4];
	char report_buf[256];

	const char arg0[] = "cmdtest2";
	const char arg1[] = "cmdtest2.2";
	const char arg2[] = "cmdtest2.2.1";
	const char arg3[] = "arg0";
	char report_expected[256]  = "cmdtest1: argc=1, arg[0]=arg0""\n";

	cmdtree_d cmdtree2   = new_cmdtree_create("cmdtest2", 	 	"cmd test 2", 	  NULL, 	CMDTREE_NO_PARENT);
	cmdtree_d cmdtree21  = new_cmdtree_create("cmdtest2.1", 	"cmd test 2.1",   NULL, 	"cmdtest2");
	cmdtree_d cmdtree22  = new_cmdtree_create("cmdtest2.2", 	"cmd test 2.2",   NULL, 	"cmdtest2");
	cmdtree_d cmdtree221 = new_cmdtree_create("cmdtest2.2.1", 	"cmd test 2.2.1", cmdtest1, "cmdtest2 cmdtest2.2");
	cmdtree_d cmdtree23  = new_cmdtree_create("cmdtest2.3", 	"cmd test 2.3",   NULL, 	"cmdtest2");

	argc 	= 4;
	argv[0] = (char *)arg0;
	argv[1] = (char *)arg1;
	argv[2] = (char *)arg2;
	argv[3] = (char *)arg3;
	memset(report_buf, 0, sizeof(report_buf));
	cmdtree_exec(argc, argv, report_buf, sizeof(report_buf));

	STRCMP_EQUAL(report_expected, report_buf);

	cmdtree_destroy(cmdtree23);
	cmdtree_destroy(cmdtree221);
	cmdtree_destroy(cmdtree22);
	cmdtree_destroy(cmdtree21);
	cmdtree_destroy(cmdtree2);
}
