# search for libraray like bla.cpython-312-darwin.so in the build directory:
# import sys
# sys.path.append('/home/peter/uni/scicomp/bla-group/build')
# from bla import Vector

# import from the installed pepbla package:
from pepbla.bla import Vector

x = Vector(3)
y = Vector(3)

for i in range(len(x)):
    x[i] = i
y[:] = 2

print ("x =", x)
print ("y =", y)
print ("x+3*y =", x+3*y)


x = Vector(10)
x[0:] = 1
print (x)

x[3:7] = 2
print (x)

x[0:10:2] = 3
print (x)
