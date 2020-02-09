#pragma once
int globus::Window::show(){
    this->me = XCreateWindow(xda->dpy,*this->parent,0,0,600,600,0,\
    xda->vi->depth,InputOutput,xda->vi->visual,CWColormap|CWEventMask,&xda->swa);
    XSelectInput(xda->dpy, me, StructureNotifyMask | ExposureMask
    | KeyPressMask | ButtonPressMask | PointerMotionMask);
    XMapWindow(xda->dpy, this->me);
    XStoreName(xda->dpy,this->me,this->name);
    this->glc = glXCreateContext(xda->dpy, xda->vi, NULL, GL_TRUE);
    glEnable(GL_DEPTH_TEST); 
    glOrtho(0,100,0,100,0,0);
}