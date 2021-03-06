#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <ctime>
#include "controller.hh"
#include "timestamp.hh"

using namespace std;


/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), throughput(0.0), latency(40), current_window_size(10), 
    last_update_timestamp(0), old_throughput(0.0), old_latency(40) 
{ 
    srand(time(NULL));
}

Controller::MarkovType Controller::markov_chain = []
{
    MarkovType mk;
    return mk;
}();

Controller::MarkovKey Controller::current_state = []
{
    MarkovKey mkey;
    mkey.window_size_state = 50;
    mkey.pastaction = none;
    return mkey;
}();


void Controller::do_best_action()
{
    // Update current value for current action
    markov_chain[current_state] = (markov_chain[current_state] * (1 - REWARD_FACTOR) + (reward() * REWARD_FACTOR));
    cerr << "Current state: " << current_state.window_size_state << endl;

    double bestscore = (double) INT_MIN;
    Controller::Action bestaction = none;
    Controller::MarkovKey temp = current_state;
	
    int num_unvisited = 0;

    for (int i = sub16; i <= add16; i++)
    {
        temp.pastaction = static_cast<Controller::Action>(i);
	temp.window_size_state = this->get_next_window_size(static_cast<Controller::Action>(i));

	double v;

        if (markov_chain.count(temp) > 0) {
	    v = markov_chain[temp];
	    cerr << "Action: " << i << " Value: " << v << endl;
	} else {
	    markov_chain[temp] = -abs(i-none);
	    cerr << -abs(i-none) << endl;
	    v = -abs(i-none);
	    num_unvisited++;
	}
	
        if (v > bestscore) 
        {
            bestscore = v;
            bestaction = static_cast<Controller::Action>(i);
        }
    }

    if (rand() % add16 < (num_unvisited * 2))
	     bestaction = static_cast<Controller::Action>(rand() % add16);


    cerr << "Taking Action: " << bestaction << endl << endl;
 
    Controller::take_action(bestaction);
    old_latency = latency;
    old_throughput = throughput;
}
// TODO FIX REWARD!!!
double Controller::reward()
{
    double delta_latency = latency - old_latency;
    double delta_throughput = throughput - old_throughput;
    
    double reward = (delta_throughput) - (delta_latency*10);  
    
    cerr << "Reward: " << reward << " delta_latency: " << delta_latency << " delta_throughput: " << delta_throughput << endl; 
    return reward;
}


unsigned int Controller::get_next_window_size(Controller::Action a)
{
    int delta;
    switch (a)
    {
        case sub16:
            delta = -16;
            break;
        case sub8:
            delta = -8;
            break;
        case sub4:
            delta = -4;
            break;
        case sub2:
            delta = -2;
            break;
        case sub1:
            delta = -1;
            break;
        case add1:
            delta = 1;
            break;
        case add2:
            delta = 2;
            break;
        case add4:
            delta = 4;
            break;
        case add8:
            delta = 8;
	    break;
        case add16:
            delta = 16;
	    break;
        default:
            delta = 0;
            break;
    }

    unsigned int next_window_size = this->current_window_size + delta;   

    if (delta < 0) 
    {
        if (this->current_window_size <= (-1 * delta))
	{
	    next_window_size = 1;
	}
    }

    if (next_window_size > 300)
	next_window_size = 300;

    return next_window_size;
}


void Controller::take_action(Controller::Action a)
{
    unsigned int nw  = this->get_next_window_size(a);
    this->current_window_size = nw;

    current_state.pastaction = a;
    current_state.window_size_state = nw;
}

/* Get current window size, in datagrams */
unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of 100 outstanding datagrams */
  /* unsigned int the_window_size = 50;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
  }
  */
  return this->current_window_size;
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
    if ((current_time_stamp - packets[i]) < TPUT_UPDATE_TIME) {
      current_throughput += PACKET_SIZE;	  
    }
  }
  throughput = current_throughput / TPUT_UPDATE_TIME;
}

void Controller::update_markov(uint64_t current_time_stamp) {
  if (current_time_stamp - this->last_update_timestamp > STATE_UPDATE_TIME) {
    this->last_update_timestamp = current_time_stamp;
    do_best_action();
  }
}
