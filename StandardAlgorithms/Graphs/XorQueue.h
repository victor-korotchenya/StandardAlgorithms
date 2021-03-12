#pragma once
#include "../Utilities/ExceptionUtilities.h"

namespace Privet::Algorithms::Numbers
{
    //A queue, storing items as XORed linked list.
    //It uses less memory than traditional queue, but it is slower.
    template <typename T>
    class XorQueue final
    {
        T* _Head;
        T* _Tail;
        size_t _Count;

        using AddressType = uintptr_t;

    public:
        using TType = T;

        XorQueue();

        void Enqueue(T* node);

        T* Dequeue();
    };

    template <typename T>
    XorQueue<T>::XorQueue()
        : _Head(nullptr), _Tail(nullptr), _Count(0)
    {
    }

    template <typename T>
    void XorQueue<T>::Enqueue(
        T* node)
    {
        ThrowIfNull(node, "node");

        switch (_Count)
        {
        case 0:
            node->Next = nullptr;
            _Head = node;
            break;

        case 1:
            _Tail = node;
            _Tail->Next = _Head;
            _Head->Next = _Tail;
            break;

        default:
            const AddressType address1 = reinterpret_cast<AddressType>(_Tail->Next);
            const AddressType address2 = reinterpret_cast<AddressType>(node);

            node->Next = _Tail;
            _Tail->Next = reinterpret_cast<T*>(address1 ^ address2);
            _Tail = node;
            break;
        }

        ++_Count;
    }

    template <typename T>
    T* XorQueue<T>::Dequeue()
    {
        T* result = _Head;

        switch (_Count)
        {
        case 0:
            return nullptr;

        case 1:
            _Head = nullptr;
            break;

        case 2:
            _Head = _Tail;
            _Head->Next = nullptr;
            _Tail = nullptr;
            break;

        default:
            _Head = _Head->Next;
            const AddressType difference = reinterpret_cast<AddressType>(_Head->Next);
            const AddressType b = reinterpret_cast<AddressType>(result);
            _Head->Next = reinterpret_cast<T*>(difference ^ b);
            break;
        }

        --_Count;

        result->Next = nullptr;
        return result;
    }
}