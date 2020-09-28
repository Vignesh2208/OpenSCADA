Installation
============

To setup OpenSCADA, Kronos, grpc and bazel package manager need to be first installed. 

Installing Kronos
^^^^^^^^^^^^^^^^^

To install Kronos, follow steps included in the `documentation <https://kronoz.readthedocs.io/en/latest/index.html>`_. To install all the other dependencies, follow the steps given below.

Installing Dependencies
^^^^^^^^^^^^^^^^^^^^^^^
* Ensure default python is python 3.6 or above.
  	Follow instructions: https://linuxconfig.org/how-to-change-from-default-to-alternative-python-version-on-debian-linux

* Install pip::

	sudo apt-get install python3-pip
        pip3 install --upgrade pip

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
	export OSCADA_PROTO_PATH=${OSCADA_INSTALLATION}/bazel-out/k8-fastbuild/genfiles/py_access_service_proto_pb
	export PYTHONPATH="${PYTHONPATH}:${OSCADA_INSTALLATION}:${OSCADA_PROTO_PATH}"

* Preserve some environment variables across sudo. Append the following lines after running command sudo visudo::

	Defaults        env_keep += PYTHONPATH
	Defaults        env_keep += OSCADA_INSTALLATION

Installing CORE Network Emulator (Optional)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This step is optional. To run example on a CORE network follow these steps.

* Install iproute2 4.5+. Clone and install from: https://github.com/shemminger/iproute2
	
  
* Install CORE emulator: Follow instructions from here: http://coreemu.github.io/core/install.html.
  Note: On ubuntu 16.04, the installation process fails to install OSPF-MDR because of conflicting
  header files. This limits CORE functionality to some extent.

* Edit /usr/local/bin/core-python script by pre-pending the following two lines before the exec statement::

	export PYTHONPATH="${PYTHONPATH}:${HOME}/.local/lib/python3.x/site-packages"
	export PYTHONPATH="${PYTHONPATH}:/usr/local/lib/python3.x/dist-packages"

  where python3.x should be changed according to the python version installed on your system. This ensures that
  core-python can find other packages installed on your system outside the virtual environment created by CORE.

  	

Ready to use VM
^^^^^^^^^^^^^^^

Link to a ready to use Kronos VM (version 1.3) can be downloaded from `here <https://drive.google.com/drive/folders/1wwJ6tO7XaGj3C_8ZophNsmbc2vO-oxH4?usp=sharing>`_.
The VM also contains installations of companion projects `OpenSCADA <http://github.com/Vignesh2208/OpenSCADA>`_ and `Melody <http://github.com/Vignesh2208/Melody>`_.
which use Kronos for cyber-physical system modelling.

Username: kronos
Password: kronos

Make sure Kronos is loaded after each reboot before running experiments which use it::

  cd ~/Kronos
  sudo make load



Uninstalling OpenSCADA
^^^^^^^^^^^^^^^^^^^^^^

* To uninstall/cleanup run the following command::

	sudo ./setup.sh uninstall


Uninstalling CORE
^^^^^^^^^^^^^^^^^

To uninstall CORE, follow instructions described here: http://coreemu.github.io/core/install.html
	
