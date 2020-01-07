.. ecFlow documentation master file, created by
   sphinx-quickstart on Mon Feb 28 11:23:56 2011.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

=============
ecFlow Python
=============

License
-------

 Copyright 2009-2020 ECMWF.
 This software is licensed under the terms of the Apache Licence version 2.0 
 which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 In applying this licence, ECMWF does not waive the privileges and immunities 
 granted to it by virtue of its status as an intergovernmental organisation 
 nor does it submit to any jurisdiction. 

Introduction
------------

   ecFlow is a work flow package that enables users to run a large
   number of programs (with dependencies on each other and on time)
   in a controlled environment. It provides reasonable tolerance for
   hardware and software failures, combined with good restart capabilities.

   ecFlow submits tasks (jobs) and receives acknowledgements from tasks
   when they change status and when they send events, using child commands
   embedded in the scripts. Ecflow stores the relationship between tasks, and is
   able to submit tasks dependent on triggers.  

Contents
--------

* :ref:`python_api`

Reference
---------

.. toctree::
   :maxdepth: 2
   :titlesonly:
     
   python_api/python_api

Indices and tables
------------------

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

