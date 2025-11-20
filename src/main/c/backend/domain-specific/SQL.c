#include <sql.h>


void initializeSqlModule()
{
    _logger = createLogger("Sql");
}

boolean validateAttsExpressionAndOrder(Order* order);
boolean expressionHasProjection(Expression* expression);
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
    

}

boolean validateOrder(Order* order){
    int i=1;
    if(expressionHasProjection(order->input)){
        i= validateAttsExpressionAndOrder(order->input, order);
    }
    Attributes* a = o->attributes;
    Directions* d = o->directions;

    while (a && d) {
        a = a->next;
        d = d->next;
    }
    if (a || d) return false;

     if (!validateExpression(o->input))
        return false;
 
}


bool expressionHasProjection(Expression* e) {
    while (e) {
        if (e->type == PROJECTION) return true;

        
        switch (e->type) {
            case PROJECTION: return true;
            case SELECTION: e = e->selection.input; break;
            case BASE_TABLE: return false;
            case JOIN: 
                return expression_has_projection(e->join.left)
                    || expression_has_projection(e->join.right);
            default:
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

