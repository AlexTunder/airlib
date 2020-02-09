#include<stdio.h>
#include<stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

Display                 *dpy;
Window                  root;
GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;

bool disp(){
    //put here drawing code
    
    //return false; to return error message
    return true; //to return Ok message
}

int main(int argc, char **argv){
    glClearColor(0,0,0,0.1);

    dpy = XOpenDisplay(NULL);

    if(dpy == NULL){
        printf("XSERVER fail");
        return 1;
    }
    
    root = DefaultRootWindow(dpy);

    vi = glXChooseVisual(dpy,0,att);

    if(vi == NULL){
        printf("att not found!");
        return 2;
    } else {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid);
    }

    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask;

    win = XCreateWindow(dpy,root,0,0,600,600,0,vi->depth,InputOutput,vi->visual,CWColormap|CWEventMask,&swa);
    XMapWindow(dpy, win);

    XStoreName(dpy,win,"Omega IDE");

    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);

    glEnable(GL_DEPTH_TEST); 
    glOrtho(0,100,0,100,0,0);
    while(1) {
        XNextEvent(dpy, &xev);
        
        if(xev.type == Expose) {
                XGetWindowAttributes(dpy, win, &gwa);
                glViewport(0, 0, gwa.width, gwa.height);
                glClear(GL_COLOR_BUFFER_BIT);
                if(!disp()) return 0;
                glXSwapBuffers(dpy, win);
        }
                
        else if(xev.type == KeyPress) {
                glXMakeCurrent(dpy, None, NULL);
                glXDestroyContext(dpy, glc);
                XDestroyWindow(dpy, win);
                XCloseDisplay(dpy);
                exit(0);
        }
    }
}