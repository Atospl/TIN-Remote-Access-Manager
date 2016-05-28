#!/usr/bin/python
'''
SimpleSecureHTTPServer.py - simple HTTP server supporting SSL.
usage: python SimpleSecureHTTPServer.py
'''
import socket, os
import csv
import time
import hashlib
import base64
import Cookie
import uuid
from threading import Lock
from SocketServer import ThreadingMixIn
from SocketServer import BaseServer
from BaseHTTPServer import HTTPServer
from SimpleHTTPServer import SimpleHTTPRequestHandler
from OpenSSL import SSL
import urlparse

CERT = 'server.pem'
KEY = 'key.pem'

class UserSessionStore():
    lock = Lock()
    userSessions = {}
    
    def __init__(self):
        self.importUsers()
    
    def importUsers(self):
        with open('../src/config/clients', 'rb') as clientFile:
            clientReader = csv.DictReader(clientFile)
            for row in clientReader:
                self.userSessions[row['login']] = {'passHash' : row['passHash']}
                
    def genSessionID(self, user):
        """generate unique session id with timeout for an hour"""
        sessionUID = uuid.uuid1()
        self.lock.acquire()
        self.userSessions[user]['sessionID'] = sessionUID
        self.userSessions[user]['timeout'] = time.time() + 3600
        self.lock.release()
        return sessionUID
    
    def checkSessionID(self, user, userSID):
        self.lock.acquire()
        if self.userSessions[user]['sessionID'] == userSID:
            self.lock.release()
            return True
        else:
            self.lock.release()
            return False
        
store = UserSessionStore()
    


class SecureHTTPServer(HTTPServer):
    def __init__(self, server_address, HandlerClass):
        BaseServer.__init__(self, server_address, HandlerClass)
        ctx = SSL.Context(SSL.SSLv23_METHOD)
        ctx.use_privatekey_file (KEY)
        ctx.use_certificate_file(CERT)
        self.socket = SSL.Connection(ctx, socket.socket(self.address_family,
                                                        self.socket_type))
        self.server_bind()
        self.server_activate()
        
    def shutdown_request(self,request): 
        request.shutdown()

        

class SecureHTTPRequestHandler(SimpleHTTPRequestHandler):
    def setup(self):
        self.connection = self.request
        self.rfile = socket._fileobject(self.request, "rb", self.rbufsize)
        self.wfile = socket._fileobject(self.request, "wb", self.wbufsize)
    
    def do_POST(self):
        """Processes POST requests"""
        print(self.headers)
#        self.send_response(200)
        content_len = int(self.headers.getheader('content-length', 0))
        post_data = urlparse.parse_qs(self.rfile.read(content_len).decode('utf-8'))
        for (key, value) in post_data.iteritems():
            if key == "login":
                login = value
            elif key == "password":
                password = value
        self.signIn(login, password)


    def signIn(self, login, password):
        """Check if user is registered and send appropriate response"""
        if store.userSessions[login[0]]['passHash'] == hashlib.sha512(password[0]).hexdigest():
            sessionId = store.genSessionID(login[0])
            cookie = Cookie.SimpleCookie()
            cookie["session-id"] = sessionId
            self.send_response(200, 'OK')
            self.wfile.write(cookie.output())
            self.end_headers()
            return True
        else:
            self.send_response(401, "Invalid login/password")
            self.end_headers()                
            return False

        
class HTTPSServerMT(ThreadingMixIn, SecureHTTPServer):
    """Handle requests in separate thread"""

def test():
    server = HTTPSServerMT(('localhost', 4443), SecureHTTPRequestHandler)
    server.serve_forever()


if __name__ == '__main__':
    test()
#    store = UserSessionStore()
#    print store.userSessions.items()
#    store.genSessionID('22')
#    print store.userSessions.items()
