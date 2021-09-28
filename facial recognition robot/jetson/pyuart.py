import time
import serial
import struct

def uart_init():
# initialize, wait for 'start'. if received, start main function
#     serial_port = serial.Serial(
#         port="/dev/ttyTHS0",                                      #串口设备地址
#         baudrate=115200,                                          #信号波特率
#         bytesize=serial.EIGHTBITS,                                #数据位
#         parity=serial.PARITY_NONE,  						    #是否启用奇偶校验
#         stopbits=serial.STOPBITS_ONE,                             #停止位
#     )# moved to main as global
    time.sleep(0.1)
    # txdata='Hello. This is Jetson'
    # for i in txdata :
    #     serial_port.write(i.encode('utf-8'))
    #     time.sleep(0.001)
    data=''
    global serial_port
    while True:
        if serial_port.inWaiting() > 0:
            data += serial_port.read().decode('utf-8')
            if data == 'start':
                print(data)
                break
            if len(data)>5:
                exit(3)



if __name__ == '__main__':
    uart_init()
    # wait until 'start' has been received

