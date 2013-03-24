#ifndef _LIB_FLOYD_HPP_
#define _LIB_FLOYD_HPP_

/********************* Header Files ***********************/
/* C++ Headers */
#include <iostream> /* Input/output objects. */
#include <fstream> /* File operations. */
//#include <sstream> /* String stream. */
//#include <string> /* C++ String class. */
//#include <new> /* Defines bad_malloc exception, new functions. */
//#include <typeinfo> /* Casting header. */
//#include <exception> /* Top level exception header. */
//#include <stdexcept> /* Derived exception classes. */

/* STL Headers */
//#include <vector>
//#include <list>
//#include <deque>
//#include <stack>
//#include <queue>
//#include <priority_queue>
//#include <bitset>
//#include <set> // multiset for multiple keys allowed.
//#include <map> // multimap for multiple keys allowed.
//#include <utility> // Has pair for map.
//#include <algorithm>
//#include <numeric>

/* C Headers */
//#include <cstdlib>
//#include <cstddef>
//#include <cctype>
//#include <cstring>
//#include <cstdio>
//#include <climits>
//#include <cassert>

/* Project Headers */

/******************* Constants/Macros *********************/
#define INF			1000

/******************* Type Definitions *********************/
/* For enums: Try to namesapce the common elements.
 * typedef enum {
 *	VAL_,
 * } name_e;
 */

/* For structs:
 * typedef struct name_s {
 *	int index;
 * } name_t;
 */

/****************** Class Definitions *********************/
/*
 * Public fields reduce this to mostly a struct.
 */
namespace floyd {
	class Matrix {
	public:
		Matrix() : size(0), a(NULL) { }
		Matrix(int _size);
		Matrix(const Matrix& m);
		virtual ~Matrix();
		Matrix& operator=(const Matrix& o);

		/* Functions */
		void read(std::istream& in);
		void print(std::ostream& out);

		/* Public vars by design, not good oop. */
		int size;
		int **a;
	};

	void init_path(Matrix& p);
}

/********************* Prototypes *************************/


#endif /* _LIB_FLOYD_HPP_ */
