#!/usr/bin/python
"""App serves registering users to TIN Remote Access Manager"""

import sys, getpass, hashlib, csv
import string, random


if __name__ == "__main__":
    print 'Welcome to regUsr.py - user registration tool for TIN Remote Access Manager'
    print 'Do you want to add new user? [Y/N]'
    # salt size
    N = 10
    response = raw_input('-->')
    while(response != 'N'):
        if(response == 'Y'):
            print("login:")
            login = raw_input('-->')
            print("password:")
            password = getpass.getpass('-->')
            salt = ''.join(random.SystemRandom().choice(string.ascii_letters + string.digits) for _ in range(N))
            
            with open('./src/config/clients', 'a') as clientsCSVFile:
                fieldnames = ['login', 'passHash', 'salt']
                writer = csv.DictWriter(clientsCSVFile, fieldnames=fieldnames, lineterminator='\n')
                writer.writerow({'login' : login, 'passHash' : hashlib.sha512(password).hexdigest(), 'salt' : salt})
        
            print('User registered!')
        print('Do you want to add new user? [Y/N]')
        response = raw_input('-->')
        
