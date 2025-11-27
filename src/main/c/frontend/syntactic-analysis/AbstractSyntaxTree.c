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
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
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

        case BASE_TABLE:
            free(expression->base.tableName);
            break;
        case JOIN: 
            destroyExpression(expression->join.left);
			destroyExpression(expression->join.right);
			destroyCondition(expression->join.condition);
            break;
		case UNION:
		case INTERSECTION:
		case DIFFERENCE:
        case PRODUCT:
			destroyExpression(expression->binary.left);
			destroyExpression(expression->binary.right);
			break;
        case AGGREGATION:
                destroyExpression(expression->aggregation.input);
                destroyAttributes(expression->aggregation.group_by);
                destroyAggregation(expression->aggregation.aggregations);
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
            destroyCondition(condition->unary.expression);
            condition->unary.expression = NULL;
            break;

        case BINARY:
            destroyCondition(condition->binary.left);
            destroyCondition(condition->binary.right);
            free(condition->binary.operator);

            break;

        case COMPARISON:
            free(condition->comparison.leftOperand);
            free(condition->comparison.rightOperand);
            free(condition->comparison.operator);
            break;

        default:
            break;
    }

    free(condition);
}

void destroyOrder(Order* order){
    if(order!=NULL){
        destroyAttributes(order->attributes);
        destroyDirections(order->directions);
        destroyExpression(order->input);
        free(order);

    }
}

void destroyDirections(Directions *directions){
    while (directions != NULL) {
        Directions *next = directions->next;
        free(directions);
        directions = next;
    }
}


void destroyProgram(Program *program) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program == NULL) return;
  

    if (program->expression != NULL) {
        destroyExpression(program->expression);
        program->expression = NULL;
    }
    if(program->order !=NULL){
        destroyOrder(program->order);
    }

    free(program);
}

void destroyAttributes(Attributes *attributes) {
    while (attributes != NULL) {
        Attributes *next = attributes->next;
        free(attributes->value);   
        free(attributes);
        attributes = next;
    }
}
void destroyAggregation(Aggregation *aggregation) {
    while (aggregation != NULL) {
        Aggregation *next = aggregation->next;
        free(aggregation->function);   
        destroyAttributes(aggregation->attribute);   
        free(aggregation);
        aggregation = next;
    }
}
