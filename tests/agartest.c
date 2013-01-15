/*	Public domain	*/
/*
 * Execute the Agar test suite.
 */

#include "agartest.h"

#include <string.h>

#include <agar/config/have_opengl.h>
#include "config/have_agar_au.h"
#include "config/datadir.h"

extern const AG_TestCase charsetsTest;
extern const AG_TestCase compositingTest;
extern const AG_TestCase configSettingsTest;
extern const AG_TestCase consoleTest;
extern const AG_TestCase customWidgetTest;
extern const AG_TestCase fixedResTest;
extern const AG_TestCase focusingTest;
extern const AG_TestCase fsPathsTest;
extern const AG_TestCase glviewTest;
extern const AG_TestCase imageLoadingTest;
extern const AG_TestCase keyEventsTest;
extern const AG_TestCase loaderTest;
extern const AG_TestCase mathTest;
extern const AG_TestCase maximizedTest;
extern const AG_TestCase minimalTest;
extern const AG_TestCase modalWindowHandlerTest;
#ifdef AG_NETWORK
extern const AG_TestCase networkTest;
#endif
extern const AG_TestCase objSystemTest;
extern const AG_TestCase paneTest;
extern const AG_TestCase plottingTest;
extern const AG_TestCase renderToSurfaceTest;
extern const AG_TestCase scrollbarTest;
extern const AG_TestCase scrollviewTest;
extern const AG_TestCase socketsTest;
extern const AG_TestCase stringTest;
extern const AG_TestCase tableTest;
extern const AG_TestCase textboxTest;
extern const AG_TestCase textDlgTest;
extern const AG_TestCase themesTest;
extern const AG_TestCase threadsTest;
extern const AG_TestCase timeoutsTest;
extern const AG_TestCase unitconvTest;
extern const AG_TestCase windowsTest;

const AG_TestCase *testCases[] = {
	&charsetsTest,
	&compositingTest,
	&configSettingsTest,
	&consoleTest,
	&customWidgetTest,
	&fixedResTest,
	&focusingTest,
	&fsPathsTest,
#ifdef HAVE_OPENGL
	&glviewTest,
#endif
	&imageLoadingTest,
	&keyEventsTest,
	&loaderTest,
	&mathTest,
	&maximizedTest,
	&minimalTest,
	&modalWindowHandlerTest,
#ifdef AG_NETWORK
	&networkTest,
#endif
	&objSystemTest,
	&paneTest,
	&plottingTest,
	&renderToSurfaceTest,
	&scrollbarTest,
	&scrollviewTest,
	&socketsTest,
	&stringTest,
	&tableTest,
	&textboxTest,
	&textDlgTest,
	&themesTest,
	&threadsTest,
	&timeoutsTest,
	&unitconvTest,
	&windowsTest,
	NULL
};

TAILQ_HEAD_(ag_test_instance) tests;		/* Running tests */
AG_Statusbar *statusBar;
AG_Label *status;
AG_Console *console;
AG_Button *btnTest, *btnBench;

static void
SelectedTest(AG_Event *event)
{
	AG_TlistItem *it = AG_PTR(1);
	AG_TestCase *tc = it->p1;

	AG_LabelText(status, "%s: %s", tc->name, tc->descr);
	if (tc->test != NULL || tc->testGUI != NULL) {
		AG_WidgetEnable(btnTest);
	} else {
		AG_WidgetDisable(btnTest);
	}
	if (tc->bench != NULL) {
		AG_WidgetEnable(btnBench);
	} else {
		AG_WidgetDisable(btnBench);
	}
}

#ifdef AG_THREADS
static void *
RunBenchmarks(void *arg)
{
	AG_TestInstance *ti = arg;

	if (ti->tc->bench(ti) == 0) {
		AG_ConsoleMsg(console, _("%s: Success"), ti->tc->name);
	} else {
		AG_ConsoleMsg(console, _("%s: Failed (%s)"), ti->tc->name,
		    AG_GetError());
	}
	return (NULL);
}
#endif

static AG_TestInstance *
CreateTestInstance(AG_TestCase *tc)
{
	AG_TestInstance *ti;

	if ((ti = TryMalloc(tc->size)) == NULL) {
		goto fail;
	}
	ti->name = tc->name;
	ti->tc = tc;
	ti->flags = 0;
	ti->score = 1.0;
	ti->console = console;
	ti->win = NULL;

	if (tc->init != NULL &&
	    tc->init(ti) == -1) {
		AG_ConsoleMsg(console, _("%s: Failed: %s"), tc->name, AG_GetError());
		goto fail;
	}
	return (ti);
fail:
	AG_LabelTextS(status, AG_GetError());
	return (NULL);
}

static void
RequestTestClose(AG_Event *event)
{
	AG_Window *win = AG_PTR(1);
	
	AG_PostEvent(NULL, win, "window-close", NULL);
}

static void
RunTest(AG_Event *event)
{
	AG_Tlist *tl = AG_PTR(1);
	AG_Window *winParent = AG_PTR(2);
	AG_TestCase *tc = AG_TlistSelectedItemPtr(tl);
	AG_TestInstance *ti;

	if (tc == NULL || (tc->test == NULL && tc->testGUI == NULL))
		return;

	if ((ti = CreateTestInstance(tc)) == NULL)
		return;

	if (tc->test != NULL) {
		AG_ConsoleMsg(console, _("Running test: %s..."), tc->name);
		ti->score = 100.0;
		if (tc->test(ti) == 0) {
			if (ti->score != 100.0) {
				AG_ConsoleMsg(console, _("%s: Success (%f%%)"),
				    tc->name, ti->score);
			} else {
				AG_ConsoleMsg(console, _("%s: Success"), tc->name);
			}
		} else {
			AG_ConsoleMsg(console, _("%s: Failed (%s)"), tc->name,
			    AG_GetError());
			AG_LabelTextS(status, AG_GetError());
			goto fail;
		}
	}
	if (tc->testGUI != NULL) {
		AG_Window *win;

		if ((win = AG_WindowNew(0)) == NULL) {
			AG_LabelTextS(status, AG_GetError());
			return;
		}
		AG_WindowSetCaption(win, "agartest: %s", ti->name);
		AG_SetEvent(win, "window-close", TestWindowClose, "%p", ti);
	
		if (tc->testGUI(ti, win) == 0) {
			ti->win = win;
			AG_ConsoleMsg(console, _("%s: Interactive test started"),
			    tc->name);
			AG_SeparatorNewHoriz(win);
			AG_ButtonNewFn(win, AG_BUTTON_HFILL, _("Close this test"),
			    RequestTestClose, "%p", win);
			AG_WindowSetPosition(win, AG_WINDOW_MC, 0);
			AG_WindowAttach(winParent, win);
			AG_WindowShow(win);
		} else {
			AG_ConsoleMsg(console, _("%s: Failed to start (%s)"),
			    tc->name, AG_GetError());
			AG_ObjectDetach(win);
			goto fail;
		}
	}
	TAILQ_INSERT_TAIL(&tests, ti, instances);
	return;
fail:
	Free(ti);
}

static void
RunBench(AG_Event *event)
{
	AG_Tlist *tl = AG_PTR(1);
	AG_TestCase *tc = AG_TlistSelectedItemPtr(tl);
	AG_TestInstance *ti;

	if (tc == NULL || tc->bench == NULL)
		return;
	
	if ((ti = CreateTestInstance(tc)) == NULL)
		return;

	{
#ifdef AG_THREADS
		AG_Thread th;
		AG_ThreadCreate(&th, RunBenchmarks, ti);
#else
		if (tc->bench(ti) == 0) {
			AG_ConsoleMsg(console, _("%s: Success"), tc->name);
		} else {
			AG_ConsoleMsg(console, _("%s: Failed (%s)"), tc->name,
			    AG_GetError());
			AG_LabelTextS(status, AG_GetError());
			free(ti);
			return;
		}
#endif
	}
	TAILQ_INSERT_TAIL(&tests, ti, instances);
	return;
}

/* Close an interactive test. */
void
TestWindowClose(AG_Event *event)
{
	AG_TestInstance *ti = AG_PTR(1);
	
	AG_ConsoleMsg(console, _("Test %s: terminated"), ti->name);
	AG_ObjectDetach(ti->win);
	TAILQ_REMOVE(&tests, ti, instances);
	free(ti);
}

/* Write a message to the test console (format string). */
void
TestMsg(void *obj, const char *fmt, ...)
{
	AG_TestInstance *ti = obj;
	va_list args;
	char *s;

	va_start(args, fmt);
	if (TryVasprintf(&s, fmt, args) == -1) {
		return;
	}
	va_end(args);
	AG_ConsoleMsgS(ti->console, s);
	free(s);
}

/* Write a message to the test console (C string). */
void
TestMsgS(void *obj, const char *s)
{
	AG_TestInstance *ti = obj;
	AG_ConsoleMsgS(ti->console, s);
}

#undef RDTSC
#if (defined(i386) || defined(__i386__) || defined(__x86_64__))
# define RDTSC(t) __asm __volatile__ (".byte 0x0f, 0x31; " : "=A" (t))
#endif

/* Execute a benchmark module (called from bench() op) */
void
TestExecBenchmark(void *obj, AG_Benchmark *bm)
{
	AG_TestInstance *ti = obj;
	Uint i, j, fIdx;
#if defined(HAVE_64BIT)
	Uint64 t1, t2;
	Uint64 tTot, tRun;
#else
	Uint32 t1, t2;
	Uint32 tTot, tRun;
#endif

	for (fIdx = 0; fIdx < bm->nFuncs; fIdx++) {
		char pbuf[64];
		AG_BenchmarkFn *bfn = &bm->funcs[fIdx];
		AG_ConsoleLine *cl;

		bfn->clksMax = 0;
		cl = AG_ConsoleMsg(ti->console, "\t%s: ...", bfn->name);
#ifdef RDTSC
		if (agCPU.ext & AG_EXT_TSC) {
			for (i = 0, tTot = 0; i < bm->runs; i++) {
retry:
				RDTSC(t1);
				for (j = 0; j < bm->iterations; j++) {
					bfn->run(ti);
				}
				RDTSC(t2);
				tRun = (t2 - t1) / bm->iterations;
				
				Snprintf(pbuf, sizeof(pbuf),
				    "\t%s: %lu clks [%i/%i]",
				    bfn->name,
				    (Ulong)tRun, i, bm->runs);
				AG_ConsoleMsgEdit(cl, pbuf);

				if (bm->maximum > 0 && tRun > bm->maximum) {
					Snprintf(pbuf, sizeof(pbuf),
					    "\t%s: <preempted>", bfn->name);
					AG_ConsoleMsgEdit(cl, pbuf);
					goto retry;
				}
				bfn->clksMax = AG_MAX(bfn->clksMax,tRun);
				bfn->clksMin = (bfn->clksMin > 0) ?
				               AG_MIN(bfn->clksMin,tRun) :
					       tRun;
				tTot += tRun;
			}
			bfn->clksAvg = (tTot / bm->runs);
			Snprintf(pbuf, sizeof(pbuf), "\t%s: %lu clks [%i]",
			    bfn->name, (Ulong)bfn->clksAvg, bm->runs);
			AG_ConsoleMsgEdit(cl, pbuf);
		} else
#endif /* RDTSC */
		{
			for (i = 0, tTot = 0; i < bm->runs; i++) {
				t1 = AG_GetTicks();
				for (j = 0; j < bm->iterations; j++) {
					bfn->run(ti);
				}
				t2 = AG_GetTicks();
				tRun = (t2 - t1);
				Snprintf(pbuf, sizeof(pbuf),
				    "\t%s: %lu ticks [%i/%i]",
				    bfn->name, (Ulong)tRun, i, bm->runs);
				AG_ConsoleMsgEdit(cl, pbuf);
				bfn->clksMax = AG_MAX(bfn->clksMax,tRun);
				bfn->clksMin = (bfn->clksMin > 0) ?
				               AG_MIN(bfn->clksMin,tRun) :
					       tRun;
				tTot += tRun;
			}
			Snprintf(pbuf, sizeof(pbuf), "\t%s: %lu ticks [%i]",
			    bfn->name, (Ulong)bfn->clksAvg, bm->runs);
			AG_ConsoleMsgEdit(cl, pbuf);
		}
	}
}

#ifdef AG_DEBUG
static void
StartDebugger(void)
{
	AG_Window *win;

	if ((win = AG_GuiDebugger(agWindowFocused)) != NULL)
		AG_WindowShow(win);
}
#endif

int
main(int argc, char *argv[])
{
	char *driverSpec = NULL, *fontSpec = NULL, *optArg;
	AG_Window *win;
	AG_Tlist *tl;
	const AG_TestCase **pTest;
	AG_Pane *pane;
	AG_Box *hBox;
	int c, i, optInd;
	Uint initFlags = AG_VERBOSE;

	TAILQ_INIT(&tests);

	while ((c = AG_Getopt(argc, argv, "?hd:t:T", &optArg, &optInd)) != -1) {
		switch (c) {
		case 'd':
			driverSpec = optArg;
			break;
		case 't':
			fontSpec = optArg;
			break;
		case 'T':
			initFlags |= AG_SOFT_TIMERS;
			break;
		case '?':
		case 'h':
		default:
			printf("Usage: agartest [-T] [-d agar-driver] [-t font] [test1 test2 ...]\n");
			return (1);
		}
	}
	if (AG_InitCore("agartest", initFlags) == -1) {
		printf("Agar-Core initialization failed: %s\n", AG_GetError());
		return (1);
	}
	if (fontSpec != NULL) {
		AG_TextParseFontSpec(fontSpec);
	}
	if (AG_InitGraphics(driverSpec) == -1) {
		printf("Agar-GUI initialization failed: %s\n", AG_GetError());
		return (1);
	}
#ifdef __APPLE__
	AG_BindGlobalKey(AG_KEY_EQUALS,	AG_KEYMOD_META,	AG_ZoomIn);
	AG_BindGlobalKey(AG_KEY_MINUS,	AG_KEYMOD_META,	AG_ZoomOut);
	AG_BindGlobalKey(AG_KEY_0,	AG_KEYMOD_META,	AG_ZoomReset);
	AG_BindGlobalKey(AG_KEY_Q,	AG_KEYMOD_META,	AG_QuitGUI);
# ifdef AG_DEBUG
	AG_BindGlobalKey(AG_KEY_D,	AG_KEYMOD_ANY,	StartDebugger);
# endif
	AG_BindGlobalKey(AG_KEY_C,	AG_KEYMOD_ANY,	AG_ViewCapture);
#else
	AG_BindGlobalKey(AG_KEY_EQUALS,	AG_KEYMOD_CTRL,	AG_ZoomIn);
	AG_BindGlobalKey(AG_KEY_MINUS,	AG_KEYMOD_CTRL,	AG_ZoomOut);
	AG_BindGlobalKey(AG_KEY_0,	AG_KEYMOD_CTRL,	AG_ZoomReset);
	AG_BindGlobalKey(AG_KEY_ESCAPE,	AG_KEYMOD_ANY,	AG_QuitGUI);
# ifdef AG_DEBUG
	AG_BindGlobalKey(AG_KEY_F12,	AG_KEYMOD_ANY,	StartDebugger);
# endif
	AG_BindGlobalKey(AG_KEY_F8,	AG_KEYMOD_ANY,	AG_ViewCapture);
#endif

	if (strcmp(DATADIR, "NONE") != 0) {
		char path[AG_PATHNAME_MAX];
		AG_GetString(agConfig, "load-path", path, sizeof(path));
		AG_Strlcat(path, ":", sizeof(path));
		AG_Strlcat(path, DATADIR, sizeof(path));
		AG_SetString(agConfig, "load-path", path);
	}
/*	(void)AG_ConfigLoad(); */

	if ((win = AG_WindowNew(0)) == NULL) {
		return (1);
	}
	AG_WindowSetCaptionS(win, "agartest");

	pane = AG_PaneNewHoriz(win, AG_PANE_EXPAND);

	AG_LabelNewS(pane->div[0], 0, _("Available tests: "));
	tl = AG_TlistNew(pane->div[0], AG_TLIST_EXPAND);
	AG_TlistSizeHint(tl, "XXXXXXXXXXXXXXXXXX", 5);
	for (pTest = &testCases[0]; *pTest != NULL; pTest++) {
		AG_TlistAddPtr(tl, NULL, (*pTest)->name, (void *)*pTest);
	}

	hBox = AG_BoxNewHoriz(pane->div[0], AG_BOX_HFILL);
	{
		btnTest = AG_ButtonNew(hBox, AG_BUTTON_EXCL, _("Run Test"));
		btnBench = AG_ButtonNew(hBox, AG_BUTTON_EXCL, _("Run Benchmark"));
		AG_WidgetDisable(btnTest);
		AG_WidgetDisable(btnBench);
	}
	console = AG_ConsoleNew(pane->div[1], AG_CONSOLE_EXPAND);
/*	AG_SetFontFamily(console, "Terminus,Terminal"); */
	{
		char drvNames[256];
		AG_AgarVersion av;

		AG_GetVersion(&av);
		AG_ConsoleMsg(console, _("Agar %d.%d.%d (\"%s\")"),
		    av.major, av.minor, av.patch, av.release);
		AG_ConsoleMsg(console, _("Current Agar driver: %s (%s)"),
		    AGWIDGET(win)->drvOps->name,
		    (AGWIDGET(win)->drvOps->type == AG_FRAMEBUFFER) ?
		    _("framebuffer-based") : _("vector-based"));
		AG_ListDriverNames(drvNames, sizeof(drvNames));
		AG_ConsoleMsg(console, _("Available Agar drivers: %s"), drvNames);
#ifdef __APPLE__
		AG_ConsoleMsg(console, _("Press Command-[-] and Command-[=] to zoom"));
# ifdef AG_DEBUG
		AG_ConsoleMsg(console, _("Press Command-D to debug active window"));
# endif
#else
		AG_ConsoleMsg(console, _("Press Ctrl-[-] and Ctrl-[=] to zoom"));
# ifdef AG_DEBUG
		AG_ConsoleMsg(console, _("Press F12 to debug active window"));
# endif
#endif
	}

	AG_TlistSetChangedFn(tl, SelectedTest, NULL);
	AG_TlistSetDblClickFn(tl, RunTest, "%p,%p", tl, win);
	AG_SetEvent(btnTest, "button-pushed", RunTest, "%p,%p", tl, win);
	AG_SetEvent(btnBench, "button-pushed", RunBench, "%p", tl);

	statusBar = AG_StatusbarNew(win, AG_STATUSBAR_HFILL);
	status = AG_StatusbarAddLabel(statusBar, _("Please select a test"));

	AG_WindowSetGeometryAlignedPct(win, AG_WINDOW_MC, 60, 60);
	AG_WindowShow(win);
	
	for (i = optInd; i < argc; i++) {
		AG_Event ev;

		for (pTest = &testCases[0]; *pTest != NULL; pTest++) {
			if (strcmp((*pTest)->name, argv[i]) == 0)
				break;
		}
		if (*pTest == NULL) {
			AG_ConsoleMsg(console, _("No such test: %s"), argv[i]);
			continue;
		}
		AG_TlistSelectPtr(tl, (void *)(*pTest));
		AG_EventArgs(&ev, "%p,%p", tl, win);
		RunTest(&ev);
		RunBench(&ev);
	}

	AG_EventLoop();
	AG_Destroy();
	return (0);
}
