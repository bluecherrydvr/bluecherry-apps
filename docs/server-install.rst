*********************************
Installation of Bluecherry server
*********************************

Server Installation

======================================

Requirements
------------

Version 3 (beta) supports Ubuntu LTS releases (Ubuntu 16.04 and Ubuntu 18.04), Debian 9 (Stretch) and Debian 10 (Buster).  Support for CentOS has been removed.

Installing Bluecherry from the Bluecherry repository
----------------------------------------------------
Download instructions

Currently version 3 is in beta which means 'unstable'.  

Copy and paste the commands to add our repository to Ubuntu 16.04 / Ubuntu 18.04 / Ubuntu 20.04 / Debian 9 / Debian 10.

sudo bash -c "$(wget -O - https://dl.bluecherrydvr.com/scripts/install-unstable)"::


Installing the server
---------------------
.. image:: img/term-mysql-setup.png

The instructions below assume you have received the download instructions, or installed from our CD installer.

.. image:: img/term-bluecherry-setup.png

If you do not already have mysql-server installed you will be prompted to create a password for the ‘root’ MySQL account. While Ubuntu lists this as optional, it is recommended that you set a password.

.. image:: img/term-bluecherry-2.png

You will now be asked to configure Bluecherry. You must hit Yes, or the installation process will fail.

.. image:: img/term-bluecherry-3.png

Enter the database administrative password that you set previously for the ‘root’ user.

.. image:: img/term-bluecherry-4.png

Enter the password that you want to create for the bluecherry database. This can be the same password that you provided for the root MySQL account, but we recommend keeping the password unique. You will be asked to enter this password twice to verify the password matched.

.. image:: img/term-bluecherry-5.png

At this point the bluecherry server will be installed. If you plan to install the client on the same system, then please follow the client installation guide.
