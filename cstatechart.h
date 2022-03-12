
#ifndef cstate_h
#define cstate_h

#include "lc.h"
#include "time.h"
#include "cstatechart_settings.h"



/* Generate an event in the statechart. This is buffered. */
void cs_add_event( int event );

/* Generate an event with a parameter. This is buffered. */
void cs_add_event_with_parameter( int _event, char* parameter );

/* This is called automatically in EXECUTE_END to dequeue the next event
   from the internal buffer. */
void cs_get_next_event();

/* Returns 1 if there are no events, and 0 otherwise. */
int cs_event_buffer_empty();

/* Returns the parameter that was used to enter the event. */
char* cs_get_event_parameter();




/* MACROS below, beware. */

#if DOCUMENT != 0
/* This collects names in document mode. */
struct transition_data
{
  const char* ev;
  const char* target_name; 
};
typedef struct transition_data td_t;
#endif


/* This is the main event structure. */
struct cs_event
{
  int event;
  char parameter[MAX_PARAMETER_SIZE];
};
typedef struct cs_event cs_event_t;


/* This is the main struc for statecharts that is passed around. */
struct cs
{
  /* Real execution related. */
  #if DOCUMENT == 0
  cs_event_t* event;
  lc_t lc;
  int execute_on_enter;
  int execute_on_exit;
  #endif
  
  double timer;  /* timer in seconds to trigger transitions */
  
  /* Documentation related. */
  #if DOCUMENT != 0
  const char* current_state_name;
  const char* parent_state_name;
  td_t transition_data[100];
  int transition_count;
  int show_initial_transition;
  #endif
};
typedef struct cs cs_t;

#define CS_WAITING 0
#define CS_YIELDED 1
#define CS_EXITED  2
#define CS_ENDED   3


#if DOCUMENT == 1
/* This is the documentation mode for graphviz / dot. */

#define STATE(cs,name)                \
  cs->current_state_name = #name;     \
  cs->transition_count = 0;               \
  CS_PRINTF("subgraph cluster_%s_%s {\n",cs->parent_state_name,#name); \
  CS_PRINTF("style = rounded\n");   \
  CS_PRINTF("label = %s\n",#name);    \
  CS_PRINTF("%s_%s_C [ style=invis, shape = point, label=\"\", height=0, width=0 ];\n",\
            cs->parent_state_name,#name); 

#define TRANSITION(cs,event,name)     \
   cs->transition_data[cs->transition_count].ev = #event; \
   cs->transition_data[cs->transition_count].target_name = #name; \
   cs->transition_count++;
   
#define TIME_TRANSITION(cs,time,name)     \
   cs->transition_data[cs->transition_count].ev = #time "s"; \
   cs->transition_data[cs->transition_count].target_name = #name; \
   cs->transition_count++;
  
            
#define ON_ENTER if( 0 )
#define ON_EXIT if( 0 )
#define INIT(cs)  
#define BEGIN(cs)  cs->show_initial_transition = 1;\
  CS_PRINTF("node [margin=0 width=0.2 height=0.2 shape=circle style=filled]\n");\
  CS_PRINTF("%s_initial [label=\"\" fillcolor=black]\n",cs->parent_state_name);
#define END(cs)   
#define ENDSTATE(cs,name) CS_PRINTF("}\n");	\
    if(cs->show_initial_transition == 1) {                \
    CS_PRINTF("%s_initial -> %s_%s_C [lhead=cluster_%s_%s ];\n",           \
            cs->parent_state_name,              \
            cs->parent_state_name,              \
            cs->current_state_name, \
            cs->parent_state_name,              \
            cs->current_state_name);\
      cs->show_initial_transition = 0; }        \
    for(int i = 0; i < cs->transition_count; i++)   \
    CS_PRINTF("%s_%s_C -> %s_%s_C [ltail=cluster_%s_%s lhead=cluster_%s_%s label=\"%s\" ];\n",           \
            cs->parent_state_name,              \
            cs->current_state_name,             \
            cs->parent_state_name,              \
            cs->transition_data[i].target_name,     \
            cs->parent_state_name,              \
            cs->current_state_name,             \
            cs->parent_state_name,              \
            cs->transition_data[i].target_name,      \
            cs->transition_data[i].ev);             \
    cs->current_state_name = NULL;		
#define EXECUTE_BEGIN                           \
  CS_PRINTF("digraph D {\ncompound=true\n");    \
  CS_PRINTF("graph [pad=\"0.5\", nodesep=\"1\", ranksep=\"1.75\"];");
  
#define EXECUTE_END CS_PRINTF("}\n");
#define RUN( state_func, state_data )           \
  CS_PRINTF( "subgraph cluster_%s {\nstyle = dashed\n", #state_func );    \
  CS_PRINTF( "  label = %s\n", #state_func );\
  state_data.parent_state_name = #state_func;  \
  state_func( &state_data );                   \
  CS_PRINTF( "}\n" );   


#else

/* This is the main execution mode generation. */

/* The current active event applied on all statecharts. */
extern cs_event_t* __ev;

/* This is used to measure the elapsed time. */
extern clock_t __execution_start_time;

/* This is the elapsed time. */
extern double __elapsed_time;


/* This is taken from protothread, cf. yield / wait. */
#define STATE(cs,name)                          \
    state_##name:                               \
    LC_SET((cs)->lc);				\
    if(CS_YIELD_FLAG == 0) 			\
      return CS_YIELDED;			\
    if(cs->execute_on_enter)                    \
      cs->timer = 0;

#define TRANSITION(cs,ev,name)                  \
  do {                                          \
    if(cs->event && cs->event->event == ev)   { \
      if(cs->execute_on_exit == 0) {            \
        cs->execute_on_exit = 1;                \
      } else {                                  \
        cs->execute_on_enter=1;                 \
        cs->execute_on_exit = 0;                \
        goto state_##name;                      \
      }                                         \
    }                                           \
  } while (0)

#define TIME_TRANSITION(cs,time,name)           \
  do {                                          \
    if(cs->timer > time) {                      \
      if(cs->execute_on_exit == 0) {            \
        cs->execute_on_exit = 1;                \
      } else {                                  \
        cs->execute_on_enter=1;                 \
        cs->execute_on_exit = 0;                \
        goto state_##name;                      \
      }                                         \
    }                                           \
  } while (0)
#define ON_ENTER if( cs->execute_on_enter )
#define ON_EXIT if( cs->execute_on_exit )
#define INIT(cs)   LC_INIT((cs)->lc); (cs)->execute_on_enter = 1; (cs)->execute_on_exit = 0; 
#define BEGIN(cs)  { char CS_YIELD_FLAG = 1; LC_RESUME((cs)->lc)
#define END(cs)    LC_END((pt)->lc); CS_YIELD_FLAG = 0; \
                    INIT(cs); return CS_ENDED; }
#define ENDSTATE(cs,name)			\
    cs->timer += __elapsed_time;                \
    cs->execute_on_enter=0;                     \
    if(cs->execute_on_exit == 0)                \
      CS_YIELD_FLAG = 0;		        \
    goto state_##name; 				
#define EXECUTE_BEGIN  while(1) { __execution_start_time = clock();
#define EXECUTE_END                            \
  __elapsed_time = (double)(clock()            \
  - __execution_start_time) / CLOCKS_PER_SEC;  \
  cs_get_next_event(); }
#define RUN( state_func, p_state_data )        \
  p_state_data.event = __ev;                   \
  state_func( &p_state_data );

#endif  /* #else of document mode */


#endif
