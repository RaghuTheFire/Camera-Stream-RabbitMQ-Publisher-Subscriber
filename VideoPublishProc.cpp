
#include <iostream>
#include <opencv2/opencv.hpp>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>

int main() 
{
    // Initialize RabbitMQ connection
    amqp_connection_state_t conn = amqp_new_connection();
    amqp_socket_t *socket = NULL;
    int status = amqp_socket_open(socket, "localhost", 5672);
    if (status) 
    {
        std::cerr << "Failed to open socket" << std::endl;
        return 1;
    }

    amqp_rpc_reply_t reply = amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) 
    {
        std::cerr << "Failed to login" << std::endl;
        return 1;
    }

    amqp_channel_open(conn, 1);
    amqp_bytes_t queueName = amqp_cstring_bytes("opencv_frames");
    amqp_queue_declare_ok_t *queue = amqp_queue_declare(conn, 1, queueName, 0, 0, 0, 1, amqp_empty_table);

    // Open camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) 
    {
        std::cerr << "Failed to open camera" << std::endl;
        return 1;
    }

    cv::Mat frame;
    while (true) 
    {
        cap >> frame;
        if (frame.empty()) 
        {
            std::cerr << "Failed to capture frame" << std::endl;
            break;
        }

        // Encode frame as JPEG
        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer);

        // Publish frame to RabbitMQ
        amqp_basic_properties_t props;
        props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG;
        props.content_type = amqp_cstring_bytes("image/jpeg");

        amqp_bytes_t messageBody = {(char*)buffer.data(), buffer.size()};
        amqp_basic_publish(conn, 1, queueName, 0, 0, &props, messageBody);
    }

    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);

    return 0;
}
