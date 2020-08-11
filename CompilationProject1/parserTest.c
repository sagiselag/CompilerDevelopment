///*GLOBAL_VARS*/
///*all correct options*/
//int x;
//float y;
//int arr[1, 2, 3];
//float arr2[4];
//
///*FUNC_PREDEFS*/
//int func1();
//float func2(int a);
//void func3(float b);
//int func4(int c, float d);
//int func5(int e, float f, int g, float h);
//int func6(int e, float f, int g, float h);
//int func7(int i[10], float j[1,2,3], int k);
//
///*FUNC_FULL_DEFS*/
//int func1() {
//	/*VAR_DEC_LIST*/
//	int x;
//	float y;
//	int arr[1, 2, 3];
//	float arr2[4];
//	
//	/*STMT_LIST -> STMT   STMT_LIST'  */
//	/*STMT -> id VAR_WITH_EXPR_OR_CALL' | COMP_STMT | IF_STMT | RETURN_STMT*/
//	/*STMT -> id VAR_WITH_EXPR_OR_CALL'*/
//	/*VAR_WITH_EXPR_OR_CALL'-> epsilon = TERM | TERM+TERM >*/
//	x = y;
//	x = 1;
//	x = 1.1e1;
//	x = 1+2;
//	x = y + 3;
//	x = y + 3;
//	x = y + 3+3;
//	x = y + 3 +3;
//	x = y + 3  + 3;
//	x = y + 3+ 3;
//	x = y + (3 + 3);
//	x = y + (3 + 3);
//	x = y + (3*x + 3);
//	x = y + (3*1.1e1 + 3);
//	/*VAR_WITH_EXPR_OR_CALL'-> [ EXPR_LIST ] = TERM | TERM+TERM */
//	x[y] = y;
//	x[3] = 1;
//	x[3+3] = 1.1e1;
//	x[3*3] = 1 + 2;
//	x[y+3] = y + 3;
//	x[3+(3 +3)] = y + 3;
//	/*VAR_WITH_EXPR_OR_CALL'-> (ARGS) = TERM | TERM+TERM >*/
//	x();
//	x(3);
//	x(3 + 3);
//	x(3 * 3);
//	x(y + 3);
//	x(3 + (3 + 3));
//
//	/*STMT_LIST -> STMT   STMT_LIST'  */
//	/*STMT -> id VAR_WITH_EXPR_OR_CALL' | COMP_STMT | IF_STMT | RETURN_STMT*/
//	/*STMT -> COMP_STMT */
//	/*COMP_STMT -> { VAR_DEC_LIST STMT_LIST } */
//	/*COMP_STMT -> { VAR_DEC STMT } */
//	/*STMT -> id VAR_WITH_EXPR_OR_CALL'*/
//	/*VAR_WITH_EXPR_OR_CALL'-> epsilon = TERM | TERM+TERM >*/
//	{
//		int x;
//		float y;
//		int arr[1, 2, 3];
//		float arr2[4];
//
//		x = y;
//		x = 1;
//		x = 1.1e1;
//		x = 1 + 2;
//		x = y + 3;
//		x = y + 3;
//		x = y + 3 + 3;
//		x = y + 3 + 3;
//		x = y + 3 + 3;
//		x = y + 3 + 3;
//		x = y + (3 + 3);
//		x = y + (3 + 3);
//		x = y + (3 * x + 3);
//		x = y + (3 * 1.1e1 + 3);
//		x[y] = y;
//		x[3] = 1;
//		x[3 + 3] = 1.1e1;
//		x[3 * 3] = 1 + 2;
//		x[y + 3] = y + 3;
//		x[3 + (3 + 3)] = y + 3;
//		x();
//		x(3);
//		x(3 + 3);
//		x(3 * 3);
//		x(y + 3);
//		x(3 + (3 + 3))
//	};
//	/*IF_STMT*/
//	if (x == y) x = 5;
//	if (x != y) x = 5;
//	if (x > y) x = 5;
//	if (x >= y) x = 5;
//	if (x < y) x = 5;
//	if (x <= y) x = 5;
//
//	/*RETURN_STMT*/
//	/*RETURN_STMT->return RETURN_STMT'->return EXPR->*/
//	/*EXPR-> TERM EXPR' -> FACTOR TERM'-> id VAR_OR_CALL'-> */
//	return;
//	return x;
//	return x * y;
//	return x * y + x;
//	return x * y + 5;
//	return x + 5;
//	return (x + 5);
//}