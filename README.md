# Camera-Stream-RabbitMQ-Publisher-Subscriber

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
