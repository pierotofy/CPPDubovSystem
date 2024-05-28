//
//  LinkedList.hpp
//  DubovSystem
//
//  Created by Michael Shapiro on 5/11/24.
//

// Copyright 2024 Michael Shapiro
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LinkedList_hpp
#define LinkedList_hpp

#include <stdio.h>
#include <vector>
#include "Player.hpp"

#endif /* LinkedList_hpp */

class LinkedListNode {
    
public:
    std::vector<Player> data;
    LinkedListNode* next;
    
    LinkedListNode();
    LinkedListNode(std::vector<Player> data);
    
};

class LinkedList {
private:
    LinkedListNode* head;
public:
    LinkedList();
    void insertNode(LinkedListNode);
    LinkedListNode* getHead() {return head;}
};
