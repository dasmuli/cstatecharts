#ifndef cstatechart_settings_h
#define cstatechart_settings

#include "stdio.h" /* For CS_PRINTF, see below. */

/* DOCUMENT 0 disables documentation mode. Statecharts run as normal. +
   DOCUMENT 1 enables graphviz / dot language output so that a 
              graphical representation of all statemachine is
              generated.
              The output of this mode can be visualized using the
              command line tool dot or the website http://viz-js.com/. 
*/
#define DOCUMENT 0


/* Change this if your printf has a different name.
   Change the stdio.h include above, too, if needed. */
#define CS_PRINTF printf


/* This is the number of events that is buffered. 
   This amounts to EVENT_BUFFER_SIZE * 5 * 4 bytes. */
#define EVENT_BUFFER_SIZE 100


/* The maximum size of parameters in bytes. */
#define MAX_PARAMETER_SIZE 4
 

#endif
