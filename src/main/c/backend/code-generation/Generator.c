 #include "Generator.h"
 static Logger * _logger = NULL;


// /** PRIVATE FUNCTIONS */

static void _generateProgram(Program *program);
static void _generateExpression(Expression *expression);
static void _generateSelection(Expression *expression);
static void _generateProjection(Expression *expression);
static void _generateRenaming(Expression *expression);
static void _generateAggregationExpr(Expression *expression);
static void _generateBaseTable(Expression *expression);
static void _generateJoin(Expression *expression);
static void _generateUnion(Expression *expression);
static void _generateIntersection(Expression *expression);
static void _generateDiff(Expression *expression);
static void _generateProduct(Expression *expression);

static void _generateCondition(Condition *condition);
static void _generateComparison(Condition *condition);
static void _generateBinaryCondition(Condition *condition);
static void _generateUnaryCondition(Condition *condition);

static void _generateAttributes(Attributes *attributes);
static void _generateAggregationList(Aggregation *aggregation);
static void _generateOrder(Order *order);
static void _generateExpressionSubquery(Expression *expression);
static void _generateBinaryExpressionSubquery(Expression *expression);


static void _checkNullInput(Expression * expression, char * context);





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

static void _checkNullInput(Expression * expression, char * context) {
	if(expression->binary.left == NULL) {
		logError(_logger, "%s expression has null left input.", context);
		return;
	}
	if(expression->binary.right == NULL) {
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

static void _generateExpression(Expression *expression) {
	if(expression == NULL) {
		logError(_logger, "Expression is null.");
		return;
	}
	switch(expression->type) {
		case SELECTION:
			_generateSelection(expression);
			break;
		case PROJECTION:
			_generateProjection(expression);
			break;
		case RHO:
			_generateRenaming(expression);
			break;
		case AGGREGATION:
			_generateAggregationExpr(expression);
			break;
		case BASE_TABLE:
			_generateBaseTable(expression);
			break;
		case JOIN:
			_generateJoin(expression);
			break;
		case UNION:
			_generateUnion(expression);
			break;
		case INTERSECTION:
			_generateIntersection(expression);
			break;
		case DIFFERENCE:
			_generateDiff(expression);
			break;
		case PRODUCT:
			_generateProduct(expression);
			break;
		default:
		logError(_logger, "Unknown expression type: %d.", expression->type);
		break;
	}

}

static void _generateSelection(Expression *expression) {
	if(expression->selection.input == NULL) {
		logError(_logger, "Selection expression has null input.");
		return;
	}
	if(expression->selection.condition == NULL) {
		logError(_logger, "Selection expression has null condition.");
		return;
	}
	_outputSql("SELECT * FROM ");
	_generateExpressionSubquery(expression->selection.input);
	_outputSql(" WHERE ");
	_generateCondition(expression->selection.condition);
}

static void _generateProjection(Expression *expression) {
	if(expression->projection.input == NULL) {
		logError(_logger, "Selection projection has null input.");
		return;
	}
	if(expression->projection.attributes == NULL) {
		logError(_logger, "Selection projection has null attributes.");
		return;
	}
	_outputSql("SELECT ");
	_generateAttributes(expression->projection.attributes);
	_outputSql(" FROM ");
	_generateExpressionSubquery(expression->projection.input);
}

static void _generateRenaming(Expression *expression) {
	if(expression->renaming.input == NULL) {
		logError(_logger, "Renaming expression has null input.");
		return;
	}
	if(expression->renaming.newName == NULL) {
		logError(_logger, "Renaming expression has null new name.");
		return;
	}
	_outputSql("SELECT * FROM ");
	
	_generateExpressionSubquery(expression->renaming.input);
	_outputSql(" AS %s", expression->renaming.newName);
}

static void _generateAggregationExpr(Expression *expression) {
    if (expression->aggregation.input == NULL) {
        logError(_logger, "Aggregation expression has null input.");
        return;
    }
    Attributes  *group_by = expression->aggregation.group_by;
    Aggregation *aggr     = expression->aggregation.aggregations;
    _outputSql("SELECT ");
   
    if (group_by != NULL) {
        _generateAttributes(group_by);
        if (aggr != NULL) {
            _outputSql(", ");
        }
    }
    if (aggr != NULL) {
        _generateAggregationList(aggr);
    } else if (group_by == NULL) {
        logError(_logger, "Aggregation expression must have aggregation functions or group by.");; 
		return;
	}
    _outputSql(" FROM ");
    _generateExpressionSubquery(expression->aggregation.input);
    if (group_by != NULL) {
        _outputSql(" GROUP BY ");
        _generateAttributes(group_by);
    }
}


static void _generateBaseTable(Expression *expression) {
	if(expression->base.tableName == NULL) {
		logError(_logger, "Base table expression has null table name.");
		return;
	}
	_outputSql("%s", expression->base.tableName);
}

static void _generateExpressionSubquery(Expression *expression) {
	bool cond= expression->type != BASE_TABLE;
    if (cond)
        _outputSql("(");

    _generateExpression(expression);

    if (cond)
        _outputSql(")");
}




static void _generateJoin(Expression *expression) {
	if(expression->join.left == NULL) {
		logError(_logger, "Join expression has null left input.");
		return;
	}
	if(expression->join.right == NULL) {
		logError(_logger, "Join expression has null right input.");
		return;
	}
	if(expression->join.condition == NULL) {
		logError(_logger, "Join expression has null condition.");
		return;
	}
	_generateBinaryExpressionSubquery(expression->join.left);
	_outputSql(" JOIN ");
	_generateExpressionSubquery(expression->join.right);
	_outputSql(" ON ");
	_generateCondition(expression->join.condition);
}



static void _generateUnion(Expression *expression) {
	_checkNullInput(expression, "Union");
	_generateBinaryExpressionSubquery(expression->binary.left);
	_outputSql(" UNION ");
	_generateBinaryExpressionSubquery(expression->binary.right);
}

static void _generateBinaryExpressionSubquery(Expression *expression){
    if (expression->type == BASE_TABLE)
       _outputSql("SELECT * FROM ");

    _generateExpression(expression);
}


static void _generateIntersection(Expression *expression) {
	_checkNullInput(expression, "Intersection");
	_generateBinaryExpressionSubquery(expression->binary.left);
	_outputSql(" INTERSECT ");
	_generateBinaryExpressionSubquery(expression->binary.right);
}

static void _generateDiff(Expression *expression) {
	_checkNullInput(expression, "Difference");
	_generateBinaryExpressionSubquery(expression->binary.left);
	_outputSql(" EXCEPT ");
	_generateBinaryExpressionSubquery(expression->binary.right);
}

static void _generateProduct(Expression *expression) {
	_checkNullInput(expression, "Cartesian Product");
	_outputSql("SELECT * FROM ");
    _generateExpressionSubquery(expression->binary.left);
    _outputSql(", ");
    _generateExpressionSubquery(expression->binary.right);
}

static void _generateCondition(Condition *condition) {
	if(condition == NULL) {
		logError(_logger, "Condition is null.");
		return;
	}
	switch(condition->type) {
		case COMPARISON:
			_generateComparison(condition);
			break;
		case BINARY:
			_generateBinaryCondition(condition);
			break;
		case UNARY:
			_generateUnaryCondition(condition);
			break;
		default:
		logError(_logger, "Unknown condition type: %d.", condition->type);
		break;
	}
}

static void _generateComparison(Condition *condition) {
	if(condition->comparison.leftOperand == NULL) {
		logError(_logger, "Comparison condition has null left operand.");
		return;
	}
	if(condition->comparison.rightOperand == NULL) {
		logError(_logger, "Comparison condition has null right operand.");
		return;
	}
	if(condition->comparison.operator == NULL) {
		logError(_logger, "Comparison condition has null operator.");
		return;
	}
	
		_outputSql("%s %s %s", condition->comparison.leftOperand, condition->comparison.operator, condition->comparison.rightOperand);

	
	
}

static void _generateBinaryCondition(Condition *condition) {
	if(condition->binary.left == NULL) {
		logError(_logger, "Binary condition has null left condition.");
		return;
	}
	if(condition->binary.right == NULL) {
		logError(_logger, "Binary condition has null right condition.");
		return;
	}
	if(condition->binary.operator == NULL) {
		logError(_logger, "Binary condition has null operator.");
		return;
	}
	_outputSql("(");
	_generateCondition(condition->binary.left);
	_outputSql(") %s (", condition->binary.operator);
	_generateCondition(condition->binary.right);
	_outputSql(")");
}

static void _generateUnaryCondition(Condition *condition) {
	if(condition->unary.expression == NULL) {
		logError(_logger, "Unary condition has null expression.");
		return;
	}
	_outputSql("NOT (");
    _generateCondition(condition->unary.expression);
    _outputSql(")");
}

static void _generateAttributes(Attributes *attributes) {
	Attributes *current = attributes;
    while (current != NULL) {
        if (current->value == NULL) {
            logError(_logger, "Attribute with null value encountered.");
            return;
        }
        _outputSql("%s", current->value);
        if (current->next != NULL) {
            _outputSql(", ");
        }
        current = current->next;
    }
}

static void _generateAggregationList(Aggregation *aggregation) {
    Aggregation *current = aggregation;
    while (current != NULL) {
        if (current->function == NULL) {
            logError(_logger, "Aggregation function is null.");
            return;
        }
		if (current->attribute == NULL) {
            logError(_logger, "Aggregation attribute is null.");
            return;
        }
        _outputSql("%s(", current->function);
        _generateAttributes(current->attribute);
        _outputSql(")");
        if (current->next != NULL) {
            _outputSql(", ");
        }
        current = current->next;
    }
}

static void _generateOrder(Order *order) {
	if(order->input == NULL) {
		logError(_logger, "Order has null input expression.");
		return;
	}
	if(order->attributes == NULL) {
		logError(_logger, "Order has null attributes.");
		return;
	}
	_generateExpression(order->input);
    _outputSql(" ORDER BY ");

    Attributes *attribute = order->attributes;
    Directions *direction  = order->directions;
	    while (attribute != NULL) {
        if (attribute->value == NULL) {
            logError(_logger, "Order attribute has null value.");
            return;
        }

        _outputSql("%s", attribute->value);
		      if (direction != NULL) {
            switch (direction->value) {
                case ASCENDANT:
                    _outputSql(" ASC");
                    break;
                case DESCENDANT:
                    _outputSql(" DESC");
                    break;
                case DEFAULT:
                    break;
                default:
                    logError(_logger, "Unknown direction type: %d", direction->value);
                    break;
            }
            direction = direction->next;
        }

        if (attribute->next != NULL) {
            _outputSql(", ");
        }

        attribute = attribute->next;
}
}












