struct ListNode* removeNthFromEnd(struct ListNode* head, int n ) {
    // write code here
    struct ListNode *del_node = NULL;
    struct ListNode *cur = head;
    struct ListNode *cur_prev = NULL;
    struct ListNode *del_node_prev = NULL;
    struct ListNode *ret = head;
    int i = 1;
    
    while(cur) {
        if (i < n ) {
            i++;
            goto loop_end;
        } else if (i == n) {
            del_node = head;
            del_node_prev = NULL;
            i++;
            goto loop_end;
        }
        
        del_node_prev = del_node;
        del_node = del_node->next;
loop_end:
        if (cur->next == NULL) {
            break;
        }
        cur_prev = cur;
        cur = cur->next;

    }
    if (del_node == head) {
        ret = head->next;
        goto end;
    } else {
        del_node_prev->next = del_node->next;
    }
end:
    return ret;
}
