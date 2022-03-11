#ifndef cstatechart_settings_h
#define cstatechart_settings

/* DOCUMENT 0 disables documentation mode. Statecharts run as normal. +
   DOCUMENT 1 enables graphviz / dot language output so that a 
              graphical representation of all statemachine is
              generated.
              The output of this mode can be visualized using the
              command line tool dot or the website http://viz-js.com/. 
*/
#define DOCUMENT 1

/* Change this if your printf has a different name. */
#define CS_PRINTF printf

#endif
