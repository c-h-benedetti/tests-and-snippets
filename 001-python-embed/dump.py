import os
import sys
import csv

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
# table.exportTo("/home/clement/Desktop", "table.csv")
