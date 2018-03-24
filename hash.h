/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                               *
 *                    Sogang University                          *
 *         Department of Computer Science and Engineering        *
 *                                                               *
 * Subject name: System Programming                              *
 * Project title: [1] SIC/XE Machine - The Basics                *
 *                                                               *
 * Author: Inho Kim                                              *
 * Student ID: 20161577                                          *
 *                                                               *
 * File name: hash.h                                             *
 * File description: Header file for hash table related tasks.   *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

HASH_ENTRY* hashTable[HASH_SIZE]; // hash table pointer array

void opCMD(INPUT_CMD); // COMMAND: opcode
void oplistCMD();    // COMMAND: opcodelist

void hashCreate();                      // create hash table
int hashFunction(char*);                // hash function
void hashAddBucket(int, HASH_ENTRY*); // add bucket to hash table
