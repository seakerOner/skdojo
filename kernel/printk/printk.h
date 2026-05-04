#ifndef PRINTK_H
#define PRINTK_H

#include "../inttype.h"
#include "../memory/kata.h"

#define PRINTK_BUF_DYN_ORDER  ( ORDER_32KB )

void printk_init();

void printk( const ascii* msg );
void printk_err( const ascii* msg );
void printk_warn( const ascii* msg );
void printk_succ( const ascii* msg );

void putck( const ascii character );

#endif
