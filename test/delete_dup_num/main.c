struct ListNode* deleteDuplicates(struct ListNode* head ) {
    // write code here
    struct ListNode *cur = head;
    struct ListNode *cur_prev = NULL;
    struct ListNode *new_head = head;
    
    int should_delete = 0;
    
    if (!head || !head->next) {
        return head;
    }
    
    while(cur->next) {
        if (cur->next->val == cur->val) {
            cur->next = cur->next->next;
            should_delete = 1;
        } else {
            if (should_delete) {
                if (cur_prev) {
                    cur_prev->next = cur->next;
                    cur = cur->next;
                } else {
                    new_head = cur->next;
                    cur = new_head;
                }
                should_delete = 0;
                continue;
            } else {
                cur_prev = cur;
                cur = cur->next;
            }
        }
    }
    
    if (should_delete && !new_head->next) {
        return NULL;
    } 
    
    if (should_delete) {
        cur_prev->next = cur->next;
    }
    
    return new_head;
}
