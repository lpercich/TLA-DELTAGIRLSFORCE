 #include "Generator.h"
 static Logger * _logger = NULL;

static void _outputSql(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}
void initializeGeneratorModule() {
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}
void generate(Program *program) {
    if (program == NULL) {
        logError(_logger, "Program is NULL in generate().");
        return;
    }
    _generateProgram(program);
    _outputSql(";\n");
}

// /** PRIVATE FUNCTIONS */

static void _generateProgram(Program *program);
static void _generateExpression(Expression *expr);
static void _generateSelection(Expression *expr);
static void _generateProjection(Expression *expr);
static void _generateRenaming(Expression *expr);
static void _generateAggregationExpr(Expression *expr);
static void _generateBaseTable(Expression *expr);
static void _generateJoin(Expression *expr);
static void _generateUnion(Expression *expr);
static void _generateIntersection(Expression *expr);
static void _generateDiff(Expression *expr);
static void _generateProduct(Expression *expr);

static void _generateCondition(Condition *cond);
static void _generateComparison(Condition *cond);
static void _generateBinaryCondition(Condition *cond);
static void _generateUnaryCondition(Condition *cond);

static void _generateAttributes(Attributes *attrs);
static void _generateAggregationList(Aggregation *aggr);
static void _generateOrder(Order *order);
static void _generateDirectionsForAttribute(Directions **dirPtr);

static void _checkNullInput(Expression * expr, char * context);

static void _checkNullInput(Expression * expr, char * context) {
	if(expr->binary.left == NULL) {
		logError(_logger, "%s expression has null left input.", context);
		return;
	}
	if(expr->binary.right == NULL) {
		logError(_logger, "%s expression has null right input.", context);
		return;
	}
	
}
static void _generateProgram(Program * program) {
	if(program->expression != NULL) {
		_generateExpression(program->expression);
	} else if(program->order != NULL) {
		_generateOrder(program->order);
	} else {
		logError(_logger, "Program has neither expression nor order.");
	}
}

static void _generateExpression(Expression *expr) {
	if(expr == NULL) {
		logError(_logger, "Expression is null.");
		return;
	}
	switch(expr->type) {
		case SELECTION:
			_generateSelection(expr);
			break;
		case PROJECTION:
			_generateProjection(expr);
			break;
		case RHO:
			_generateRenaming(expr);
			break;
		case AGGR:
			_generateAggregationExpr(expr);
			break;
		case BASE_TABLE:
			_generateBaseTableExpr(expr);
			break;
		case JOIN:
			_generateJoin(expr);
			break;
		case UNION:
			_generateUnion(expr);
			break;
		case INTERSECTION:
			_generateIntersection(expr);
			break;
		case DIFF:
			_generateDiff(expr);
			break;
		case PRODUCT:
			_generateProduct(expr);
			break;
		default:
		logError(_logger, "Unknown expression type: %d.", expr->type);
		break;
	}

}

static void _generateSelection(Expression *expr) {
	if(expr->selection.input == NULL) {
		logError(_logger, "Selection expression has null input.");
		return;
	}
	if(expr->selection.condition == NULL) {
		logError(_logger, "Selection expression has null condition.");
		return;
	}
	_outputSql("SELECT * FROM (");
	_generateExpression(expr->selection.input);
	_outputSql(") WHERE ");
	_generateCondition(expr->selection.condition);
}

static void _generateProjection(Expression *expr) {
	if(expr->projection.input == NULL) {
		logError(_logger, "Selection projection has null input.");
		return;
	}
	if(expr->projection.attributes == NULL) {
		logError(_logger, "Selection projection has null attributes.");
		return;
	}
	_outputSql("SELECT ");
	_generateAttributes(expr->projection.attributes);
	_outputSql(" FROM (");
	_generateExpression(expr->projection.input);
	_outputSql(")");
}

static void _generateRenaming(Expression *expr) {
	if(expr->renaming.input == NULL) {
		logError(_logger, "Renaming expression has null input.");
		return;
	}
	if(expr->renaming.newName == NULL) {
		logError(_logger, "Renaming expression has null new name.");
		return;
	}
	_outputSql("(");
	_generateExpression(expr->renaming.input);
	_outputSql(") AS %s", expr->renaming.newName);
}

static void _generateAggregationExpr(Expression *expr) {
//TODO
}

static void _generateBaseTable(Expression *expr) {
	if(expr->base.tableName == NULL) {
		logError(_logger, "Base table expression has null table name.");
		return;
	}
	_outputSql("%s", expr->base.tableName);
}

static void _generateJoin(Expression *expr) {
	if(expr->join.left == NULL) {
		logError(_logger, "Join expression has null left input.");
		return;
	}
	if(expr->join.right == NULL) {
		logError(_logger, "Join expression has null right input.");
		return;
	}
	if(expr->join.condition == NULL) {
		logError(_logger, "Join expression has null condition.");
		return;
	}
	_outputSql("SELECT * FROM (");
	_generateExpression(expr->join.left);
	_outputSql(") JOIN (");
	_generateExpression(expr->join.right);
	_outputSql(") ON ");
	_generateCondition(expr->join.condition);
}

static void _generateUnion(Expression *expr) {
	_checkNullInput(expr, "Union");
	_outputSql("(");
	_generateExpression(expr->binary.left);
	_outputSql(") UNION (");
	_generateExpression(expr->binary.right);
	_outputSql(")");
}

static void _generateIntersection(Expression *expr) {
	_checkNullInput(expr, "Intersection");
	_outputSql("(");
	_generateExpression(expr->binary.left);
	_outputSql(") INTERSECT (");
	_generateExpression(expr->binary.right);
	_outputSql(")");
}

static void _generateDiff(Expression *expr) {
	_checkNullInput(expr, "Difference");
	_outputSql("(");
	_generateExpression(expr->binary.left);
	_outputSql(") EXCEPT (");
	_generateExpression(expr->binary.right);
	_outputSql(")");
}

static void _generateProduct(Expression *expr) {
	_checkNullInput(expr, "Cartesian Product");
	_outputSql("SELECT * FROM (");
    _generateExpression(expr->binary.left);
    _outputSql("), (");
    _generateExpression(expr->binary.right);
    _outputSql(")");
}

static void _generateCondition(Condition *cond) {
	if(cond == NULL) {
		logError(_logger, "Condition is null.");
		return;
	}
	switch(cond->type) {
		case COMPARISON:
			_generateComparison(cond);
			break;
		case BINARY:
			_generateBinaryCondition(cond);
			break;
		case UNARY:
			_generateUnaryCondition(cond);
			break;
		default:
		logError(_logger, "Unknown condition type: %d.", cond->type);
		break;
	}
}

static void _generateComparison(Condition *cond) {
	if(cond->comparison.leftOperand == NULL) {
		logError(_logger, "Comparison condition has null left operand.");
		return;
	}
	if(cond->comparison.rightOperand == NULL) {
		logError(_logger, "Comparison condition has null right operand.");
		return;
	}
	if(cond->comparison.operator == NULL) {
		logError(_logger, "Comparison condition has null operator.");
		return;
	}
	_outputSql("%s %s %s", cond->comparison.leftOperand, cond->comparison.operator, cond->comparison.rightOperand);
}

static void _generateBinaryCondition(Condition *cond) {
	if(cond->binary.left == NULL) {
		logError(_logger, "Binary condition has null left condition.");
		return;
	}
	if(cond->binary.right == NULL) {
		logError(_logger, "Binary condition has null right condition.");
		return;
	}
	if(cond->binary.operator == NULL) {
		logError(_logger, "Binary condition has null operator.");
		return;
	}
	_outputSql("(");
	_generateCondition(cond->binary.left);
	_outputSql(") %s (", cond->binary.operator);
	_generateCondition(cond->binary.right);
	_outputSql(")");
}

static void _generateUnaryCondition(Condition *cond) {
	if(cond->unary.expr == NULL) {
		logError(_logger, "Unary condition has null expression.");
		return;
	}
	_outputSql("NOT (");
    _generateCondition(cond->unary.expr);
    _outputSql(")");
}

static void _generateAttributes(Attributes *attrs) {
	
}

static void _generateAggregationList(Aggregation *aggr) {

}

static void _generateOrder(Order *order) {

}

static void _generateDirectionsForAttribute(Directions **dirPtr) {

}












