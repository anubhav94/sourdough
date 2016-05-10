#ifndef CONTROLLER_HH
#define CONTROLLER_HH

#include <cstdint>
#include <map>

#define PACKET_SIZE 1424
#define NUM_PACKETS 1000
/* Congestion controller interface */

class Controller
{
 
  struct MarkovKey
  {
      uint64_t throughput;
      uint64_t interarrival;
      uint64_t pastaction;
  };

  using MarkovType = std::map<MarkovKey, std::string>;

private:
  bool debug_; /* Enables debugging output */

  static MarkovType markov_;

  double throughput;
  double latency;
  uint64_t packets[NUM_PACKETS];


  /* Add member variables here */

public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  //typedef std::map<std::string, std::string> MarkovType;

  /* Default constructor */
  Controller( const bool debug );

  void initialize_markov( void );

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
