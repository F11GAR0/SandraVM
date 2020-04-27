#ifndef STACK_H
#define STACK_H
#include "main.h"

#define STACK_SIZE 256

class Stack{
public:
    Stack(){
        m_uiStackPointer = 0;
    }
    void push(DWORD data){
        m_dwStackBase[m_uiStackPointer++] = data;
    }
    DWORD pop(){
        return m_dwStackBase[--m_uiStackPointer];
    }
    unsigned int get_current_stack_pointer(){
        return m_uiStackPointer;
    }
private:
    Stack( const Stack& ) = delete;
    void operator=( const Stack& ) = delete;


    DWORD m_dwStackBase[STACK_SIZE];
    unsigned int m_uiStackPointer;
};

Stack g_Stack;

#endif // STACK_H
