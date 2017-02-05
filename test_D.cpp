#include <iostream>
#include <stdlib.h>

#include "dynamic_array.h"

using namespace std;

int main() {
	Dynamic_array d0;
	for (int i = 0; i < 9; i++)
		d0.insert(2*i+1, i);
	d0.print_state();
// case 1 : index  out or range bound
	try {
		d0.remove(-1, 4); // too small
	} catch (Dynamic_array::Subscript_range_exception) {
		cout << "exception caught at (-1, 4)" << endl;
	}
	try {
		d0.remove(0, 10); // too big
	} catch (Dynamic_array::Subscript_range_exception) {
		cout << "exception caught at (0, 10)" << endl;
	}
	d0.print_state();

	//case : empty interval
	d0.remove(3, 3);
	d0.print_state();
	
	//case 1 block  (start and end on same block) .. subcases, is all block deleted  elements kept
	Dynamic_array d1;
	for (int i = 0; i < 9; i++)
		d1.insert(2*i+1, i);
	d1.remove(5, 9);
	d1.print_state();

	Dynamic_array d2;
	for (int i = 0; i < 9; i++)
		d2.insert(2*i+1, i);
	d2.remove(1, 4);
	d2.print_state();
	
	//remove start and end block.
	Dynamic_array d3;
	for (int i = 0; i < 9; i++)
		d3.insert(2*i+1, i);
	d3.remove(0, 9);
	d3.print_state();
	
	// remove start to end, since 19 is on other block
	Dynamic_array d4;
	for (int i = 0; i < 25; i++)
		d4.insert(2*i+1, i);
	d4.remove(5, 20);
	d4.print_state();

	//start del, end kept. 
	Dynamic_array d5;
	for (int i = 0; i < 9; i++)
		d5.insert(2*i+1, i);
	d5.remove(0, 6);
	d5.print_state();

	Dynamic_array d6;
	for (int i = 0; i < 9; i++)
		d6.insert(2*i+1, i);
	d6.remove(2, 9);
	d6.print_state();

	Dynamic_array d7;
	for (int i = 0; i < 9; i++)
		d7.insert(2*i+1, i);
	d7.remove(4, 6);
	d7.print_state();

	Dynamic_array d8;
	for (int i = 0; i < 25; i++)
		d8.insert(2*i+1, i);
	d8.remove(6, 19);
	d8.print_state();
}
