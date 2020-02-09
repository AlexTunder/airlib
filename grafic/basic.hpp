#pragma once
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <pthread.h>
#include "event.hpp"

#define GRAFIC_EVENT_NEWWINDOW      0x1C5F
#define GRAFIC_EVENT_XEVENT_NONE    0xAC5F
#define GRAFIC_EVENT_XEVENT_KEYP    0xC0F1
#define GRAFIC_GL_CONTEXT_FAIL      0xC3F5 
// #define GDrawList (void*)()

namespace globus{
    int defaultDraw(void *args){
        glClear(GL_COLOR_BUFFER_BIT);
    }
    class XData{ //It's root XSERVER data with init & others
    private:
        GLXContext              **glcl;
        XID **pages;        int size = 8;
        pthread_t wthread;  int withread; //Thread variables
        static void *throwMe(void *args);
        int enable(Display *dpy, XID *root, GLint *att,XVisualInfo *vi, Colormap *cmap,\
            XSetWindowAttributes *swa, GLXContext *glc, XWindowAttributes *gwa, XEvent *xev);
        struct argList{
            char *__TRANSERBUFFER__ = new char;
            Display                 *dpy;
            XID                     *root;
            GLint                   *att;
            XVisualInfo             *vi;
            Colormap                *cmap;
            XSetWindowAttributes    *swa;
            GLXContext              *glc;
            XWindowAttributes       *gwa;
            XEvent                  *xev;
            int                     *cop;
            XID                     **wl;
            GLXContext              **glcl;
            int (**drw)(void *args);
        };
        argList ARL;
    public:
        int pushToPL(XID *xid, GLXContext *glcl, int (*draw)(void *args));
        XData(int pagesCount = 8) : pages(new XID*[pagesCount]), size(pagesCount),\
            glcl(new GLXContext*[pagesCount]), drw(new (int(*)(void*))){
            glClearColor(1,1,1,0.1);
            this->dpy = XOpenDisplay(NULL);
            if(this->dpy == nullptr){
                printf("XSERVER fail");
            }
            this->root = DefaultRootWindow(dpy);
            this->vi = glXChooseVisual(dpy,0,att);
            if(this->vi == NULL){
                printf("att not found!");
            } else printf("\n\tvisual %p selected\n", (void *)this->vi->visualid);
            this->cmap = XCreateColormap(this->dpy, this->root, this->vi->visual, AllocNone);
            this->swa.colormap = this->cmap;
            this->swa.event_mask = ExposureMask | KeyPressMask;
            ARL.dpy = this->dpy;
            ARL.root = &this->root;
            ARL.att = this->att;
            ARL.vi = this->vi;
            ARL.cmap = &this->cmap;
            ARL.swa = &this->swa;
            ARL.glc = &this->glc;
            ARL.gwa = &this->gwa;
            ARL.xev = &this->xev;
            ARL.wl = this->pages;
            ARL.cop = &this->size;
            ARL.glcl = this->glcl;
            init();
        }
        char *__TRANSERBUFFER__ = new char;
        Display                 *dpy;
        XID                     root;
        GLint                   att[10] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        XVisualInfo             *vi;
        Colormap                cmap;
        XSetWindowAttributes    swa;
        GLXContext              glc;
        XWindowAttributes       gwa;
        XEvent                  xev;
        int (**drw)(void *args);
        int init();
    };
    class GLOrtho{ //It's a ortho settings. Need for convert local ortho to global

    };
    class GLSpace{ //It's a class for GL drawing. It's need to make local space fpr ddrawing

    };
    class Drawable{ //It's need to display any elements in window
    private:
    public:
        float clearColor[3];
        bool isConstrain;
        void *args = nullptr;
        int (*draw)(void *args);
        Drawable(bool isConstrain = false, int (*draw)(void *args) = nullptr, void *arg = nullptr)
            :isConstrain(isConstrain), draw(draw), args(arg){
        }
        int operator()(void *arg = nullptr){
            if(arg != nullptr)
                draw(arg);
            else if(args != nullptr)
                draw(args);
        }
    };
    class Component{ //It's a typical visual component. It's United events & processes
    private:

    public:

    };
};
#include "basic.cpp"