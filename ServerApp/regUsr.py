#!/usr/bin/python
"""App serves registering users to TIN Remote Access Manager"""

import sys, getpass, hashlib, csv



if __name__ == "__main__":
    print 'Welcome to regUsr.py - user registration tool for TIN Remote Access Manager'
    print 'Do you want to add new user? [Y/N]'
    response = raw_input('-->')
    while(response != 'N'):
        if(response == 'Y'):
            print("login:")
            login = raw_input('-->')
            print("password:")
            password = getpass.getpass('-->')
            salt = "22"
            
            with open('./src/config/clients', 'a') as clientsCSVFile:
                fieldnames = ['login', 'passHash', 'salt']
                writer = csv.DictWriter(clientsCSVFile, fieldnames=fieldnames, lineterminator='\n')
                writer.writerow({'login' : login, 'passHash' : hashlib.sha512(password).hexdigest(), 'salt' : salt})
        
            print('User registered!')
        print('Do you want to add new user? [Y/N]')
        response = raw_input('-->')
        