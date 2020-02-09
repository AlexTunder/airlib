#include <X11/X.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>

Display *display;
Window winA;
Window winB;
Window rootwindow;

unsigned long white_pixel, black_pixel;

void checkEvents() {

	XEvent xev;
	XWindowAttributes gwa;

	while (1) {

		printf("Next Iteration\n");

		// alternativly you can get the event of a determined window
		//XWindowEvent(display, winA, PointerMotionMask, &xev);

		// the next call will lock until an event was received.
		XNextEvent(display, &xev);

		switch (xev.type) {
		case Expose: {
			XGetWindowAttributes(display, winA, &gwa);
			printf("Exposed Window\n");
			XGetWindowAttributes(display, winB, &gwa);
			printf("Exposed Window\n");
			break;
		}
		case MotionNotify: {
			int MouseX = xev.xmotion.x;
			int MouseY = xev.xmotion.y;
			if (xev.xclient.window == winA) {
				printf("MouseA %i , %i\n", MouseX, MouseY);
			} else {
				printf("MouseB %i , %i\n", MouseX, MouseY);
			}
			break;
		}
		case ButtonPressMask: {
			printf("ButtonPressed - Destroying window\n");
			if (xev.xclient.window == winA) {
				XDestroyWindow(display, winA);
			} else {
				XDestroyWindow(display, winB);
			}
			break;
		}
		case KeyPress: {
			printf("Stopping event loop\n");
			return;
		}
		}
	}
}

void displayWinA() {

	printf("Creating window A\n");
	winA = XCreateSimpleWindow(display, rootwindow, 220, 220, 200, 200, 5,
			black_pixel, white_pixel);

	// name the window (titlebar)
	XStoreName(display, winA, "Window A");

	// since we used XCreateSimpleWindow we need to enable event listening for this window
	XSelectInput(display, winA, StructureNotifyMask | ExposureMask
			| KeyPressMask | ButtonPressMask | PointerMotionMask);

	// map the window (make it visible)
	XMapWindow(display, winA);

}

void displayWinB() {

	printf("Creating window B\n");
	winB = XCreateSimpleWindow(display, rootwindow, 0, 0, 100, 100, 1, 0, 0);
	XStoreName(display, winB, "Window B");
	XSelectInput(display, winB, StructureNotifyMask | ExposureMask
			| KeyPressMask | ButtonPressMask | PointerMotionMask);

	XMapWindow(display, winB);

}

int main(void) {

	int screen;

	display = XOpenDisplay(NULL);
	screen = DefaultScreen(display);

	white_pixel = WhitePixel (display, screen);
	black_pixel = BlackPixel (display, screen);

	rootwindow = RootWindow(display,screen);

	displayWinA();
	displayWinB();

	checkEvents();
	XFlush(display);

	XDestroyWindow(display, winA);
	XDestroyWindow(display, winB);
	XCloseDisplay(display);
	return (0);
}