#ifndef PRINTK_H
#define PRINTK_H


void printk( const char* msg );
void printk_err( const char* msg );
void printk_warn( const char* msg );
void printk_succ( const char* msg );

void putck( const char character );

#endif
