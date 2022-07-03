
#include "gas.h"

#define TREE_ERROR (((uint32_t)0x00)-1)

static int size_table[] = {
    [OT_LABEL] = 0,
    [OT_PP_MARKER] = 0,
    [OT_CLASS0_INSTR] = sizeof(uint8_t),
    [OT_CLASS1_INSTR] = sizeof(uint8_t)+sizeof(uint16_t),
    [OT_CLASS2_INSTR] = sizeof(uint8_t)+sizeof(uint8_t),
    [OT_CLASS3_INSTR] = sizeof(uint8_t)+sizeof(uint8_t),
    [OT_CLASS4_INSTR] = sizeof(uint8_t)+sizeof(uint32_t),
    [OT_CLASS5_INSTR] = sizeof(uint8_t)+sizeof(Value),
    [OT_CLASS6_INSTR] = sizeof(uint8_t)+sizeof(uint16_t),
    [OT_CLASS7A_INSTR] = sizeof(uint8_t)+sizeof(uint8_t),+sizeof(ValIdx),
    [OT_CLASS7B_INSTR] = sizeof(uint8_t)+sizeof(uint8_t),+sizeof(Value),
    [OT_CLASS7C_INSTR] = sizeof(uint8_t)+sizeof(uint8_t),+sizeof(ValIdx),
    [OT_DATA_DEFINITION] = 0
};

typedef struct _lab_tab {
    const char* key;
    uint32_t val;
    struct _lab_tab* left;
    struct _lab_tab* right;
} LabTab;

static LabTab* ltable;

static void add_lnode(LabTab* root, LabTab* node)
{
    int val = strcmp(root->key, node->key);
    if(val > 0) {
        if(root->right != NULL)
            add_lnode(root->right, node);
        else
            root->right = node;
    }
    else if(val < 0) {
        if(root->left != NULL)
            add_lnode(root->left, node);
        else
            root->left = node;
    }
    else
        syntaxError("symbol %s has already been defined", node->key);
}

static uint32_t find_lnode(LabTab* tree, const char* key)
{
    int val = strcmp(tree->key, key);
    if(val > 0) {
        if(tree->right != NULL)
            return find_lnode(tree->right, key);
        else
            return TREE_ERROR;
    }
    else if(val < 0) {
        if(tree->left != NULL)
            return find_lnode(tree->left, key);
        else
            return TREE_ERROR;
    }
    else
        return tree->val;
}

static void addLabTab(const char* key, uint32_t val)
{
    LabTab* node = _alloc_ds(LabTab);
    node->key = _copy_str(key);
    node->val = val;

    if(ltable != NULL)
        add_lnode(ltable, node);
    else
        ltable = node;
}

static uint32_t findLabTab(const char* key)
{
    if(ltable != NULL)
        return find_lnode(ltable, key);
    else
        return TREE_ERROR;
}


typedef struct _val_tab {
    const char* key;
    Value* val;
    ValIdx idx;
    struct _val_tab* left;
    struct _val_tab* right;
} ValTab;

static ValTab* vtable;

static void add_vnode(ValTab* root, ValTab* node)
{
    int val = strcmp(root->key, node->key);
    //printf(">>>> val: %d key: %s\n", val, node->key);
    if(val > 0) {
        if(root->right != NULL)
            add_vnode(root->right, node);
        else {
            //printf(">>>> (right) node->key: %s\n", node->key);
            root->right = node;
        }
    }
    else if(val < 0) {
        if(root->left != NULL)
            add_vnode(root->left, node);
        else {
            //printf(">>>> (left) node->key: %s\n", node->key);
            root->left = node;
        }
    }
    else
        syntaxError("symbol %s has already been defined", node->key);
}

static ValTab* find_vnode(ValTab* tree, const char* key)
{
    int val = strcmp(tree->key, key);
    if(val > 0) {
        if(tree->right != NULL)
            return find_vnode(tree->right, key);
        else
            return NULL;
    }
    else if(val < 0) {
        if(tree->left != NULL)
            return find_vnode(tree->left, key);
        else
            return NULL;
    }
    else
        return tree;
}

static void addValTab(const char* key, Value* val, ValIdx idx)
{
    ValTab* node = _alloc_ds(ValTab);
    node->key = _copy_str(key);
    node->val = val;
    node->idx = idx;

    if(vtable != NULL)
        add_vnode(vtable, node);
    else
        vtable = node;
}

static ValTab* findValTab(const char* key)
{
    if(vtable != NULL)
        return find_vnode(vtable, key);
    else
        return NULL;
}

static void dump_vtable(ValTab* node)
{
    if(node->left != NULL)
        dump_vtable(node->left);
    if(node->right != NULL)
        dump_vtable(node->right);

    printf("    index: %d\t", node->idx);
    printVal(node->val);
    printf(" key: %s\n", node->key);
}

static void dump_ltable(LabTab* node)
{
    if(node->left != NULL)
        dump_ltable(node->left);
    if(node->right != NULL)
        dump_ltable(node->right);

    printf("    address: %-3d\tkey: %s\n", node->val, node->key);
}

void dumpPostTables()
{
    printf("\n------- Dump Post Tables -------\n");
    printf("------- Values\n");
    dump_vtable(vtable);
    printf("------- Labels\n");
    dump_ltable(ltable);
    printf("------- End Dump -------\n");
}

/*
 * Write the instructions to the instruction stream and get the address of all
 * of the labels.
 */
static void instrs(Module* mod)
{
    Object* obj = mod->first;
    while(obj != NULL) {

        switch(obj->type) {
                case OT_PP_MARKER:
                    // do nothing
                    break;
                case OT_DATA_DEFINITION: {
                        //printf(">>>> adding %s\n", ptr->name);
                    }
                    break;
                case OT_LABEL: {
//                         Label* ptr = (Label*)obj;
//                         ptr->addr = getAddr();
//                         addLabTab(ptr->name, ptr->addr);
                    }
                    break;
                case OT_CLASS0_INSTR: {
                        Class0* ptr = (Class0*)obj;
                        writeInst8(ptr->op);
                    }
                    break;
                case OT_CLASS1_INSTR: {
                        Class1* ptr = (Class1*)obj;
                        writeInst8(ptr->op);
                        writeInst16((ptr->right & 0xF) |
                                    ((ptr->left & 0xF) << 4) |
                                    ((ptr->dest & 0xF) << 8));
                    }
                    break;
                case OT_CLASS2_INSTR: {
                        Class2* ptr = (Class2*)obj;
                        writeInst8(ptr->op);
                        writeInst8((ptr->right & 0xF) |
                                    ((ptr->left) & 0xF << 4));
                    }
                    break;
                case OT_CLASS3_INSTR: {
                        Class3* ptr = (Class3*)obj;
                        writeInst8(ptr->op);
                        writeInst8(ptr->reg & 0xF);
                    }
                    break;
                case OT_CLASS4_INSTR: {
                        Class4* ptr = (Class4*)obj;
                        writeInst8(ptr->op);
// get the address and write it to the stream
                        writeInst32(0); // fake address
                    }
                    break;
                case OT_CLASS5_INSTR: {
                        Class5* ptr = (Class5*)obj;
                        writeInst8(ptr->op);
                        writeInstObj(ptr->val, sizeof(Value));
                    }
                    break;
                case OT_CLASS6_INSTR: {
                        Class6* ptr = (Class6*)obj;
                        writeInst8(ptr->op);
                        writeInst16(ptr->tnum);
                    }
                    break;
                case OT_CLASS7A_INSTR: {
                        Class7a* ptr = (Class7a*)obj;
                        writeInst8(ptr->op);
                        writeInst8(ptr->reg & 0x0F);
                        // value has not been associated with instr yet
// get the value ID and write it
//                         Value* val = createValue(ERROR);
//                         writeInstObj(val, sizeof(Value));
                    }
                    break;
                case OT_CLASS7B_INSTR: {
                        Class7b* ptr = (Class7b*)obj;
                        writeInst8(ptr->op);
                        // immediate value
                        Value* val = createValue(ERROR);
                        writeInstObj(val, sizeof(Value));
                    }
                    break;
                case OT_CLASS7C_INSTR: {
                        Class7c* ptr = (Class7c*)obj;
                        writeInst8(ptr->op);
                        // value has not been associated with instr yet
// get the value ID and write it
//                         Value* val = createValue(ERROR);
//                         writeInstObj(val, sizeof(Value));
                        writeInst8(ptr->reg & 0x0F);
                    }
                    break;
            default:
                printf("unknown object type!: %d\n", obj->type);
                return;
        }

        obj = obj->next;
    }
}

/*
 * Scan the labels and assign the address.
 */
static void label_scan(Module* mod)
{
    Object* obj = mod->first;
    while(obj != NULL) {

        switch(obj->type) {
                case OT_PP_MARKER:
                case OT_DATA_DEFINITION:
                case OT_LABEL:
                case OT_CLASS0_INSTR:
                case OT_CLASS1_INSTR:
                case OT_CLASS2_INSTR:
                case OT_CLASS3_INSTR:
                case OT_CLASS5_INSTR:
                case OT_CLASS6_INSTR:
                case OT_CLASS7B_INSTR:
                    break;
                case OT_CLASS4_INSTR: {
                        Class4* ptr = (Class4*)obj;
                        uint32_t addr = findLabTab(ptr->sym);
                        if(addr != TREE_ERROR)
                            ptr->addr = addr;
                        else
                            syntaxError("label %s definition not found", ptr->sym);
                    }
                    break;
                case OT_CLASS7A_INSTR: {
                        Class7a* ptr = (Class7a*)obj;
                        ValTab* tab = findValTab(ptr->sym);
                        if(tab != NULL) {
                            ptr->val = tab->val;
                            ptr->idx = tab->idx;
                        }
                    }
                    break;
                case OT_CLASS7C_INSTR: {
                        Class7c* ptr = (Class7c*)obj;
                        ValTab* tab = findValTab(ptr->sym);
                        if(tab != NULL) {
                            ptr->val = tab->val;
                            ptr->idx = tab->idx;
                        }
                    }
                    break;
            default:
                printf("unknown object type!: %d\n", obj->type);
                return;
        }

        obj = obj->next;
    }
}

/*
 * Scan the references to labels and set their addresses.
 */
static void addr_scan(Module* mod)
{
    uint32_t addr = 0;
    Object* obj = mod->first;
    while(obj != NULL) {

        switch(obj->type) {
                case OT_PP_MARKER:
                case OT_DATA_DEFINITION: {
                        DataDef* ptr = (DataDef*)obj;
                        ptr->idx = addValBuf(ptr->val);
                        addValTab(ptr->name, ptr->val, ptr->idx);
                    }
                    break;
                case OT_LABEL: {
                        Label* ptr = (Label*)obj;
                        ptr->addr = addr;
                    }
                    break;
                case OT_CLASS0_INSTR: addr += size_table[OT_CLASS0_INSTR]; break;
                case OT_CLASS1_INSTR: addr += size_table[OT_CLASS1_INSTR]; break;
                case OT_CLASS2_INSTR: addr += size_table[OT_CLASS2_INSTR]; break;
                case OT_CLASS3_INSTR: addr += size_table[OT_CLASS3_INSTR]; break;
                case OT_CLASS4_INSTR: addr += size_table[OT_CLASS4_INSTR]; break;
                case OT_CLASS5_INSTR: addr += size_table[OT_CLASS5_INSTR]; break;
                case OT_CLASS6_INSTR: addr += size_table[OT_CLASS6_INSTR]; break;
                case OT_CLASS7A_INSTR: addr += size_table[OT_CLASS7A_INSTR]; break;
                case OT_CLASS7B_INSTR: addr += size_table[OT_CLASS7B_INSTR]; break;
                case OT_CLASS7C_INSTR: addr += size_table[OT_CLASS7C_INSTR]; break;
            default:
                printf("unknown object type!: %d\n", obj->type);
                return;
        }

        obj = obj->next;
    }
}

/*
 * Verify that all of the data definitions have references.
 */
static void reference_scan(Module* mod)
{
}

void doPostProcess(Module* mod)
{
    if(mod->first == NULL)
        fatalError("module is corrupt or empty");

    initInstStream();
    initValBuf();

    addr_scan(mod);
    label_scan(mod);
    //instrs(mod);
    reference_scan(mod);
}