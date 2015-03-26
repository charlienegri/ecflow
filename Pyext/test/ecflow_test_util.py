#////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
# Name        :
# Author      : Avi
# Revision    : $Revision: #10 $
#
# Copyright 2009-2014 ECMWF.
# This software is licensed under the terms of the Apache Licence version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#
# Utility code used in the tests.
#
#////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
from socket import gethostname 
import os
import fcntl
import shutil   # used to remove directory tree

from ecflow import Client, debug_build, File

# Enable to stop data being deleted, and stop server from being terminated
def debugging() : return False

def ecf_home(port): 
    # debug_build() is defined for ecflow. Used in test to distinguish debug/release ecflow
    # Vary ECF_HOME based on debug/release/port allowing multiple invocations of these tests
    if debug_build():
        return os.getcwd() + "/test/data/ecf_home_debug_" + str(port)
    return os.getcwd() + "/test/data/ecf_home_release_" + str(port)

def get_parent_dir(file_path):
    return os.path.dirname(file_path)

def get_root_source_dir():
    cwd = os.getcwd()
    #print "get_root_source_dir from: " + cwd
    while (1):
        # Get to directory that has ecflow
        head, tail = os.path.split(cwd)
        #print "   head:" + head
        #print "   tail:" + tail
        if tail.find("ecflow") != -1 :
            
            # bjam, already at the source directory
            if os.path.exists(cwd + "/VERSION.cmake"): 
                print "   Found VERSION.cmake in " + cwd
                return cwd
        
        if tail != "Pyext" and tail != "migrate":
            # in cmake, we may be in the build directory, hence we need to determine source directory
            file = cwd + "/CTestTestfile.cmake"
            #print "   searching for " + file
            if os.path.exists(file):
                # determine path by looking into this file:
                for line in open(file):
                    ## Source directory: /tmp/ma0/clientRoot/workspace/working-directory/ecflow/Acore
                    if line.find("Source directory"):
                        tokens = line.split()
                        if len(tokens) == 4:
                            #print "   returning root_source_dir:", tokens[3]
                            return tokens[3]
                raise RuntimeError("ERROR could not find Source directory in CTestTestfile.cmake")
            else:
                raise RuntimeError("ERROR could not find file CTestTestfile.cmake in " + cwd)
                
        cwd = head
    return cwd


def log_file_path(port): return "./" + gethostname() + "." + port + ".ecf.log"
def checkpt_file_path(port): return "./" + gethostname() + "." + port + ".ecf.check"
def backup_checkpt_file_path(port): return "./" + gethostname() + "." + port + ".ecf.check.b"
def white_list_file_path(port): return "./" + gethostname() + "." + port + ".ecf.lists"

def clean_up_server(port):
    print "   clean_up " + port
    try: os.remove(log_file_path(port))
    except: pass
    try: os.remove(checkpt_file_path(port))
    except: pass
    try: os.remove(backup_checkpt_file_path(port))
    except: pass
    try: os.remove(white_list_file_path(port))  
    except: pass
    
def clean_up_data(port):
    print "   Attempting to Removing ECF_HOME " + ecf_home(port)
    try: 
        shutil.rmtree(ecf_home(port),True)   # True means ignore errors 
        print "   Remove OK" 
    except: 
        print "   Remove Failed" 
        pass
        
# =======================================================================================
class EcfPortLock(object):
    """allow debug and release version of python tests to run at the same
    time, buy generating a unique port each time"""
    def __init__(self):
        print "   EcfPortLock:__init__"
        pass
    
    def find_free_port(self,seed_port):
        print "   EcfPortLock:find_free_port starting with " + str(seed_port)
        port = seed_port
        while 1:
            if self._free_port(port) == True:
                print "   *FOUND* free server port " + str(port)
                if self._do_lock(port) == True:
                    break;
            else:
                 print "   *Server* port " + str(port) + " busy, trying next port"
            port = port + 1
            
        return str(port)  
    
    def _free_port(self,port):
        try:
            ci = Client()
            ci.set_host_port("localhost",str(port))
            ci.ping() 
            return False
        except RuntimeError, e:
            return True
            
    def _do_lock(self,port):
        file = self._lock_file(port)
        try:
            fp = open(file, 'w') 
            try:
                fcntl.lockf(fp, fcntl.LOCK_EX | fcntl.LOCK_NB)
                self.lock_file_fp = fp
                print "   *LOCKED* file " + file
                return True;
            except IOError:
                print "   Could *NOT* lock file " + file + " trying next port"
                return False
        except IOError, e:
             print "   Could not open file " + file + " for write trying next port"
             return False
        
    def remove(self,port):
        self.lock_file_fp.close()
        os.remove(self._lock_file(port))
    
    def _lock_file(self,port):
        lock_file = str(port) + ".lock"
        return lock_file
        
# ===============================================================================

class Server(object):
    """TestServer: allow debug and release version of python tests to run at the same
    time, by generating a unique port each time"""
    def __init__(self):
        print "Server:__init__: Starting server"      
        if not debugging():
            seed_port = 3153
            if debug_build(): seed_port = 3152
            self.lock_file = EcfPortLock()
            self.the_port = self.lock_file.find_free_port(seed_port)   
        else:
            self.the_port = "3152"
     
        # Only worth doing this test, if the server is running
        # ON HPUX, having only one connection attempt, sometimes fails
        #ci.set_connection_attempts(1)     # improve responsiveness only make 1 attempt to connect to server
        #ci.set_retry_connection_period(0) # Only applicable when make more than one attempt. Added to check api.
        self.ci = Client("localhost", self.the_port)
     
    def __enter__(self):
        try:
            print "Server:__enter__: About to ping localhost:" + self.the_port       
            self.ci.ping() 
            print "   ------- Server all ready running *UNEXPECTED* ------"
        except RuntimeError, e:
            print "   ------- Server not running as *EXPECTED* ------ " 
            print "   ------- Start the server on port " + self.the_port + " ---------"  
            clean_up_server(str(self.the_port))
            clean_up_data(str(self.the_port))
    
            server_exe = File.find_server();
            assert len(server_exe) != 0, "Could not locate the server executable"
        
            server_exe += " --port=" + self.the_port + " --ecfinterval=4 &"
            print "   TestClient.py: Starting server ", server_exe
            os.system(server_exe) 
        
            print "   Allow time for server to start"
            if self.ci.wait_for_server_reply() :
                print "   Server has started"
            else:
                print "   Server failed to start after 60 second !!!!!!"
                assert False , "Server failed to start after 60 second !!!!!!"
            
        print "   Run the tests, leaving Server:__enter__:" 

        # return the Client, that can call to the server
        return self.ci
    
    def __exit__(self,exctype,value,tb):
        print "   Server:__exit__: Kill the server, clean up log file, check pt files and lock files, ECF_HOME"
        print "   exctype:==================================================="
        print exctype
        print "   value:=====================================================" 
        print value
        print "   tb:========================================================"; 
        print tb
        print "   Terminate server ===================================================="
        self.ci.terminate_server()  
        print "   Terminate server OK ================================================="
        print "   Remove lock file"
        self.lock_file.remove(self.the_port)
        clean_up_server(str(self.the_port))
        
        # Do not clean up data, if an assert was raised. This allow debug
        if exctype == None:
            clean_up_data(str(self.the_port))
        return False
        
  