#ifndef MEMORY_H
#define MEMORY_H
#include "main.h"

class Memory
{
public:
    Memory(){

    }
    ~Memory(){
        for(auto iter : m_lHeap){
            free(iter.data);
        }
    }
    //only for load
    void MallocAtAddr(DWORD addr, DWORD size){
        heap_entity ent;
        ent.virtual_addr = addr;
        ent.data = malloc(size);
        ent.len = size;
        add_heap(ent);
    }
    DWORD Malloc(DWORD size){
        heap_entity ent;
        ent.virtual_addr = find_free_addr(size);
        if(ent.virtual_addr == -1){
            return 0;
        } else {
            ent.data = malloc(size);
            ent.len = size;
            add_heap(ent);
            return ent.virtual_addr;
        }
    }
    void Free(DWORD a_VirtualAddr){
        heap_entity ent;
        ent.virtual_addr = a_VirtualAddr;
        remove_heap(ent);
    }
    DWORD GetRealAddr(DWORD a_VirtualAddr){
        auto ent = find_heap(a_VirtualAddr);
        if(ent){
            return (DWORD)ent->data;
        } else {
            return 0;
        }
    }
    int MemCmp(DWORD dest_virtual, DWORD src_virtual){
        heap_entity *dest_ent = find_heap(dest_virtual);
        heap_entity *src_ent = find_heap(src_virtual);
        if(dest_ent && src_ent){
            for(int i = 0; i < dest_ent->len && i < src_ent->len; i++){
                if(*(BYTE*)((DWORD)dest_ent->data + i) != *(BYTE*)((DWORD)src_ent->data + i)){
                    return -1;
                }
            }
            return 0;
        }
        return 1;
    }
    void MemSet(DWORD dest_virtual, DWORD dest_size, void *data, DWORD data_len){
        heap_entity *ent = find_heap(dest_virtual);
        if(ent){
            if(ent->len <= dest_size){
                if(data_len <= ent->len){
                    for(int i = 0; i < data_len; i++){
                        *(BYTE*)((DWORD)ent->data + i) = *(BYTE*)((DWORD)data + i);
                    }
                }
            }
        }
    }
private:
    struct heap_entity{
        DWORD virtual_addr;
        DWORD len;
        void *data;
    };
    std::list<heap_entity> m_lHeap;

    DWORD find_free_addr(DWORD len){
        DWORD last_len = 0;
        DWORD ret_addr = 0x0;
        DWORD prev_addr = 0x0;

        heap_entity last;
        if(m_lHeap.size() > 0) last = m_lHeap.back();

        if(m_lHeap.size() == 0){
            return rand() % 0x0FFFFF + 0xF00000;
        }

        for(auto iter = m_lHeap.begin(); iter != m_lHeap.end(); *iter++){
            if(prev_addr == 0){
                prev_addr = iter->virtual_addr;
                continue;
            } else {
                last_len = iter->virtual_addr - prev_addr;
                //ret_addr = iter->virtual_addr + iter->len;
                if(last_len >= len){

                    heap_entity *prev_ent = find_heap(prev_addr);
                    if(last_len - prev_ent->len >= len)
                        return prev_ent->virtual_addr + prev_ent->len;
                }
                prev_addr = iter->virtual_addr;
            }
        }
        if(ret_addr == 0x0){
            return last.virtual_addr + last.len;
        }
        return ret_addr;
    }
    void add_heap(heap_entity ent){
        m_lHeap.push_back(ent);
        /*std::sort(m_lHeap.begin(), m_lHeap.end(), [](const heap_entity& first, const heap_entity& sec) -> int{
            return first.virtual_addr > sec.virtual_addr;
        });
        */
       // m_lHeap.sort();
    }
    void remove_heap(heap_entity ent){
        for(auto iter = m_lHeap.begin(); iter != m_lHeap.end(); *iter++){
            if(iter->virtual_addr == ent.virtual_addr){
                free(iter->data);
                m_lHeap.erase(iter);
                break;
            }
        }
    }
    heap_entity *find_heap(DWORD virtual_addr){
        for(auto iter = m_lHeap.begin(); iter != m_lHeap.end(); *iter++){
            if(iter->virtual_addr == virtual_addr){
                return &(*iter);
            }
        }
        return nullptr;
    }
};

static Memory g_Memory;



template <typename T>
class var{
private:
    DWORD virtual_addr;
public:
    typedef T & deref;
    T& getValue(){
        return *(T*)g_Memory.GetRealAddr(virtual_addr);
    }
    T* operator &(){
        return (T*)g_Memory.GetRealAddr(virtual_addr);
    }
    bool operator ==(const var& right){
        return g_Memory.MemCmp(right.virtual_addr, virtual_addr);
    }
    const var& operator =(const T& right){
        g_Memory.MemSet(virtual_addr, sizeof(T), (PVOID)&right, sizeof(T));
        return *this;
    }
    operator T(){
        return getValue();
    }
    var(const T& value){
        this->virtual_addr = g_Memory.Malloc(sizeof(T));
        g_Memory.MemSet(virtual_addr, sizeof(T), (PVOID)&value, sizeof(T));
    }
    var(const T*& value){
        this->virtual_addr = g_Memory.Malloc(sizeof(T));
        g_Memory.MemSet(virtual_addr, sizeof(T), (PVOID)&(*value), sizeof(T));
    }
    explicit var(){
        this->virtual_addr = g_Memory.Malloc(sizeof(T));
    }
    ~var(){
        g_Memory.Free(this->virtual_addr);
    }
};

#endif // MEMORY_H
