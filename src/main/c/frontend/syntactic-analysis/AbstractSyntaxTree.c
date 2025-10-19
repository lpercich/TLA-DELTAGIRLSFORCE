#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression *expression) {
    if (expression == NULL) return;
    switch (expression->type) {
        case SELECTION:
            destroyExpression(expression->selection.input);
            destroyCondition(expression->selection.condition);
            break;

        case PROJECTION:
            destroyExpression(expression->projection.input);
			destroyAttributes(expression->projection.attributes);
            break;

        case RHO:
            destroyExpression(expression->renaming.input);
            free(expression->renaming.newName);
            break;

        default:
            break;
    }

    free(expression);
}

void destroyCondition(Condition *condition) {
    if (condition == NULL)
        return;

    switch (condition->type) {
        case UNARY:
            destroyCondition(condition->unary.expr);
            condition->unary.expr = NULL;
            break;

        case BINARY:
            destroyCondition(condition->binary.left);
            destroyCondition(condition->binary.right);
            free(condition->binary.operator);
            condition->binary.left = NULL;
            condition->binary.right = NULL;
            condition->binary.operator = NULL;
            break;

        case COMPARISON:
            free(condition->comparison.leftOperand);
            free(condition->comparison.rightOperand);
            free(condition->comparison.operator);
            condition->comparison.leftOperand = NULL;
            condition->comparison.rightOperand = NULL;
            condition->comparison.operator = NULL;
            break;

        default:
            break;
    }

    free(condition);
}


void destroyRelation(Relation *relation){
	if (relation != NULL) {
		switch (relation->type) {
			case BASE_TABLE:
			free(relation->base.tableName);
			break;
			case JOIN: 
			destroyCondition(relation->binary.condition);
			case UNION:
			case INTERSECTION:
			case DIFF:
			destroyRelation(relation->binary.left);
			destroyRelation(relation->binary.right);
			break;
		}
		free(relation);
	}
}

void destroyProgram(Program *program) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program == NULL) return;
    if (program->relation != NULL) {
        destroyRelation(program->relation);
        program->relation = NULL;
    }

    if (program->expression != NULL) {
        destroyExpression(program->expression);
        program->expression = NULL;
    }

    free(program);
}

void destroyAttributes(Attributes *attrs) {
    while (attrs != NULL) {
        Attributes *next = attrs->next;
        free(attrs->value);   
        free(attrs);
        attrs = next;
    }
}