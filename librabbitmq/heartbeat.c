#include "amqp.h"

#include <stdio.h>

#include "../examples/utils.h"

int main()
{
  int sockfd;
  amqp_channel_t channel;
  amqp_connection_state_t conn = amqp_new_connection();


  die_on_error(sockfd = amqp_open_socket("localhost", 5672), "Opening socket");
  amqp_set_sockfd(conn, sockfd);

  die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 2, AMQP_SASL_METHOD_PLAIN, "guest", "guest"), "logging in");

  channel = 1;
  amqp_channel_open(conn, channel);
  die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

  /* Wait around for something to happen */

  while (1)
  {
    amqp_frame_t frame;
    int err;
    if ((err = amqp_simple_wait_frame(conn, &frame)))
    {
      const char* errstr = amqp_error_string(err);
      printf("Error is: %s\n", errstr);
      break;
    }

    printf("Frame received channel: %d type %d\n", frame.channel, frame.frame_type);

    amqp_frame_t hb;
    hb.channel = 0;
    hb.frame_type = AMQP_FRAME_HEARTBEAT;

    if ((err = amqp_send_frame(conn, &hb)))
    {
      const char* errstr = amqp_error_string(err);
      printf("Error is: %s\n", errstr);
      break;
    }
    printf("Heartbeat sent\n");
  }
  
  return 0;
}