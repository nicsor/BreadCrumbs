# BreadCrumbs

Collection of small components and applications around a potentialy stupid management system :)
- Work in progress -

## Build Dependencies:
The docker image attached is provided to help build the applications.
It's slightly bigger than necesary as it includes some libraries to be used in the future.

To create the docker image simply run from within the docker folder:
 docker image build -t nna/generic_builder:latest .

## Global structure
The project uses CMake to manage the build.

The intention is to have in:
 - apps: applications
 - cmake: script to find and set up environment variables for external libraries
 - components: optional libraries
 - core: common libraries
 - docker: common libraries
 - samples: sample applications and libraries

To create a new component/application it is enough to copy an existing one and give it a new name. The name of the parent folder shall be used as the component/application name. There could be collision so, unique names should be chosen, in the 4 source directories.

Current feature:
 - dinamic initialization of components based on a prototype pattern
 - potential logger implementations
 - a message dispatcher among components
 - some helper classes.

## Building and running
 - git clone https://github.com/nicsor/BreadCrumbs.git
 - docker image build -f BreadCrumbs/docker/Dockerfile -t nna/generic_builder:latest .
 - docker run -i -v ${PWD}:${PWD} -w ${PWD} -t nna/generic_builder
 - mkdir build && cd build
 - cmake -G "Unix Makefiles" ../BreadCrumbs
 - make all
   or 
 - make <specific component>
   or
 - make tests

For running the LogGenerator app, copy the sample config.json from 'BreadCrumbs/apps/LogGenerator/config/config.json' to 'build/apps/LogGenerator', and simply run the app: './LogGenerator'
