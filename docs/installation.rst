Installation
============

To setup OpenSCADA, Kronos, grpc and bazel package manager need to be first installed. 

Installing Kronos
^^^^^^^^^^^^^^^^^

To install Kronos, follow steps included in the `documentation <https://kronoz.readthedocs.io/en/latest/index.html>`_. To install all the other dependencies, follow the steps given below.

Installing Dependencies
^^^^^^^^^^^^^^^^^^^^^^^
* Install pip::

	sudo apt-get install python-pip
        pip install --upgrade pip

* To install grpc and other dependencies for python execute the following commands::

	sudo apt-get install python-tk
	sudo python -m pip install grpcio grpcio-tools numpy opencv-python matplotlib

Installing Bazel
^^^^^^^^^^^^^^^^

* Install OpenJDK::

	sudo apt-get install openjdk-8-jdk

* Install Bazel 0.23.1::

	wget https://github.com/bazelbuild/bazel/releases/download/0.23.1/bazel-0.23.1-installer-linux-x86_64.sh
	chmod +x bazel-0.23.1-installer-linux-x86_64.sh
	./bazel-0.23.1-installer-linux-x86_64.sh
	

Make sure the version of bazel is atleast 0.23.1 (run command: bazel version)

Installing OpenSCADA
^^^^^^^^^^^^^^^^^^^^

* Clone the git repository to $HOME directory::

	cd ~/ && git clone https://github.com/Vignesh2208/OpenSCADA.git && cd OpenSCADA

* Run the installation setup script. This would take a while the first time it is run because bazel downloads and installs other dependencies::

	sudo ./setup.sh install

* Update environment variables. Add OSCADA_INSTALLATION and update PYTHONPATH in bashrc::

	export OSCADA_INSTALLATION=<path to installation directory>
	export PYTHONPATH="${PYTHONPATH}:${OSCADA_INSTALLATION}"
	
Ready to use VM
^^^^^^^^^^^^^^^

A link to use VM containing OpenSCADA and Kronos will be provided upon request. Please contact projectmoses@illinois.edu.


Uninstalling OpenSCADA
^^^^^^^^^^^^^^^^^^^^^^

* To uninstall/cleanup run the following command::

	sudo ./setup.sh uninstall

 
	
