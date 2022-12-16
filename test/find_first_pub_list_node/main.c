size_t get_list_len(struct ListNode *head) {
    struct ListNode *cur = head;
    int len = 0;
    while(cur) {
        len++;
        cur = cur->next;
    }
    return len;
}
struct ListNode* list_step(struct ListNode *head, int step) {
    struct ListNode *cur = head;
    while (step) {
        cur = cur->next;
        step--;
    }
    return cur;
}
struct ListNode* FindFirstCommonNode(struct ListNode* pHead1, struct ListNode* pHead2 ) {
    // write code here
    size_t list1_len = 0;
    size_t list2_len = 0;
    size_t step_len = 0;
    struct ListNode* list1_step_head = pHead1;
    struct ListNode* list2_step_head = pHead2;
    struct ListNode* list1_cur = NULL;
    struct ListNode* list2_cur = NULL;
    struct ListNode* ret = NULL;
    list1_len = get_list_len(pHead1);
    list2_len = get_list_len(pHead2);
    
    if (list1_len > list2_len) {
        step_len = list1_len - list2_len;
        list1_step_head = list_step(list1_step_head, step_len);
    } else {
        step_len = list2_len - list1_len;
        list2_step_head = list_step(list2_step_head, step_len);
    }
    list1_cur = list1_step_head;
    list2_cur = list2_step_head;
    while(list1_cur && list2_cur) {
        if (list1_cur == list2_cur) {
            ret = list1_cur;
            break;
        } 
        list1_cur = list1_cur->next;
        list2_cur = list2_cur->next;
    }
    return ret; 
}
