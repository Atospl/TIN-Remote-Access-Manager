'''
SimpleSecureHTTPServer.py - simple HTTP server supporting SSL.

- replace fpem with the location of your .pem server file.
- the default port is 443.

usage: python SimpleSecureHTTPServer.py
'''
import socket, os
from SocketServer import ThreadingMixIn
from SocketServer import BaseServer
from BaseHTTPServer import HTTPServer
from SimpleHTTPServer import SimpleHTTPRequestHandler
from OpenSSL import SSL

CERT = 'server.pem'
KEY = 'key.pem'



class SecureHTTPServer(HTTPServer):
    def __init__(self, server_address, HandlerClass):
        BaseServer.__init__(self, server_address, HandlerClass)
        ctx = SSL.Context(SSL.SSLv23_METHOD)
        #server.pem's location (containing the server private key and
        #the server certificate).
        fpem = '/path/server.pem'
        ctx.use_privatekey_file (KEY)
        ctx.use_certificate_file(CERT)
        self.socket = SSL.Connection(ctx, socket.socket(self.address_family,
                                                        self.socket_type))
        self.server_bind()
        self.server_activate()
        
    def shutdown_request(self,request): request.shutdown()

class SecureHTTPRequestHandler(SimpleHTTPRequestHandler):
    def setup(self):
        self.connection = self.request
        self.rfile = socket._fileobject(self.request, "rb", self.rbufsize)
        self.wfile = socket._fileobject(self.request, "wb", self.wbufsize)

class HTTPSServerMT(ThreadingMixIn, SecureHTTPServer):
    """Handle requests in separate thread"""

def test():
    server = HTTPSServerMT(('localhost', 4443), SecureHTTPRequestHandler)
    server.serve_forever()


if __name__ == '__main__':
    test()