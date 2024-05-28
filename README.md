# Camera-Stream-RabbitMQ-Publisher-Subscriber

# VideoPublishProc.cpp
This C++ code is designed to capture frames from a camera and publish them to a RabbitMQ message queue as JPEG images. Here's a breakdown of what the code does: 
1. The necessary headers are included: `iostream` for standard input/output, `opencv2/opencv.hpp` for OpenCV functionality, and `rabbitmq-c/amqp.h` and `rabbitmq-c/tcp_socket.h` for RabbitMQ client library.
2. In the `main` function, a connection to RabbitMQ is established by creating a new connection state (`amqp_new_connection()`), opening a socket to the RabbitMQ server running on `localhost` at port `5672`, and authenticating with the default guest user credentials.
3. A new channel is opened on the RabbitMQ connection, and a queue named `"opencv_frames"` is declared using `amqp_queue_declare`.
4. An OpenCV `VideoCapture` object is created to access the default camera (device index 0).
5. The main loop begins, where frames are continuously captured from the camera using `cap >> frame`.
6. If a frame is successfully captured, it is encoded as a JPEG image using `cv::imencode`.
7. The encoded JPEG data is published to the `"opencv_frames"` queue on RabbitMQ using `amqp_basic_publish`. The message properties are set to indicate that the content type is `"image/jpeg"`.
8. After the loop exits (e.g., if the camera fails to capture a frame), the RabbitMQ channel and connection are closed, and resources are cleaned up. In summary, this code sets up a pipeline to continuously capture frames from a camera, encode them as JPEG images, and publish them to a RabbitMQ message queue named `"opencv_frames"`.
9. This could be useful in scenarios where you need to stream video data from a camera to other applications or services in a distributed system.
    
- Compilation command
- g++ -std=c++11 -o VideoPublisProc pkg-config --cflags --libs opencv4 -lrabbitmq VideoPublishProc.cpp

# VideoSubscribeProc.cpp
  
- Compilation command
- g++ -std=c++11 -o VideoSubscribeProc pkg-config --cflags --libs opencv4 -lrabbitmq VideoSubscribeProc.cpp


# Publisher&Subscriber.cpp

This C++ code demonstrates a simple publisher-subscriber system using RabbitMQ for transmitting video frames captured from a webcam. Here's a breakdown of the code: 
1. The code includes necessary headers for OpenCV (for video capture and processing), RabbitMQ-C (for RabbitMQ communication), and C++ standard libraries.
2. The `publish_frames` function is responsible for capturing frames from the default webcam (0), encoding them as JPEG images, and publishing them to a RabbitMQ queue named "frames". It establishes a persistent connection with RabbitMQ and publishes each frame as a message to the queue.
3. The `consume_frames` function is responsible for consuming messages from the "frames" queue, decoding the JPEG image data, and displaying the received frames using OpenCV's `imshow` function.
4. In the `main` function, a new RabbitMQ connection is created, and a queue named "frames" is declared.
5. Two threads are created: one for the publisher (`publish_frames`) and one for the subscriber (`consume_frames`). Both threads share the same RabbitMQ connection and queue name.
6. The main thread waits for both publisher and subscriber threads to complete using `join()`.
7. The code also includes a compilation command for compiling the C++ code with OpenCV and RabbitMQ-C libraries. In summary, this code demonstrates how to use RabbitMQ as a message broker to transmit video frames from a publisher (webcam) to a subscriber (display window). The publisher captures frames, encodes them as JPEG images, and publishes them to a RabbitMQ queue. The subscriber consumes messages from the same queue, decodes the JPEG data, and displays the received frames in a window.

- Compilation command
- g++ -std=c++11 -o rabbitmq_opencv `pkg-config --cflags --libs opencv4` -lrabbitmq Publisher&Subscriber.cpp
