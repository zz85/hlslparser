#ifndef HLSL_TREE_H
#define HLSL_TREE_H

//#include "Engine/StringPool.h"
#include "Engine.h"

#include <new>

namespace M4
{

enum HLSLNodeType
{
    HLSLNodeType_Root,
    HLSLNodeType_Declaration,
    HLSLNodeType_Struct,
    HLSLNodeType_StructField,
    HLSLNodeType_Buffer,
    HLSLNodeType_BufferField,
    HLSLNodeType_Function,
    HLSLNodeType_Argument,
    HLSLNodeType_ExpressionStatement,
    HLSLNodeType_Expression,
    HLSLNodeType_ReturnStatement,
    HLSLNodeType_DiscardStatement,
    HLSLNodeType_BreakStatement,
    HLSLNodeType_ContinueStatement,
    HLSLNodeType_IfStatement,
    HLSLNodeType_ForStatement,
    HLSLNodeType_BlockStatement,
    HLSLNodeType_UnaryExpression,
    HLSLNodeType_BinaryExpression,
    HLSLNodeType_ConditionalExpression,
    HLSLNodeType_CastingExpression,
    HLSLNodeType_LiteralExpression,
    HLSLNodeType_IdentifierExpression,
    HLSLNodeType_ConstructorExpression,
    HLSLNodeType_MemberAccess,
    HLSLNodeType_ArrayAccess,
    HLSLNodeType_FunctionCall,
    HLSLNodeType_StateAssignment,
    HLSLNodeType_SamplerState,
    HLSLNodeType_Pass,
    HLSLNodeType_Technique,
    HLSLNodeType_Attribute,
    HLSLNodeType_Pipeline,
    HLSLNodeType_Stage,
};

enum HLSLBaseType
{
    HLSLBaseType_Unknown,
    HLSLBaseType_Void,    
    HLSLBaseType_Float,
    HLSLBaseType_FirstNumeric = HLSLBaseType_Float,
    HLSLBaseType_Float2,
    HLSLBaseType_Float3,
    HLSLBaseType_Float4,
    HLSLBaseType_Float3x3,
    HLSLBaseType_Float4x4,
    HLSLBaseType_Half,
    HLSLBaseType_Half2,
    HLSLBaseType_Half3,
    HLSLBaseType_Half4,
    HLSLBaseType_Half3x3,
    HLSLBaseType_Half4x4,
    HLSLBaseType_Bool,
    HLSLBaseType_FirstInteger = HLSLBaseType_Bool,
    HLSLBaseType_Int,
    HLSLBaseType_Int2,
    HLSLBaseType_Int3,
    HLSLBaseType_Int4,
    HLSLBaseType_Uint,
    HLSLBaseType_Uint2,
    HLSLBaseType_Uint3,
    HLSLBaseType_Uint4,
    HLSLBaseType_LastInteger = HLSLBaseType_Uint4,
    HLSLBaseType_LastNumeric = HLSLBaseType_Uint4,
    HLSLBaseType_Texture,
    HLSLBaseType_Sampler,           // @@ use type inference to determine sampler type.
    HLSLBaseType_Sampler2D,
    HLSLBaseType_Sampler3D,
    HLSLBaseType_SamplerCube,
    HLSLBaseType_Sampler2DShadow,
    HLSLBaseType_Sampler2DMS,
    HLSLBaseType_UserDefined,       // struct
    
    HLSLBaseType_Count,
    HLSLBaseType_NumericCount = HLSLBaseType_LastNumeric - HLSLBaseType_FirstNumeric + 1
};

inline bool IsSamplerType(HLSLBaseType baseType)
{
    return baseType == HLSLBaseType_Sampler ||
           baseType == HLSLBaseType_Sampler2D ||
           baseType == HLSLBaseType_Sampler3D ||
           baseType == HLSLBaseType_SamplerCube ||
           baseType == HLSLBaseType_Sampler2DShadow ||
           baseType == HLSLBaseType_Sampler2DMS;
}

enum HLSLBinaryOp
{
    HLSLBinaryOp_And,
    HLSLBinaryOp_Or,
    HLSLBinaryOp_Add,
    HLSLBinaryOp_Sub,
    HLSLBinaryOp_Mul,
    HLSLBinaryOp_Div,
    HLSLBinaryOp_Less,
    HLSLBinaryOp_Greater,
    HLSLBinaryOp_LessEqual,
    HLSLBinaryOp_GreaterEqual,
    HLSLBinaryOp_Equal,
    HLSLBinaryOp_NotEqual,
    HLSLBinaryOp_BitAnd,
    HLSLBinaryOp_BitOr,
    HLSLBinaryOp_BitXor,
    HLSLBinaryOp_Assign,
    HLSLBinaryOp_AddAssign,
    HLSLBinaryOp_SubAssign,
    HLSLBinaryOp_MulAssign,
    HLSLBinaryOp_DivAssign,
};

enum HLSLUnaryOp
{
    HLSLUnaryOp_Negative,       // -x
    HLSLUnaryOp_Positive,       // +x
    HLSLUnaryOp_Not,            // !x
    HLSLUnaryOp_PreIncrement,   // ++x
    HLSLUnaryOp_PreDecrement,   // --x
    HLSLUnaryOp_PostIncrement,  // x++
    HLSLUnaryOp_PostDecrement,  // x++
    HLSLUnaryOp_BitNot,         // ~x
};

enum HLSLArgumentModifier
{
    HLSLArgumentModifier_None,
    HLSLArgumentModifier_In,
    HLSLArgumentModifier_Out,
    HLSLArgumentModifier_Inout,
    HLSLArgumentModifier_Uniform,
};

enum HLSLTypeFlags
{
    HLSLTypeFlag_None = 0,
    HLSLTypeFlag_Const = 0x01,
    HLSLTypeFlag_Static = 0x02,
    //HLSLTypeFlag_Uniform = 0x04,
    //HLSLTypeFlag_Extern = 0x10,
    //HLSLTypeFlag_Volatile = 0x20,
    //HLSLTypeFlag_Shared = 0x40,
    //HLSLTypeFlag_Precise = 0x80,

    HLSLTypeFlag_Input = 0x100,
    HLSLTypeFlag_Output = 0x200,

    // Interpolation modifiers.
    HLSLTypeFlag_Linear = 0x10000,
    HLSLTypeFlag_Centroid = 0x20000,
    HLSLTypeFlag_NoInterpolation = 0x40000,
    HLSLTypeFlag_NoPerspective = 0x80000,
    HLSLTypeFlag_Sample = 0x100000,
};

enum HLSLAttributeType
{
    HLSLAttributeType_Unroll,
    HLSLAttributeType_Branch,
    HLSLAttributeType_Flatten,
};


struct HLSLNode;
struct HLSLRoot;
struct HLSLStatement;
struct HLSLAttribute;
struct HLSLDeclaration;
struct HLSLStruct;
struct HLSLStructField;
struct HLSLBuffer;
//struct HLSLBufferField;
struct HLSLFunction;
struct HLSLArgument;
struct HLSLExpressionStatement;
struct HLSLExpression;
struct HLSLBinaryExpression;
struct HLSLLiteralExpression;
struct HLSLIdentifierExpression;
struct HLSLConstructorExpression;
struct HLSLFunctionCall;
struct HLSLArrayAccess;
struct HLSLAttribute;

struct HLSLType
{
    explicit HLSLType(HLSLBaseType _baseType = HLSLBaseType_Unknown)
    { 
        baseType    = _baseType;
        typeName    = NULL;
        array       = false;
        arraySize   = NULL;
        flags  = 0;
    }
    HLSLBaseType        baseType;
    const char*         typeName;       // For user defined types.
    bool                array;
    HLSLExpression*     arraySize;
    int                 flags;
};

inline bool IsSamplerType(const HLSLType & type)
{
    return IsSamplerType(type.baseType);
}


/** Base class for all nodes in the HLSL AST */
struct HLSLNode
{
    HLSLNodeType        nodeType;
    const char*         fileName;
    int                 line;
};

struct HLSLRoot : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Root;
    HLSLRoot()          { statement = NULL; }
    HLSLStatement*      statement;          // First statement.
};

struct HLSLStatement : public HLSLNode
{
    HLSLStatement() 
    { 
        nextStatement   = NULL; 
        attributes      = NULL;
        hidden          = false;
    }
    HLSLStatement*      nextStatement;      // Next statement in the block.
    HLSLAttribute*      attributes;
    mutable bool        hidden;
};

struct HLSLAttribute : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Attribute;
    HLSLAttribute()
    {
        argument        = NULL;
        nextAttribute   = NULL;
    }
    HLSLAttributeType   attributeType;
    HLSLExpression*     argument;
    HLSLAttribute*      nextAttribute;
};

struct HLSLDeclaration : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Declaration;
    HLSLDeclaration()
    {
        name            = NULL;
        registerName    = NULL;
        semantic        = NULL;
        nextDeclaration = NULL;
        assignment      = NULL;
    }
    const char*         name;
    HLSLType            type;
    const char*         registerName;       // @@ Store register index?
    const char*         semantic;
    HLSLDeclaration*    nextDeclaration;    // If multiple variables declared on a line.
    HLSLExpression*     assignment;
};

struct HLSLStruct : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Struct;
    HLSLStruct()
    {
        name            = NULL;
        field           = NULL;
    }
    const char*         name;
    HLSLStructField*    field;              // First field in the structure.
};

struct HLSLStructField : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_StructField;
    HLSLStructField()
    {
        name            = NULL;
        semantic        = NULL;
        sv_semantic     = NULL;
        nextField       = NULL;
		hidden			= false;
    }
    const char*         name;
    HLSLType            type;
    const char*         semantic;
    const char*         sv_semantic;
    HLSLStructField*    nextField;      // Next field in the structure.
	bool				hidden;
};

/** A cbuffer or tbuffer declaration. */
struct HLSLBuffer : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Buffer;
    HLSLBuffer()
    {
        name            = NULL;
        registerName    = NULL;
        field           = NULL;
    }
    const char*         name;
    const char*         registerName;
    HLSLDeclaration*    field;
};

/** Field declaration inside of a cbuffer or tbuffer */ // @@ Isn't this just a regular declaration?
/*struct HLSLBufferField : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_BufferField;
    HLSLBufferField()
    {
        name            = NULL;
        nextField       = NULL;
    }
    const char*         name;
    HLSLType            type;
    HLSLBufferField*    nextField;      // Next field in the cbuffer.
};*/

/** Function declaration */
struct HLSLFunction : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Function;
    HLSLFunction()
    {
        name            = NULL;
        semantic        = NULL;
        sv_semantic     = NULL;
        statement       = NULL;
        argument        = NULL;
        numArguments    = 0;
    }
    const char*         name;
    HLSLType            returnType;
    const char*         semantic;
    const char*         sv_semantic;
    int                 numArguments;
    HLSLArgument*       argument;
    HLSLStatement*      statement;
};

/** Declaration of an argument to a function. */
struct HLSLArgument : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Argument;
    HLSLArgument()
    {
        name            = NULL;
        modifier        = HLSLArgumentModifier_None;
        semantic        = NULL;
        sv_semantic     = NULL;
        defaultValue    = NULL;
        nextArgument    = NULL;
    }
    const char*             name;
    HLSLArgumentModifier    modifier;
    HLSLType                type;
    const char*             semantic;
    const char*             sv_semantic;
    HLSLExpression*         defaultValue;
    HLSLArgument*           nextArgument;
};

/** A expression which forms a complete statement. */
struct HLSLExpressionStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_ExpressionStatement;
    HLSLExpressionStatement()
    {
        expression = NULL;
    }
    HLSLExpression*     expression;
};

struct HLSLReturnStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_ReturnStatement;
    HLSLReturnStatement()
    {
        expression = NULL;
    }
    HLSLExpression*     expression;
};

struct HLSLDiscardStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_DiscardStatement;
};

struct HLSLBreakStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_BreakStatement;
};

struct HLSLContinueStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_ContinueStatement;
};

struct HLSLIfStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_IfStatement;
    HLSLIfStatement()
    {
        condition     = NULL;
        statement     = NULL;
        elseStatement = NULL;
    }
    HLSLExpression*     condition;
    HLSLStatement*      statement;
    HLSLStatement*      elseStatement;
};

struct HLSLForStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_ForStatement;
    HLSLForStatement()
    {
        initialization = NULL;
        condition = NULL;
        increment = NULL;
        statement = NULL;
    }
    HLSLDeclaration*    initialization;
    HLSLExpression*     condition;
    HLSLExpression*     increment;
    HLSLStatement*      statement;
};

struct HLSLBlockStatement : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_BlockStatement;
    HLSLBlockStatement()
    {
        statement = NULL;
    }
    HLSLStatement*      statement;
};


/** Base type for all types of expressions. */
struct HLSLExpression : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Expression;
    HLSLExpression()
    {
        nextExpression = NULL;
    }
    HLSLType            expressionType;
    HLSLExpression*     nextExpression; // Used when the expression is part of a list, like in a function call.
};

struct HLSLUnaryExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_UnaryExpression;
    HLSLUnaryExpression()
    {
        expression = NULL;
    }
    HLSLUnaryOp         unaryOp;
    HLSLExpression*     expression;
};

struct HLSLBinaryExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_BinaryExpression;
    HLSLBinaryExpression()
    {
        expression1 = NULL;
        expression2 = NULL;
    }
    HLSLBinaryOp        binaryOp;
    HLSLExpression*     expression1;
    HLSLExpression*     expression2;
};

/** ? : construct */
struct HLSLConditionalExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_ConditionalExpression;
    HLSLConditionalExpression()
    {
        condition       = NULL;
        trueExpression  = NULL;
        falseExpression = NULL;
    }
    HLSLExpression*     condition;
    HLSLExpression*     trueExpression;
    HLSLExpression*     falseExpression;
};

struct HLSLCastingExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_CastingExpression;
    HLSLCastingExpression()
    {
        expression = NULL;
    }
    HLSLType            type;
    HLSLExpression*     expression;
};

/** Float, integer, boolean, etc. literal constant. */
struct HLSLLiteralExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_LiteralExpression;
    HLSLBaseType        type;   // Note, not all types can be literals.
    union
    {
        bool            bValue;
        float           fValue;
        int             iValue;
    };
};

/** An identifier, typically a variable name or structure field name. */
struct HLSLIdentifierExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_IdentifierExpression;
    HLSLIdentifierExpression()
    {
        name     = NULL;
        global  = false;
    }
    const char*         name;
    bool                global; // This is a global variable.
};

/** float2(1, 2) */
struct HLSLConstructorExpression : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_ConstructorExpression;
    HLSLConstructorExpression()
    {
        argument = NULL;
    }
    HLSLType            type;
    HLSLExpression*     argument;
};

/** object.member **/
struct HLSLMemberAccess : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_MemberAccess;
    HLSLMemberAccess()
    {
        object = NULL;
        field  = NULL;
    }
    HLSLExpression*     object;
    const char*         field;
};

/** array[index] **/
struct HLSLArrayAccess : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_ArrayAccess;
    HLSLArrayAccess()
    {
        array = NULL;
        index = NULL;
    }
    HLSLExpression*     array;
    HLSLExpression*     index;
};

struct HLSLFunctionCall : public HLSLExpression
{
    static const HLSLNodeType s_type = HLSLNodeType_FunctionCall;
    HLSLFunctionCall()
    {
        function        = NULL;
        argument        = NULL;
        numArguments    = 0;
    }
    const HLSLFunction* function;
    int                 numArguments;
    HLSLExpression*     argument;
};

struct HLSLStateAssignment : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_StateAssignment;
    HLSLStateAssignment()
    {
        stateName = NULL;
        sValue = NULL;
        nextStateAssignment = NULL;
    }

    const char*             stateName;
    int                     d3dRenderState;
    union {
        int                 iValue;
        float               fValue;
        const char *        sValue;
    };
    HLSLStateAssignment*    nextStateAssignment;
};

struct HLSLSamplerState : public HLSLExpression // @@ Does this need to be an expression? Does it have a type? I guess type is useful.
{
    static const HLSLNodeType s_type = HLSLNodeType_SamplerState;
    HLSLSamplerState()
    {
        numStateAssignments = 0;
        stateAssignments = NULL;
    }

    int                     numStateAssignments;
    HLSLStateAssignment*    stateAssignments;
};

struct HLSLPass : public HLSLNode
{
    static const HLSLNodeType s_type = HLSLNodeType_Pass;
    HLSLPass()
    {
        name = NULL;
        numStateAssignments = 0;
        stateAssignments = NULL;
        nextPass = NULL;
    }
    
    const char*             name;
    int                     numStateAssignments;
    HLSLStateAssignment*    stateAssignments;
    HLSLPass*               nextPass;
};

struct HLSLTechnique : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Technique;
    HLSLTechnique()
    {
        name = NULL;
        numPasses = 0;
        passes = NULL;
    }

    const char*         name;
    int                 numPasses;
    HLSLPass*           passes;
};

struct HLSLPipeline : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Pipeline;
    HLSLPipeline()
    {
        name = NULL;
        numStateAssignments = 0;
        stateAssignments = NULL;
    }
    
    const char*             name;
    int                     numStateAssignments;
    HLSLStateAssignment*    stateAssignments;
};

struct HLSLStage : public HLSLStatement
{
    static const HLSLNodeType s_type = HLSLNodeType_Stage;
    HLSLStage()
    {
        name = NULL;
        statement = NULL;
        inputs = NULL;
        outputs = NULL;
    }

    const char*             name;
    HLSLStatement*          statement;
    HLSLDeclaration*        inputs;
    HLSLDeclaration*        outputs;
};


/**
 * Abstract syntax tree for parsed HLSL code.
 */
class HLSLTree
{

public:

    explicit HLSLTree(Allocator* allocator);
    ~HLSLTree();

    /** Adds a string to the string pool used by the tree. */
    const char* AddString(const char* string);

    /** Returns true if the string is contained within the tree. */
    bool GetContainsString(const char* string) const;

    /** Returns the root block in the tree */
    HLSLRoot* GetRoot() const;

    /** Adds a new node to the tree with the specified type. */
    template <class T>
    T* AddNode(const char* fileName, int line)
    {
        HLSLNode* node = new (AllocateMemory(sizeof(T))) T();
        node->nodeType  = T::s_type;
        node->fileName  = fileName;
        node->line      = line;
        return static_cast<T*>(node);
    }

    HLSLFunction * FindFunction(const char * name);
    HLSLDeclaration * FindGlobalDeclaration(const char * name);
    HLSLStruct * FindGlobalStruct(const char * name);
    HLSLTechnique * FindTechnique(const char * name);
    HLSLPipeline * FindFirstPipeline();
    HLSLPipeline * FindNextPipeline(HLSLPipeline * current);
    HLSLPipeline * FindPipeline(const char * name);
    HLSLBuffer * FindBuffer(const char * name);

    bool GetExpressionValue(HLSLExpression * expression, int & value);
	//bool GetExpressionValue(HLSLExpression * expression, float & value);

private:

    void* AllocateMemory(size_t size);
    void  AllocatePage();

private:

    static const size_t s_nodePageSize = 1024 * 4;

    struct NodePage
    {
        NodePage*   next;
        char        buffer[s_nodePageSize];
    };

    Allocator*      m_allocator;
    StringPool      m_stringPool;
    HLSLRoot*       m_root;

    NodePage*       m_firstPage;
    NodePage*       m_currentPage;
    size_t          m_currentPageOffset;

};



class HLSLTreeVisitor
{
public:
    virtual void VisitType(HLSLType & type);

    virtual void VisitRoot(HLSLRoot * node);
    virtual void VisitTopLevelStatement(HLSLStatement * node);
    virtual void VisitStatements(HLSLStatement * statement);
    virtual void VisitStatement(HLSLStatement * node);
    virtual void VisitDeclaration(HLSLDeclaration * node);
    virtual void VisitStruct(HLSLStruct * node);
    virtual void VisitStructField(HLSLStructField * node);
    virtual void VisitBuffer(HLSLBuffer * node);
    //virtual void VisitBufferField(HLSLBufferField * node);
    virtual void VisitFunction(HLSLFunction * node);
    virtual void VisitArgument(HLSLArgument * node);
    virtual void VisitExpressionStatement(HLSLExpressionStatement * node);
    virtual void VisitExpression(HLSLExpression * node);
    virtual void VisitReturnStatement(HLSLReturnStatement * node);
    virtual void VisitDiscardStatement(HLSLDiscardStatement * node);
    virtual void VisitBreakStatement(HLSLBreakStatement * node);
    virtual void VisitContinueStatement(HLSLContinueStatement * node);
    virtual void VisitIfStatement(HLSLIfStatement * node);
    virtual void VisitForStatement(HLSLForStatement * node);
    virtual void VisitBlockStatement(HLSLBlockStatement * node);
    virtual void VisitUnaryExpression(HLSLUnaryExpression * node);
    virtual void VisitBinaryExpression(HLSLBinaryExpression * node);
    virtual void VisitConditionalExpression(HLSLConditionalExpression * node);
    virtual void VisitCastingExpression(HLSLCastingExpression * node);
    virtual void VisitLiteralExpression(HLSLLiteralExpression * node);
    virtual void VisitIdentifierExpression(HLSLIdentifierExpression * node);
    virtual void VisitConstructorExpression(HLSLConstructorExpression * node);
    virtual void VisitMemberAccess(HLSLMemberAccess * node);
    virtual void VisitArrayAccess(HLSLArrayAccess * node);
    virtual void VisitFunctionCall(HLSLFunctionCall * node);
    virtual void VisitStateAssignment(HLSLStateAssignment * node);
    virtual void VisitSamplerState(HLSLSamplerState * node);
    virtual void VisitPass(HLSLPass * node);
    virtual void VisitTechnique(HLSLTechnique * node);

    virtual void VisitFunctions(HLSLRoot * root);
    virtual void VisitParameters(HLSLRoot * root);

    HLSLFunction * FindFunction(HLSLRoot * root, const char * name);
    HLSLDeclaration * FindGlobalDeclaration(HLSLRoot * root, const char * name);
    HLSLStruct * FindGlobalStruct(HLSLRoot * root, const char * name);
};


// Tree transformations:
extern void PruneTree(HLSLTree* tree, const char* entryName0, const char* entryName1 = NULL);
extern void SortTree(HLSLTree* tree);
extern void GroupParameters(HLSLTree* tree);



} // M4

#endif