#ifndef CONTROLLER_HH
#define CONTROLLER_HH

#include <cstdint>
#include <map>

#define PACKET_SIZE 1424
#define NUM_PACKETS 1000
/* Congestion controller interface */

class Controller
{
  enum Action { sub16, sub8, sub4, sub2, sub1, none, add1, add2, add4, add8, add16 };
 
  struct MarkovKey
  {
      double throughput;
      double latency;
      Action pastaction;
  };

  using MarkovType = std::map<MarkovKey, std::string>;

private:
  bool debug_; /* Enables debugging output */

  double throughput;
  double latency;

  static MarkovKey current_state;
  static MarkovType markov_chain;
  int current_window_size;

  void do_best_action();
  void take_action(Action a);
  int value(Controller::MarkovKey mk);

  uint64_t packets[NUM_PACKETS];


  /* Add member variables here */

public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  //typedef std::map<std::string, std::string> MarkovType;

  /* Default constructor */
  Controller( const bool debug );

  /* Get current window size, in datagrams */
  unsigned int window_size( void );

  /* A datagram was sent */
  void datagram_was_sent( const uint64_t sequence_number,
			  const uint64_t send_timestamp );

  /* An ack was received */
  void ack_received( const uint64_t sequence_number_acked,
		     const uint64_t send_timestamp_acked,
		     const uint64_t recv_timestamp_acked,
		     const uint64_t timestamp_ack_received );

  /* How long to wait (in milliseconds) if there are no acks
     before sending one more datagram */
  unsigned int timeout_ms( void );
  
  double current_latency ( void );
  double current_throughput ( void );
  void update_latency (uint64_t packetRTT);
  void update_throughput (uint64_t timestamp);

};

#endif
