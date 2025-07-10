# -*- mode: ruby -*-
# vi: set ft=ruby :

# if you update this file, please consider updating .travis.yml too

require 'yaml'

current_dir    = File.dirname(File.expand_path(__FILE__))
configfile     = YAML.load_file("#{current_dir}/.vagrantconfig.yml")
yaml_config    = configfile['configs']['dev']

Vagrant.configure(2) do |config|
  # Updated to Ubuntu 24.04 Noble Numbat
  config.vm.box = "ubuntu/jammy64"
  config.vm.provider :virtualbox do |vb|
    vb.customize ["modifyvm", :id, "--memory", "4096"]
    vb.customize ["modifyvm", :id, "--cpus", "4"]
  end

  $config_shell = <<-'SHELL'
    set -e
    set -x

    export DEBIAN_FRONTEND=noninteractive

    # Update and Upgrade
    sudo apt-get update -y
    sudo apt-get dist-upgrade -y

    # Essential Packages
    sudo apt install -y ccache wget git build-essential \
      libglu1-mesa-dev flite1-dev libsdl2-dev libsdl1.2-dev libsndfile-dev libssl-dev libudev-dev \
      python3-serial python3-pexpect python3-pip patchelf

    # Install Qt dependencies
    sudo apt install -y qt5-qmake qtbase5-dev qtscript5-dev libqt5webkit5-dev \
      libqt5serialport5-dev libqt5svg5-dev libqt5opengl5-dev qml-module-qtquick-controls \
      libqt5datavisualization5-dev
          

    # Clean up apt cache to save space
    sudo apt-get clean
    sudo rm -rf /var/lib/apt/lists/*

    # Initialize submodules
    echo 'Initialising submodules'
    su - vagrant -c 'cd %{project_root_dir}; git submodule update --init --recursive'

    # Install aqtinstall for Qt setup
    #su - vagrant -c "pip3 install --user aqtinstall"

    # Install Qt dependencies
    # This is the old way to install pre-built version of Qt5.
    # dir="%{qt_deps_unpack_dir}"
    # version="%{qt_deps_ver}"
    # host="linux"
    # target="desktop"
    # modules="qtcharts"
    # su - vagrant -c "rm -rf ${dir}"
    # su - vagrant -c "mkdir -p ${dir}"
    # su - vagrant -c "python3 -m aqt install-qt -O ${dir} ${host} ${target} ${version} -m ${modules}"

    # # Copy Qt deps into the shadow-build directory
    # su - vagrant -c 'mkdir -p %{qt_deps_dir}'
    # su - vagrant -c 'cp -a %{qt_deps_bin_unpack_dir} %{qt_deps_bin_dir}'
    # su - vagrant -c 'cp -a %{qt_deps_lib_unpack_dir} %{qt_deps_lib_dir}'
    # su - vagrant -c 'cp -a %{qt_deps_plugins_unpack_dir} %{qt_deps_plugins_dir}'
    # su - vagrant -c 'cp -a %{qt_deps_qml_unpack_dir} %{qt_deps_qml_dir}'

    # Write out scripts for build commands
    su - vagrant -c "cat <<'QMAKE' >do-qmake.sh
#!/bin/bash

set -e
set -x

cd %{shadow_build_dir}
#export LD_LIBRARY_PATH=%{qt_deps_lib_unpack_dir}
export PATH=%{qt_deps_bin_unpack_dir}:\$PATH
qmake -r %{pro} CONFIG+=\${CONFIG} CONFIG+=WarningsAsErrorsOn -spec %{spec}
QMAKE
"
    su - vagrant -c "cat <<'MAKE' >do-make.sh
#!/bin/bash

set -e
set -x

cd %{shadow_build_dir}
#export LD_LIBRARY_PATH=%{qt_deps_lib_unpack_dir}
export PATH=%{qt_deps_bin_unpack_dir}:\$PATH
make -j4
MAKE
"
    su - vagrant -c "chmod +x do-qmake.sh do-make.sh"

    # Run the build scripts
    su - vagrant -c ./do-qmake.sh
    su - vagrant -c ./do-make.sh

  SHELL

  config.vm.provision "dev", type: "shell", inline: $config_shell % {
    :shadow_build_dir => yaml_config['shadow_build_dir'],
    :pro => yaml_config['pro'],
    :spec => yaml_config['spec'],
    :project_root_dir => yaml_config['project_root_dir'],

    :qt_deps_ver => yaml_config['qt_deps_ver'],

    :qt_deps_unpack_parent_dir => yaml_config['qt_deps_unpack_parent_dir'],
    :qt_deps_unpack_dir => yaml_config['qt_deps_unpack_dir'],
    :qt_deps_bin_unpack_dir => yaml_config['qt_deps_bin_unpack_dir'],
    :qt_deps_lib_unpack_dir => yaml_config['qt_deps_lib_unpack_dir'],
    :qt_deps_plugins_unpack_dir => yaml_config['qt_deps_plugins_unpack_dir'],
    :qt_deps_qml_unpack_dir => yaml_config['qt_deps_qml_unpack_dir'],

    :qt_deps_dir => yaml_config['qt_deps_dir'],
    :qt_deps_bin_dir => yaml_config['qt_deps_bin_dir'],
    :qt_deps_lib_dir => yaml_config['qt_deps_lib_dir'],
    :qt_deps_plugins_dir => yaml_config['qt_deps_plugins_dir'],
    :qt_deps_qml_dir => yaml_config['qt_deps_qml_dir'],
  }

end
