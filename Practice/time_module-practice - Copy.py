import datetime

now = datetime.datetime.now()
print(now)
now = now.strftime("%H:%M:%S | %d.%m.%y") #specify the location of each time unit
print(now)