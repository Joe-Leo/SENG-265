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

	// case 3: 0 interval
	if (start == end) {
/*
		//find the block
		Block_position position = find_block(start);
		//remove the current block 
		remove_blocks(NULL,position.block_p, position.block_p);
*/
		return;
	}

	// find the blocks
	Block_position position_start = find_block(start);
	Block_position position_end = find_block(end);

	// case 4: start and end are on the same block
	if (position_start.block_p == position_end.block_p ) {
	
		// case 4a: start and end are on the ends also need to make sure block is full
		if ((start % BLOCK_SIZE) == 0 && (end % BLOCK_SIZE) == 4 && position_start.block_p->size == BLOCK_SIZE) {
			remove_blocks(position_start.pre_block_p, position_start.block_p, position_start.block_p);
			size = size - BLOCK_SIZE;		
			return;

		// case 4b: start and end are somewhere in between in the block	and block is full
		} else if ((start % BLOCK_SIZE) != 0 && (end % BLOCK_SIZE) != 4 && position_start.block_p->size == BLOCK_SIZE){ 
			
			//shift elements to the left and then reduce size.
			for (int j = 0; j < BLOCK_SIZE - position_end.i ; j++) {
				position_start.block_p->a[j + position_start.i] = position_start.block_p->a[j + position_end.i]; 	
			}

			//reset the size of the block 
			//remaining = Total - removed
			position_start.block_p->size = position_start.block_p->size - (position_end.i - position_start.i); 
			size = size - (end - start) ;
			return;
		
		//case 4c: start and end are somewhere in between block and block is not full
		} else {
	
			//shift elements to the left and then reduce size.
			for (int j = 0; j < position_start.block_p->size - position_end.i ; j++) {
				position_start.block_p->a[j + position_start.i] = position_start.block_p->a[j + position_end.i];	
			} 

			//reset the size of the block 
			//remaining = Total - removed
			position_start.block_p->size = position_start.block_p->size - (position_end.i - position_start.i); 

			//remove block if empty
			if (position_start.block_p->size == 0) {
				remove_blocks(position_start.pre_block_p, position_start.block_p, position_start.block_p);
			}

			size = size - (end - start);
			}
	}
	
	// case 5: start and end are on different blocks
	if (position_start.block_p != position_end.block_p && (end-start) > BLOCK_SIZE) {

		// how many blocks in between start and end?
		// check cases
		// ex: start = 0, end = 14 , [5][s][][][][]->[5][][][][][]->[5][][][][][e] = 2
		// ex: start = 1, end = 14 , [5][][s][][][]->[5][][][][][]->[5][][][][][e] = 1
		// ex: start = 7, end = 14 , [5][][][][][]->[5][][][s][][]->[5][][][][][e] = 0
		// ex: start = 6, end = 11 , [5][][][][][]->[5][e][][][][]->[5][][e][][][] = 0
		int num_of_Blocks = 0;
		Block_position blocks_between = find_block(start);
//		cout << "\n num of blocks values b4 loop " << start << " " << end << " " << num_of_Blocks << endl;
		
		//this works for all cases where end does not fall on the first number of new block
		if (position_end.i != 0) {
//			cout << "\nCASE e not 0 " << endl;
			for ( int i = 0; i < end; i+=BLOCK_SIZE ) {
				if (blocks_between.block_p == position_end.block_p) {
					return;
				}
				else {
//					cout << "\n num of blocks values " << i << " " << start + i << " " << end << " " << num_of_Blocks << endl;
					++num_of_Blocks;
				}
				blocks_between = find_block(start+i);
			}
			//case where the end [5][][][][][]->[5][e][][][][] 
		} else{
//			cout << "\nCASE e @ 0 " << endl;
			for ( int i = 0; i < end; i+=BLOCK_SIZE ) {
				if (blocks_between.block_p == position_end.pre_block_p) {
					return;
				}
				else {
	//				cout << "\n num of blocks values " << i << " " << start + i << " " << end << " " << num_of_Blocks << " case E @ 0" << endl;
					num_of_Blocks ++;
				}
				blocks_between = find_block(start+i);
			}
		}
//		cout << "\nnum_ofBlocks b4 correction is " << num_of_Blocks << endl;

		//if num_of_Blocks ==0, then on same block, if 1 then on adjacent block.
		if (num_of_Blocks <= 1) {
			cout << "Enterted if num_of_Blocks <=1" << endl;
			num_of_Blocks = 0;		
			cout << "Num of Blocks should be = 0  but num_of_Blocks is " << num_of_Blocks << " with START " << start << " END " << end << endl; 	
		}
		// if 2 or more, then it is num_blocks away from your starting block.
		else {
			num_of_Blocks = num_of_Blocks -1;	
//			cout << "Num of Blocks should be > 0  but num_of_Blocks is " << num_of_Blocks << " with START " << start << " END " << end << endl; 
		}

		// case 5a: start @ begining of Block
		if( (start % BLOCK_SIZE) == 0 ) {
			
			// case5a.a: remove to end of a block.
			if ((end-1) % BLOCK_SIZE == 4 && end % BLOCK_SIZE == 0) {
				size = size - num_of_Blocks*BLOCK_SIZE - position_end.block_p->size;
				remove_blocks(position_start.pre_block_p, position_start.block_p, position_end.block_p);
				return;

				// case5a.b: remove to somewhere in middle of end block
			} else {
				//cout << "Case Found diff Block: 0 and not 4" << endl;
				
				//shift elements left 
				//move elements from end pos to the 0th pos 
				//case example thought: 
				//remove(0,6)=
				// [5][s][x][x][x][x][x]->[4][x][e][][][][]
				// =[4][e][x][x][x][x]
				//remove(0,7)
				// [5][s][x][x][x][x]->[3][x][x][e][][]	
				for (int j = 0; j < (position_end.block_p->size - position_end.i); j++ ) {
					position_end.block_p->a[j] = position_end.block_p->a[ j + position_end.i];
				}

				// update size
				position_end.block_p->size = position_end.block_p->size - position_end.i;
				size = size - num_of_Blocks*BLOCK_SIZE - position_end.i;

				//need to remove from start to block before block remains.
				remove_blocks(position_start.pre_block_p, position_start.block_p, position_end.pre_block_p);

				return;
			}
		}
			


		// case 5b : start in middle 
		if ( (start % BLOCK_SIZE) != 0) {

			cout << "THESE ARE ALL CASES WHERE START is NOT 0" << endl;
			// case 5b.a remove all blocks up to end block
			if ( ((end-1) % BLOCK_SIZE) == 4 && (end % BLOCK_SIZE) == 0) {
				// First Block, reset size
				// if pos = 3, [5][][][][3][r], [0][1][2] 3 elements remain, (Block - pos)= 2 removed.
				// if pos = 2, [5][][][2][r][r], [0][1] remain, 
				position_start.block_p->size = position_start.i;

				// update size
				// [5][][][2][r][r] = 5-2 = 3
				size = size - num_of_Blocks*BLOCK_SIZE - position_start.block_p->size ;

				// remove blocks in between start block and end block
				remove_blocks(position_start.block_p, position_start.block_p->next_p, position_end.block_p);
				return;
			}

			//case 5b.b remove mid to mid
			if ( ((end-1) % BLOCK_SIZE) != 4 && (end % BLOCK_SIZE) != 0) {
				cout << "Case Found diff Block: not 0 and not 4 " << endl;
				cout << "\n THIS IS THE ACTIVE CASE WE ARE LOOKING AT " << endl;
				cout << "\n \n starting values START " << start << " END " << end << " P.size " << position_start.block_p->size << " E.size " << position_end.block_p->size << " num_of_Blocks " << num_of_Blocks << endl;  
				cout << "\n size " << size << " numblocks*size " << num_of_Blocks*BLOCK_SIZE << " end block size " << position_end.block_p->size << endl;

			
				// shift left from end block
				for (int j = 0; j < position_end.block_p->size - position_end.i ; j++) {
				position_end.block_p->a[j] = position_end.block_p->a[j + position_end.i]; 	
				}

				// update size
				position_start.block_p->size = position_start.i;
				position_end.block_p->size = position_end.i;
				size = size - num_of_Blocks*BLOCK_SIZE - position_end.i - position_start.i;

				// remove the blocks
				// case adjacent blocks
				if ( num_of_Blocks <= 1) {
					remove_blocks(position_start.pre_block_p, position_start.block_p, position_end.block_p);
				}
				// case there are blocks between start bock and end block
				else {
					remove_blocks(position_start.pre_block_p, position_start.block_p->next_p, position_end.pre_block_p);
				}

				// shift numbers from 
				cout << "\n \n ending values  START " << start << " END " << end << " S.size " << position_start.block_p->size << " E.Size " << position_end.block_p->size << endl;
				cout << "\nThis is end of ACTIVE CASE " << endl;
				return;
			}
		}





		
	}
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
