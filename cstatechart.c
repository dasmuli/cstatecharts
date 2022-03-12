
#include "cstatechart.h"
#include "string.h"

cs_event_t* __ev;

clock_t __execution_start_time;

double __elapsed_time;

static cs_event_t cyclic_event_buffer[EVENT_BUFFER_SIZE];
static int cs_event_index_execution = 0;
static int cs_event_index_adding = 0;


void cs_add_event( int _event )
{
  cs_add_event_with_parameter( _event, NULL );
}

void cs_add_event_with_parameter( int _event, char* parameter )
{
  if((cs_event_index_adding + 1) % EVENT_BUFFER_SIZE ==
    cs_event_index_execution )
    CS_PRINTF("* Event buffer overflow, please increase EVENT_BUFFER_SIZE *\n");
  else
  {
    cyclic_event_buffer[cs_event_index_adding].event = _event;
    if(parameter != NULL)
      memcpy(cyclic_event_buffer[cs_event_index_adding].parameter, 
        parameter, MAX_PARAMETER_SIZE);
    cs_event_index_adding = (cs_event_index_adding + 1) % EVENT_BUFFER_SIZE;
  }
}

void cs_get_next_event()
{
  if(cs_event_index_execution != cs_event_index_adding)
  {
    __ev = &cyclic_event_buffer[cs_event_index_execution];
    cs_event_index_execution = (cs_event_index_execution + 1) % EVENT_BUFFER_SIZE;
  }
  else
    __ev = NULL;
}

int cs_event_buffer_empty()
{
  if(cs_event_index_execution == cs_event_index_adding)
    return 1;
  else
    return 0;
}

char* cs_get_event_parameter()
{
  if(__ev == NULL)  /* Hm. Initial / time transition do not have events. */
    return (char*)&cyclic_event_buffer[0].parameter;
  return __ev->parameter;
}
