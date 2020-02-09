#pragma once
#define a 0x8j//abcdefgHIJ
#include "basic.hpp"
int globus::XData::enable(Display *dpy, XID *root, GLint *att,XVisualInfo *vi, Colormap *cmap,\
    XSetWindowAttributes *swa, GLXContext *glc, XWindowAttributes *gwa, XEvent *xev){
    glClearColor(1,1,1,0.1);
    dpy = XOpenDisplay(NULL);
    if(dpy == nullptr){
        printf("XSERVER fail");
        return 1;
    }
    *root = DefaultRootWindow(dpy);
    vi = glXChooseVisual(dpy,0,att);
    if(vi == NULL){
        printf("att not found!");
        return 2;
    } else printf("\n\tvisual %p selected\n", (void *)vi->visualid);
    *cmap = XCreateColormap(dpy, *root, vi->visual, AllocNone);
    swa->colormap = *cmap;
    swa->event_mask = ExposureMask | KeyPressMask;
    XInitThreads();
    return 0;
}
void *globus::XData::throwMe(void *args){
    argList *argsl = (argList*)(args);
    while(true){
        glClearColor(1,1,1,0.5);
        XNextEvent(argsl->dpy, argsl->xev);
        globus::event::Event("X server event", argsl->xev->type, nullptr).push();
        for(int i = 0; i < *argsl->cop; i++){
            #ifdef stdinLOG
                printf("Checking (%i) window in list...",i);
            #endif
            if(argsl->wl[i] == nullptr){
            #ifdef stdinLOG
                printf("\t[!!] Fault: nullptr(%i)\n",i);
            #endif
                continue;
            }
            else if(argsl->xev->xclient.window==*argsl->wl[i]){
                #ifdef stdinLOG
                    printf("\t[**] Sucess: window(%i:%p)\n",i,argsl->wl[i]);
                #endif
                glXChooseVisual(argsl->dpy, argsl->vi->screen, argsl->att);
                XGetWindowAttributes(argsl->dpy, *argsl->wl[i], argsl->gwa);
                glXMakeContextCurrent(argsl->dpy, *argsl->wl[i], *argsl->wl[i], *argsl->glcl[i]);
                glViewport(0, 0, argsl->gwa->width, argsl->gwa->height);
                glEnable(GL_DEPTH_TEST); 
                glOrtho(0,100,0,100,0,0);
                #ifdef stdinLOG
                    printf("Event (%p:%i) called from (%p:%i) window\n",argsl->xev,argsl->xev->type,*argsl->wl[i]);
                    printf("Updatin (%p:%i) window\n", argsl->wl[i], *argsl->wl[i]);
                #endif
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glXSwapBuffers(argsl->dpy, *argsl->wl[i]);
                if(argsl->xev->type == 22) {
                    
                }       
                else if(argsl->xev->type == KeyPress) {
                    glXMakeCurrent(argsl->dpy, None, NULL);
                    glXDestroyContext(argsl->dpy, *argsl->glc);
                    XDestroyWindow(argsl->dpy, *argsl->wl[i]);
                    argsl->wl[i] = nullptr;
                }
            }
        }
    }
}
int globus::XData::init(){
    withread = pthread_create(&wthread,0,&throwMe,&ARL);
    pthread_detach(wthread);
    return 0;
}
int globus::XData::pushToPL(XID *xid, GLXContext *glcl, int (*draw)(void *args)){
    for(int i = 0; i < size; i++)
        if(pages[i] == nullptr){
            pages[i] = xid;
            this->glcl[i] = glcl;
            drw[i] = draw;
            return 1;
        } 
    return 0;
}