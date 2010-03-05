/*	Public domain	*/
/*
 * This test ensures Agar can be destroyed and re-initialized.
 */

#include <agar/core.h>
#include <agar/gui.h>

int
main(int argc, char *argv[])
{
	AG_Window *win;
	int i;

	for (i = 0; i < 100; i++) {
		printf("Test %d/100:\n", i);
		printf("\tInitCore()\n");
		if (AG_InitCore("agar-reinit-demo", 0) == -1) {
			printf("AG_InitCore(%d): %s\n", i, AG_GetError());
			exit(1);
		}
		printf("\tInitGraphics()\n");
		if (AG_InitGraphics(0) == -1) {
			printf("AG_InitGraphics(%d): %s\n", i, AG_GetError());
			exit(1);
		}
		printf("\tCreate Window\n");
		win = AG_WindowNew(0);
		AG_WindowSetCaption(win, "foo");
		AG_WindowShow(win);
		printf("\tDestroyVideo()\n");
		AG_DestroyVideo();
		printf("\tDestroy()\n");
		AG_Destroy();
	}
	printf("Test successful\n");
	return (0);
}
