/***************************************************************************
 *   Copyright (C) 2008 by Vitali Lovich   *
 *   vlovich@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "qoptions.h"

#include <QCoreApplication>
#include <QtDebug>

#include <stdarg.h>

bool paramsMatch(QStringList paramList, char* arg1, ...)
{
	va_list expectedParams;
	va_start(expectedParams, arg1);

	size_t numElements = 0;
	bool match = true;
	for(char* i = arg1; i != NULL; i = va_arg(expectedParams, char*))
	{
		if (numElements == paramList.size() || i != paramList.at(numElements))
		{
			match = false;
			break;
		}
		++numElements;
	}
	if (match)
	{
		match = (numElements == paramList.size());
	}
	va_end(expectedParams);

	return match;
}

bool inexactMatch(QStringList paramList, char* arg1, ...)
{
	va_list expectedParams;
	va_start(expectedParams, arg1);

	size_t numElements = 0;
	bool match = true;
	for(char* i = arg1; i != NULL; i = va_arg(expectedParams, char*))
	{
		if (!paramList.contains(arg1))
		{
			break;
			match = false;
		}
		++numElements;
	}
	if (match)
	{
		match = (numElements == paramList.size());
	}
	va_end(expectedParams);

	return match;
}

static size_t numFailures = 0;

#define TEST(name, failMsg, expectedShortCode, expectedParams...) \
static bool result_##name = false; \
static void check_##name(bool shortCode, QStringList actualParams)\
{\
	result_##name = (expectedShortCode == shortCode && paramsMatch(actualParams, ##expectedParams, NULL));\
}\
\
static void checkResult_##name()\
{\
	static bool checked = false;\
	if (!result_##name && !checked)\
	{\
		++numFailures;\
		qCritical() << failMsg;\
	}\
	checked = true;\
}

TEST(opt1, "Failed longopt csv arguments test", false, "arg1", "arg2");
TEST(opt2, "Failed longopt space separated arguments test", false,  "arg3", "arg4");
TEST(opt3, "Failed longopt no arguments test", false);
TEST(a, "Failed short opt w/ 1 separate argument test", true, "arg5");
TEST(b, "Failed short opt w/ 3 csv merged arguments w/ 1 empty test", true, "arg6", "", "arg7");
TEST(c, "Failed short opt w/ 2 csv merged arguments test", true, "arg8", "arg9");
TEST(d, "Failed short opt w/ 2 space separated merged arguments test", true, "arg10", "arg11");
TEST(e, "Failed short opt w/ no arguments test", true);
TEST(f, "Failed short opt w/ 1 merged argument test", true, "arg12");
TEST(nonOption, "Failed non-option parameters test", false, "fake1", "fake,2", "fake 3");


int main(int argc, char ** argv)
{
	char * t_argv[] = {
		argv[0],
		"--opt1",
		"arg1,arg2",
		"--opt2",
		"arg3 arg4",
		"-darg10 arg11",
		"--opt3",
		"fake1",
		"-a",
		"arg5",
		"-barg6,,arg7",
		"fake,2",
		"-carg8,arg9",
		"fake 3",
		"-e",
		"-farg12",
	};
	int t_argc = sizeof(argv) / sizeof(argv[0]);

	QCoreApplication app(t_argc, t_argv);
	QOptions options(app.arguments());
	options += QOption("general").setLongCode("opt1").setArguments(2);
	options += QOption("general").setLongCode("opt2").setArguments(2, ' ');
	options += QOption("general").setLongCode("opt3").setArguments(0);
	options += QOption("general").setShortCode('a').setArguments(1);
	options += QOption("general").setShortCode('b').setArguments(3);
	options += QOption("general").setShortCode('c').setArguments(2);
	options += QOption("general").setShortCode('d').setArguments(2, ' ');
	options += QOption("general").setShortCode('e').setArguments(0);
	options += QOption("general").setShortCode('f').setArguments(1);

	QString option;
	QStringList params;
	bool shortCode;

	while (!(option = options.nextOption(shortCode, params)).isEmpty())
	{
		if (option == "opt1")
		{
			check_opt1(shortCode, params);
		}
		else if (option == "opt2")
		{
			check_opt2(shortCode, params);
		}
		else if (option == "opt3")
		{
			check_opt3(shortCode, params);
		}
		else if (option == "a")
		{
			check_a(shortCode, params);
		}
		else if (option == "b")
		{
			check_b(shortCode, params);
		}
		else if (option == "c")
		{
			check_c(shortCode, params);
		}
		else if (option == "d")
		{
			check_d(shortCode, params);
		}
		else if (option == "e")
		{
			check_e(shortCode, params);
		}
		else if (option == "f")
		{
			check_f(shortCode, params);
		}
		else
		{
			++numFailures;
			qCritical() << "Got an extra option: " << option;
		}
	}
	check_nonOption(false, options.nonOptionParameters());

	checkResult_opt1();
	checkResult_opt2();
	checkResult_opt3();
	checkResult_a();
	checkResult_b();
	checkResult_c();
	checkResult_d();
	checkResult_e();
	checkResult_f();
	checkResult_nonOption();

	if (numFailures)
	{
		if (numFailures == 1)
			qCritical() << numFailures << "test failure";
		else
			qCritical() << numFailures << "test failures";
	}
	return numFailures;
}
