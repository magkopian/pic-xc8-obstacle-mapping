#Program for obstacles mapping

import serial
import turtle
import time

previus_car_msg = 0
previus_degrees = 0
degrees = 0
previus_obstacle = 0
default_cm_per_msg = 1
width_flag = 1

car_str = 'p'

#turns the car
def turn(current_degrees):	
	global previus_degrees
	global width_flag
	
	degrees = 0
	current_degrees = int(current_degrees)
	m = abs(current_degrees - previus_degrees)
	
	if (current_degrees == 0 and previus_degrees == 270):
		degrees = 90
	elif (current_degrees == 270 and previus_degrees == 0):
		degrees = -90
	elif current_degrees > previus_degrees:
		degrees = m
	elif current_degrees < previus_degrees:
		degrees = -m
	print(degrees)
	if degrees==0:
		#time.sleep(0.00001)
		s2 = "straight"
		print(s2)
	else:
		car.right(degrees)
		#car.forward(car_width)	#automato forward oso einai to paxos tou autokinhtou
		width_flag = 1
		s3 = "turn"
		print(s3)

	print(previus_degrees , current_degrees , degrees)
	previus_degrees = current_degrees
	
#moves the car
def fw(car_msg):
	global previus_car_msg
	global default_cm_per_msg
	
	car_msg.remove('p')
	car_msg.remove('f')
	car_msg.remove('w')
	car_msg.remove(';')
	car_str1 = ''.join(car_msg)
	cur = int(car_str1)
	move = cur - previus_car_msg
	
	if move==0:
		time.sleep(0.00001)
	elif move < 0:
		time.sleep(0.00001)
	elif cur > 105:
		car.forward(default_cm_per_msg)
	else:
		car.forward(move)
	#print(previus_car_msg , cur)
	previus_car_msg = cur
	

#clears the space until the obstacle and returns to the previous point
def obstacle(cm):
	global previus_obstacle
	
	if previus_obstacle == 0:
		car.speed(8)
		car.forward(cm) 
		car.backward(cm) 
		car.speed(1) 
		s2 = "obstacle"
		print(s2)
	else:
		s3 = "same"
		print(s3)
	previus_obstacle = 1
	
def width():
	global width_flag
	
	width_flag = 0
	car.forward(car_width)
	s5 = "forward width"
	print(s5)
	
	
	
	
	
ser = serial.Serial(port='COM10', baudrate=9600)
print('Connected to: ' + ser.portstr)

wn = turtle.Screen()
car = turtle.Turtle()

wn.title("Room mapping")
wn.bgcolor("gray")

car_width = 5
car.color("gray","gray")
car.setposition(-328,-270)
car.color("white","red")
car.pensize(car_width)
car.speed(1)
car.left(90)#first position

#print(bytes('S', encoding='ascii'))
time.sleep(0.4)
ser.write(bytes('S', encoding='ascii'))
#time.sleep(0.1)

	

previus_degrees = 0

while True: 
	try:
		
		line = ser.readline()
		car_msg = line.decode('utf-8')
		car_msg = list(car_msg)
		car_msg[0] = car_msg[0].replace('\x00','p');
		#print(type(car_msg))
		
		car_msg.remove('\r')
		car_msg.remove('\n')

		#print(car_msg)
		
		if (car_msg[1]=='f'):
			previus_obstacle = 0
			if previus_car_msg > 0:
				fw(car_msg)
			else:
				car_msg.remove('p')
				car_msg.remove('f')
				car_msg.remove('w')
				car_msg.remove(';')
				car_str = ''.join(car_msg)
				previus_car_msg = int(car_str)
				#print(car_msg)
		elif (car_msg[2] == 'b'):
			car_msg.remove('p')
			car_msg.remove('o')
			car_msg.remove('b')
			car_msg.remove(';')
			car_str3 = ''.join(car_msg)
			mov = int(car_str3)
			obstacle(mov)
		elif (car_msg[1] == 't'):
			previus_obstacle = 0
			car_msg.remove('p')
			car_msg.remove('t')
			car_msg.remove('u')
			car_msg.remove('r')
			car_msg.remove('n')
			car_msg.remove('_')
			car_str2 = ''.join(car_msg)
			turn(car_str2)
		elif (car_msg[1] == 'w'):
			#car_msg.remove('p')
			#car_msg.remove('w')
			#car_msg.remove('i')
			#car_msg.remove('d')
			#car_msg.remove('t')
			#car_msg.remove('h')
			if (width_flag == 1):
				width()
				print(car_msg)
		else:
			str = "it was nothing.\n"
			print(str)
			
		#time.sleep(0.4)
	except KeyboardInterrupt:
		ser.close()
		break
car.mainloop()