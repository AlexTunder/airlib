#pragma once
#include "basic.hpp"
namespace globus{
    class Window{
    private:
        GLXContext glc;
        XData *xda;
        struct XWINADC{
            float ortho[2];
            bool isConstrain;
            char *name;
        };
    public:
        // Drawable drawable;
        void setBackground(float r, float g, float b);
        Drawable drawable;
        XID *parent, me;
        char *name = new char;
        Window(XData *xda, XID *parent, const char *name = "XWIN"){
            this->xda = xda;
            this->parent = parent;
            for(int i = 0; name[i] != 0; i++)
                this->name[i] = name[i];
            this->xda->pushToPL(&me, &this->glc, drawable.draw);
            this->glc = glXCreateContext(this->xda->dpy, this->xda->vi, NULL, GL_TRUE);
        }
        int show();
    };
};
#include "window.cpp"