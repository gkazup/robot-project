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

import smbus


class LCDSubscriber(Node):

    def __init__(self, address):
        super().__init__('lcd_subscriber')
        self.subscription = self.create_subscription(
            String,
            'lcd_data',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning
        # for RPI version 2, use bus No. 1 for I2C
        self.bus = smbus.SMBus(1)
        # This is the target address we setup in the Arduino Program
        self.address = address

    # callback function
    def listener_callback(self, msg):
        self.get_logger().info('Received: "%s"' % msg.data)
        self.writeWireString(msg.data)

    # send string to the i2c wire
    def writeWireString(self, value):
        byte_list = []
        count = len(value)
        if (count > 1):
            for index in range(1,(count)):
                byte_list.append(ord(value[index]))
        try:
            self.bus.write_i2c_block_data(self.address, ord(value[0]), byte_list)
        except IOError:
            self.get_logger().warning('IOError occured while sending')


def main(args=None):
    rclpy.init(args=args)

    my_subscriber = LCDSubscriber(0x08)

    rclpy.spin(my_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    my_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
