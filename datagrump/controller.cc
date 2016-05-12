#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), throughput(0.0), latency(-1), current_window_size(50), last_update_timestamp(0)
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
        case sub8:
            this->current_window_size = this->current_window_size - 8;
            break;
        case sub4:
            this->current_window_size = this->current_window_size - 4;
            break;
        case sub2:
            this->current_window_size = this->current_window_size - 2;
            break;
        case sub1:
            this->current_window_size = this->current_window_size - 1;
            break;
        case add1:
            this->current_window_size = this->current_window_size + 1;
            break;
        case add2:
            this->current_window_size = this->current_window_size + 2;
            break;
        case add4:
            this->current_window_size = this->current_window_size + 4;
            break;
        case add8:
            this->current_window_size = this->current_window_size + 8;
	    break;
        case add16:
            this->current_window_size = this->current_window_size + 16;
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
  packets[sequence_number % NUM_PACKETS] = send_timestamp;
  update_throughput(send_timestamp);
  update_markov(send_timestamp);
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
  update_latency(timestamp_ack_received - send_timestamp_acked);
  update_throughput(timestamp_ack_received);
  update_markov(timestamp_ack_received);
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
  return throughput;
}

/* Send current latency value - calculated as an EWMA */
double Controller::current_latency( void)
{
  return latency;
}

/* Exponential weighted moving average of RTT for packets */ 
/* Currently set alpha value to be 0.5 */
void Controller::update_latency( uint64_t packet_RTT)
{
  double alpha = 0.01;
  latency = latency * (1 - alpha) + packet_RTT * alpha; 	
}

void Controller::update_throughput(uint64_t current_time_stamp ) { 
  int i;
  double current_throughput = 0;
  for (i = 0; i < NUM_PACKETS; i++) {
    if (packets[i] < current_time_stamp && (current_time_stamp - packets[i] < 5000)) {
      current_throughput += PACKET_SIZE;	  
    } 
  }
  throughput = current_throughput / 5000;
}

void Controller::update_markov(uint64_t current_time_stamp) {
  if (current_time_stamp - this->last_update_timestamp > 50) {
    this->last_update_timestamp = current_time_stamp;
    do_best_action();
  }
}
