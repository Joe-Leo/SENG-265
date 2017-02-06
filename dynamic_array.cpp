#include <iostream>										//-
#include <string.h>										//-
#include "dynamic_array.h"									//-
												//-
using namespace std;										//-
												//-
// ********** public functions **********							//-
												//-
Dynamic_array::Dynamic_array() {								//-
	head_p = NULL;										//-
	size = 0;										//-
}												//-
												//-
Dynamic_array::Dynamic_array(Dynamic_array & d) {						//-
}												//-
												//-
Dynamic_array &Dynamic_array::operator=(Dynamic_array & d) {					//-
	return *this;										//-
}												//-
												//-
Dynamic_array::~Dynamic_array() {								//-
}												//-
												//-
void Dynamic_array::print_state(void) {								//-
	cout << "size:" << size << endl;							//-
	for (Block * p = head_p; p != NULL; p = p->next_p) {					//-
		cout << "\tsize:" << p->size << " elements:";					//-
		for (int i = 0; i < p->size; i++) {						//-
			cout << p->a[i] << ' ';							//-
		}										//-
		cout << endl;									//-
	}											//-
}												//-
												//-
int Dynamic_array::get_size(void) {								//-
	return size;
}												//-
												//-
int& Dynamic_array::operator[](int i) {								//-
	// check i for range error								//-
	if (i < 0 || i >= size) {								//-
		throw Subscript_range_exception();						//-
	}											//-
												//-
	// find target block and index								//-
	Block_position position = find_block(i);						//-
												//-
	// return element at index i								//-
	return position.block_p->a[position.i];							//-
}												//-
												//-
void Dynamic_array::insert(int x, int i) {							//-
	// case 1: range error									//-
	if (i < 0 || i > size) {								//-
		throw Subscript_range_exception();						//-
	}											//-
												//-
	// case 2: empty array									//-
	if (size == 0) {									//-
		// create linked list consisting of a single new block				//-
		Block * new_block_p = new Block;						//-
		new_block_p->size = 1;								//-
		new_block_p->a[0] = x;								//-
		new_block_p->next_p = NULL;							//-
												//-
		// insert new block								//-
		insert_blocks(NULL, new_block_p);						//-
												//-
		// update total size								//-
		size++;										//-
												//-
		return;										//-
	}											//-
												//-
	// find target block and index								//-
	Block_position position = find_block(i);						//-
												//-
	// case 3: non-empty array; new block not needed					//-
	if (position.block_p->size < BLOCK_SIZE) {						//-
		// shift block array right							//-
		for (int j = position.block_p->size; j > position.i; j--) {			//-
			position.block_p->a[j] = position.block_p->a[j-1];			//-
		}										//-
												//-
		// assign x									//-
		position.block_p->a[position.i] = x;						//-
												//-
		// update array and block size							//-
		size++;										//-
		position.block_p->size++;							//-
												//-
	// case 4: non-empty array; new block needed						//-
	} else {										//-
		// create new block								//-
		Block * new_block_p = new Block;						//-
		new_block_p->size = 1;								//-
		new_block_p->next_p = NULL;							//-
												//-
		// case 4.a: insert x into old block						//-
		if (position.i < BLOCK_SIZE) {							//-
			// copy last array element to new block					//-
			new_block_p->a[0] = position.block_p->a[BLOCK_SIZE-1];			//-
												//-
			// shift old block array right						//-
			for (int j = position.block_p->size-1; j > position.i; j--) {		//-
				position.block_p->a[j] = position.block_p->a[j-1];		//-
			}									//-
												//-
			// assign x into old block						//-
			position.block_p->a[position.i] = x;					//-
												//-
		// case 4.b: insert x into new block						//-
		} else {									//-
			new_block_p->a[0] = x;							//-
		}										//-
												//-
		// update total size								//-
		size++;										//-
												//-
		// insert new block into linked list						//-
		insert_blocks(position.block_p, new_block_p);					//-
	}											//-
}												//-
												//-
void Dynamic_array::insert(Dynamic_array &p, int i) {						//-
	// case 1: range error									//-
	if (i < 0 || i > size) {
		throw Subscript_range_exception();
	}

	// case 2: parameter array empty							//-
	if (p.size == 0) {	
		return;
	}

	// case 3: array empty									//-
	if (size == 0) {
		Block * copy_p = copy_blocks(p.head_p);

		// inserts copy_p so that position_p points to it. 
		insert_blocks(NULL, copy_p);

		// update total size of array
		size = size + p.size;
 
		return;
	}
	// find target block and index								//-
	Block_position position = find_block(i);						//-
	
	// case 4: array non-empty; new blocks not needed					//-
	if ( (position.block_p->size + p.size) < BLOCK_SIZE) {	
		
		//shift element to the right of (i + p.size)
		for (int j = (position.block_p-> size + p.size); j > position.i; j--) {
			position.block_p->a[j] = position.block_p->a[j-p.size];

		}

		// insert elements of p into position i
		for (int j = position.i, k=0; k<p.size ; j++,k++) {
			position.block_p->a[j] = p[k];
		}
		
		//update size
		size = size + p.size;
		position.block_p->size = position.block_p->size + p.size;
		
		return;

	// case 5: array non-empty; new blocks needed						//-
	} else {
		// copy p									//-
		Block * copy_p = copy_blocks(p.head_p);						//-

		// case 5.a: insert position at start of block					//-
		if (position.i == 0) {
			insert_blocks(position.pre_block_p, copy_p);
			size = size + p.size;
			return;
		}

		// case 5.b: insert position at middle of block					//-
		if (position.i < position.block_p->size) {
			
			//create new block
			Block * new_block_p = new Block;
			
			//make it the size of the remander of old block
			new_block_p->size = position.block_p->size - position.i;
			
			//fill blocks from ith pos of old block to new block.
			for (int j=position.i ,h=0; j<BLOCK_SIZE; j++,h++){
				new_block_p->a[h] = position.block_p->a[j];
			}

			//connect old block to p block
			insert_blocks(position.block_p,copy_p);

			//reduce the size of old block by i.
			position.block_p->size = position.block_p->size - position.i;

			//connect p block to new block.
			insert_blocks(copy_p, new_block_p);	

			//update size
			size = size + p.size;

			return;
		}
		
		// case 5.c: insert position after end of block					//-
		// this needs to (BLOCK_SIZE -1) since i goes from 0 to 4, not 5
		if (position.i == (BLOCK_SIZE-1)) {
			insert_blocks(position.block_p, copy_p);

			//update size of array by size of p
			size = size + p.size;
			return;
		}

	}
		// update total size								//-
}												//-
												//-
void Dynamic_array::remove(int i) {								//-
	// case 1: range error									//-
	if (i < 0 || i >= size) {
		throw Subscript_range_exception();
	}
	// find target block and index								//-
	Block_position position = find_block(i);

	// case 2: block size == 1								//-
	if (position.block_p->size == 1) {
		remove_blocks(position.pre_block_p, position.block_p, position.block_p);
		size--;
	}

	// case 3: block size > 1								//-
	else if (position.block_p->size > 1) {
		// loop throughout all elements after i, replace element preceding them
		for (int j = i; j < position.block_p->size; j++) {
			position.block_p->a[j] = position.block_p->a[j+1];
		};
		position.block_p->size--; // update block size
		size--; // update array size
	}
	// update total size									//-

}												//-
												//-
void Dynamic_array::remove(int start, int end) {						//-
	// case 1: range error
	if ((start < 0 || start > end) || (end < start || end > size)) {
		throw Subscript_range_exception();
		return;
	}

	// case 2: Array empty
	if (size == 0) {
		return;
	}

	//case 2: 0 interval 
	if (start == end) {
		//find the block
		Block_position position = find_block(start);
		//remove the current block 
		remove_blocks(NULL,position.block_p, position.block_p);
		return;
	}
	
	//case 3: start and end are on the same block
	// find the blocks
	Block_position position_start = find_block(start);
	Block_position position_end = find_block(end);
	//if they are the same block
	if (position_start.block_p == position_end.block_p ) {
	
//	cout << "There are start - end on same block: " << endl;

		// case 3a: start and end are on the extremes
		if( (start % BLOCK_SIZE) == 0 && (end % BLOCK_SIZE) == 4 ) {
	//		cout << "There is a case where we are at the extremes of one cell \n" << endl;
			remove_blocks(position_start.pre_block_p, position_start.block_p, position_start.block_p);
			size= size - BLOCK_SIZE;
			return;
		//else start and end are somewhere in between in the block	
		} else { 
			//cout << "the else case was enterted \n" << endl;
			//cout << "start @ " << position_start.i << " end @" << position_end.i << "\n" << endl;
			//shift elements to the left and then reduce size.
			for (int j = 0; j < BLOCK_SIZE - position_end.i ; j++) {
				
				//cout << "A[" << position_start.i+j << "] <- A[" <<  position_end.i +j <<"]" << endl;
				position_start.block_p->a[j + position_start.i] = position_start.block_p->a[j + position_end.i]; 
			}
			//reset the size of the block 
			position_start.block_p->size = (position_end.i - position_start.i); 
			size = size - (end - start) ;
			return;
		}
	
	}
	// case 4: start and end are on different blocks
	if (position_start.block_p != position_end.block_p) {

		//case 4a : start and end are on the ends of the blocks.
		if( (start % BLOCK_SIZE) == 0 && (end % BLOCK_SIZE) == 4 ) {
			remove_blocks(position_start.pre_block_p, position_start.block_p, position_end.block_p);
			size= size - BLOCK_SIZE;
			return;
		
		//case 4b : start in middle, end at end
		if( (start % BLOCK_SIZE) != 0 && (end % BLOCK_SIZE) == 4 ) {
			remove_blocks(position_start.pre_block_p, position_start.block_p, position_end.block_p);
			size= size - BLOCK_SIZE;
			return;

		// case 4c: start and end @ middle

		// case 4d: start @ mid, end @ mid
		
	}

	
	
/*
	//case 3: start on one blockend and end before reaching new block.
	if ( (start % BLOCK_SIZE) == 0 && (end % BLOCK_SIZE) == 4 ) {
	
		// n is the amount of blocks needed to be del, 
		int n = (end-start) / BLOCK_SIZE;

		//cout << "case 3 was entered and the number of blocks between is " << n <<  endl;
		if (start == 0) {

		}
		for ( int i = 0 ; i < n; i++) {
			Block_position position = find_block(start+(i*BLOCK_SIZE));
		//	cout << "The element at block position is " << position.block_p->a[start+(i*BLOCK_SIZE)]<< endl;;
			remove_blocks(position.pre_block_p,position.block_p, position.block_p);
		}

		return;
	}
*/
}												//-
												//-
// ********** private functions **********							//-
												//-
// purpose											//-
//	return b where										//-
//		if i < size									//-
//			b.block_p->a[b.i] is the ith element overall				//-
//		else										//-
//			b.block_p->a[b.i-1] is the i-1st element overall			//-
//												//-
//		if b.block_p == head_p								//-
//			b.pre_block_p is NULL							//-
//		else										//-
//			b.pre_block_p points to block preceding b.block_p			//-
// preconditions										//-
//	i in [1..size]										//-
Dynamic_array::Block_position Dynamic_array::find_block(int i) {				//-
	Block_position position;								//-
												//-
	// scan Block list									//-
	position.i = i;										//-
	position.pre_block_p = NULL;								//-
	for (position.block_p = head_p;								//-
	position.block_p != NULL;								//-
	position.block_p = position.block_p->next_p) {						//-
		// found in current block							//-
		if (position.i < position.block_p->size) {					//-
			break;									//-
		}										//-
												//-
		// special case: insert just after last element					//-
		if (i == size && position.block_p->next_p == NULL) {				//-
			break;									//-
		}										//-
												//-
		// not found yet: advance							//-
		position.pre_block_p = position.block_p;					//-
		position.i -= position.block_p->size;						//-
	}											//-
												//-
	return position;									//-
}												//-
												//-
// purpose											//-
//	create a new linked list which is a copy of the list pointed to p			//-
//	return a pointer to the head of the new linked list					//-
// preconditions										//-
//	p is the head of a possibly empty linked list of blocks					//-
Dynamic_array::Block * Dynamic_array::copy_blocks(Block * p) {					//-
	Block * new_head_p = NULL;								//-
	Block * new_p;										//-
	while (p != NULL) {									//-
		// allocate and link in new block						//-
		if (new_head_p == NULL) {							//-
			new_p = new Block;							//-
			new_head_p = new_p;							//-
		} else {									//-
			new_p->next_p = new Block;						//-
			new_p = new_p->next_p;							//-
		}										//-
												//-
		// copy the elements								//-
		new_p->size = p->size;								//-
		for (int i = 0; i < p->size; i++) {						//-
			new_p->a[i] = p->a[i];							//-
		}										//-
												//-
		// advance									//-
		p = p->next_p;									//-
	}											//-
												//-
	// terminate new list									//-
	if (new_head_p != NULL) {								//-
		new_p->next_p = NULL;								//-
	}											//-
												//-
	return new_head_p;									//-
}												//-
												//-
// purpose											//-
//	insert the list headed by src_p into the list headed by head_p				//-
//	if dst_p is NULL									//-
//		insert the list at the start of the list headed by head_p			//-
//	else											//-
//		insert the list just after block dst_p						//-
// preconditions										//-
//	list headed by src_p is non-empty							//-
//	list headed by src_p has no blocks in common with the list headed by head_p		//-
void Dynamic_array::insert_blocks(Block * dst_p, Block * src_p) {				//-
	// find the last block in the list headed by src_p					//-
	Block * p = src_p;									//-
	while (p->next_p != NULL) {								//-
		p = p->next_p;									//-
	}											//-
												//-
	// insert at head									//-
	if (dst_p == NULL) { // insert at head							//-
		p->next_p = head_p;								//-
		head_p = src_p;									//-
												//-
	// insert after dst_p									//-
	} else { // insert after dst_p								//-
		p->next_p = dst_p->next_p;							//-
		dst_p->next_p = src_p;								//-
	}											//-
}												//-
												//-
// purpose											//-
//	remove the blocks pointed to by start_p and end_p, and all the blocks between		//-
// preconditions										//-
//	start_p and end_p point to blocks in the list headed by head_p				//-
//	end_p points to either the same block as start_p or a block to its right		//-
//	if start_p == head_p									//-
//		pre_start_p == NULL								//-
//	else											//-
//		pre_start_p points to the block immediately preceding start_p			//-
//												//-
void Dynamic_array::remove_blocks(Block * pre_start_p, Block * start_p, Block * end_p) {	//-
	// release the blocks									//-
	while (1) {										//-
		// release start_p								//-
		Block * p = start_p->next_p;							//-
		delete start_p;									//-
												//-
		// advance									//-
		if (start_p == end_p) {								//-
			break;									//-
		} else {									//-
			start_p = p;								//-
		}										//-
	}											//-
												//-
	// link left and right sublists								//-
	if (pre_start_p == NULL) {								//-
		head_p = end_p->next_p;								//-
	} else {										//-
		pre_start_p->next_p = start_p->next_p;						//-
	}											//-
}												//-
