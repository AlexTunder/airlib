#include "../grafic/basic.hpp"
#include "../grafic/event.hpp"
#include "../grafic/window.hpp"

int darwWin1(void *arg){
    glBegin(GL_POLYGON);
    glOrtho(1,0,1,0,1,0);
    glColor3f(1,0,0.42);
    glVertex2f(0.4,0.4);
    glVertex2f(0.6,0.4);
    glVertex2f(0.6,0.6);
    glVertex2f(0.4,0.6);
    glEnd();
}

globus::XData xdata(8);
globus::Window window1(&xdata,&xdata.root,"First launch");
int main(){
    window1.drawable.draw = darwWin1;
    window1.show();
    while(1);
    return 0;
}
