//#include "Engine/Assert.h"
#include "Engine.h"

#include "HLSLTree.h"

namespace M4
{

HLSLTree::HLSLTree(Allocator* allocator) :
    m_allocator(allocator), m_stringPool(allocator)
{
    m_firstPage         = m_allocator->New<NodePage>();
    m_firstPage->next   = NULL;

    m_currentPage       = m_firstPage;
    m_currentPageOffset = 0;

    m_root              = AddNode<HLSLRoot>(NULL, 1);
}

HLSLTree::~HLSLTree()
{
    NodePage* page = m_firstPage;
    while (page != NULL)
    {
        NodePage* next = page->next;
        m_allocator->Delete(page);
        page = next;
    }
}

void HLSLTree::AllocatePage()
{
    NodePage* newPage    = m_allocator->New<NodePage>();
    newPage->next        = NULL;
    m_currentPage->next  = newPage;
    m_currentPageOffset  = 0;
    m_currentPage        = newPage;
}

const char* HLSLTree::AddString(const char* string)
{   
    return m_stringPool.AddString(string);
}

bool HLSLTree::GetContainsString(const char* string) const
{
    return m_stringPool.GetContainsString(string);
}

HLSLRoot* HLSLTree::GetRoot() const
{
    return m_root;
}

void* HLSLTree::AllocateMemory(size_t size)
{
    if (m_currentPageOffset + size > s_nodePageSize)
    {
        AllocatePage();
    }
    void* buffer = m_currentPage->buffer + m_currentPageOffset;
    m_currentPageOffset += size;
    return buffer;
}

// @@ This doesn't do any parameter matching. Simply returns the first function with that name.
HLSLFunction * HLSLTree::FindFunction(const char * name)
{
    HLSLStatement * statement = m_root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Function)
        {
            HLSLFunction * function = (HLSLFunction *)statement;
            if (String_Equal(name, function->name))
            {
                return function;
            }
        }

        statement = statement->nextStatement;
    }

    return NULL;
}

HLSLDeclaration * HLSLTree::FindGlobalDeclaration(const char * name)
{
    HLSLStatement * statement = m_root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Declaration)
        {
            HLSLDeclaration * declaration = (HLSLDeclaration *)statement;
            if (String_Equal(name, declaration->name))
            {
                return declaration;
            }
        }
        else if (statement->nodeType == HLSLNodeType_Buffer)
        {
            HLSLBuffer* buffer = (HLSLBuffer*)statement;

            HLSLDeclaration* field = buffer->field;
            while (field != NULL)
            {
                ASSERT(field->nodeType == HLSLNodeType_Declaration);
                if (String_Equal(name, field->name))
                {
                    return field;
                }
                field = (HLSLDeclaration*)field->nextStatement;
            }
        }

        statement = statement->nextStatement;
    }

    return NULL;
}

HLSLStruct * HLSLTree::FindGlobalStruct(const char * name)
{
    HLSLStatement * statement = m_root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Struct)
        {
            HLSLStruct * declaration = (HLSLStruct *)statement;
            if (String_Equal(name, declaration->name))
            {
                return declaration;
            }
        }

        statement = statement->nextStatement;
    }

    return NULL;
}

HLSLTechnique * HLSLTree::FindTechnique(const char * name)
{
    HLSLStatement * statement = m_root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Technique)
        {
            HLSLTechnique * technique = (HLSLTechnique *)statement;
            if (String_Equal(name, technique->name))
            {
                return technique;
            }
        }

        statement = statement->nextStatement;
    }

    return NULL;
}

HLSLPipeline * HLSLTree::FindFirstPipeline()
{
    return FindNextPipeline(NULL);
}

HLSLPipeline * HLSLTree::FindNextPipeline(HLSLPipeline * current)
{
    HLSLStatement * statement = current ? current : m_root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Pipeline)
        {
            return (HLSLPipeline *)statement;
        }

        statement = statement->nextStatement;
    }

    return NULL;
}

HLSLPipeline * HLSLTree::FindPipeline(const char * name)
{
    HLSLStatement * statement = m_root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Pipeline)
        {
            HLSLPipeline * pipeline = (HLSLPipeline *)statement;
            if (String_Equal(name, pipeline->name))
            {
                return pipeline;
            }
        }

        statement = statement->nextStatement;
    }

    return NULL;
}

HLSLBuffer * HLSLTree::FindBuffer(const char * name)
{
    HLSLStatement * statement = m_root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Buffer)
        {
            HLSLBuffer * buffer = (HLSLBuffer *)statement;
            if (String_Equal(name, buffer->name))
            {
                return buffer;
            }
        }

        statement = statement->nextStatement;
    }

    return NULL;
}



bool HLSLTree::GetExpressionValue(HLSLExpression * expression, int & value)
{
    ASSERT (expression != NULL);

    // Expression must be constant.
    if ((expression->expressionType.flags & HLSLTypeFlag_Const) == 0) 
    {
        return false;
    }

    // We are expecting an integer scalar. @@ Add support for type conversion from other scalar types.
    if (expression->expressionType.baseType != HLSLBaseType_Int) 
    {
        return false;
    }

    if (expression->expressionType.array) 
    {
        return false;
    }

    if (expression->nodeType == HLSLNodeType_BinaryExpression) 
    {
        HLSLBinaryExpression * binaryExpression = (HLSLBinaryExpression *)expression;

        int value1, value2;
        if (!GetExpressionValue(binaryExpression->expression1, value1) ||
            !GetExpressionValue(binaryExpression->expression2, value2))
        {
            return false;
        }

        switch(binaryExpression->binaryOp)
        {
            case HLSLBinaryOp_And:
                value = value1 && value2;
                return true;
            case HLSLBinaryOp_Or:
                value = value1 || value2;
                return true;
            case HLSLBinaryOp_Add:
                value = value1 + value2;
                return true;
            case HLSLBinaryOp_Sub:
                value = value1 - value2;
                return true;
            case HLSLBinaryOp_Mul:
                value = value1 * value2;
                return true;
            case HLSLBinaryOp_Div:
                value = value1 / value2;
                return true;
            case HLSLBinaryOp_Less:
                value = value1 < value2;
                return true;
            case HLSLBinaryOp_Greater:
                value = value1 > value2;
                return true;
            case HLSLBinaryOp_LessEqual:
                value = value1 <= value2;
                return true;
            case HLSLBinaryOp_GreaterEqual:
                value = value1 >= value2;
                return true;
            case HLSLBinaryOp_Equal:
                value = value1 == value2;
                return true;
            case HLSLBinaryOp_NotEqual:
                value = value1 != value2;
                return true;
            case HLSLBinaryOp_BitAnd:
                value = value1 & value2;
                return true;
            case HLSLBinaryOp_BitOr:
                value = value1 | value2;
                return true;
            case HLSLBinaryOp_BitXor:
                value = value1 ^ value2;
                return true;
        }
    }
    else if (expression->nodeType == HLSLNodeType_UnaryExpression) 
    {
        HLSLUnaryExpression * unaryExpression = (HLSLUnaryExpression *)expression;

        if (!GetExpressionValue(unaryExpression->expression, value))
        {
            return false;
        }

        switch(unaryExpression->unaryOp)
        {
            case HLSLUnaryOp_Negative:
                value = -value;
                return true;
            case HLSLUnaryOp_Positive:
                // nop.
                return true;
            case HLSLUnaryOp_Not:
                value = !value;
                return true;
            case HLSLUnaryOp_BitNot:
                value = ~value;
                return true;
        }
    }
    else if (expression->nodeType == HLSLNodeType_IdentifierExpression)
    {
        HLSLIdentifierExpression * identifier = (HLSLIdentifierExpression *)expression;

        HLSLDeclaration * declaration = FindGlobalDeclaration(identifier->name);
        if (declaration == NULL) 
        {
            return false;
        }
        if ((declaration->type.flags & HLSLTypeFlag_Const) == 0)
        {
            return false;
        }

        return GetExpressionValue(declaration->assignment, value);
    }
    else if (expression->nodeType == HLSLNodeType_LiteralExpression) 
    {
        HLSLLiteralExpression * literal = (HLSLLiteralExpression *)expression;
        value = literal->iValue;
        return true;
    }

    return false;
}

/*
bool HLSLTree::GetExpressionValue(HLSLExpression * expression, float & value)
{
    ASSERT (expression != NULL);

    // Expression must be constant.
    if ((expression->expressionType.flags & HLSLTypeFlag_Const) == 0) 
    {
        return false;
    }

    // We are expecting an integer scalar. @@ Add support for type conversion from other scalar types.
    if (expression->expressionType.baseType != HLSLBaseType_Float) 
    {
        return false;
    }

    if (expression->expressionType.array) 
    {
        return false;
    }

    if (expression->nodeType == HLSLNodeType_BinaryExpression) 
    {
        HLSLBinaryExpression * binaryExpression = (HLSLBinaryExpression *)expression;

        int value1, value2;
        if (!GetExpressionValue(binaryExpression->expression1, value1) ||
            !GetExpressionValue(binaryExpression->expression2, value2))
        {
            return false;
        }

        switch(binaryExpression->binaryOp)
        {
            case HLSLBinaryOp_Add:
                value = value1 + value2;
                return true;
            case HLSLBinaryOp_Sub:
                value = value1 - value2;
                return true;
            case HLSLBinaryOp_Mul:
                value = value1 * value2;
                return true;
            case HLSLBinaryOp_Div:
                value = value1 / value2;
                return true;
        }
    }
    else if (expression->nodeType == HLSLNodeType_UnaryExpression) 
    {
        HLSLUnaryExpression * unaryExpression = (HLSLUnaryExpression *)expression;

        if (!GetExpressionValue(unaryExpression->expression, value))
        {
            return false;
        }

        switch(unaryExpression->unaryOp)
        {
            case HLSLUnaryOp_Negative:
                value = -value;
                return true;
            case HLSLUnaryOp_Positive:
                // nop.
                return true;
        }
    }
    else if (expression->nodeType == HLSLNodeType_IdentifierExpression)
    {
        HLSLIdentifierExpression * identifier = (HLSLIdentifierExpression *)expression;

        HLSLDeclaration * declaration = FindGlobalDeclaration(identifier->name);
        if (declaration == NULL) 
        {
            return false;
        }
        if ((declaration->type.flags & HLSLTypeFlag_Const) == 0)
        {
            return false;
        }

        return GetExpressionValue(declaration->assignment, value);
    }
    else if (expression->nodeType == HLSLNodeType_LiteralExpression)
    {
        HLSLLiteralExpression * literal = (HLSLLiteralExpression *)expression;
        value = literal->iValue;
        return true;
    }

    return false;
}
*/




void HLSLTreeVisitor::VisitType(HLSLType & type)
{
}

void HLSLTreeVisitor::VisitRoot(HLSLRoot * root)
{
    HLSLStatement * statement = root->statement;
    while (statement != NULL) {
        VisitTopLevelStatement(statement);
        statement = statement->nextStatement;
    }
}

void HLSLTreeVisitor::VisitTopLevelStatement(HLSLStatement * node)
{
    if (node->nodeType == HLSLNodeType_Declaration) {
        VisitDeclaration((HLSLDeclaration *)node);
    }
    else if (node->nodeType == HLSLNodeType_Struct) {
        VisitStruct((HLSLStruct *)node);
    }
    else if (node->nodeType == HLSLNodeType_Buffer) {
        VisitBuffer((HLSLBuffer *)node);
    }
    else if (node->nodeType == HLSLNodeType_Function) {
        VisitFunction((HLSLFunction *)node);
    }
    else if (node->nodeType == HLSLNodeType_Technique) {
        VisitTechnique((HLSLTechnique *)node);
    }
    else {
        ASSERT(0);
    }
}

void HLSLTreeVisitor::VisitStatements(HLSLStatement * statement)
{
    while (statement != NULL) {
        VisitStatement(statement);
        statement = statement->nextStatement;
    }
}

void HLSLTreeVisitor::VisitStatement(HLSLStatement * node)
{
    // Function statements
    if (node->nodeType == HLSLNodeType_Declaration) {
        VisitDeclaration((HLSLDeclaration *)node);
    }
    else if (node->nodeType == HLSLNodeType_ExpressionStatement) {
        VisitExpressionStatement((HLSLExpressionStatement *)node);
    }
    else if (node->nodeType == HLSLNodeType_ReturnStatement) {
        VisitReturnStatement((HLSLReturnStatement *)node);
    }
    else if (node->nodeType == HLSLNodeType_DiscardStatement) {
        VisitDiscardStatement((HLSLDiscardStatement *)node);
    }
    else if (node->nodeType == HLSLNodeType_BreakStatement) {
        VisitBreakStatement((HLSLBreakStatement *)node);
    }
    else if (node->nodeType == HLSLNodeType_ContinueStatement) {
        VisitContinueStatement((HLSLContinueStatement *)node);
    }
    else if (node->nodeType == HLSLNodeType_IfStatement) {
        VisitIfStatement((HLSLIfStatement *)node);
    }
    else if (node->nodeType == HLSLNodeType_ForStatement) {
        VisitForStatement((HLSLForStatement *)node);
    }
    else if (node->nodeType == HLSLNodeType_BlockStatement) {
        VisitBlockStatement((HLSLBlockStatement *)node);
    }
    else {
        ASSERT(0);
    }
}

void HLSLTreeVisitor::VisitDeclaration(HLSLDeclaration * node)
{
    VisitType(node->type);
    /*do {
        VisitExpression(node->assignment);
        node = node->nextDeclaration;
    } while (node);*/
    if (node->assignment != NULL) {
        VisitExpression(node->assignment);
    }
    if (node->nextDeclaration != NULL) {
        VisitDeclaration(node->nextDeclaration);
    }
}

void HLSLTreeVisitor::VisitStruct(HLSLStruct * node)
{
    HLSLStructField * field = node->field;
    while (field != NULL) {
        VisitStructField(field);
        field = field->nextField;
    }
}

void HLSLTreeVisitor::VisitStructField(HLSLStructField * node)
{
    VisitType(node->type);
}

void HLSLTreeVisitor::VisitBuffer(HLSLBuffer * node)
{
    HLSLDeclaration * field = node->field;
    while (field != NULL) {
        ASSERT(field->nodeType == HLSLNodeType_Declaration);
        VisitDeclaration(field);
        ASSERT(field->nextDeclaration == NULL);
        field = (HLSLDeclaration *)field->nextStatement;
    }
}

/*void HLSLTreeVisitor::VisitBufferField(HLSLBufferField * node)
{
    VisitType(node->type);
}*/

void HLSLTreeVisitor::VisitFunction(HLSLFunction * node)
{
    VisitType(node->returnType);

    HLSLArgument * argument = node->argument;
    while (argument != NULL) {
        VisitArgument(argument);
        argument = argument->nextArgument;
    }

    VisitStatements(node->statement);
}

void HLSLTreeVisitor::VisitArgument(HLSLArgument * node)
{
    VisitType(node->type);
    if (node->defaultValue != NULL) {
        VisitExpression(node->defaultValue);
    }
}

void HLSLTreeVisitor::VisitExpressionStatement(HLSLExpressionStatement * node)
{
    VisitExpression(node->expression);
}

void HLSLTreeVisitor::VisitExpression(HLSLExpression * node)
{
    VisitType(node->expressionType);

    if (node->nodeType == HLSLNodeType_UnaryExpression) {
        VisitUnaryExpression((HLSLUnaryExpression *)node);
    }
    else if (node->nodeType == HLSLNodeType_BinaryExpression) {
        VisitBinaryExpression((HLSLBinaryExpression *)node);
    }
    else if (node->nodeType == HLSLNodeType_ConditionalExpression) {
        VisitConditionalExpression((HLSLConditionalExpression *)node);
    }
    else if (node->nodeType == HLSLNodeType_CastingExpression) {
        VisitCastingExpression((HLSLCastingExpression *)node);
    }
    else if (node->nodeType == HLSLNodeType_LiteralExpression) {
        VisitLiteralExpression((HLSLLiteralExpression *)node);
    }
    else if (node->nodeType == HLSLNodeType_IdentifierExpression) {
        VisitIdentifierExpression((HLSLIdentifierExpression *)node);
    }
    else if (node->nodeType == HLSLNodeType_ConstructorExpression) {
        VisitConstructorExpression((HLSLConstructorExpression *)node);
    }
    else if (node->nodeType == HLSLNodeType_MemberAccess) {
        VisitMemberAccess((HLSLMemberAccess *)node);
    }
    else if (node->nodeType == HLSLNodeType_ArrayAccess) {
        VisitArrayAccess((HLSLArrayAccess *)node);
    }
    else if (node->nodeType == HLSLNodeType_FunctionCall) {
        VisitFunctionCall((HLSLFunctionCall *)node);
    }
    else {
        ASSERT(0);
    }
}

void HLSLTreeVisitor::VisitReturnStatement(HLSLReturnStatement * node)
{
    VisitExpression(node->expression);
}

void HLSLTreeVisitor::VisitDiscardStatement(HLSLDiscardStatement * node) {}
void HLSLTreeVisitor::VisitBreakStatement(HLSLBreakStatement * node) {}
void HLSLTreeVisitor::VisitContinueStatement(HLSLContinueStatement * node) {}

void HLSLTreeVisitor::VisitIfStatement(HLSLIfStatement * node)
{
    VisitExpression(node->condition);
    VisitStatements(node->statement);
    if (node->elseStatement) {
        VisitStatements(node->elseStatement);
    }
}

void HLSLTreeVisitor::VisitForStatement(HLSLForStatement * node)
{
    if (node->initialization) {
        VisitDeclaration(node->initialization);
    }
    if (node->condition) {
        VisitExpression(node->condition);
    }
    if (node->increment) {
        VisitExpression(node->increment);
    }
    VisitStatements(node->statement);
}

void HLSLTreeVisitor::VisitBlockStatement(HLSLBlockStatement * node)
{
    VisitStatements(node->statement);
}

void HLSLTreeVisitor::VisitUnaryExpression(HLSLUnaryExpression * node)
{
    VisitExpression(node->expression);
}

void HLSLTreeVisitor::VisitBinaryExpression(HLSLBinaryExpression * node)
{
    VisitExpression(node->expression1);
    VisitExpression(node->expression2);
}

void HLSLTreeVisitor::VisitConditionalExpression(HLSLConditionalExpression * node)
{
    VisitExpression(node->condition);
    VisitExpression(node->falseExpression);
    VisitExpression(node->trueExpression);
}

void HLSLTreeVisitor::VisitCastingExpression(HLSLCastingExpression * node)
{
    VisitType(node->type);
    VisitExpression(node->expression);
}

void HLSLTreeVisitor::VisitLiteralExpression(HLSLLiteralExpression * node) {}
void HLSLTreeVisitor::VisitIdentifierExpression(HLSLIdentifierExpression * node) {}

void HLSLTreeVisitor::VisitConstructorExpression(HLSLConstructorExpression * node)
{
    HLSLExpression * argument = node->argument;
    while (argument != NULL) {
        VisitExpression(argument);
        argument = argument->nextExpression;
    }
}

void HLSLTreeVisitor::VisitMemberAccess(HLSLMemberAccess * node)
{
    VisitExpression(node->object);
}

void HLSLTreeVisitor::VisitArrayAccess(HLSLArrayAccess * node)
{
    VisitExpression(node->array);
    VisitExpression(node->index);
}

void HLSLTreeVisitor::VisitFunctionCall(HLSLFunctionCall * node)
{
    HLSLExpression * argument = node->argument;
    while (argument != NULL) {
        VisitExpression(argument);
        argument = argument->nextExpression;
    }
}

void HLSLTreeVisitor::VisitStateAssignment(HLSLStateAssignment * node) {}

void HLSLTreeVisitor::VisitSamplerState(HLSLSamplerState * node)
{
    HLSLStateAssignment * stateAssignment = node->stateAssignments;
    while (stateAssignment != NULL) {
        VisitStateAssignment(stateAssignment);
        stateAssignment = stateAssignment->nextStateAssignment;
    }
}

void HLSLTreeVisitor::VisitPass(HLSLPass * node)
{
    HLSLStateAssignment * stateAssignment = node->stateAssignments;
    while (stateAssignment != NULL) {
        VisitStateAssignment(stateAssignment);
        stateAssignment = stateAssignment->nextStateAssignment;
    }
}

void HLSLTreeVisitor::VisitTechnique(HLSLTechnique * node)
{
    HLSLPass * pass = node->passes;
    while (pass != NULL) {
        VisitPass(pass);
        pass = pass->nextPass;
    }
}

void HLSLTreeVisitor::VisitFunctions(HLSLRoot * root)
{
    HLSLStatement * statement = root->statement;
    while (statement != NULL) {
        if (statement->nodeType == HLSLNodeType_Function) {
            VisitFunction((HLSLFunction *)statement);
        }

        statement = statement->nextStatement;
    }
}

void HLSLTreeVisitor::VisitParameters(HLSLRoot * root)
{
    HLSLStatement * statement = root->statement;
    while (statement != NULL) {
        if (statement->nodeType == HLSLNodeType_Declaration) {
            VisitDeclaration((HLSLDeclaration *)statement);
        }

        statement = statement->nextStatement;
    }
}


class ResetHiddenFlagVisitor : public HLSLTreeVisitor
{
public:
    virtual void VisitTopLevelStatement(HLSLStatement * statement)
    {
        statement->hidden = true;

        if (statement->nodeType == HLSLNodeType_Buffer)
        {
            VisitBuffer((HLSLBuffer*)statement);
        }
    }

    // Hide buffer fields.
    virtual void VisitDeclaration(HLSLDeclaration * node)
    {
        node->hidden = true;
    }
};

class MarkVisibleStatementsVisitor : public HLSLTreeVisitor
{
public:
    HLSLTree * tree;
    MarkVisibleStatementsVisitor(HLSLTree * tree) : tree(tree) {}

    virtual void VisitFunction(HLSLFunction * node)
    {
        node->hidden = false;
        HLSLTreeVisitor::VisitFunction(node);
    }

    virtual void VisitFunctionCall(HLSLFunctionCall * node)
    {
        HLSLTreeVisitor::VisitFunctionCall(node);

        if (node->function->hidden)
        {
            VisitFunction(const_cast<HLSLFunction*>(node->function));
        }
    }

    virtual void VisitIdentifierExpression(HLSLIdentifierExpression * node)
    {
        HLSLTreeVisitor::VisitIdentifierExpression(node);

        if (node->global)
        {
            HLSLDeclaration * declaration = tree->FindGlobalDeclaration(node->name);
            if (declaration != NULL && declaration->hidden)
            {
                declaration->hidden = false;
                VisitDeclaration(declaration);
            }
        }
    }

    virtual void VisitType(HLSLType & type)
    {
        if (type.baseType == HLSLBaseType_UserDefined)
        {
            HLSLStruct * globalStruct = tree->FindGlobalStruct(type.typeName);
            if (globalStruct != NULL)
            {
                globalStruct->hidden = false;
                VisitStruct(globalStruct);
            }
        }
    }

};


void PruneTree(HLSLTree* tree, const char* entryName0, const char* entryName1/*=NULL*/)
{
    HLSLRoot* root = tree->GetRoot();

    // Reset all flags.
    ResetHiddenFlagVisitor reset;
    reset.VisitRoot(root);

    // Mark all the statements necessary for these entrypoints.
    HLSLFunction* entry = tree->FindFunction(entryName0);
    if (entry != NULL)
    {
        MarkVisibleStatementsVisitor mark(tree);
        mark.VisitFunction(entry);
    }

    if (entryName1 != NULL)
    {
        entry = tree->FindFunction(entryName1);
        if (entry != NULL)
        {
            MarkVisibleStatementsVisitor mark(tree);
            mark.VisitFunction(entry);
        }
    }

    // Mark buffers visible, if any of their fields is visible.
    HLSLStatement * statement = root->statement;
    while (statement != NULL)
    {
        if (statement->nodeType == HLSLNodeType_Buffer)
        {
            HLSLBuffer* buffer = (HLSLBuffer*)statement;

            HLSLDeclaration* field = buffer->field;
            while (field != NULL)
            {
                ASSERT(field->nodeType == HLSLNodeType_Declaration);
                if (!field->hidden)
                {
                    buffer->hidden = false;
                    break;
                }
                field = (HLSLDeclaration*)field->nextStatement;
            }
        }

        statement = statement->nextStatement;
    }
}


void SortTree(HLSLTree * tree)
{
    // Stable sort so that statements are in this order:
    // structs, declarations, functions, techniques.
	// but their relative order is preserved.

    HLSLRoot* root = tree->GetRoot();

    HLSLStatement* structs = NULL;
    HLSLStatement* lastStruct = NULL;
    HLSLStatement* constDeclarations = NULL;
    HLSLStatement* lastConstDeclaration = NULL;
    HLSLStatement* declarations = NULL;
    HLSLStatement* lastDeclaration = NULL;
    HLSLStatement* functions = NULL;
    HLSLStatement* lastFunction = NULL;
    HLSLStatement* other = NULL;
    HLSLStatement* lastOther = NULL;

    HLSLStatement* statement = root->statement;
    while (statement != NULL) {
        HLSLStatement* nextStatement = statement->nextStatement;
        statement->nextStatement = NULL;

        if (statement->nodeType == HLSLNodeType_Struct) {
            if (structs == NULL) structs = statement;
            if (lastStruct != NULL) lastStruct->nextStatement = statement;
            lastStruct = statement;
        }
        else if (statement->nodeType == HLSLNodeType_Declaration || statement->nodeType == HLSLNodeType_Buffer) {
			if (statement->nodeType == HLSLNodeType_Declaration && (((HLSLDeclaration *)statement)->type.flags & HLSLTypeFlag_Const)) {
				if (constDeclarations == NULL) constDeclarations = statement;
				if (lastConstDeclaration != NULL) lastConstDeclaration->nextStatement = statement;
				lastConstDeclaration = statement;
			}
			else {
				if (declarations == NULL) declarations = statement;
				if (lastDeclaration != NULL) lastDeclaration->nextStatement = statement;
				lastDeclaration = statement;
			}
        }
        else if (statement->nodeType == HLSLNodeType_Function) {
            if (functions == NULL) functions = statement;
            if (lastFunction != NULL) lastFunction->nextStatement = statement;
            lastFunction = statement;
        }
        else {
            if (other == NULL) other = statement;
            if (lastOther != NULL) lastOther->nextStatement = statement;
            lastOther = statement;
        }

        statement = nextStatement;
    }

    // Chain all the statements in the order that we want.
    HLSLStatement * firstStatement = structs;
    HLSLStatement * lastStatement = lastStruct;

    if (firstStatement == NULL) firstStatement = constDeclarations;
    else lastStatement->nextStatement = constDeclarations;
    lastStatement = lastConstDeclaration;

    if (firstStatement == NULL) firstStatement = declarations;
    else lastStatement->nextStatement = declarations;
    lastStatement = lastDeclaration;

    if (firstStatement == NULL) firstStatement = functions;
    else lastStatement->nextStatement = functions;
    lastStatement = lastFunction;

    if (firstStatement == NULL) firstStatement = other;
    else lastStatement->nextStatement = other;
    lastStatement = lastOther;

    root->statement = firstStatement;
}





// First and last can be the same.
void AddStatements(HLSLRoot * root, HLSLStatement * before, HLSLStatement * first, HLSLStatement * last)
{
    if (before == NULL) {
        last->nextStatement = root->statement;
        root->statement = first;
    }
    else {
        last->nextStatement = before->nextStatement;
        before->nextStatement = first;
    }
}

void AddSingleStatement(HLSLRoot * root, HLSLStatement * before, HLSLStatement * statement)
{
    AddStatements(root, before, statement, statement);
}



// @@ This is very game-specific. Should be moved to pipeline_parser or somewhere else.
void GroupParameters(HLSLTree * tree)
{
    // Sort parameters based on semantic and group them in cbuffers.

    HLSLRoot* root = tree->GetRoot();

    HLSLDeclaration * firstPerItemDeclaration = NULL;
    HLSLDeclaration * lastPerItemDeclaration = NULL;

    HLSLDeclaration * firstPerPassDeclaration = NULL;
    HLSLDeclaration * lastPerPassDeclaration = NULL;

    HLSLDeclaration * firstPerItemSampler = NULL;
    HLSLDeclaration * lastPerItemSampler = NULL;

    HLSLDeclaration * firstPerPassSampler = NULL;
    HLSLDeclaration * lastPerPassSampler = NULL;

    HLSLStatement * statementBeforeBuffers = NULL;
    
    HLSLStatement* previousStatement = NULL;
    HLSLStatement* statement = root->statement;
    while (statement != NULL) {
        HLSLStatement* nextStatement = statement->nextStatement;

        if (statement->nodeType == HLSLNodeType_Struct) // Do not remove this, or it will mess the else clause below.
        {   
            statementBeforeBuffers = statement;
        }
        else if (statement->nodeType == HLSLNodeType_Declaration)
        {
            HLSLDeclaration* declaration = (HLSLDeclaration*)statement;

			// We insert buffers after the last const declaration.
			if ((declaration->type.flags & HLSLTypeFlag_Const) != 0)
			{
				statementBeforeBuffers = statement;
			}

            // Do not move samplers or static/const parameters.
            if ((declaration->type.flags & (HLSLTypeFlag_Static|HLSLTypeFlag_Const)) == 0)
            {
                // Unlink statement.
                statement->nextStatement = NULL;
                if (previousStatement != NULL) previousStatement->nextStatement = nextStatement;
                else root->statement = nextStatement;

                while(declaration != NULL)
                {
                    HLSLDeclaration* nextDeclaration = declaration->nextDeclaration;

                    // Select group based on type and semantic.
                    HLSLDeclaration ** first, ** last;
                    if (declaration->semantic == NULL || String_EqualNoCase(declaration->semantic, "PER_ITEM") || String_EqualNoCase(declaration->semantic, "PER_MATERIAL"))
                    {
                        if (IsSamplerType(declaration->type))
                        {
                            first = &firstPerItemSampler;
                            last = &lastPerItemSampler;
                        }
                        else 
                        {
                            first = &firstPerItemDeclaration;
                            last = &lastPerItemDeclaration;
                        }
                    }
                    else 
                    {
                        if (IsSamplerType(declaration->type))
                        {
                            first = &firstPerPassSampler;
                            last = &lastPerPassSampler;
                        }
                        else 
                        {
                            first = &firstPerPassDeclaration;
                            last = &lastPerPassDeclaration;
                        }
                    }

                    // Add declaration to new list.
                    if (*first == NULL) *first = declaration;
                    else (*last)->nextStatement = declaration;
                    *last = declaration;

                    // Unlink from declaration list.
                    declaration->nextDeclaration = NULL;

                    // Reset attributes.
                    declaration->registerName = NULL;
                    declaration->semantic = NULL;

                    declaration = nextDeclaration;
                }
            }
        }
        /*else
        {
            if (statementBeforeBuffers == NULL) {
                // This is the location where we will insert our buffers.
                statementBeforeBuffers = previousStatement;
            }
        }*/

        if (statement->nextStatement == nextStatement) {
            previousStatement = statement;
        }
        statement = nextStatement;
    }


    // Add samplers.
    if (firstPerItemSampler != NULL) {
        AddStatements(root, statementBeforeBuffers, firstPerItemSampler, lastPerItemSampler);
        statementBeforeBuffers = lastPerItemSampler;
    }
    if (firstPerPassSampler != NULL) {
        AddStatements(root, statementBeforeBuffers, firstPerPassSampler, lastPerPassSampler);
        statementBeforeBuffers = lastPerPassSampler;
    }


    // @@ We are assuming per_item and per_pass buffers don't already exist. @@ We should assert on that.

    if (firstPerItemDeclaration != NULL)
    {
        // Create buffer statement.
        HLSLBuffer * perItemBuffer = tree->AddNode<HLSLBuffer>(firstPerItemDeclaration->fileName, firstPerItemDeclaration->line-1);
        perItemBuffer->name = tree->AddString("per_item");
        perItemBuffer->registerName = tree->AddString("b0");
        perItemBuffer->field = firstPerItemDeclaration;

        // Add buffer to statements.
        AddSingleStatement(root, statementBeforeBuffers, perItemBuffer);
        statementBeforeBuffers = perItemBuffer;
    }

    if (firstPerPassDeclaration != NULL)
    {
        // Create buffer statement.
        HLSLBuffer * perPassBuffer = tree->AddNode<HLSLBuffer>(firstPerPassDeclaration->fileName, firstPerPassDeclaration->line-1);
        perPassBuffer->name = tree->AddString("per_pass");
        perPassBuffer->registerName = tree->AddString("b1");
        perPassBuffer->field = firstPerPassDeclaration;

        // Add buffer to statements.
        AddSingleStatement(root, statementBeforeBuffers, perPassBuffer);
    }
}





} // M4

