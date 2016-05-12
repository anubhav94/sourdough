#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), throughput(0.0), latency(-1), current_window_size(50)
{ 
}

void Controller::do_best_action()
{
    int bestscore = 0;
    Controller::Action bestaction = none;
    Controller::MarkovKey temp = current_state;
    
    for (int i = sub16; i <= add16; i++)
    {
        temp.pastaction = static_cast<Controller::Action>(i);
        int v = Controller::value(temp);
        if (v > bestscore) 
        {
            bestscore = v;
            bestaction = static_cast<Controller::Action>(i);
        }
    }

    Controller::take_action(bestaction);
}

void Controller::take_action(Controller::Action a)
{
    switch (a)
    {
        case sub16:
            this->current_window_size = this->current_window_size - 16;
            break;
        default:
            break;
    }
}

int Controller::value(Controller::MarkovKey mk)
{
    if (mk.pastaction == none)
    {
        return 1;
    }
    return 1;
}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */
  unsigned int the_window_size = 50;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
  }

  return the_window_size;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
				    /* of the sent datagram */
				    const uint64_t send_timestamp )
                                    /* in milliseconds */
{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << send_timestamp
	 << " sent datagram " << sequence_number << endl;
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
			       /* what sequence number was acknowledged */
			       const uint64_t send_timestamp_acked,
			       /* when the acknowledged datagram was sent (sender's clock) */
			       const uint64_t recv_timestamp_acked,
			       /* when the acknowledged datagram was received (receiver's clock)*/
			       const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Default: take no action */

  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
	 << " received ack for datagram " << sequence_number_acked
	 << " (send @ time " << send_timestamp_acked
	 << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
	 << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms( void )
{
  return 1000; /* timeout of one second */
}

double Controller::current_throughput( void)
{
  return 5.5;
}

double Controller::current_latency( void)
{
  return 5.5;
}
