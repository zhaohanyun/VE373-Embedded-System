import serial
import struct

serial_port = serial.Serial(
	port="/dev/ttyTHS0",  # 串口设备地址
	baudrate=115200,  # 信号波特率
	bytesize=serial.EIGHTBITS,  # 数据位
	parity=serial.PARITY_NONE,  # 是否启用奇偶校验
	stopbits=serial.STOPBITS_ONE,  # 停止位
)
dist=1.1
theta=100
distserial = struct.pack('f', dist)
thetaserial = struct.pack('f', theta)
serial_port.write('\r\n'.encode('utf-8'))
serial_port.write(distserial)
serial_port.write(thetaserial)
serial_port.write('\n\r'.encode('utf-8'))
print('end')
data=''
while True :
	if serial_port.inWaiting() > 0 :
		data += serial_port.read().decode('utf-8')
		print(data)
		if data == 'sssss' :
			print(data)
			break
		if len(data) > 5 :
			exit(3)
