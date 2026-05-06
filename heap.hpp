//
//  heap.hpp
//  chess
//
//  Created by Marcel Haupt on 14.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#pragma once
#include <vector>



template<class T>
class Heap {
private:
    std::vector<T*> heapItems;
public:
    Heap() {
        heapItems = std::vector<T*>();
    }
    
    
    void insert(T & item) {
        unsigned int heapIndex = (unsigned int)heapItems.size();
        item.setHeapIndex(heapIndex);
        heapItems.push_back(&item);
        
        sortUp(item);
    }
    
    T &removeFirst() {
        T *firstItem = heapItems[0];
        unsigned int lastIndex = (unsigned int)heapItems.size() - 1;
        heapItems[0] = heapItems[lastIndex];
        heapItems[0]->setHeapIndex(0);
        heapItems.pop_back();

        if (heapItems.size() > 0) {
            sortDown(*heapItems[0]);
        }
        
        return *firstItem;
    }
    
    void sortUp(T & item) {
        int parentIndex = (int) (item.getHeapIndex()-1)/2;
        T & parentItem = *heapItems[parentIndex];
        
        if (item.getHeapIndex() != parentIndex && parentItem.heapCompareTo(item) > 0) {
            swap(item, parentItem);
            sortUp(item);
        }
    }
    
    void sortDown(T & item) {
        unsigned int childIndex1 = (unsigned int) (item.getHeapIndex()*2+1);
        unsigned int childIndex2 = (unsigned int) (item.getHeapIndex()*2+2);
        unsigned int lastIndex = (unsigned int)heapItems.size()-1;
        
        //both indices out of range -> nothing to do
        if (childIndex1 > lastIndex && childIndex2 > lastIndex) {
            return;
        }
        
        //one index out of range
        if (childIndex2 > lastIndex && childIndex1 == lastIndex) {
            T &child1 = *heapItems[childIndex1];
            if (item.heapCompareTo(child1) > 0) {
                swap(child1, item);
                return;
            }
        }
        
        //both indices in range
        T &child1 = *heapItems[childIndex1];
        T &child2 = *heapItems[childIndex2];
        
        if (item.heapCompareTo(child1) > 0 && child1.heapCompareTo(child2) < 0) {
            swap(child1, item);
            sortDown(item);
            return;
        }
        if (item.heapCompareTo(child2) > 0) {
            swap(child2, item);
            sortDown(item);
            return;
        }
    }
    
    void swap(T & itemA, T & itemB) {
        int hpiA = itemA.getHeapIndex();
        int hpiB = itemB.getHeapIndex();
        
        heapItems[hpiA] = &itemB;
        heapItems[hpiB] = &itemA;
        
        itemB.setHeapIndex(hpiA);
        itemA.setHeapIndex(hpiB);
    }
    
    unsigned int size() {
        return (int)heapItems.size();
    }
    
    bool contains(const T & item) {
        int index = item.getHeapIndex();
        if (index >= heapItems.size()) {
            return false;
        }
        
        if (heapItems[index] == &item) {
            return true;
        }
        
        return false;
    }
};
