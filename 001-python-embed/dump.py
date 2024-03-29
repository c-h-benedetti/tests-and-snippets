import os
import csv
import spam
from thing import theFunction
import custom
from geo2d import Point2D

spam.system("ls -a")
k = 5
print(f"Factorial of {k} is {spam.factorial(k)}.")

theFunction()

class CSVtable(object):

    def __init__(self, ttls, d="-"):
        self.titles  = [str(t) for t in ttls]
        self.lines   = []
        self.default = d
    
    def getTitles(self):
        return self.titles

    def newRow(self):
        self.lines.append([self.default for i in range(len(self.titles))])

    def cancelRow(self):
        self.lines.pop()
    
    def _nameToIndex(self, name):
        n = str(name)
        for i, c in enumerate(self.titles):
            if c == n:
                return i

        return -1

    def setValue(self, col, val):
        idx = self._nameToIndex(col)
        if idx < 0:
            return
        self.lines[-1][idx] = str(val)
    
    def exportTo(self, dirPath, name):
        if not os.path.isdir(dirPath):
            return
        
        fullPath = os.path.join(dirPath, name)

        with open(fullPath, 'w') as csvfile:
            writer = csv.writer(csvfile, delimiter=',')
            writer.writerow(self.titles)
            for row in self.lines:
                writer.writerow(row)

    def display(self):
        maxlen = max([len(c) for c in self.titles])
        buffer = "|"
        for title in self.titles:
            newTitle = title.ljust(maxlen)
            buffer += (newTitle + "|")
        lineSize = len(buffer)
        buffer += "\n"
        sep = "".join(['-' for i in range(lineSize)])
        buffer += (sep + "\n")
        newLine = ""
        for line in self.lines:
            newLine = "|"
            for item in line:
                newLine += (str(item).ljust(maxlen) + "|")
            buffer += (newLine + "\n")
        print(buffer)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

rows  = ['one', 'two', 'three', 'four', 'five']
table = CSVtable(rows)
itr   = 1

for i in range(5):
    table.newRow()
    for j, e in enumerate(rows):
        table.setValue(e, itr*i*j)
    itr += 1

table.display()

print(spam.replace_range("abcdefghijklmnopqrstuvwxyz"))
print(spam.replace_range("abcdefghijklmnopqrstuvwxyz", '_'))
print(spam.replace_range("abcdefghijklmnopqrstuvwxyz", '_', 5))
print(spam.replace_range("abcdefghijklmnopqrstuvwxyz", '_', 5, 12))

spam.super_print("some stuff and yet", fg=1, bg=2)

print(spam.total_length("a", "bc", "def", "ghij"))

print('------------------------------------')

c1 = custom.Custom()
c2 = custom.Custom("some")
c3 = custom.Custom("some", "content", -42)

c1.number = 10
c1.first_name = 48

print(f"Name: {c1.name()}, -> {c1.number}")
print(f"Name: {c2.name()}, -> {c2.number}")
print(f"Name: {c3.name()}, -> {c3.number}")

print('------------------------------------')

p1 = Point2D()
p2 = Point2D((2.1, 5.2))
p3 = Point2D((3.1, 5.4), '#ab77feff', None)
p4 = Point2D((3.1, 5.4), '#ab77feff', "1.0.5")

print(p1)
print(p2)
print(p3)
print(p4)

k = 42
try:
    p1.partition = k
except:
    print(f"Failed to set partition to {k}.")
else:
    print(f"Succesfully set partition to {k}.")