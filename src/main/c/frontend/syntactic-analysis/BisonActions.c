#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Condition *BinaryConditionSemanticAction(Condition *left, Condition *right, const char *op) {
    Condition *cond = malloc(sizeof(Condition));
    cond->type = BINARY;
    cond->binary.left = left;
    cond->binary.right = right;
    cond->binary.operator = strdup(op);
    return cond;
}

Condition *UnaryConditionSemanticAction(Condition *expr) {
    Condition *cond = malloc(sizeof(Condition));
    cond->type = NOT;
    cond->unary.expr = expr;
    return cond;
}

Condition *ComparisonConditionSemanticAction(char *left, char *op, char *right) {
    Condition *cond = malloc(sizeof(Condition));
    cond->type = COMPARISON;
    cond->comparison.leftOperand = left;
    cond->comparison.operator = op;
    cond->comparison.rightOperand = right;
    return cond;
}

Expression * SelectionSemanticAction( Condition * condition, Expression *input){
	Expression * exp = malloc(sizeof(Expression));
	exp->type= SELECTION;
	exp->selection.condition= condition; //preg si hay que reservar espacio ????
	exp->selection.input=input;
	return exp;
}

Expression * ProjectionSemanticAction(Expression *input, Attributes * atts){
	Expression * exp = malloc(sizeof(Expression));
	exp->type= PROJECTION;
	exp->projection.attributes=atts;
	exp->projection.input=input;
	return exp;
}

Expression * RenameSemanticAction(Expression* input, char * newName){
	Expression * exp = malloc(sizeof(Expression));
	exp->type=RHO;
	exp->renaming.input=input;
	exp->renaming.newName=newName;
	return exp;
}

/*
Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}*/

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * ExpressionProgramSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}
Program * RelationProgramSemanticAction(Relation * relation) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->relation = relation;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}


Expression *BinaryExpressionSemanticAction(RelationType type, Expression *left, Expression *right, Condition * cond) {
    Expression *e = calloc(1, sizeof(Expression));
    switch (type) {
        case JOIN:
            e->type = JOIN;
            e->join.left = left;
            e->join.right = right;
            e->join.condition = cond;
            break;
        case PRODUCT:
            e->type = PRODUCT;
            e->binary.left = left;
            e->binary.right = right;
            break;
        case UNION:
            e->type = UNION;
            e->binary.left = left;
            e->binary.right = right;
            break;
        case INTERSECTION:
            e->type = INTERSECTION;
            e->binary.left = left;
            e->binary.right = right;
            break;
        case DIFF:
            e->type = DIFF;
            e->binary.left = left;
            e->binary.right = right;
            break;
        default:
		break;
	}
		return e;
}



Expression *BaseExpressionSemanticAction(char *tableName) {
    Expression *e = malloc(sizeof(Expression));
    e->type = BASE_TABLE;
    e->base.tableName = tableName;
    return e;
}

Attributes * AtributeSemanticAction(char * next, Attributes * list){
		Attributes * ats= malloc(sizeof(Attributes));
		ats->value=next;
		ats->next=list;
		return ats;	
}
Expression * AggregationSemanticAction(Attributes *group_by, Aggregation *aggs, Expression *input) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression *expr = calloc(1, sizeof(Expression));
  	expr->type = AGGR;
    expr->aggregation.input = input;
    expr->aggregation.group_by = group_by;
    expr->aggregation.aggregations = aggs;
    return expr;
}

Aggregation * AggregationFunctionSemanticAction(char *func, char *attribute) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Aggregation *agg = calloc(1, sizeof(Aggregation));
    agg->function = strdup(func);
    agg->attribute = strdup(attribute);
    return agg;
}

