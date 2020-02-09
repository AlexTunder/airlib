#include "./basic.hpp"
namespace globus{
    namespace inspect{
        class BasicFragment{
        public:
            BasicFragment *child = nullptr;
            int *Xvertexes;
            int *Yvertexes;
            int *XFvertexes;
            int *YFvertexes;
            float bg[3], fg[3];
            size_t *algo;
            size_t *Falgo;
            BasicFragment(int sizeB, int sizeF, float fg[3], float bg[3], int *bxv,\
                int *byv, int *fxv = nullptr, int *fyv = nullptr, BasicFragment *child = nullptr){
                Xvertexes = bxv;
                Yvertexes = byv;
                XFvertexes = fxv;
                YFvertexes = fyv;
                algo = new size_t[sizeB];
                Falgo = new size_t[sizeF];
            }
            void draw(){
                glColor3f(bg[0],bg[1],bg[2]);
                glBegin(GL_POLYGON);
                for(int i = 0; i < sizeof(unsigned long); i++){
                    glVertex2f(Xvertexes[algo[i]],Yvertexes[algo[i]]);
                }
                glEnd();
                glColor3f(fg[0],fg[1],fg[2]);
                glBegin(GL_POLYGON);
                for(int i = 0; i < (1<<(sizeof(unsigned long)*8)); i++){
                    glVertex2f(XFvertexes[Falgo[i]],YFvertexes[Falgo[i]]);
                }
                glEnd();
            }
        };
        
    }
}