#Program for obstacles mapping

import serial
import turtle
import time

previus_car_msg = 0
previus_degrees = 0
degrees = 0
i = 0

car_str = 'p'

#turns the car
def turn(current_degrees):	
	global previus_degrees
	
	degrees = 0
	current_degrees = int(current_degrees)
	if (current_degrees == 0 and previus_degrees == 270):
		degrees = 90
	elif (current_degrees == 270 and previus_degrees == 0):
		degrees = -90
	elif current_degrees > previus_degrees:
		degrees = 90
	elif current_degrees < previus_degrees:
		degrees = -90
	print(degrees)
	if degrees==0:
		time.sleep(0.001)
		s2 = "straight"
		print(s2)
	else:
		car.right(degrees)
		car.forward(car_width)	#automato forward oso einai to paxos tou autokinhtou
		s3 = "turn"
		print(s3)

	print(previus_degrees , current_degrees , degrees)
	previus_degrees = current_degrees
	
#moves the car
def fw(car_msg):
	global previus_car_msg
	global i
	car_msg.remove('p')
	car_msg.remove('f')
	car_msg.remove(';')
	car_msg.remove('w')
	car_str1 = ''.join(car_msg)
	cur = int(car_str1)
	move = cur - previus_car_msg
	
	if move==0:
		i=i+1
		if i==3:
			obstacle(cur)
			previus_car_msg = 0
	elif move<0:
		time.sleep(0.001)
	else:
		car.forward(move)
		i=0
	print(previus_car_msg , cur)
	previus_car_msg = cur
	

#clears the space until the obstacle and returns to the previous point
def obstacle(cm):
	car.speed(8)
	car.forward(cm) 
	car.backward(cm) 
	car.speed(1)
	
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
#car.left(-90)#first position

while True: 
	try:
		time.sleep(0.4)
		line = ser.readline()
		car_msg = line.decode('utf-8')
		car_msg = list(car_msg)
		car_msg[0] = car_msg[0].replace('\x00','p');
		print(type(car_msg))
		
		car_msg.remove('\r')
		car_msg.remove('\n')

		print(car_msg)
		
		if car_msg[1]=='f':
			if previus_car_msg > 0:
				fw(car_msg)
			else:
				car_msg.remove('p')
				car_msg.remove('f')
				car_msg.remove(';')
				car_msg.remove('w')
				car_str = ''.join(car_msg)
				previus_car_msg = int(car_str)
				print(car_msg)
		elif  (car_msg[1] == 't'):
				car_msg.remove('p')
				car_msg.remove('t')
				car_msg.remove('u')
				car_msg.remove('r')
				car_msg.remove('n')
				car_msg.remove('_')
				car_str2 = ''.join(car_msg)
				turn(car_str2)
		else:
			str = "it was nothing.\n"
			print(str)
			
		
	except KeyboardInterrupt:
		ser.close()
		break