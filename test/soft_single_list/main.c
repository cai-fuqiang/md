struct ListNode* sortInList(struct ListNode* head ) {
    // write code here
    struct ListNode* new_head = NULL;
    struct ListNode* new_cur = NULL;
    struct ListNode* new_prev = NULL;
    struct ListNode* cur = head;
    struct ListNode* cur_next = NULL;
    while (cur) {
        if (!new_head) {
            new_head = head;
            cur = cur->next;
            new_head->next = NULL;
            continue;
        }
        cur_next = cur->next;
        new_cur = new_head;
        while (new_cur) {
            if (new_cur->val < cur->val) {
                new_prev = new_cur;
                if (new_cur->next) {
                    new_cur = new_cur->next;
                    continue;
                } else {
                    new_cur->next = cur;
                    cur->next = NULL;
                    break;
                }
            } else {
                if (new_head == new_cur) {
                    cur->next = new_cur;
                    new_head = cur;
                    break;
                }
                if (new_prev) {
                    new_prev->next = cur;
                    cur->next = new_cur;
                } else {
                    cur->next = new_cur;
                    new_head = cur;
                }
                break;
            }
        }
loop_end:
        cur = cur_next;
    }

    return new_head;
}
