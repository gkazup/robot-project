from setuptools import setup

package_name = 'morgi_lcd'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    author='Gabor Kazup',
    author_email='gkazup@gmail.com',
    maintainer='Gabor Kazup',
    maintainer_email='gkazup@gmail.com',
    keywords=['ROS2'],
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python',
        'Topic :: Software Development',
    ],
    description='Morgi robot external LCD functions using rclpy.',
    license='Apache License, Version 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'lcd_publisher = morgi_lcd.lcd_publisher:main',
            'lcd_subscriber = morgi_lcd.lcd_subscriber:main',
        ],
    },
)
