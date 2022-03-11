
#include "cstatechart.h"

int __ev;

struct cs_event
{
  int event;
  int p1;
  int p2;
  int p3;
  int p4;
};
typedef struct cs_event cs_event_t;

static cs_event_t cyclic_event_buffer[EVENT_BUFFER_SIZE];
static int cs_event_index_execution = 0;
static int cs_event_index_adding = 0;


void cs_add_event( int _event )
{
  cs_add_event_with_parameters( _event, 0, 0, 0, 0 );
}

void cs_add_event_with_parameters( int _event, int p1, int p2, int p3, int p4 )
{
  if((cs_event_index_adding + 1) % EVENT_BUFFER_SIZE ==
    cs_event_index_execution )
    CS_PRINTF("* Event buffer overflow, please increase EVENT_BUFFER_SIZE *\n");
  else
  {
    cyclic_event_buffer[cs_event_index_adding].event = _event;
    cyclic_event_buffer[cs_event_index_adding].p1 = p1;
    cyclic_event_buffer[cs_event_index_adding].p2 = p2;
    cyclic_event_buffer[cs_event_index_adding].p3 = p3;
    cyclic_event_buffer[cs_event_index_adding].p4 = p4;
    cs_event_index_adding = (cs_event_index_adding + 1) % EVENT_BUFFER_SIZE;
  }
}

void cs_get_next_event()
{
  if(cs_event_index_execution != cs_event_index_adding)
  {
    __ev = cyclic_event_buffer[cs_event_index_execution].event;
    cs_event_index_execution = (cs_event_index_execution + 1) % EVENT_BUFFER_SIZE;
  }
}

int cs_event_buffer_empty()
{
  if(cs_event_index_execution == cs_event_index_adding)
    return 1;
  else
    return 0;
}
