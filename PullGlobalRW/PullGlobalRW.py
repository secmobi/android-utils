#!/usr/bin/env python

import subprocess

def runCmd(cmd):
    return subprocess.check_output([cmd], shell=True)

def PullGlobalRW(root = "/data/data", outdir = "output"):
    log = open('log.txt', 'w')

    dirs = [root]
    files = []
    
    while(len(dirs) > 0):
        curdir = dirs.pop(0)

        for l in runCmd("adb shell ls -l " + curdir).split('\r\n'):
            l = l.split(' ')
            if len(l) < 2:
                continue
            attr, name = l[0], l[-1]

            if(attr[0] == 'd'):
                if(attr[-1] != 'x'): continue
                dirs.append(curdir + '/' + name)

            elif(attr[0] == '-'):
                if ((attr[7] == 'r') or (attr[8] == 'w')):
                    name = curdir + '/' + name
                    print "Pulling: " + name
                    runCmd("adb pull " + name + " " + outdir + name)
                    log.write(attr + '   ' + name + '\r\n')

            else:
                continue

    log.close()


if __name__ == '__main__':
    PullGlobalRW()
