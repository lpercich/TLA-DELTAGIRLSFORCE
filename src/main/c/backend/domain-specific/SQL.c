#include <sql.h>


void initializeSqlModule()
{
    _logger = createLogger("Sql");
}

boolean validateAttsExpressionAndOrder(Order* order);
boolean expressionHasProjection(Expression* expression);
boolean attributesExist(Attributes * orderAtts, Attributes* projAtts);
void getProjectionAttributes(Expression * expression, Attributes * res);
boolean validateExpression(Expression* expression);

boolean validateProgram(Program *program){
    if (!program) return false;
    if(program->expression && program->order){
        return false;
    }else if (program->expression){
        return validateExpression(program->expression);
    }else if (program->order){
        return validateOrder(program->order);
    }
    return false;

}

boolean validateExpression(Expression* expression){
//falta esto
}

boolean validateOrder(Order* order){
    Attributes* a = o->attributes;
    Directions* d = o->directions;

    while (a && d) {
        a = a->next;
        d = d->next;
    }
    if (a || d) return false;

    if (!validateExpression(o->input))
        return false;
    if(!expressionHasProjection(order->input))
        return true;


    if (!validateAttsExpressionAndOrder(order->input, order)) {
        //esto seria para logger CAMBIARLO
        prlintf("ERROR: atributo en ORDER BY no está en la PROJECTION\n");
        return false;
    }
    return true;
 
}

boolean validateAttsExpressionAndOrder(Order* order){
    if (!o) return true;

    if (!expression_has_projection(o->input)) {
        return true;
    }

    Attributes* projected;
    getProjectionAttributes(o->input, projected);


    if (!attributesExist(o->attributes, projected)) {
        return false;
    }

    return true;

}

void getProjectionAttributes(Expression * expression, Attributes * res){
    switch (expression->type) {
        case SELECTION:
            getProjectionAttributes(expression->selection.input, res);
        case PROJECTION:
            res=expression->projection.attributes;
            return;
        case RHO:
            getProjectionAttributes(expression->renaming.input, res);
            break;
        case BASE_TABLE:
            return;
        case JOIN: 
            getProjectionAttributes(expression->join.left, res);
		    getProjectionAttributes(expression->join.right, res);
		case UNION:
		case INTERSECTION:
		case DIFF:
        case PRODUCT:
			getProjectionAttributes(expression->binary.left, res);
			getProjectionAttributes(expression->binary.right, res);
			break;
        case AGGR: //esto me da duda, podemos tener una projection adentro de una aggregation ????
                getProjectionAttributes(expression->aggregation.input, res);
                break;
        default:
            break;
    }
}

boolean attributesExist(Attributes * orderAtts, Attributes* projAtts){
    for (Attributes *o = orderAtts; o != NULL; o = o->next) {
        bool found = false;

        // Buscar o->value dentro de la lista projAtts
        for (Attributes *p = projAtts; p != NULL; p = p->next) {
            if (strcmp(o->value, p->value) == 0) {
                found = true;
                break;
            }
        }

    if (!found) return false;
    }

    return true;
}


bool expressionHasProjection(Expression* e) {
    while (e) {
        
        switch (e->type) {
            case PROJECTION: return true;
            case SELECTION: e = e->selection.input; break;
            case BASE_TABLE: return false;
            case JOIN: 
                return expression_has_projection(e->join.left)
                    || expression_has_projection(e->join.right);
            case JOIN: 
		case UNION:
		case INTERSECTION:
		case DIFF:
        case PRODUCT:
                return expression_has_projection(e->binary.left)
                    || expression_has_projection(e->binary.right);
        }
    }
    return false;
}


// struct Condition {
//     ConditionType type;

//     union {
//         struct { // Comparación simple: columna operador valor
//             char *leftOperand;   // ej: "edad"
//             char *operator;      // ej: ">"
//             char *rightOperand;  // ej: "18"
//         } comparison;

//         struct { // Binaria: AND / OR
//             Condition *left;
//             Condition *right;
//             char *operator;     // "AND" o "OR"
//         } binary;

//         struct { // Unaria: solo NOT
//             Condition *expr;
//         } unary;
//     };
// };


boolean validateCondition(Condition* condition){
    if (condition==NULL)
    {
        // AGREGAR LOG 
        return false; 
    }
    switch (condition->type)
    {
    case COMPARISON:
     if (condition->comparison.leftOperand == NULL || condition->comparison.rightOperand  == NULL || condition->comparison.operator  == NULL)
     {
        // AGREGAR LOG 
        return false

     }
     case BINARY:   
    if (condition->binary.left == NULL || condition->binary.right  == NULL || condition->binary.operator  == NULL)
    
     {
        // AGREGAR LOG 
        return false
     }
     case UNARY:
     if (condition->unary/expr==NULL)
     {
        // AGREGAR LOG 
        return false   
      }
     

    default:
        // AGREGAR LOG 
        return false   
    }

    


}



boolean validateAttributes(Attributes *attributes){
    if (attributes ==NULL){
        return true;
    }
//no termine


}

