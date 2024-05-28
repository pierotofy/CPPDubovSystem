//
//  LinkedList.cpp
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

#include "LinkedList.hpp"

LinkedListNode::LinkedListNode() {
    this->next = NULL;
}

LinkedListNode::LinkedListNode(std::vector<Player> data) {
    this->data = data;
}

LinkedList::LinkedList() {
    this->head = NULL;
}

void LinkedList::insertNode(LinkedListNode n) {
    LinkedListNode* d = new LinkedListNode(n.data);
    if(this->head == NULL) {
        this->head = d;
        return;
    }
    LinkedListNode* c = this->head;
    while(c->next != NULL) {
        c = c->next;
    }
    c->next = d;
}
