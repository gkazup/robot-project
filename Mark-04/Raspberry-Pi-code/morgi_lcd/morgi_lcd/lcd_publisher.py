# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import rclpy
from rclpy.node import Node

from std_msgs.msg import String

import sys
import os
import socket


class LCDPublisher(Node):

    def __init__(self):
        super().__init__('lcd_publisher')
        self.publisher_ = self.create_publisher(String, 'lcd_data', 10)
        timer_period = 5  # seconds
        self.timer = self.create_timer(timer_period, self.timer_callback)
        self.i = 0

    def timer_callback(self):
        msg = String()

        msg.data = 'cls'
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg.data)

        msg.data = 'host: %s' % self.get_hostname()
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg.data)

        msg.data = 'ssid: %s' % self.get_ssid()
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg.data)

        msg.data = '%s' % self.get_ipaddress()
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg.data)

        msg.data = 'loop: %d' % self.i
        self.publisher_.publish(msg)
        self.get_logger().info('Publishing: "%s"' % msg.data)

        self.i += 1

    def get_hostname(self):
        hostname = socket.gethostname()
        return hostname

    def get_ipaddress(self):
        ipaddress = os.popen("ifconfig wlan0 \
                     | grep 'inet ' \
                     | awk '{print $2}'").read()
        ipaddress = ipaddress.rstrip("\n\r")
        return ipaddress

    def get_ssid(self):
        ssid = os.popen("iwconfig wlan0 \
                    | grep 'ESSID' \
                    | awk '{print $4}' \
                    | awk -F\\\" '{print $2}'").read()
        ssid = ssid.rstrip("\n\r")
        return ssid


def main(args=None):
    rclpy.init(args=args)

    my_publisher = LCDPublisher()

    rclpy.spin(my_publisher)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    my_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
