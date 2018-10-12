
SD6WSN: A SDWSN framework for 6LoWPAN using Contiki and Node.js controller
==============================================================================


To install this test-bed: 
-------------------------
Download and install the Instant-Contiki 3.0 virtual machine following the instructions of http://www.contiki-os.org , on "getting start" folder;

Update the Contiki to the last release: 

	$ cd ~/
	$ mv contiki contiki-ori
	$ git clone --recursive https://github.com/contiki-os/contiki.git

Install libcoap:

	$ cd ~/
	$ wget https://github.com/obgm/libcoap/archive/master.zip
	$ unzip master.zip
	$ mv libcoap-master/ libcoap
	$ sudo apt-get install autogen
	$ cd libcoap
	$ ./autogen.sh
   	$ sudo touch /usr/bin/a2x
   	$ sudo chmod +x /usr/bin/a2x
	$ ./configure
	$ make

Install some prerequisites:

	$ cd ~/
	$ sudo apt-get update
	$ sudo apt-get install wireshark-qt
	$ sudo apt-get install eclipse-cdt
	$ sudo apt-get install curl
	$ curl -sL https://deb.nodesource.com/setup_7.x | sudo -E bash -
	$ sudo apt-get install nodejs
	$ npm install commander --save
	$ npm install mongodb --save
	$ sudo npm install coap-cli -g
	$ npm install coap --save
	$ npm install node-dijkstra --save
 
Clone this repository:

	$ cd ~/contiki/examples
	$ git clone --recursive https://github.com/marciolm/sd6wsn.git 

Compile the Cooja [3]

	$ cd ~/contiki/tools/cooja
	$ ant jar
	
Rename the file ~/.cooja.user.properties to a backup and insert at the end of the original one: 

	DEFAULT_PROJECTDIRS=[CONTIKI_DIR]/tools/cooja/apps/mrm;[CONTIKI_DIR]/tools/cooja/apps/mspsim;[CONTIKI_DIR]/tools/cooja/apps/avrora;[CONTIKI_DIR]/tools/cooja/apps/serial_socket;[CONTIKI_DIR]/tools/cooja/apps/collect-view;[CONTIKI_DIR]/tools/cooja/apps/powertracker;[CONTIKI_DIR]/tools/cooja/apps/serial2pty;[CONTIKI_DIR]/tools/cooja/apps/radiologger-headless
	PATH_CONTIKI=/home/user/contiki

Install the Radiologger

	$ git clone https://github.com/cetic/cooja-radiologger-headless.git

Rename it to 'radiologger-headless' and move it to tools/cooja/apps
go to radiologger-headless folder:

    $ mv cooja-radiologger-headless apps/radiologger-headless
    $ cd apps/radiologger-headless
	$ ant jar
	
Untar the file serial2.pty.tar.gz and copy the content to ~/contiki/tools/cooja/apps:

	$ cd ~/contiki/examples/sd6wsn
    $ tar -xvzf serial2.pty.tar.gz
    $ mv serial2pty ../../tools/cooja/apps/

Compile the program tunslip6:

	$ pushd ~/contiki/tools/
	$ make tunslip6
	$ popd

The original ~/contiki/core/net/ip/tcpip.c file should be replaced by the altered version. This is the only change in the Contiki tree in this project. The original file will be renamed to .ori extension.

    $ cd ~/contiki/examples/sd6wsn
    $ cp ~/contiki/core/net/ip/tcpip.c ~/contiki/core/net/ip/tcpip.c.ori
    $ cp newtcpip.c ~/contiki/core/net/ip/tcpip.c

Open a terminal and run Cooja simulation:

	$ cd ~/contiki/examples/sd6wsn
	$ ./inicia-cooja.sh 

* To connect your Linux host machine in the simulated WSN, you have to start the Slip connection to 6LBR, start the bridge br0 and enable the IPv6 routing using the inicia-router.sh script.

Open a second terminal:

	$ cd ~/contiki/examples/sd6wsn
	$ ./inicia-router-loop.sh

The SD6WSN Node Controller and a application to install the flows into the nodes is on the scripts folder. To run them, start a third terminal:

	$ cd ~/contiki/examples/sd6wsn/scripts
	$ node sd6wsn-controller.js

* [1] https://github.com/contiki-os/contiki/tree/master/examples/er-rest-example
* [2] http://libcoap.net
* [3] https://github.com/contiki-os/contiki/wiki/An-Introduction-to-Cooja
