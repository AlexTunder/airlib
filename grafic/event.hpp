#pragma once
namespace globus{
    namespace event{
        void *eventBuffer[256];
        class Event{
        public:
            char *CODE;
            long long id;
            void **pArg = new void*;
            int push();
            Event(char *CODE, long long id, void **pArg);
        };
        class Event32 : public Event{
        public:
            void (*doAtEvent)(void*args[256]);
        };
        class Handler{

        };
        Event getNextEvent(){
            Event event = *(Event*)eventBuffer[0];
            for(int i = 0; i<255&&eventBuffer[i]!=nullptr; i++)
                eventBuffer[i] = eventBuffer[i+1];
            return event;
        }
        int Event::push(){
            int last = 0;
            for(;eventBuffer[last]!=nullptr;last++);
            eventBuffer[last] = new Event(*this);
            return 0;
        }
        Event::Event(char *CODE, long long id, void **pArg){
            this->CODE = CODE;
            this->id = id;
            this->pArg = pArg;
        }
    };
};

