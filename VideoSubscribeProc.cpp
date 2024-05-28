#include <iostream>
#include <opencv2/opencv.hpp>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
#include <string>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) 
{
    // Initialize RabbitMQ connection
    amqp_socket_t* socket = NULL;
    amqp_connection_state_t conn;
    conn = amqp_new_connection();
    socket = amqp_tcp_socket_new(AMQP_TCP_SOCKET_KEEPALIVE);
    if (!socket) 
    {
        cout << "Failed to create TCP socket" << endl;
        return 1;
    }

    amqp_socket_open(socket, "localhost", 5672);
    amqp_set_sockfd(conn, amqp_socket_get_sockfd(socket));

    // Open channel
    amqp_channel_open(conn, 1);
    amqp_bytes_t queueName = amqp_cstring_bytes("opencv_frames");

    // Declare queue
    amqp_queue_declare_ok_t* queueDeclareOk = amqp_queue_declare(conn, 1, queueName, 0, 0, 0, 1, amqp_empty_table);

    // Start consuming messages
    amqp_basic_consume(conn, 1, queueDeclareOk->queue, amqp_empty_bytes, 0, 0, 0, amqp_empty_table);

    // Receive and process frames
    while (true) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_consume_message(conn, &envelope, NULL, 0);

        if (envelope.message.body.len > 0) 
        {
            // Convert message body to OpenCV Mat
            vector<uchar> data(envelope.message.body.bytes, envelope.message.body.bytes + envelope.message.body.len);
            Mat frame = imdecode(data, IMREAD_COLOR);

            // Display the frame
            imshow("Frame", frame);
            waitKey(1);
        }

        amqp_destroy_envelope(&envelope);
    }

    // Clean up
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);

    return 0;
}
