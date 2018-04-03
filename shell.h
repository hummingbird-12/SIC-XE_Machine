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
 * File name: shell.h                                            *
 * File description: Header file for shell related tasks.        *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void helpCMD(); // COMMAND: help
void dirCMD();  // COMMAND: dir
void quitCMD(); // COMMAND: quit
void histCMD(); // COMMAND: history
void typeCMD(); // COMMAND: type

void histAdd(char*);    // function to add input command into history
void histFree();        // function to free history linked list
