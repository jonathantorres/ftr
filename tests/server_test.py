import ftplib
import os
import signal
import subprocess
import time
import unittest

def compile_binary():
    print("compiling binary...")
    os.chdir("build")
    subprocess.run(["cmake", "--build", ".", "--target", "ftr"])

def start_server():
    print("starting server process...")
    try:
        subprocess.run(["./ftr/ftr", "-d"])
    except:
        print("ERROR! There was a problem starting the server process")

def get_server_pid():
    f = open("../ftr.pid", 'r')
    server_pid = int(f.readline())
    f.close()
    return server_pid

def login_to_server():
    ftp = ftplib.FTP()
    ftp.connect("localhost", 9090, 10)
    ftp.login("jt", "test")
    ftp.cwd("/")
    return ftp

class FtrTests(unittest.TestCase):
    def setUp(self):
        compile_binary()
        start_server()
        time.sleep(1)

    def tearDown(self):
        server_pid = get_server_pid()
        print("terminating server, pid: ", str(server_pid))

        try:
            subprocess.run(["kill", "-s", "SIGTERM", str(server_pid)])
        except:
            print("ERROR! There was a problem terminating the server process")

    def test_simple_login(self):
        ftp = login_to_server()
        self.assertEqual(ftp.getwelcome(), "220 Service Ready. ")
        ftp.dir("/")
        ftp.quit()

if __name__ == '__main__':
    unittest.main()

