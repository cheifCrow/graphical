typedef struct llmember llmember;
typedef struct LList LList;

struct llmember {
    llmember* next;
    llmember* prev;
    void* content;
};

struct LList {
    llmember* head;
    llmember* tail;
};

void freell(LList* list);
int llempty(LList* list);
void add_llmember_head(LList* list, llmember* index);
void add_llmember(LList* list, llmember* index);
void delete_llmember(LList* list, llmember* index);
