// lab2_sequence_structures.cpp
// Реализация DynamicArray, LinkedList, Sequence (mutable/immutable), UI и тесты

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <functional>
using namespace std;

// ----------------------- DynamicArray -----------------------
template <typename T>
class DynamicArray {
private:
    T* data;
    int size;

public:
    DynamicArray(int size) {
        if (size < 0) throw invalid_argument("Negative size");
        this->size = size;
        data = new T[size];
    }

    DynamicArray(T* items, int count) : DynamicArray(count) {
        for (int i = 0; i < count; ++i)
            data[i] = items[i];
    }

    DynamicArray(const DynamicArray& other) : DynamicArray(other.size) {
        for (int i = 0; i < size; ++i)
            data[i] = other.data[i];
    }

    ~DynamicArray() {
        delete[] data;
    }

    T Get(int index) const {
        if (index < 0 || index >= size) throw out_of_range("Index out of range");
        return data[index];
    }

    void Set(int index, T value) {
        if (index < 0 || index >= size) throw out_of_range("Index out of range");
        data[index] = value;
    }

    int GetSize() const {
        return size;
    }

    void Resize(int newSize) {
        if (newSize < 0) throw invalid_argument("Negative size");
        T* newData = new T[newSize];
        for (int i = 0; i < min(size, newSize); ++i)
            newData[i] = data[i];
        delete[] data;
        data = newData;
        size = newSize;
    }
};

// ----------------------- LinkedList -----------------------
template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(T val) : data(val), next(nullptr) {}
    };
    Node* head;
    int length;

public:
    LinkedList() : head(nullptr), length(0) {}

    LinkedList(T* items, int count) : LinkedList() {
        for (int i = 0; i < count; ++i)
            Append(items[i]);
    }

    LinkedList(const LinkedList& other) : LinkedList() {
        Node* curr = other.head;
        while (curr) {
            Append(curr->data);
            curr = curr->next;
        }
    }

    ~LinkedList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    T Get(int index) const {
        if (index < 0 || index >= length) throw out_of_range("Index out of range");
        Node* curr = head;
        for (int i = 0; i < index; ++i)
            curr = curr->next;
        return curr->data;
    }

    void Append(T item) {
        Node* newNode = new Node(item);
        if (!head) head = newNode;
        else {
            Node* curr = head;
            while (curr->next) curr = curr->next;
            curr->next = newNode;
        }
        length++;
    }

    int GetLength() const {
        return length;
    }
};

// ----------------------- Sequence -----------------------
template <typename T>
class Sequence {
public:
    virtual T Get(int index) const = 0;
    virtual int GetLength() const = 0;
    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Clone() const = 0;
    virtual ~Sequence() {}
};
// ArraySequence (mutable/immutable)
template <typename T>
class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T> items;
public:
    ArraySequence() : items(0) {}
    ArraySequence(T* arr, int count) : items(arr, count) {}
    ArraySequence(const ArraySequence& other) : items(other.items) {}
    ArraySequence(const DynamicArray<T>& array) : items(array) {} // 🔧 Добавлен конструктор

    T Get(int index) const override {
        return items.Get(index);
    }

    int GetLength() const override {
        return items.GetSize();
    }

    Sequence<T>* Append(T item) override {
        DynamicArray<T> copy = items;
        copy.Resize(copy.GetSize() + 1);
        copy.Set(copy.GetSize() - 1, item);
        return new ArraySequence<T>(copy);
    }

    Sequence<T>* Clone() const override {
        return new ArraySequence<T>(*this);
    }
};

// MutableArraySequence: изменения в себе
template <typename T>
class MutableArraySequence : public ArraySequence<T> {
public:
    using ArraySequence<T>::items;
    Sequence<T>* Append(T item) override {
        items.Resize(items.GetSize() + 1);
        items.Set(items.GetSize() - 1, item);
        return this;
    }
    Sequence<T>* Clone() const override {
        return new MutableArraySequence<T>(*this);
    }
};

// ImmutableArraySequence: возвращает копии
template <typename T>
class ImmutableArraySequence : public ArraySequence<T> {
public:
    using ArraySequence<T>::items;
    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(const DynamicArray<T>& array) : ArraySequence<T>(array) {} // 🔧 Добавлен конструктор

    Sequence<T>* Append(T item) override {
        DynamicArray<T> copy = items;
        copy.Resize(copy.GetSize() + 1);
        copy.Set(copy.GetSize() - 1, item);
        return new ImmutableArraySequence<T>(copy);
    }
    Sequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }
};
// ---------------- ListSequence (mutable/immutable) ----------------
template <typename T>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T> items;
public:
    ListSequence() : items() {}
    ListSequence(T* arr, int count) : items(arr, count) {}
    ListSequence(const LinkedList<T>& list) : items(list) {}
    ListSequence(const ListSequence<T>& other) : items(other.items) {}

    T Get(int index) const override {
        return items.Get(index);
    }

    int GetLength() const override {
        return items.GetLength();
    }

    Sequence<T>* Append(T item) override {
        LinkedList<T> copy = items;
        copy.Append(item);
        return new ListSequence<T>(copy);
    }

    Sequence<T>* Clone() const override {
        return new ListSequence<T>(*this);
    }
};

template <typename T>
class MutableListSequence : public ListSequence<T> {
public:
    using ListSequence<T>::items;
    Sequence<T>* Append(T item) override {
        items.Append(item);
        return this;
    }
    Sequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }
};

template <typename T>
class ImmutableListSequence : public ListSequence<T> {
public:
    using ListSequence<T>::items;
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(const LinkedList<T>& list) : ListSequence<T>(list) {}

    Sequence<T>* Append(T item) override {
        LinkedList<T> copy = items;
        copy.Append(item);
        return new ImmutableListSequence<T>(copy);
    }
    Sequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }
};

// ----------------------- Тесты -----------------------
void RunTests() {
    // DynamicArray
    DynamicArray<int> da(3);
    da.Set(0, 1); da.Set(1, 2); da.Set(2, 3);
    assert(da.Get(1) == 2);

    // LinkedList
    int arr[] = {1, 2, 3};
    LinkedList<int> ll(arr, 3);
    assert(ll.Get(0) == 1);
    assert(ll.GetLength() == 3);

    // MutableArraySequence
    MutableArraySequence<int> mas;
    mas.Append(10);
    assert(mas.GetLength() == 1);

    // ImmutableArraySequence
    ImmutableArraySequence<int> ias;
    Sequence<int>* newSeq = ias.Append(20);
    assert(ias.GetLength() == 0);
    assert(newSeq->GetLength() == 1);
    
    // MutableListSequence
    MutableListSequence<int> mls;
    mls.Append(100);
    assert(mls.GetLength() == 1);
    assert(mls.Get(0) == 100);

    // ImmutableListSequence
    ImmutableListSequence<int> ils;
    Sequence<int>* ils2 = ils.Append(200);
    assert(ils.GetLength() == 0);
    assert(ils2->GetLength() == 1);
    assert(ils2->Get(0) == 200);

    cout << "[OK] Все тесты пройдены!\n";
}

// ----------------------- UI -----------------------
void ShowMenu() {
    cout << "===== МЕНЮ =====\n";
    cout << "1. Тест DynamicArray\n";
    cout << "2. Тест LinkedList\n";
    cout << "3. Тест Mutable/Immutable Sequence\n";
    cout << "4. Тест ListSequence (mutable/immutable)\n";
    cout << "0. Выход\n";
    cout << "> ";
}

int main() {
    RunTests();

    while (true) {
        ShowMenu();
        int choice;
        cin >> choice;
        if (choice == 0) break;
        switch (choice) {
            case 1: {
                DynamicArray<int> da(2);
                da.Set(0, 42);
                da.Set(1, 99);
                cout << da.Get(0) << ", " << da.Get(1) << endl;
                break;
            }
            case 2: {
                int arr[] = {5, 6, 7};
                LinkedList<int> ll(arr, 3);
                for (int i = 0; i < ll.GetLength(); ++i)
                    cout << ll.Get(i) << " ";
                cout << endl;
                break;
            }
            case 3: {
                MutableArraySequence<int> mas;
                mas.Append(11);
                mas.Append(22);
                for (int i = 0; i < mas.GetLength(); ++i)
                    cout << mas.Get(i) << " ";
                cout << endl;
                ImmutableArraySequence<int> ias;
                Sequence<int>* newSeq = ias.Append(33);
                for (int i = 0; i < newSeq->GetLength(); ++i)
                    cout << newSeq->Get(i) << " ";
                cout << endl;
                break;
            }
             case 4: {
                MutableListSequence<int> mls;
                mls.Append(7)->Append(8);
                for (int i = 0; i < mls.GetLength(); ++i)
                    cout << mls.Get(i) << " ";
                cout << endl;

                ImmutableListSequence<int> ils;
                Sequence<int>* newList = ils.Append(99);
                for (int i = 0; i < newList->GetLength(); ++i)
                    cout << newList->Get(i) << " ";
                cout << endl;
                break;
             }
        }
    }
    return 0;
}