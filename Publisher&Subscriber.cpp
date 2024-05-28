// Publisher
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <rabbitmq-c/amqp.h>

void publish_frames(amqp_connection_state_t conn, amqp_bytes_t queuename) 
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) 
    {
        std::cerr << "Failed to open camera" << std::endl;
        return;
    }

    amqp_channel_open(conn, 1);
    amqp_basic_properties_t props;
    props._flags = AMQP_BASIC_DELIVERY_MODE_FLAG;
    props.delivery_mode = 2; // persistent message

    cv::Mat frame;
    while (true) 
    {
        cap >> frame;
        if (frame.empty()) 
        {
            std::cerr << "Failed to capture frame" << std::endl;
            break;
        }

        std::vector<uchar> buf;
        cv::imencode(".jpg", frame, buf);

        amqp_bytes_t message = {(void*)buf.data(), buf.size()};
        amqp_basic_publish(conn, 1, queuename, 0, 0, &props, message);
    }

    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
}

// Subscriber
void consume_frames(amqp_connection_state_t conn, amqp_bytes_t queuename) 
{
    amqp_channel_open(conn, 1);
    amqp_basic_consume(conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);

    amqp_frame_t frame;
    int status;
    while ((status = amqp_simple_wait_frame(conn, &frame)) == AMQP_RESPONSE_NORMAL) 
    {
        if (frame.frame_type == AMQP_FRAME_METHOD) 
        {
            switch (frame.payload.method.id) 
            {
                case AMQP_BASIC_DELIVER_METHOD: 
                  {
                    amqp_basic_deliver_t *delivery = (amqp_basic_deliver_t*)frame.payload.method.decoded;
                    amqp_bytes_t body = amqp_read_message(conn);

                    std::vector<uchar> buf(body.bytes, body.bytes + body.len);
                    cv::Mat img = cv::imdecode(buf, cv::IMREAD_COLOR);
                    cv::imshow("Received Frame", img);
                    cv::waitKey(1);

                    amqp_basic_ack(conn, 1, delivery->delivery_tag, 0);
                    break;
                }
            }
        }
    }

    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
}

int main() 
{
    amqp_connection_state_t conn = amqp_new_connection();
    amqp_socket_t *socket = amqp_tcp_socket_new(conn);
    if (!socket) 
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    amqp_bytes_t queuename = amqp_cstring_bytes("frames");

    std::thread publisher([&]() 
    {
        publish_frames(conn, queuename);
    });

    std::thread subscriber([&]() {
        consume_frames(conn, queuename);
    });

    publisher.join();
    subscriber.join();

    return 0;
}
