namespace brep {

namespace {

// auto template (since C++ 20)
int count_nodes(auto head) {
    if (head == nullptr) {
        return 0;
    }

    int cnt = 0;
    auto cur = head;
    do {
        cnt++;
        cur = cur->next;
    } while (cur != head);

    return cnt;
}

}
}