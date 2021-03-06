#include <cstdlib>
#include <string>
using namespace std;

#include "Logger.h"
#include "StringUtils.h"
#include "ParsableString.h"
using namespace mlpl;

#include <cutter.h>
#include <cppcutter.h>
#include <glib.h>

#include "loggerTester.h"

namespace testLogger {

static gchar *g_standardOutput = NULL;
static gchar *g_standardError = NULL;
static GError *g_error = NULL;
static gint g_exitStatus;
static gboolean g_spawnRet;

static void failAndShowSpawnResult(void)
{
	const char *err_msg = "";
	if (g_error)
		err_msg = g_error->message; 

	cut_fail(
	  "ret: %d, exitStatus: %d, msg: %s\n"
	  "<<stdout>>\n"
	  "%s\n"
	  "<<stderr>>\n"
	  "%s\n",
	  g_spawnRet, g_exitStatus, err_msg, g_standardOutput, g_standardError);
}

static void assertSpawnResult(void)
{
	if (g_spawnRet != TRUE)
		goto err;
	if (g_exitStatus != EXIT_SUCCESS)
		goto err;
	cut_assert(true);
	return;
err:
	failAndShowSpawnResult();
}

static void _assertLogOutput(const char *envLevel, const char *outLevel,
                             bool expectOut)
{
	cppcut_assert_equal(0, setenv(Logger::LEVEL_ENV_VAR_NAME, envLevel, 1));
	const gchar *testDir = cut_get_test_directory();
	testDir = testDir ? testDir : ".";
	const gchar *commandPath = cut_build_path(testDir, "loggerTestee",
						   NULL);
	string commandLine = commandPath + string(" ") + string(outLevel);
	g_spawnRet = g_spawn_command_line_sync(commandLine.c_str(),
	                                       &g_standardOutput,
	                                       &g_standardError,
	                                       &g_exitStatus, &g_error);
	cut_trace(assertSpawnResult());
	if (!expectOut) {
		cppcut_assert_equal(true, string(g_standardError).empty());
		return;
	}

	ParsableString pstr(g_standardError);
	string word = pstr.readWord(ParsableString::SEPARATOR_SPACE);
	string expectStr = StringUtils::sprintf("[%s]", outLevel);
	cppcut_assert_equal(expectStr, word);

	word = pstr.readWord(ParsableString::SEPARATOR_SPACE);
	word = pstr.readWord(ParsableString::SEPARATOR_SPACE);

	expectStr = testString;
	expectStr += "\n";
	cppcut_assert_equal(expectStr, word);
}
#define assertLogOutput(EL,OL,EXP) cut_trace(_assertLogOutput(EL,OL,EXP))

void teardown(void)
{
	if (g_standardOutput) {
		g_free(g_standardOutput);
		g_standardOutput = NULL;
	}
	if (g_standardError) {
		g_free(g_standardError);
		g_standardError = NULL;
	}
	if (g_error) {
		g_error_free(g_error);
		g_error = NULL;
	}
}

// ---------------------------------------------------------------------------
// test cases
// ---------------------------------------------------------------------------
void test_envLevelDBG(void)
{
	assertLogOutput("DBG", "DBG",  true);
	assertLogOutput("DBG", "INFO", true);
	assertLogOutput("DBG", "WARN", true);
	assertLogOutput("DBG", "ERR",  true);
	assertLogOutput("DBG", "CRIT", true);
	assertLogOutput("DBG", "BUG",  true);
}

void test_envLevelINFO(void)
{
	assertLogOutput("INFO", "DBG",  false);
	assertLogOutput("INFO", "INFO", true);
	assertLogOutput("INFO", "WARN", true);
	assertLogOutput("INFO", "ERR",  true);
	assertLogOutput("INFO", "CRIT", true);
	assertLogOutput("INFO", "BUG",  true);
}

void test_envLevelWARN(void)
{
	assertLogOutput("WARN", "DBG",  false);
	assertLogOutput("WARN", "INFO", false);
	assertLogOutput("WARN", "WARN", true);
	assertLogOutput("WARN", "ERR",  true);
	assertLogOutput("WARN", "CRIT", true);
	assertLogOutput("WARN", "BUG",  true);
}

void test_envLevelERR(void)
{
	assertLogOutput("ERR", "DBG",  false);
	assertLogOutput("ERR", "INFO", false);
	assertLogOutput("ERR", "WARN", false);
	assertLogOutput("ERR", "ERR",  true);
	assertLogOutput("ERR", "CRIT", true);
	assertLogOutput("ERR", "BUG",  true);
}

void test_envLevelCRIT(void)
{
	assertLogOutput("CRIT", "DBG",  false);
	assertLogOutput("CRIT", "INFO", false);
	assertLogOutput("CRIT", "WARN", false);
	assertLogOutput("CRIT", "ERR",  false);
	assertLogOutput("CRIT", "CRIT", true);
	assertLogOutput("CRIT", "BUG",  true);
}

void test_envLevelBUG(void)
{
	assertLogOutput("BUG", "DBG",  false);
	assertLogOutput("BUG", "INFO", false);
	assertLogOutput("BUG", "WARN", false);
	assertLogOutput("BUG", "ERR",  false);
	assertLogOutput("BUG", "CRIT", false);
	assertLogOutput("BUG", "BUG",  true);
}

} // namespace testLogger
